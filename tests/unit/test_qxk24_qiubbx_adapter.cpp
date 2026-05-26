// =============================================================================
// test_qxk24_qiubbx_adapter.cpp
// QXK24 Universal Kernel — QIUBBX Adapter Tests
// =============================================================================
// Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
// Created : 2026-05-26
// Repository : https://github.com/qxengine/qxk24
// Path : tests/unit/test_qxk24_qiubbx_adapter.cpp
// License : Apache 2.0
//
// Description:
// Tests the QIUBBX Constitutional Adapter protocol for practical application
// governance decisions.
//

#include <cstring>
#include <gtest/gtest.h>
#include "qxk24/qiubbx/qxk24_qiubbx_adapter.h"

class QIUBBXAdapterTest : public ::testing::Test {
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

TEST_F(QIUBBXAdapterTest, CreateReturnsAdapter) {
    EXPECT_NE(adapter, nullptr);
}

TEST_F(QIUBBXAdapterTest, DefaultModeIsEnforce) {
    QXK24QIUBBXMode mode = QXK24_QIUBBX_MODE_SHADOW;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_get_mode(adapter, &mode));
    EXPECT_EQ(QXK24_QIUBBX_MODE_ENFORCE, mode);
}

TEST_F(QIUBBXAdapterTest, CreateRejectsNullKernel) {
    qxk24_qiubbx_adapter_t out = nullptr;
    EXPECT_EQ(QXK24_ERR_NULL_PARAM, qxk24_qiubbx_adapter_create(nullptr, &out));
}

TEST_F(QIUBBXAdapterTest, UserActionAllowsLowRiskConsent) {
    QXK24QIUBBXUserAction input = {};
    input.risk_score = 0.05f;
    input.has_consent = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_user_action(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, out.decision);
    EXPECT_EQ(QXK24_QIUBBX_FLOW_USER_ACTION, out.flow);
}

TEST_F(QIUBBXAdapterTest, UserActionBlocksWithoutConsent) {
    QXK24QIUBBXUserAction input = {};
    input.risk_score = 0.05f;
    input.has_consent = false;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_user_action(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, out.decision);
}

TEST_F(QIUBBXAdapterTest, UserActionReviewsHighRisk) {
    QXK24QIUBBXUserAction input = {};
    input.risk_score = 0.80f;
    input.has_consent = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_user_action(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_REVIEW, out.decision);
}

TEST_F(QIUBBXAdapterTest, TransactionAllowsTrustedFlow) {
    QXK24QIUBBXTransaction input = {};
    input.amount = 42.0;
    input.buyer_trust = 0.95f;
    input.seller_trust = 0.95f;
    input.inventory_available = true;
    input.payment_authorized = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_transaction(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, out.decision);
}

TEST_F(QIUBBXAdapterTest, TransactionBlocksNoPayment) {
    QXK24QIUBBXTransaction input = {};
    input.amount = 42.0;
    input.buyer_trust = 0.95f;
    input.seller_trust = 0.95f;
    input.inventory_available = true;
    input.payment_authorized = false;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_transaction(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, out.decision);
}

TEST_F(QIUBBXAdapterTest, ListingAllowsTruthfulStockedItem) {
    QXK24QIUBBXListing input = {};
    input.price = 10.0;
    input.stock_quantity = 3;
    input.truthful_description = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_listing(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, out.decision);
}

TEST_F(QIUBBXAdapterTest, ListingBlocksProhibitedItem) {
    QXK24QIUBBXListing input = {};
    input.price = 10.0;
    input.stock_quantity = 3;
    input.truthful_description = true;
    input.prohibited_item = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_listing(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, out.decision);
}

TEST_F(QIUBBXAdapterTest, PaymentAllowsAuthorizedLowRisk) {
    QXK24QIUBBXPayment input = {};
    input.amount = 20.0;
    input.fraud_score = 0.05f;
    input.authorized = true;
    input.balance_available = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_payment(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, out.decision);
}

TEST_F(QIUBBXAdapterTest, PaymentBlocksFraudRisk) {
    QXK24QIUBBXPayment input = {};
    input.amount = 20.0;
    input.fraud_score = 0.95f;
    input.authorized = true;
    input.balance_available = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_payment(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, out.decision);
}

TEST_F(QIUBBXAdapterTest, DisputeRoutesToReviewWhenSellerMissing) {
    QXK24QIUBBXDispute input = {};
    input.evidence_count = 2;
    input.buyer_claim_present = true;
    input.seller_response_present = false;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_dispute(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_REVIEW, out.decision);
}

TEST_F(QIUBBXAdapterTest, DisputeBlocksWithoutClaim) {
    QXK24QIUBBXDispute input = {};
    input.evidence_count = 2;
    input.buyer_claim_present = false;
    input.seller_response_present = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_dispute(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, out.decision);
}

TEST_F(QIUBBXAdapterTest, ReportCountsDecisions) {
    QXK24QIUBBXUserAction input = {};
    input.has_consent = true;
    input.risk_score = 0.05f;
    QXK24QIUBBXDecisionResult out;
    qxk24_qiubbx_govern_user_action(adapter, &input, &out);
    input.has_consent = false;
    qxk24_qiubbx_govern_user_action(adapter, &input, &out);
    QXK24QIUBBXAdapterReport report;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_get_constitutional_report(adapter, &report));
    EXPECT_EQ(2u, report.decisions_total);
    EXPECT_EQ(1u, report.allow_count);
    EXPECT_EQ(1u, report.block_count);
}

TEST_F(QIUBBXAdapterTest, ShadowModeAllowsButKeepsBlockRecommendation) {
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_set_mode(
        adapter, QXK24_QIUBBX_MODE_SHADOW));
    QXK24QIUBBXPayment input = {};
    input.amount = 20.0;
    input.fraud_score = 0.95f;
    input.authorized = true;
    input.balance_available = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_payment(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, out.decision);
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, out.recommended_decision);
    EXPECT_EQ(QXK24_QIUBBX_MODE_SHADOW, out.mode);
}

TEST_F(QIUBBXAdapterTest, ReviewOnlyDowngradesBlockToReview) {
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_set_mode(
        adapter, QXK24_QIUBBX_MODE_REVIEW_ONLY));
    QXK24QIUBBXPayment input = {};
    input.amount = 20.0;
    input.fraud_score = 0.95f;
    input.authorized = true;
    input.balance_available = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_payment(adapter, &input, &out));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_REVIEW, out.decision);
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, out.recommended_decision);
}

TEST_F(QIUBBXAdapterTest, ReportTracksModeAndRecommendation) {
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_set_mode(
        adapter, QXK24_QIUBBX_MODE_SHADOW));
    QXK24QIUBBXUserAction input = {};
    input.has_consent = false;
    QXK24QIUBBXDecisionResult out;
    qxk24_qiubbx_govern_user_action(adapter, &input, &out);
    QXK24QIUBBXAdapterReport report;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_get_constitutional_report(adapter, &report));
    EXPECT_EQ(QXK24_QIUBBX_MODE_SHADOW, report.mode);
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, report.last_decision);
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK, report.last_recommended_decision);
}

TEST_F(QIUBBXAdapterTest, AuditEventCopiesDecisionFields) {
    QXK24QIUBBXPayment input = {};
    input.amount = 20.0;
    input.fraud_score = 0.95f;
    input.authorized = true;
    input.balance_available = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_payment(adapter, &input, &out));
    QXK24QIUBBXAuditEvent event;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_create_audit_event(
        &out, "req-1", "payment-1", "hash-1", &event));
    EXPECT_GT(event.timestamp_ms, 0ULL);
    EXPECT_STREQ("req-1", event.request_id);
    EXPECT_STREQ("payment-1", event.subject_id);
    EXPECT_STREQ("hash-1", event.input_hash);
    EXPECT_EQ(out.decision, event.decision);
    EXPECT_EQ(out.recommended_decision, event.recommended_decision);
    EXPECT_STREQ(QXK24_VERSION_STRING, event.qxk24_version);
    EXPECT_STREQ(QXK24_QIUBBX_ADAPTER_VERSION, event.adapter_version);
}

TEST_F(QIUBBXAdapterTest, DecisionJsonContainsBackendContract) {
    QXK24QIUBBXPayment input = {};
    input.amount = 20.0;
    input.fraud_score = 0.95f;
    input.authorized = true;
    input.balance_available = true;
    QXK24QIUBBXDecisionResult out;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_govern_payment(adapter, &input, &out));
    char json[QXK24_QIUBBX_JSON_LEN];
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_decision_to_json(
        &out, "req-2", json, sizeof(json)));
    EXPECT_NE(nullptr, std::strstr(json, "\"decision\":\"BLOCK\""));
    EXPECT_NE(nullptr, std::strstr(json, "\"flow\":\"PAYMENT\""));
    EXPECT_NE(nullptr, std::strstr(json, "\"mode\":\"ENFORCE\""));
    EXPECT_NE(nullptr, std::strstr(json, "\"requestId\":\"req-2\""));
}

TEST_F(QIUBBXAdapterTest, DecisionJsonDetectsSmallBuffer) {
    QXK24QIUBBXDecisionResult out = {};
    out.decision = QXK24_QIUBBX_DECISION_ALLOW;
    out.recommended_decision = QXK24_QIUBBX_DECISION_ALLOW;
    out.flow = QXK24_QIUBBX_FLOW_USER_ACTION;
    out.mode = QXK24_QIUBBX_MODE_ENFORCE;
    char json[8];
    EXPECT_EQ(QXK24_ERR_CAPACITY_FULL, qxk24_qiubbx_decision_to_json(
        &out, "req-small", json, sizeof(json)));
}

TEST_F(QIUBBXAdapterTest, TransactionPilotReturnsDecisionAuditAndJson) {
    QXK24QIUBBXTransactionPilotRequest request = {};
    std::snprintf(request.request_id, sizeof(request.request_id), "req-tx-1");
    std::snprintf(request.transaction_id, sizeof(request.transaction_id), "tx-1");
    std::snprintf(request.input_hash, sizeof(request.input_hash), "hash-tx-1");
    request.amount = 100.0;
    request.buyer_trust = 0.95f;
    request.seller_trust = 0.95f;
    request.inventory_available = true;
    request.payment_authorized = true;
    QXK24QIUBBXTransactionPilotResponse response;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_pilot_transaction(
        adapter, &request, &response));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, response.decision.decision);
    EXPECT_STREQ("req-tx-1", response.audit_event.request_id);
    EXPECT_STREQ("tx-1", response.audit_event.subject_id);
    EXPECT_NE(nullptr, std::strstr(response.json, "\"flow\":\"TRANSACTION\""));
}

TEST_F(QIUBBXAdapterTest, TransactionPilotShadowAllowsBlockedRecommendation) {
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_set_mode(
        adapter, QXK24_QIUBBX_MODE_SHADOW));
    QXK24QIUBBXTransactionPilotRequest request = {};
    std::snprintf(request.request_id, sizeof(request.request_id), "req-tx-2");
    std::snprintf(request.transaction_id, sizeof(request.transaction_id), "tx-2");
    request.amount = 100.0;
    request.buyer_trust = 0.95f;
    request.seller_trust = 0.95f;
    request.inventory_available = true;
    request.payment_authorized = false;
    QXK24QIUBBXTransactionPilotResponse response;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_pilot_transaction(
        adapter, &request, &response));
    EXPECT_EQ(QXK24_QIUBBX_DECISION_ALLOW, response.decision.decision);
    EXPECT_EQ(QXK24_QIUBBX_DECISION_BLOCK,
              response.decision.recommended_decision);
    EXPECT_EQ(QXK24_QIUBBX_MODE_SHADOW, response.audit_event.mode);
}

TEST_F(QIUBBXAdapterTest, TransactionPilotRecordsAuditLedger) {
    QXK24QIUBBXTransactionPilotRequest request = {};
    std::snprintf(request.request_id, sizeof(request.request_id), "req-ledger-1");
    std::snprintf(request.transaction_id, sizeof(request.transaction_id),
                  "tx-ledger-1");
    request.amount = 50.0;
    request.buyer_trust = 0.90f;
    request.seller_trust = 0.90f;
    request.inventory_available = true;
    request.payment_authorized = true;
    QXK24QIUBBXTransactionPilotResponse response;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_pilot_transaction(
        adapter, &request, &response));
    uint32_t count = 0;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_audit_count(adapter, &count));
    EXPECT_EQ(1u, count);
    QXK24QIUBBXAuditEvent event;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_get_audit_event(
        adapter, 0, &event));
    EXPECT_STREQ("req-ledger-1", event.request_id);
    EXPECT_STREQ("tx-ledger-1", event.subject_id);
}

TEST_F(QIUBBXAdapterTest, ManualAuditRecordCanBeCleared) {
    QXK24QIUBBXAuditEvent event = {};
    std::snprintf(event.request_id, sizeof(event.request_id), "req-manual");
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_record_audit(adapter, &event));
    uint32_t count = 0;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_audit_count(adapter, &count));
    EXPECT_EQ(1u, count);
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_clear_audit(adapter));
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_audit_count(adapter, &count));
    EXPECT_EQ(0u, count);
}

TEST_F(QIUBBXAdapterTest, AuditLedgerKeepsBoundedRecentEvents) {
    for (uint32_t i = 0; i < QXK24_QIUBBX_AUDIT_CAPACITY + 2U; i++) {
        QXK24QIUBBXAuditEvent event = {};
        std::snprintf(event.request_id, sizeof(event.request_id), "req-%u", i);
        ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_record_audit(adapter, &event));
    }
    uint32_t count = 0;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_audit_count(adapter, &count));
    EXPECT_EQ(QXK24_QIUBBX_AUDIT_CAPACITY, count);
    QXK24QIUBBXAuditEvent first;
    ASSERT_EQ(QXK24_OK, qxk24_qiubbx_adapter_get_audit_event(
        adapter, 0, &first));
    EXPECT_STREQ("req-2", first.request_id);
}

TEST_F(QIUBBXAdapterTest, AuditLedgerRejectsOutOfRangeRead) {
    QXK24QIUBBXAuditEvent event;
    EXPECT_EQ(QXK24_ERR_NOT_FOUND, qxk24_qiubbx_adapter_get_audit_event(
        adapter, 0, &event));
}

TEST_F(QIUBBXAdapterTest, NamesAreStable) {
    EXPECT_STREQ("ALLOW",
                 qxk24_qiubbx_decision_name(QXK24_QIUBBX_DECISION_ALLOW));
    EXPECT_STREQ("PAYMENT",
                 qxk24_qiubbx_flow_name(QXK24_QIUBBX_FLOW_PAYMENT));
    EXPECT_STREQ("REVIEW_ONLY",
                 qxk24_qiubbx_mode_name(QXK24_QIUBBX_MODE_REVIEW_ONLY));
}

TEST_F(QIUBBXAdapterTest, NullInputsRejected) {
    QXK24QIUBBXDecisionResult out;
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_govern_user_action(nullptr, nullptr, &out));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_get_constitutional_report(nullptr, nullptr));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_create_audit_event(nullptr, nullptr, nullptr,
                                               nullptr, nullptr));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_decision_to_json(nullptr, nullptr, nullptr, 0));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_pilot_transaction(nullptr, nullptr, nullptr));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_adapter_record_audit(nullptr, nullptr));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_adapter_audit_count(nullptr, nullptr));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_adapter_get_audit_event(nullptr, 0, nullptr));
    EXPECT_EQ(QXK24_ERR_NULL_PARAM,
              qxk24_qiubbx_adapter_clear_audit(nullptr));
}
