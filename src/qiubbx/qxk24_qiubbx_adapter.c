// =============================================================================
// qxk24_qiubbx_adapter.c
// QXK24 Universal Kernel — QIUBBX Constitutional Adapter
// =============================================================================
// Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
// Created : 2026-05-26
// Repository : https://github.com/qxengine/qxk24
// Path : src/qiubbx/qxk24_qiubbx_adapter.c
// License : Apache 2.0
//
// Description:
// Implements practical QIUBBX governance decisions for application flows using
// QXK24 kernel health and constitutional scoring.
//

#include "qxk24/qiubbx/qxk24_qiubbx_adapter.h"
#include "qxk24/kernel/qxk24_report.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct QXK24QIUBBXAdapterImpl {
    qxk24_kernel_t kernel;
    QXK24QIUBBXAdapterReport report;
    QXK24QIUBBXMode mode;
    pthread_mutex_t lock;
    QXK24QIUBBXAuditEvent audit_events[QXK24_QIUBBX_AUDIT_CAPACITY];
    uint32_t audit_count;
    uint32_t audit_next;
    uint32_t audit_total;
};

static const char *s_decision_names[] = {"ALLOW", "WARN", "REVIEW", "BLOCK"};
static const char *s_flow_names[] = {
    "USER_ACTION", "TRANSACTION", "LISTING", "PAYMENT", "DISPUTE"
};
static const char *s_mode_names[] = {"SHADOW", "REVIEW_ONLY", "ENFORCE"};

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
        (uint64_t)(ts.tv_nsec / 1000000ULL);
}

static void copy_text(char *dst, size_t dst_size, const char *src) {
    if (dst_size == 0U) return;
    snprintf(dst, dst_size, "%s", src != NULL ? src : "");
}

static float kernel_health(qxk24_kernel_t kernel) {
    QXK24Report report;
    if (qxk24_report_generate(kernel, &report) == QXK24_OK) {
        return (float)report.health_score;
    }
    const double score = qxk24_health_score(kernel);
    return score < 0.0 ? 0.0f : (float)score;
}

static QXK24QIUBBXDecision decide(float score, bool hard_block,
                                  bool needs_review) {
    if (hard_block) return QXK24_QIUBBX_DECISION_BLOCK;
    if (needs_review) return QXK24_QIUBBX_DECISION_REVIEW;
    if (score >= 0.85f) return QXK24_QIUBBX_DECISION_ALLOW;
    if (score >= 0.65f) return QXK24_QIUBBX_DECISION_WARN;
    if (score >= 0.50f) return QXK24_QIUBBX_DECISION_REVIEW;
    return QXK24_QIUBBX_DECISION_BLOCK;
}

static QXK24QIUBBXDecision apply_mode(QXK24QIUBBXMode mode,
                                      QXK24QIUBBXDecision decision) {
    if (mode == QXK24_QIUBBX_MODE_SHADOW) {
        return QXK24_QIUBBX_DECISION_ALLOW;
    }
    if (mode == QXK24_QIUBBX_MODE_REVIEW_ONLY &&
        decision == QXK24_QIUBBX_DECISION_BLOCK) {
        return QXK24_QIUBBX_DECISION_REVIEW;
    }
    return decision;
}

static void update_report(qxk24_qiubbx_adapter_t adapter,
                          const QXK24QIUBBXDecisionResult *result) {
    QXK24QIUBBXAdapterReport *r = &adapter->report;
    const uint32_t before = r->decisions_total;
    r->decisions_total++;
    r->average_score =
        ((r->average_score * (float)before) + result->constitutional_score) /
        (float)(before + 1U);
    r->last_kernel_health = result->kernel_health;
    r->last_decision = result->decision;
    r->last_recommended_decision = result->recommended_decision;
    r->mode = result->mode;
    if (result->decision == QXK24_QIUBBX_DECISION_ALLOW) r->allow_count++;
    else if (result->decision == QXK24_QIUBBX_DECISION_WARN) r->warn_count++;
    else if (result->decision == QXK24_QIUBBX_DECISION_REVIEW) r->review_count++;
    else r->block_count++;
}

static qxk24_err_t finish(qxk24_qiubbx_adapter_t adapter,
                          QXK24QIUBBXDecisionResult *out,
                          QXK24QIUBBXFlow flow, float score,
                          bool hard_block, bool needs_review,
                          const char *reason) {
    memset(out, 0, sizeof(*out));
    out->flow = flow;
    out->kernel_health = kernel_health(adapter->kernel);
    out->constitutional_score = clamp01(score);
    pthread_mutex_lock(&adapter->lock);
    out->recommended_decision = decide(out->constitutional_score, hard_block,
                                       needs_review);
    out->mode = adapter->mode;
    out->decision = apply_mode(adapter->mode, out->recommended_decision);
    out->severity = out->decision == QXK24_QIUBBX_DECISION_BLOCK ? 3U :
        (out->decision == QXK24_QIUBBX_DECISION_REVIEW ? 2U :
        (out->decision == QXK24_QIUBBX_DECISION_WARN ? 1U : 0U));
    snprintf(out->reason, sizeof(out->reason), "%s", reason);
    update_report(adapter, out);
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_adapter_create(
    qxk24_kernel_t kernel, qxk24_qiubbx_adapter_t *adapter_out) {
    if (kernel == NULL || adapter_out == NULL) return QXK24_ERR_NULL_PARAM;
    qxk24_qiubbx_adapter_t adapter = calloc(1, sizeof(*adapter));
    if (adapter == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    if (pthread_mutex_init(&adapter->lock, NULL) != 0) {
        free(adapter);
        return QXK24_ERR_FAULT;
    }
    adapter->kernel = kernel;
    adapter->mode = QXK24_QIUBBX_MODE_ENFORCE;
    adapter->report.last_decision = QXK24_QIUBBX_DECISION_ALLOW;
    adapter->report.last_recommended_decision = QXK24_QIUBBX_DECISION_ALLOW;
    adapter->report.mode = adapter->mode;
    *adapter_out = adapter;
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_adapter_destroy(qxk24_qiubbx_adapter_t adapter) {
    if (adapter == NULL) return QXK24_ERR_NULL_PARAM;
    pthread_mutex_destroy(&adapter->lock);
    free(adapter);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_adapter_set_mode(
    qxk24_qiubbx_adapter_t adapter, QXK24QIUBBXMode mode) {
    if (adapter == NULL) return QXK24_ERR_NULL_PARAM;
    if (mode < QXK24_QIUBBX_MODE_SHADOW ||
        mode > QXK24_QIUBBX_MODE_ENFORCE) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    pthread_mutex_lock(&adapter->lock);
    adapter->mode = mode;
    adapter->report.mode = mode;
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_adapter_get_mode(
    qxk24_qiubbx_adapter_t adapter, QXK24QIUBBXMode *mode_out) {
    if (adapter == NULL || mode_out == NULL) return QXK24_ERR_NULL_PARAM;
    pthread_mutex_lock(&adapter->lock);
    *mode_out = adapter->mode;
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_govern_user_action(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXUserAction *input,
    QXK24QIUBBXDecisionResult *out) {
    if (adapter == NULL || input == NULL || out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    const bool hard_block = !input->has_consent || input->risk_score >= 0.95f;
    const bool review = input->requires_review || input->risk_score >= 0.75f;
    const float score = 1.0f - clamp01(input->risk_score);
    return finish(adapter, out, QXK24_QIUBBX_FLOW_USER_ACTION, score,
                  hard_block, review,
                  hard_block ? "User action violates consent or risk limit" :
                  "User action constitutionally governed");
}

qxk24_err_t qxk24_qiubbx_govern_transaction(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXTransaction *input,
    QXK24QIUBBXDecisionResult *out) {
    if (adapter == NULL || input == NULL || out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    const bool hard_block = input->amount <= 0.0 || !input->payment_authorized ||
        !input->inventory_available;
    const float trust = (clamp01(input->buyer_trust) +
        clamp01(input->seller_trust)) / 2.0f;
    return finish(adapter, out, QXK24_QIUBBX_FLOW_TRANSACTION, trust,
                  hard_block, trust < 0.65f,
                  hard_block ? "Transaction violates amount, payment, or stock" :
                  "Transaction passed QIUBBX constitutional checks");
}

qxk24_err_t qxk24_qiubbx_govern_listing(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXListing *input,
    QXK24QIUBBXDecisionResult *out) {
    if (adapter == NULL || input == NULL || out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    const bool hard_block = input->prohibited_item || input->price <= 0.0 ||
        input->stock_quantity == 0U;
    const bool review = !input->truthful_description;
    const float score = input->truthful_description ? 0.95f : 0.60f;
    return finish(adapter, out, QXK24_QIUBBX_FLOW_LISTING, score,
                  hard_block, review,
                  hard_block ? "Listing violates product, price, or stock law" :
                  "Listing governed by truthful commerce requirement");
}

qxk24_err_t qxk24_qiubbx_govern_payment(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXPayment *input,
    QXK24QIUBBXDecisionResult *out) {
    if (adapter == NULL || input == NULL || out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    const bool hard_block = input->amount <= 0.0 || !input->authorized ||
        !input->balance_available || input->fraud_score >= 0.90f;
    const bool review = input->fraud_score >= 0.55f;
    const float score = 1.0f - clamp01(input->fraud_score);
    return finish(adapter, out, QXK24_QIUBBX_FLOW_PAYMENT, score,
                  hard_block, review,
                  hard_block ? "Payment violates authority, balance, or risk" :
                  "Payment governed by authority and economy law");
}

qxk24_err_t qxk24_qiubbx_govern_dispute(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXDispute *input,
    QXK24QIUBBXDecisionResult *out) {
    if (adapter == NULL || input == NULL || out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    const bool hard_block = !input->buyer_claim_present;
    const bool review = !input->seller_response_present || input->severity >= 2U;
    float score = input->evidence_count > 0U ? 0.80f : 0.55f;
    if (input->seller_response_present) score += 0.10f;
    if (input->severity >= 3U) score -= 0.20f;
    return finish(adapter, out, QXK24_QIUBBX_FLOW_DISPUTE, score,
                  hard_block, review,
                  hard_block ? "Dispute lacks a valid claim" :
                  "Dispute routed by justice and evidence requirement");
}

qxk24_err_t qxk24_qiubbx_get_constitutional_report(
    qxk24_qiubbx_adapter_t adapter, QXK24QIUBBXAdapterReport *out) {
    if (adapter == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    pthread_mutex_lock(&adapter->lock);
    adapter->report.audit_events_total = adapter->audit_total;
    adapter->report.audit_events_stored = adapter->audit_count;
    *out = adapter->report;
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_create_audit_event(
    const QXK24QIUBBXDecisionResult *result, const char *request_id,
    const char *subject_id, const char *input_hash,
    QXK24QIUBBXAuditEvent *event_out) {
    if (result == NULL || event_out == NULL) return QXK24_ERR_NULL_PARAM;
    memset(event_out, 0, sizeof(*event_out));
    event_out->timestamp_ms = now_ms();
    snprintf(event_out->event_id, sizeof(event_out->event_id),
             "qxk24-%llu", (unsigned long long)event_out->timestamp_ms);
    copy_text(event_out->request_id, sizeof(event_out->request_id),
              request_id);
    copy_text(event_out->subject_id, sizeof(event_out->subject_id),
              subject_id);
    copy_text(event_out->input_hash, sizeof(event_out->input_hash),
              input_hash);
    event_out->flow = result->flow;
    event_out->decision = result->decision;
    event_out->recommended_decision = result->recommended_decision;
    event_out->mode = result->mode;
    event_out->constitutional_score = result->constitutional_score;
    event_out->kernel_health = result->kernel_health;
    event_out->severity = result->severity;
    copy_text(event_out->reason, sizeof(event_out->reason), result->reason);
    copy_text(event_out->qxk24_version, sizeof(event_out->qxk24_version),
              QXK24_VERSION_STRING);
    copy_text(event_out->adapter_version, sizeof(event_out->adapter_version),
              QXK24_QIUBBX_ADAPTER_VERSION);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_decision_to_json(
    const QXK24QIUBBXDecisionResult *result, const char *request_id,
    char *json_out, uint32_t json_capacity) {
    if (result == NULL || json_out == NULL || json_capacity == 0U) {
        return QXK24_ERR_NULL_PARAM;
    }
    const int written = snprintf(json_out, json_capacity,
        "{\"decision\":\"%s\",\"recommendedDecision\":\"%s\","
        "\"flow\":\"%s\",\"mode\":\"%s\",\"constitutionalScore\":%.3f,"
        "\"kernelHealth\":%.3f,\"severity\":%u,\"reason\":\"%s\","
        "\"requestId\":\"%s\",\"qxk24Version\":\"%s\","
        "\"adapterVersion\":\"%s\"}",
        qxk24_qiubbx_decision_name(result->decision),
        qxk24_qiubbx_decision_name(result->recommended_decision),
        qxk24_qiubbx_flow_name(result->flow),
        qxk24_qiubbx_mode_name(result->mode),
        result->constitutional_score,
        result->kernel_health,
        result->severity,
        result->reason,
        request_id != NULL ? request_id : "",
        QXK24_VERSION_STRING,
        QXK24_QIUBBX_ADAPTER_VERSION);
    if (written < 0 || (uint32_t)written >= json_capacity) {
        return QXK24_ERR_CAPACITY_FULL;
    }
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_pilot_transaction(
    qxk24_qiubbx_adapter_t adapter,
    const QXK24QIUBBXTransactionPilotRequest *request,
    QXK24QIUBBXTransactionPilotResponse *response_out) {
    if (adapter == NULL || request == NULL || response_out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    memset(response_out, 0, sizeof(*response_out));
    QXK24QIUBBXTransaction tx;
    memset(&tx, 0, sizeof(tx));
    copy_text(tx.transaction_id, sizeof(tx.transaction_id),
              request->transaction_id);
    tx.amount = request->amount;
    tx.buyer_trust = request->buyer_trust;
    tx.seller_trust = request->seller_trust;
    tx.inventory_available = request->inventory_available;
    tx.payment_authorized = request->payment_authorized;

    qxk24_err_t err = qxk24_qiubbx_govern_transaction(
        adapter, &tx, &response_out->decision);
    if (err != QXK24_OK) return err;
    err = qxk24_qiubbx_create_audit_event(
        &response_out->decision, request->request_id, request->transaction_id,
        request->input_hash, &response_out->audit_event);
    if (err != QXK24_OK) return err;
    err = qxk24_qiubbx_adapter_record_audit(adapter,
                                            &response_out->audit_event);
    if (err != QXK24_OK) return err;
    return qxk24_qiubbx_decision_to_json(
        &response_out->decision, request->request_id, response_out->json,
        sizeof(response_out->json));
}

qxk24_err_t qxk24_qiubbx_adapter_record_audit(
    qxk24_qiubbx_adapter_t adapter, const QXK24QIUBBXAuditEvent *event) {
    if (adapter == NULL || event == NULL) return QXK24_ERR_NULL_PARAM;
    pthread_mutex_lock(&adapter->lock);
    adapter->audit_events[adapter->audit_next] = *event;
    adapter->audit_next = (adapter->audit_next + 1U) %
        QXK24_QIUBBX_AUDIT_CAPACITY;
    if (adapter->audit_count < QXK24_QIUBBX_AUDIT_CAPACITY) {
        adapter->audit_count++;
    }
    adapter->audit_total++;
    adapter->report.audit_events_total = adapter->audit_total;
    adapter->report.audit_events_stored = adapter->audit_count;
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_adapter_audit_count(
    qxk24_qiubbx_adapter_t adapter, uint32_t *count_out) {
    if (adapter == NULL || count_out == NULL) return QXK24_ERR_NULL_PARAM;
    pthread_mutex_lock(&adapter->lock);
    *count_out = adapter->audit_count;
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_adapter_get_audit_event(
    qxk24_qiubbx_adapter_t adapter, uint32_t index,
    QXK24QIUBBXAuditEvent *event_out) {
    if (adapter == NULL || event_out == NULL) return QXK24_ERR_NULL_PARAM;
    pthread_mutex_lock(&adapter->lock);
    if (index >= adapter->audit_count) {
        pthread_mutex_unlock(&adapter->lock);
        return QXK24_ERR_NOT_FOUND;
    }
    const uint32_t start = adapter->audit_count < QXK24_QIUBBX_AUDIT_CAPACITY ?
        0U : adapter->audit_next;
    const uint32_t slot = (start + index) % QXK24_QIUBBX_AUDIT_CAPACITY;
    *event_out = adapter->audit_events[slot];
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

qxk24_err_t qxk24_qiubbx_adapter_clear_audit(
    qxk24_qiubbx_adapter_t adapter) {
    if (adapter == NULL) return QXK24_ERR_NULL_PARAM;
    pthread_mutex_lock(&adapter->lock);
    memset(adapter->audit_events, 0, sizeof(adapter->audit_events));
    adapter->audit_count = 0U;
    adapter->audit_next = 0U;
    adapter->audit_total = 0U;
    adapter->report.audit_events_total = 0U;
    adapter->report.audit_events_stored = 0U;
    pthread_mutex_unlock(&adapter->lock);
    return QXK24_OK;
}

const char *qxk24_qiubbx_decision_name(QXK24QIUBBXDecision decision) {
    return decision >= QXK24_QIUBBX_DECISION_ALLOW &&
        decision <= QXK24_QIUBBX_DECISION_BLOCK ?
        s_decision_names[(int)decision] : "UNKNOWN";
}

const char *qxk24_qiubbx_flow_name(QXK24QIUBBXFlow flow) {
    return flow >= QXK24_QIUBBX_FLOW_USER_ACTION &&
        flow <= QXK24_QIUBBX_FLOW_DISPUTE ? s_flow_names[(int)flow] :
        "UNKNOWN";
}

const char *qxk24_qiubbx_mode_name(QXK24QIUBBXMode mode) {
    return mode >= QXK24_QIUBBX_MODE_SHADOW &&
        mode <= QXK24_QIUBBX_MODE_ENFORCE ? s_mode_names[(int)mode] :
        "UNKNOWN";
}
