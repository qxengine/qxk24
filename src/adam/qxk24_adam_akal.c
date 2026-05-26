/**
 * @file qxk24_adam_akal.c
 * @brief ADAM Akal Engine implementation
 * ALAMTOLOGI – Quranic Science | Founder: Masa Bayu
 */

#include "qxk24/adam/qxk24_adam_akal.h"
#include <stdlib.h>
#include <string.h>

const uint8_t QXK24_AKAL_PATTERN_COUNT[QXK24_AKAL_LEVELS] = {
    1U, 2U, 2U, 4U, 6U, 12U, 22U
};

const char QXK24_AKAL_CODES[QXK24_AKAL_LEVELS] = {
    's', 'd', 'g', 'p', 'm', 'n', 't'
};

struct QXK24AdamAkalImpl {
    QXK24AkalSnapshot snap;
};

static uint8_t clamp_tahap(uint8_t tahap) {
    if (tahap < 1U) return 1U;
    if (tahap > QXK24_AKAL_LEVELS) return QXK24_AKAL_LEVELS;
    return tahap;
}

static float avg_next(float avg, uint32_t count, float value) {
    if (count == 0U) return value;
    return ((avg * (float)count) + value) / (float)(count + 1U);
}

qxk24_err_t qxk24_adam_akal_create(qxk24_adam_akal_t *akal_out) {
    if (akal_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AdamAkalImpl *akal = calloc(1, sizeof(*akal));
    if (akal == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    akal->snap.current_tahap = 1U;
    *akal_out = akal;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_akal_reason(qxk24_adam_akal_t akal,
                                   uint8_t tahap,
                                   QXK24AkalResult *result_out) {
    if (akal == NULL || result_out == NULL) return QXK24_ERR_NULL_PARAM;
    const uint8_t level = clamp_tahap(tahap);
    const uint8_t patterns = QXK24_AKAL_PATTERN_COUNT[level - 1U];
    const uint32_t before = akal->snap.akal_governed_count;

    result_out->tahap = level;
    result_out->current_process = QXK24_AKAL_PROC_USAHA;
    result_out->patterns_used = patterns;
    result_out->governance = QXK24_AKAL_GOVERNED;
    result_out->akal_complete = true;
    result_out->reasoning_score = (float)level / (float)QXK24_AKAL_LEVELS;

    akal->snap.current_tahap = level;
    akal->snap.total_patterns += patterns;
    akal->snap.akal_governed_count++;
    akal->snap.avg_reasoning_score =
        avg_next(akal->snap.avg_reasoning_score, before,
                 result_out->reasoning_score);
    return QXK24_OK;
}

uint8_t qxk24_adam_akal_assess_tahap(qxk24_adam_akal_t akal,
                                     const char *input) {
    (void)akal;
    if (input == NULL) return 1U;
    const size_t len = strlen(input);
    const uint8_t level = (uint8_t)((len / 16U) + 1U);
    return clamp_tahap(level);
}

qxk24_err_t qxk24_adam_akal_descriptor(uint8_t level,
                                       QXK24AkalDescriptor *desc_out) {
    if (desc_out == NULL) return QXK24_ERR_NULL_PARAM;
    const uint8_t tahap = clamp_tahap(level);
    desc_out->level = tahap;
    desc_out->code = QXK24_AKAL_CODES[tahap - 1U];
    desc_out->value = tahap;
    desc_out->pattern_count = QXK24_AKAL_PATTERN_COUNT[tahap - 1U];
    desc_out->capacity_weight = (float)tahap / (float)QXK24_AKAL_LEVELS;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_akal_check_governance(const QXK24AkalResult *result) {
    if (result == NULL) return QXK24_ERR_NULL_PARAM;
    return result->governance == QXK24_AKAL_GOVERNED
        ? QXK24_OK : QXK24_ERR_LAW_VIOLATION;
}

qxk24_err_t qxk24_adam_akal_snapshot(qxk24_adam_akal_t akal,
                                     QXK24AkalSnapshot *snap_out) {
    if (akal == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = akal->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_akal_destroy(qxk24_adam_akal_t akal) {
    if (akal == NULL) return QXK24_ERR_NULL_PARAM;
    free(akal);
    return QXK24_OK;
}

const char *qxk24_adam_akal_process_name(QXK24AkalProcess proc) {
    switch (proc) {
    case QXK24_AKAL_PROC_AKAL: return "AKAL";
    case QXK24_AKAL_PROC_FIKIR: return "FIKIR";
    case QXK24_AKAL_PROC_IKHTIAR: return "IKHTIAR";
    case QXK24_AKAL_PROC_USAHA: return "USAHA";
    default: return "UNKNOWN";
    }
}
