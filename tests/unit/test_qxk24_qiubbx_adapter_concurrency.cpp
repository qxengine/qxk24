// =============================================================================
// test_qxk24_qiubbx_adapter_concurrency.cpp
// QXK24 Universal Kernel — QIUBBX Adapter Concurrency Tests
// =============================================================================
// Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
// Created : 2026-05-26
// Repository : https://github.com/qxengine/qxk24
// Path : tests/unit/test_qxk24_qiubbx_adapter_concurrency.cpp
// License : Apache 2.0
//
// Description:
// Tests multi-threaded QIUBBX adapter calls for backend pilot safety.
//

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include "qxk24/qiubbx/qxk24_qiubbx_adapter.h"

class QIUBBXAdapterConcurrencyTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_qiubbx_adapter_t adapter = nullptr;

    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(QXK24_OK, qxk24_create(&cfg, &kernel));
        ASSERT_EQ(QXK24_OK, qxk24_start(kernel));
        ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_create(kernel, &adapter));
    }

    void TearDown() override {
        EXPECT_EQ(QXK24_OK, qxk24_qiubbx_adapter_destroy(adapter));
        qxk24_stop(kernel);
        qxk24_destroy(kernel);
    }
};

TEST_F(QIUBBXAdapterConcurrencyTest, ConcurrentTransactionPilotCallsAreCounted) {
    constexpr uint32_t kThreads = 8;
    constexpr uint32_t kCallsPerThread = 25;
    std::vector<std::thread> threads;

    for (uint32_t t = 0; t < kThreads; t++) {
        threads.emplace_back([this, t]() {
            for (uint32_t i = 0; i < kCallsPerThread; i++) {
                QXK24QIUBBXTransactionPilotRequest request = {};
                std::snprintf(request.request_id, sizeof(request.request_id),
                              "req-%u-%u", t, i);
                std::snprintf(request.transaction_id,
                              sizeof(request.transaction_id), "tx-%u-%u", t, i);
                request.amount = 10.0;
                request.buyer_trust = 0.90f;
                request.seller_trust = 0.90f;
                request.inventory_available = true;
                request.payment_authorized = true;
                QXK24QIUBBXTransactionPilotResponse response;
                EXPECT_EQ(QXK24_OK, qxk24_qiubbx_pilot_transaction(
                    adapter, &request, &response));
            }
        });
    }
    for (auto &thread : threads) thread.join();

    QXK24QIUBBXAdapterReport report;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_get_constitutional_report(
        adapter, &report));
    EXPECT_EQ(kThreads * kCallsPerThread, report.decisions_total);
    EXPECT_EQ(kThreads * kCallsPerThread, report.audit_events_total);
    EXPECT_EQ(QXK24_QIUBBX_AUDIT_CAPACITY, report.audit_events_stored);
}

TEST_F(QIUBBXAdapterConcurrencyTest, ConcurrentModeChangesRemainValid) {
    constexpr uint32_t kThreads = 4;
    std::vector<std::thread> threads;
    for (uint32_t t = 0; t < kThreads; t++) {
        threads.emplace_back([this, t]() {
            const QXK24QIUBBXMode mode = (t % 2U == 0U)
                ? QXK24_QIUBBX_MODE_SHADOW
                : QXK24_QIUBBX_MODE_REVIEW_ONLY;
            EXPECT_EQ(QXK24_OK, qxk24_qiubbx_adapter_set_mode(adapter, mode));
        });
    }
    for (auto &thread : threads) thread.join();

    QXK24QIUBBXMode mode = QXK24_QIUBBX_MODE_ENFORCE;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_get_mode(adapter, &mode));
    EXPECT_TRUE(mode == QXK24_QIUBBX_MODE_SHADOW ||
                mode == QXK24_QIUBBX_MODE_REVIEW_ONLY);
}
