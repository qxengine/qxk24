// =============================================================================
// qxk24_qiubbx_adapter.h
// QXK24 Universal Kernel — QIUBBX Constitutional Adapter
// =============================================================================
// Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
// Created : 2026-05-26
// Repository : https://github.com/qxengine/qxk24
// Path : include/qxk24/qiubbx/qxk24_qiubbx_adapter.h
// License : Apache 2.0
//
// Description:
// Declares the application-facing QIUBBX adapter protocol for governing user
// actions, transactions, listings, payments, disputes, and reports.
//

#ifndef QXK24_QIUBBX_ADAPTER_H
#define QXK24_QIUBBX_ADAPTER_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_QIUBBX_ADAPTER_VERSION "1.0.0"
#define QXK24_QIUBBX_REASON_LEN 256U
#define QXK24_QIUBBX_LABEL_LEN 64U
#define QXK24_QIUBBX_JSON_LEN 1024U
#define QXK24_QIUBBX_AUDIT_CAPACITY 64U

typedef enum {
    QXK24_QIUBBX_FLOW_USER_ACTION = 0,
    QXK24_QIUBBX_FLOW_TRANSACTION = 1,
    QXK24_QIUBBX_FLOW_LISTING = 2,
    QXK24_QIUBBX_FLOW_PAYMENT = 3,
    QXK24_QIUBBX_FLOW_DISPUTE = 4
} QXK24QIUBBXFlow;

typedef enum {
    QXK24_QIUBBX_DECISION_ALLOW = 0,
    QXK24_QIUBBX_DECISION_WARN = 1,
    QXK24_QIUBBX_DECISION_REVIEW = 2,
    QXK24_QIUBBX_DECISION_BLOCK = 3
} QXK24QIUBBXDecision;

typedef enum {
    QXK24_QIUBBX_MODE_SHADOW = 0,
    QXK24_QIUBBX_MODE_REVIEW_ONLY = 1,
    QXK24_QIUBBX_MODE_ENFORCE = 2
} QXK24QIUBBXMode;

typedef struct {
    QXK24QIUBBXFlow flow;
    QXK24QIUBBXDecision decision;
    QXK24QIUBBXDecision recommended_decision;
    QXK24QIUBBXMode mode;
    float constitutional_score;
    float kernel_health;
    uint8_t severity;
    char reason[QXK24_QIUBBX_REASON_LEN];
} QXK24QIUBBXDecisionResult;

typedef struct {
    char user_id[QXK24_QIUBBX_LABEL_LEN];
    char action[QXK24_QIUBBX_LABEL_LEN];
    float risk_score;
    bool has_consent;
    bool requires_review;
} QXK24QIUBBXUserAction;

typedef struct {
    char transaction_id[QXK24_QIUBBX_LABEL_LEN];
    double amount;
    float buyer_trust;
    float seller_trust;
    bool inventory_available;
    bool payment_authorized;
} QXK24QIUBBXTransaction;

typedef struct {
    char listing_id[QXK24_QIUBBX_LABEL_LEN];
    char category[QXK24_QIUBBX_LABEL_LEN];
    double price;
    uint32_t stock_quantity;
    bool truthful_description;
    bool prohibited_item;
} QXK24QIUBBXListing;

typedef struct {
    char payment_id[QXK24_QIUBBX_LABEL_LEN];
    double amount;
    float fraud_score;
    bool authorized;
    bool balance_available;
} QXK24QIUBBXPayment;

typedef struct {
    char dispute_id[QXK24_QIUBBX_LABEL_LEN];
    uint32_t evidence_count;
    uint8_t severity;
    bool buyer_claim_present;
    bool seller_response_present;
} QXK24QIUBBXDispute;

typedef struct {
    uint32_t decisions_total;
    uint32_t allow_count;
    uint32_t warn_count;
    uint32_t review_count;
    uint32_t block_count;
    float average_score;
    float last_kernel_health;
    QXK24QIUBBXDecision last_decision;
    QXK24QIUBBXDecision last_recommended_decision;
    QXK24QIUBBXMode mode;
    uint32_t audit_events_total;
    uint32_t audit_events_stored;
} QXK24QIUBBXAdapterReport;

typedef struct {
    char event_id[QXK24_QIUBBX_LABEL_LEN];
    char request_id[QXK24_QIUBBX_LABEL_LEN];
    char subject_id[QXK24_QIUBBX_LABEL_LEN];
    char input_hash[QXK24_QIUBBX_LABEL_LEN];
    uint64_t timestamp_ms;
    QXK24QIUBBXFlow flow;
    QXK24QIUBBXDecision decision;
    QXK24QIUBBXDecision recommended_decision;
    QXK24QIUBBXMode mode;
    float constitutional_score;
    float kernel_health;
    uint8_t severity;
    char reason[QXK24_QIUBBX_REASON_LEN];
    char qxk24_version[16];
    char adapter_version[16];
} QXK24QIUBBXAuditEvent;

typedef struct {
    char request_id[QXK24_QIUBBX_LABEL_LEN];
    char transaction_id[QXK24_QIUBBX_LABEL_LEN];
    char buyer_id[QXK24_QIUBBX_LABEL_LEN];
    char seller_id[QXK24_QIUBBX_LABEL_LEN];
    char input_hash[QXK24_QIUBBX_LABEL_LEN];
    double amount;
    float buyer_trust;
    float seller_trust;
    bool inventory_available;
    bool payment_authorized;
} QXK24QIUBBXTransactionPilotRequest;

typedef struct {
    QXK24QIUBBXDecisionResult decision;
    QXK24QIUBBXAuditEvent audit_event;
    char json[QXK24_QIUBBX_JSON_LEN];
} QXK24QIUBBXTransactionPilotResponse;

typedef struct QXK24QIUBBXAdapterImpl *qxk24_qiubbx_adapter_t;

qxk24_err_t qxk24_qiubbx_adapter_create(
    qxk24_kernel_t kernel, qxk24_qiubbx_adapter_t *adapter_out);
qxk24_err_t qxk24_qiubbx_adapter_destroy(qxk24_qiubbx_adapter_t adapter);
qxk24_err_t qxk24_qiubbx_adapter_set_mode(
    qxk24_qiubbx_adapter_t adapter, QXK24QIUBBXMode mode);
qxk24_err_t qxk24_qiubbx_adapter_get_mode(
    qxk24_qiubbx_adapter_t adapter, QXK24QIUBBXMode *mode_out);
qxk24_err_t qxk24_qiubbx_govern_user_action(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXUserAction *input,
    QXK24QIUBBXDecisionResult *out);
qxk24_err_t qxk24_qiubbx_govern_transaction(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXTransaction *input,
    QXK24QIUBBXDecisionResult *out);
qxk24_err_t qxk24_qiubbx_govern_listing(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXListing *input,
    QXK24QIUBBXDecisionResult *out);
qxk24_err_t qxk24_qiubbx_govern_payment(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXPayment *input,
    QXK24QIUBBXDecisionResult *out);
qxk24_err_t qxk24_qiubbx_govern_dispute(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXDispute *input,
    QXK24QIUBBXDecisionResult *out);
qxk24_err_t qxk24_qiubbx_get_constitutional_report(
    qxk24_qiubbx_adapter_t adapter, QXK24QIUBBXAdapterReport *out);
qxk24_err_t qxk24_qiubbx_create_audit_event(
    const QXK24QIUBBXDecisionResult *result, const char *request_id,
    const char *subject_id, const char *input_hash,
    QXK24QIUBBXAuditEvent *event_out);
qxk24_err_t qxk24_qiubbx_decision_to_json(
    const QXK24QIUBBXDecisionResult *result, const char *request_id,
    char *json_out, uint32_t json_capacity);
qxk24_err_t qxk24_qiubbx_pilot_transaction(
    qxk24_qiubbx_adapter_t adapter,
    const QXK24QIUBBXTransactionPilotRequest *request,
    QXK24QIUBBXTransactionPilotResponse *response_out);
qxk24_err_t qxk24_qiubbx_adapter_record_audit(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXAuditEvent *event);
qxk24_err_t qxk24_qiubbx_adapter_audit_count(
    qxk24_qiubbx_adapter_t adapter, uint32_t *count_out);
qxk24_err_t qxk24_qiubbx_adapter_get_audit_event(
    qxk24_qiubbx_adapter_t adapter, uint32_t index,
    QXK24QIUBBXAuditEvent *event_out);
qxk24_err_t qxk24_qiubbx_adapter_clear_audit(
    qxk24_qiubbx_adapter_t adapter);
qxk24_err_t qxk24_qiubbx_audit_event_to_json(
    const QXK24QIUBBXAuditEvent *event, char *json_out,
    uint32_t json_capacity);
const char *qxk24_qiubbx_decision_name(QXK24QIUBBXDecision decision);
const char *qxk24_qiubbx_flow_name(QXK24QIUBBXFlow flow);
const char *qxk24_qiubbx_mode_name(QXK24QIUBBXMode mode);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_QIUBBX_ADAPTER_H */
