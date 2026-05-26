/* ============================================================
 * qxk24_adam_knowledge.c
 * QXK24 Universal Kernel — ADAM Knowledge Pipeline
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/adam/qxk24_adam_knowledge.c
 * License : Apache 2.0
 *
 * Description:
 * Implements Phase 12 ADAM in-memory knowledge index, Founder
 * approval, runtime monitoring, learning requests, and governed
 * replies for the ALAMTOLOGI AI kernel.
 * ============================================================ */

#include "qxk24/adam/qxk24_adam_knowledge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct QXK24AdamKnowledgeIndexImpl {
    QXK24AdamKnowledgeEntry entries[QXK24_ADAM_KNOWLEDGE_MAX_ENTRIES];
    uint32_t count;
    uint32_t learning_requests;
};

static const char *s_domain_names[] = {
    "HUKUM_Z", "HUKUM_X", "HISAL", "RINA", "IZWA",
    "SIRA", "META", "ADAM", "QXK24", "GENERAL"
};

static const char *s_action_names[] = {
    "CONFIRM", "ENRICH", "CORRECT", "WAQF"
};

static void copy_text(char *dst, size_t dst_size, const char *src) {
    if (dst_size == 0U) return;
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }
    (void)snprintf(dst, dst_size, "%s", src);
}

static bool text_has(const char *haystack, const char *needle) {
    return haystack != NULL && needle != NULL && needle[0] != '\0' &&
        strstr(haystack, needle) != NULL;
}

static bool text_signals_conflict(const char *text) {
    return text_has(text, "not ") || text_has(text, "false") ||
        text_has(text, "contradict");
}

static int find_index(qxk24_adam_knowledge_index_t index,
                      const char *entry_id) {
    if (index == NULL || entry_id == NULL) return -1;
    for (uint32_t i = 0U; i < index->count; i++) {
        if (strncmp(index->entries[i].id, entry_id,
                    QXK24_ADAM_KNOWLEDGE_ID_LEN) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static const QXK24AdamKnowledgeEntry *find_approved_domain(
    qxk24_adam_knowledge_index_t index, QXK24AdamDomain domain) {
    if (index == NULL) return NULL;
    for (uint32_t i = 0U; i < index->count; i++) {
        const QXK24AdamKnowledgeEntry *e = &index->entries[i];
        if (e->domain == domain &&
            e->status == QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED) {
            return e;
        }
    }
    return NULL;
}

static float average4(float a, float b, float c, float d) {
    return (a + b + c + d) / 4.0f;
}

qxk24_err_t qxk24_adam_knowledge_index_create(
    qxk24_adam_knowledge_index_t *index_out) {
    if (index_out == NULL) return QXK24_ERR_NULL_PARAM;
    qxk24_adam_knowledge_index_t index = calloc(1, sizeof(*index));
    if (index == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    *index_out = index;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_knowledge_index_destroy(
    qxk24_adam_knowledge_index_t index) {
    if (index == NULL) return QXK24_ERR_NULL_PARAM;
    free(index);
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_knowledge_add(
    qxk24_adam_knowledge_index_t index,
    const QXK24AdamKnowledgeEntry *entry) {
    if (index == NULL || entry == NULL) return QXK24_ERR_NULL_PARAM;
    if (entry->id[0] == '\0') return QXK24_ERR_INVALID_ARGUMENT;
    if (index->count >= QXK24_ADAM_KNOWLEDGE_MAX_ENTRIES) {
        return QXK24_ERR_CAPACITY_FULL;
    }
    if (find_index(index, entry->id) >= 0) return QXK24_ERR_ALREADY_INITIALIZED;
    index->entries[index->count++] = *entry;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_knowledge_approve(
    qxk24_adam_knowledge_index_t index, const char *entry_id,
    uint64_t approved_at_unix) {
    const int i = find_index(index, entry_id);
    if (i < 0) return QXK24_ERR_NOT_FOUND;
    index->entries[i].status = QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED;
    index->entries[i].approved_at_unix = approved_at_unix;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_knowledge_find(
    qxk24_adam_knowledge_index_t index, const char *entry_id,
    QXK24AdamKnowledgeEntry *entry_out) {
    if (entry_out == NULL) return QXK24_ERR_NULL_PARAM;
    const int i = find_index(index, entry_id);
    if (i < 0) return QXK24_ERR_NOT_FOUND;
    *entry_out = index->entries[i];
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_knowledge_query_domain(
    qxk24_adam_knowledge_index_t index, QXK24AdamDomain domain,
    QXK24AdamKnowledgeEntry *entries_out, uint32_t capacity,
    uint32_t *count_out) {
    if (index == NULL || count_out == NULL) return QXK24_ERR_NULL_PARAM;
    uint32_t count = 0U;
    for (uint32_t i = 0U; i < index->count; i++) {
        const QXK24AdamKnowledgeEntry *e = &index->entries[i];
        if (e->domain == domain &&
            e->status == QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED) {
            if (entries_out != NULL && count < capacity) entries_out[count] = *e;
            count++;
        }
    }
    *count_out = count;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_knowledge_snapshot(
    qxk24_adam_knowledge_index_t index,
    QXK24AdamKnowledgeSnapshot *snapshot_out) {
    if (index == NULL || snapshot_out == NULL) return QXK24_ERR_NULL_PARAM;
    memset(snapshot_out, 0, sizeof(*snapshot_out));
    snapshot_out->total_entries = index->count;
    snapshot_out->learning_requests = index->learning_requests;
    for (uint32_t i = 0U; i < index->count; i++) {
        switch (index->entries[i].status) {
            case QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED:
                snapshot_out->approved_entries++;
                break;
            case QXK24_ADAM_KNOWLEDGE_RETIRED:
                snapshot_out->retired_entries++;
                break;
            default:
                snapshot_out->draft_entries++;
                break;
        }
    }
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_monitor_decide(
    qxk24_adam_knowledge_index_t index, const QXK24AdamClaim *claim,
    QXK24AdamMonitorDecision *decision_out) {
    if (index == NULL || claim == NULL || decision_out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    memset(decision_out, 0, sizeof(*decision_out));
    copy_text(decision_out->id, sizeof(decision_out->id), "decision");
    copy_text(decision_out->claim_id, sizeof(decision_out->claim_id), claim->id);
    decision_out->domain = claim->domain;
    const QXK24AdamKnowledgeEntry *entry =
        find_approved_domain(index, claim->domain);
    if (entry == NULL) {
        decision_out->action = QXK24_ADAM_MONITOR_WAQF;
        decision_out->score_total = 0.25f;
        copy_text(decision_out->reason, sizeof(decision_out->reason),
                  "Founder-approved ALAMTOLOGI knowledge required");
        index->learning_requests++;
        return QXK24_OK;
    }

    copy_text(decision_out->knowledge_ids[0],
              sizeof(decision_out->knowledge_ids[0]), entry->id);
    decision_out->knowledge_count = 1U;
    if (text_signals_conflict(claim->claim_text)) {
        decision_out->action = QXK24_ADAM_MONITOR_CORRECT;
        decision_out->score_pola = 0.80f;
        decision_out->score_kadar = 0.80f;
        decision_out->score_pasangan = 0.70f;
        decision_out->score_keseimbangan = 0.70f;
        copy_text(decision_out->replacement_text,
                  sizeof(decision_out->replacement_text), entry->definition);
        copy_text(decision_out->reason, sizeof(decision_out->reason),
                  "External claim contradicted approved ALAMTOLOGI knowledge");
    } else if (text_has(claim->claim_text, entry->definition) ||
               text_has(claim->claim_text, entry->principle)) {
        decision_out->action = QXK24_ADAM_MONITOR_CONFIRM;
        decision_out->score_pola = 1.0f;
        decision_out->score_kadar = 1.0f;
        decision_out->score_pasangan = 1.0f;
        decision_out->score_keseimbangan = 1.0f;
        copy_text(decision_out->reason, sizeof(decision_out->reason),
                  "External claim aligned with approved ALAMTOLOGI knowledge");
    } else {
        decision_out->action = QXK24_ADAM_MONITOR_ENRICH;
        decision_out->score_pola = 0.90f;
        decision_out->score_kadar = 0.80f;
        decision_out->score_pasangan = 0.80f;
        decision_out->score_keseimbangan = 0.80f;
        copy_text(decision_out->replacement_text,
                  sizeof(decision_out->replacement_text), entry->definition);
        copy_text(decision_out->reason, sizeof(decision_out->reason),
                  "External claim needed ALAMTOLOGI enrichment");
    }
    decision_out->score_total = average4(decision_out->score_pola,
        decision_out->score_kadar, decision_out->score_pasangan,
        decision_out->score_keseimbangan);
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_learning_request_create(
    const QXK24AdamMonitorDecision *decision,
    QXK24AdamLearningRequest *request_out) {
    if (decision == NULL || request_out == NULL) return QXK24_ERR_NULL_PARAM;
    if (decision->action != QXK24_ADAM_MONITOR_WAQF) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    memset(request_out, 0, sizeof(*request_out));
    copy_text(request_out->id, sizeof(request_out->id), "learning-request");
    copy_text(request_out->request_id, sizeof(request_out->request_id),
              decision->claim_id);
    request_out->missing_domain = decision->domain;
    request_out->urgency = 1U;
    snprintf(request_out->question, sizeof(request_out->question),
             "Founder knowledge required for %s",
             qxk24_adam_domain_name(decision->domain));
    copy_text(request_out->reason, sizeof(request_out->reason),
              decision->reason);
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_govern_reply(
    const char *external_text, const QXK24AdamMonitorDecision *decisions,
    uint32_t decision_count, QXK24AdamGovernedReply *reply_out) {
    if (external_text == NULL || decisions == NULL || reply_out == NULL ||
        decision_count == 0U) {
        return QXK24_ERR_NULL_PARAM;
    }
    memset(reply_out, 0, sizeof(*reply_out));
    copy_text(reply_out->id, sizeof(reply_out->id), "governed-reply");
    float total = 0.0f;
    for (uint32_t i = 0U; i < decision_count; i++) {
        total += decisions[i].score_total;
        switch (decisions[i].action) {
            case QXK24_ADAM_MONITOR_CONFIRM:
                reply_out->confirmed_count++;
                break;
            case QXK24_ADAM_MONITOR_ENRICH:
                reply_out->enriched_count++;
                break;
            case QXK24_ADAM_MONITOR_CORRECT:
                reply_out->corrected_count++;
                break;
            case QXK24_ADAM_MONITOR_WAQF:
                reply_out->waqf_count++;
                break;
        }
    }
    reply_out->score_total = total / (float)decision_count;
    if (reply_out->score_total >= 0.80f) {
        reply_out->judgment = QXK24_ADAM_JUDGMENT_MAKMUR;
    } else if (reply_out->score_total >= 0.50f) {
        reply_out->judgment = QXK24_ADAM_JUDGMENT_ISLAH;
    } else {
        reply_out->judgment = QXK24_ADAM_JUDGMENT_WAQF;
    }
    snprintf(reply_out->final_text, sizeof(reply_out->final_text),
             "[ADAM v%s | %s | score %.2f | C:%u E:%u R:%u W:%u] %s",
             QXK24_ADAM_KNOWLEDGE_VERSION,
             qxk24_adam_judgment_name(reply_out->judgment),
             reply_out->score_total, reply_out->confirmed_count,
             reply_out->enriched_count, reply_out->corrected_count,
             reply_out->waqf_count, external_text);
    return QXK24_OK;
}

const char *qxk24_adam_domain_name(QXK24AdamDomain domain) {
    return domain >= QXK24_ADAM_DOMAIN_HUKUM_Z &&
        domain <= QXK24_ADAM_DOMAIN_GENERAL ? s_domain_names[(int)domain] :
        "UNKNOWN";
}

const char *qxk24_adam_monitor_action_name(QXK24AdamMonitorAction action) {
    return action >= QXK24_ADAM_MONITOR_CONFIRM &&
        action <= QXK24_ADAM_MONITOR_WAQF ? s_action_names[(int)action] :
        "UNKNOWN";
}
