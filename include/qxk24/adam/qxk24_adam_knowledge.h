/* ============================================================
 * qxk24_adam_knowledge.h
 * QXK24 Universal Kernel — ADAM Knowledge Pipeline
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/adam/qxk24_adam_knowledge.h
 * License : Apache 2.0
 *
 * Description:
 * Declares Phase 12 ADAM knowledge records, Founder approval,
 * runtime monitoring decisions, learning requests, and governed
 * replies for the ALAMTOLOGI AI kernel.
 * ============================================================ */

#ifndef QXK24_ADAM_KNOWLEDGE_H
#define QXK24_ADAM_KNOWLEDGE_H

#include "../qxk24.h"
#include "qxk24_adam.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_ADAM_KNOWLEDGE_VERSION "1.6.0"
#define QXK24_ADAM_KNOWLEDGE_MAX_ENTRIES 64U
#define QXK24_ADAM_KNOWLEDGE_MAX_REFS 16U
#define QXK24_ADAM_KNOWLEDGE_ID_LEN 64U
#define QXK24_ADAM_KNOWLEDGE_TITLE_LEN 128U
#define QXK24_ADAM_KNOWLEDGE_TEXT_LEN 1024U
#define QXK24_ADAM_KNOWLEDGE_REPLY_LEN 8192U

typedef enum {
    QXK24_ADAM_DOMAIN_HUKUM_Z = 0,
    QXK24_ADAM_DOMAIN_HUKUM_X = 1,
    QXK24_ADAM_DOMAIN_HISAL = 2,
    QXK24_ADAM_DOMAIN_RINA = 3,
    QXK24_ADAM_DOMAIN_IZWA = 4,
    QXK24_ADAM_DOMAIN_SIRA = 5,
    QXK24_ADAM_DOMAIN_META = 6,
    QXK24_ADAM_DOMAIN_ADAM = 7,
    QXK24_ADAM_DOMAIN_QXK24 = 8,
    QXK24_ADAM_DOMAIN_GENERAL = 9
} QXK24AdamDomain;

typedef enum {
    QXK24_ADAM_SOURCE_FOUNDER_NOTE = 0,
    QXK24_ADAM_SOURCE_BOOK = 1,
    QXK24_ADAM_SOURCE_CORRECTION = 2,
    QXK24_ADAM_SOURCE_FORMULA = 3,
    QXK24_ADAM_SOURCE_EXAMPLE = 4,
    QXK24_ADAM_SOURCE_LEARNING_REQUEST = 5
} QXK24AdamSourceType;

typedef enum {
    QXK24_ADAM_KNOWLEDGE_DRAFT = 0,
    QXK24_ADAM_KNOWLEDGE_REVIEWED = 1,
    QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED = 2,
    QXK24_ADAM_KNOWLEDGE_RETIRED = 3
} QXK24AdamKnowledgeStatus;

typedef enum {
    QXK24_ADAM_MONITOR_CONFIRM = 0,
    QXK24_ADAM_MONITOR_ENRICH = 1,
    QXK24_ADAM_MONITOR_CORRECT = 2,
    QXK24_ADAM_MONITOR_WAQF = 3
} QXK24AdamMonitorAction;

typedef struct {
    char id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char title[QXK24_ADAM_KNOWLEDGE_TITLE_LEN];
    QXK24AdamSourceType source_type;
    char source_ref[256];
    QXK24AdamDomain domain;
    char principle[512];
    char formula[256];
    char definition[QXK24_ADAM_KNOWLEDGE_TEXT_LEN];
    char application[QXK24_ADAM_KNOWLEDGE_TEXT_LEN];
    QXK24AdamKnowledgeStatus status;
    uint16_t version_major;
    uint16_t version_minor;
    uint16_t version_patch;
    uint64_t created_at_unix;
    uint64_t approved_at_unix;
} QXK24AdamKnowledgeEntry;

typedef struct {
    uint32_t total_entries;
    uint32_t approved_entries;
    uint32_t draft_entries;
    uint32_t retired_entries;
    uint32_t learning_requests;
} QXK24AdamKnowledgeSnapshot;

typedef struct {
    char id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char response_id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char claim_text[QXK24_ADAM_KNOWLEDGE_TEXT_LEN];
    QXK24AdamDomain domain;
    float confidence;
    bool checked;
} QXK24AdamClaim;

typedef struct {
    char id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char claim_id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    QXK24AdamDomain domain;
    QXK24AdamMonitorAction action;
    float score_pola;
    float score_kadar;
    float score_pasangan;
    float score_keseimbangan;
    float score_total;
    char knowledge_ids[QXK24_ADAM_KNOWLEDGE_MAX_REFS]
        [QXK24_ADAM_KNOWLEDGE_ID_LEN];
    uint32_t knowledge_count;
    char reason[512];
    char replacement_text[QXK24_ADAM_KNOWLEDGE_TEXT_LEN];
} QXK24AdamMonitorDecision;

typedef struct {
    char id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char request_id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    QXK24AdamDomain missing_domain;
    char question[QXK24_ADAM_KNOWLEDGE_TEXT_LEN];
    char reason[QXK24_ADAM_KNOWLEDGE_TEXT_LEN];
    uint8_t urgency;
    bool resolved;
    char resulting_entry_id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
} QXK24AdamLearningRequest;

typedef struct {
    char id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char request_id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char external_response_id[QXK24_ADAM_KNOWLEDGE_ID_LEN];
    char final_text[QXK24_ADAM_KNOWLEDGE_REPLY_LEN];
    QXK24AdamJudgment judgment;
    float score_total;
    uint8_t tahap_akal;
    uint8_t contribution_value;
    uint32_t confirmed_count;
    uint32_t enriched_count;
    uint32_t corrected_count;
    uint32_t waqf_count;
} QXK24AdamGovernedReply;

typedef struct QXK24AdamKnowledgeIndexImpl *qxk24_adam_knowledge_index_t;

qxk24_err_t qxk24_adam_knowledge_index_create(
    qxk24_adam_knowledge_index_t *index_out);
qxk24_err_t qxk24_adam_knowledge_index_destroy(
    qxk24_adam_knowledge_index_t index);
qxk24_err_t qxk24_adam_knowledge_add(
    qxk24_adam_knowledge_index_t index,
    const QXK24AdamKnowledgeEntry *entry);
qxk24_err_t qxk24_adam_knowledge_approve(
    qxk24_adam_knowledge_index_t index, const char *entry_id,
    uint64_t approved_at_unix);
qxk24_err_t qxk24_adam_knowledge_find(
    qxk24_adam_knowledge_index_t index, const char *entry_id,
    QXK24AdamKnowledgeEntry *entry_out);
qxk24_err_t qxk24_adam_knowledge_query_domain(
    qxk24_adam_knowledge_index_t index, QXK24AdamDomain domain,
    QXK24AdamKnowledgeEntry *entries_out, uint32_t capacity,
    uint32_t *count_out);
qxk24_err_t qxk24_adam_knowledge_snapshot(
    qxk24_adam_knowledge_index_t index,
    QXK24AdamKnowledgeSnapshot *snapshot_out);
qxk24_err_t qxk24_adam_monitor_decide(
    qxk24_adam_knowledge_index_t index, const QXK24AdamClaim *claim,
    QXK24AdamMonitorDecision *decision_out);
qxk24_err_t qxk24_adam_learning_request_create(
    const QXK24AdamMonitorDecision *decision,
    QXK24AdamLearningRequest *request_out);
qxk24_err_t qxk24_adam_govern_reply(
    const char *external_text, const QXK24AdamMonitorDecision *decisions,
    uint32_t decision_count, QXK24AdamGovernedReply *reply_out);
const char *qxk24_adam_domain_name(QXK24AdamDomain domain);
const char *qxk24_adam_monitor_action_name(QXK24AdamMonitorAction action);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_KNOWLEDGE_H */
