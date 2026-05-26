/* ============================================================
 * qxk24_adam_cv.c
 * QXK24 Universal Kernel — ADAM Contribution Value Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/adam/qxk24_adam_cv.c
 * License : Apache 2.0
 *
 * Description:
 * Implements ADAM seven-tier contribution value assessment and
 * accumulated cV snapshots.
 * ============================================================ */

#include "qxk24/adam/qxk24_adam_cv.h"
#include <stdlib.h>

struct QXK24AdamCvImpl {
    QXK24CvSnapshot snap;
};

static uint8_t clamp_capacity(uint8_t capacity) {
    if (capacity < 1U) return 1U;
    if (capacity > 7U) return 7U;
    return capacity;
}

static QXK24CvLevel assess_level(float health_score,
                                 uint8_t capacity,
                                 bool hukumz_passed) {
    if (!hukumz_passed) return QXK24_CV_DIRI_SENDIRI;
    uint8_t level = clamp_capacity(capacity);
    if (health_score < 50.0f) level = 1U;
    else if (health_score < 60.0f && level > 2U) level = 2U;
    else if (health_score < 80.0f && level > 4U) level = 4U;
    else if (health_score < 95.0f && level > 6U) level = 6U;
    return (QXK24CvLevel)level;
}

qxk24_err_t qxk24_adam_cv_create(qxk24_adam_cv_t *cv_out) {
    if (cv_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AdamCvImpl *cv = calloc(1, sizeof(*cv));
    if (cv == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    *cv_out = cv;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_cv_assess(qxk24_adam_cv_t cv,
                                 float health_score,
                                 uint8_t capacity,
                                 bool hukumz_passed,
                                 QXK24CvRecord *record_out) {
    if (cv == NULL || record_out == NULL) return QXK24_ERR_NULL_PARAM;
    const QXK24CvLevel level = assess_level(health_score, capacity,
        hukumz_passed);
    record_out->level = level;
    record_out->value = qxk24_adam_cv_compute((uint8_t)level, health_score,
        2.0f);
    record_out->self_met = level >= QXK24_CV_DIRI_SENDIRI;
    record_out->family_met = level >= QXK24_CV_KELUARGA;
    record_out->community_met = level >= QXK24_CV_MASYARAKAT;
    record_out->universal_met = level >= QXK24_CV_ALAM_SEMESTA;

    const uint32_t total = (uint32_t)(cv->snap.total_cv_value > 0.0f
        ? cv->snap.level_counts[0] + cv->snap.level_counts[1] +
          cv->snap.level_counts[2] + cv->snap.level_counts[3] +
          cv->snap.level_counts[4] + cv->snap.level_counts[5] +
          cv->snap.level_counts[6]
        : 0U);
    cv->snap.level_counts[(uint8_t)level - 1U]++;
    cv->snap.total_cv_value += record_out->value;
    cv->snap.avg_cv_level =
        ((cv->snap.avg_cv_level * (float)total) + (float)level) /
        (float)(total + 1U);
    return QXK24_OK;
}

const char *qxk24_adam_cv_level_name(QXK24CvLevel level) {
    switch (level) {
    case QXK24_CV_DIRI_SENDIRI: return "DIRI_SENDIRI";
    case QXK24_CV_KELUARGA: return "KELUARGA";
    case QXK24_CV_MASYARAKAT: return "MASYARAKAT";
    case QXK24_CV_BANGSA: return "BANGSA";
    case QXK24_CV_WILAYAH: return "WILAYAH";
    case QXK24_CV_DUNIA: return "DUNIA";
    case QXK24_CV_ALAM_SEMESTA: return "ALAM_SEMESTA";
    default: return "UNKNOWN";
    }
}

float qxk24_adam_cv_compute(uint8_t capacity,
                            float health_score,
                            float fm_value) {
    return (float)clamp_capacity(capacity) * (health_score / 100.0f) *
        fm_value;
}

qxk24_err_t qxk24_adam_cv_snapshot(qxk24_adam_cv_t cv,
                                   QXK24CvSnapshot *snap_out) {
    if (cv == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = cv->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_cv_destroy(qxk24_adam_cv_t cv) {
    if (cv == NULL) return QXK24_ERR_NULL_PARAM;
    free(cv);
    return QXK24_OK;
}
