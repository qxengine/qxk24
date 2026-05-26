// =============================================================================
// test_qxk24_qiubbx_audit_sync.cpp
// QXK24 Universal Kernel — QIUBBX Audit Sync Tests
// =============================================================================
// Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
// Created : 2026-05-26
// Repository : https://github.com/qxengine/qxk24
// Path : tests/unit/test_qxk24_qiubbx_audit_sync.cpp
// License : Apache 2.0
//
// Description:
// Tests backend persistence handoff serialization for QIUBBX audits.
//

#include <gtest/gtest.h>
#include <cstring>
#include "qxk24/qiubbx/qxk24_qiubbx_adapter.h"

class QIUBBXAuditSyncTest : public ::testing::Test {
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

TEST_F(QIUBBXAuditSyncTest, AuditEventSerializesForBackendStorage) {
    QXK24QIUBBXTransactionPilotRequest request = {};
    std::snprintf(request.request_id, sizeof(request.request_id), "req-sync");
    std::snprintf(request.transaction_id, sizeof(request.transaction_id),
                  "tx-sync");
    std::snprintf(request.input_hash, sizeof(request.input_hash), "hash-sync");
    request.amount = 25.0;
    request.buyer_trust = 0.90f;
    request.seller_trust = 0.90f;
    request.inventory_available = true;
    request.payment_authorized = true;
    QXK24QIUBBXTransactionPilotResponse response;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_pilot_transaction(
        adapter, &request, &response));

    QXK24QIUBBXAuditEvent event;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_get_audit_event(
        adapter, 0, &event));
    char json[QXK24_QIUBBX_JSON_LEN];
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_audit_event_to_json(
        &event, json, sizeof(json)));
    EXPECT_NE(nullptr, std::strstr(json, "\"eventId\":\"qxk24-"));
    EXPECT_NE(nullptr, std::strstr(json, "\"requestId\":\"req-sync\""));
    EXPECT_NE(nullptr, std::strstr(json, "\"subjectId\":\"tx-sync\""));
    EXPECT_NE(nullptr, std::strstr(json, "\"inputHash\":\"hash-sync\""));
    EXPECT_NE(nullptr, std::strstr(json, "\"decision\":\"ALLOW\""));
}

TEST_F(QIUBBXAuditSyncTest, AuditEventSerializationRejectsTinyBuffer) {
    QXK24QIUBBXAuditEvent event = {};
    std::snprintf(event.event_id, sizeof(event.event_id), "event-small");
    char json[8];
    EXPECT_EQ(QXK24_ERR_CAPACITY_FULL, qxk24_qiubbx_audit_event_to_json(
        &event, json, sizeof(json)));
}

TEST_F(QIUBBXAuditSyncTest, AuditEventSerializationRejectsNulls) {
    QXK24QIUBBXAuditEvent event = {};
    char json[QXK24_QIUBBX_JSON_LEN];
    EXPECT_EQ(QXK24_ERR_NULL_PARAM, qxk24_qiubbx_audit_event_to_json(
        nullptr, json, sizeof(json)));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM, qxk24_qiubbx_audit_event_to_json(
        &event, nullptr, sizeof(json)));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM, qxk24_qiubbx_audit_event_to_json(
        &event, json, 0));
}
