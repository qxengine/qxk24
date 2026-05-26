// =============================================================================
// qxk24_qiubbx_audit.c
// QXK24 Universal Kernel — QIUBBX Audit JSON Contract
// =============================================================================
// Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
// Created : 2026-05-26
// Repository : https://github.com/qxengine/qxk24
// Path : src/qiubbx/qxk24_qiubbx_audit.c
// License : Apache 2.0
//
// Description:
// Serializes QIUBBX adapter audit events for backend persistence.
//

#include "qxk24/qiubbx/qxk24_qiubbx_adapter.h"
#include <stdio.h>

qxk24_err_t qxk24_qiubbx_audit_event_to_json(
    const QXK24QIUBBXAuditEvent *event, char *json_out,
    uint32_t json_capacity) {
    if (event == NULL || json_out == NULL || json_capacity == 0U) {
        return QXK24_ERR_NULL_PARAM;
    }
    const int written = snprintf(json_out, json_capacity,
        "{\"eventId\":\"%s\",\"requestId\":\"%s\",\"subjectId\":\"%s\","
        "\"inputHash\":\"%s\",\"timestampMs\":%llu,\"flow\":\"%s\","
        "\"decision\":\"%s\",\"recommendedDecision\":\"%s\","
        "\"mode\":\"%s\",\"constitutionalScore\":%.3f,"
        "\"kernelHealth\":%.3f,\"severity\":%u,\"reason\":\"%s\","
        "\"qxk24Version\":\"%s\",\"adapterVersion\":\"%s\"}",
        event->event_id,
        event->request_id,
        event->subject_id,
        event->input_hash,
        (unsigned long long)event->timestamp_ms,
        qxk24_qiubbx_flow_name(event->flow),
        qxk24_qiubbx_decision_name(event->decision),
        qxk24_qiubbx_decision_name(event->recommended_decision),
        qxk24_qiubbx_mode_name(event->mode),
        event->constitutional_score,
        event->kernel_health,
        event->severity,
        event->reason,
        event->qxk24_version,
        event->adapter_version);
    if (written < 0 || (uint32_t)written >= json_capacity) {
        return QXK24_ERR_CAPACITY_FULL;
    }
    return QXK24_OK;
}
