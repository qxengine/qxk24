/* ============================================================
 * qxk24_adam_hukumz.c
 * QXK24 Universal Kernel — ADAM Hukum Z Validator
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/adam/qxk24_adam_hukumz.c
 * License : Apache 2.0
 *
 * Description:
 * Implements ADAM Hukum Z validation for Pola, Kadar,
 * Pasangan, and Keseimbangan.
 * ============================================================ */

#include "qxk24/adam/qxk24_adam_hukumz.h"
#include <stdlib.h>

struct QXK24AdamHukumZImpl {
    QXK24HukumZSnapshot snap;
};

static float clamp_unit(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static float avg_next(float avg, uint32_t count, float value) {
    if (count == 0U) return value;
    return ((avg * (float)count) + value) / (float)(count + 1U);
}

qxk24_err_t qxk24_adam_hukumz_create(qxk24_adam_hukumz_t *hz_out) {
    if (hz_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AdamHukumZImpl *hz = calloc(1, sizeof(*hz));
    if (hz == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    *hz_out = hz;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_hukumz_validate(qxk24_adam_hukumz_t hz,
                                       float health_score,
                                       uint8_t capacity,
                                       float ft_total,
                                       float fm_value,
                                       QXK24HukumZResult *result_out) {
    if (hz == NULL || result_out == NULL) return QXK24_ERR_NULL_PARAM;
    result_out->pola_identified = true;
    result_out->pola_type = health_score >= 50.0f ? QXK24_POLA_GHP :
        QXK24_POLA_GCP;
    result_out->kadar_within = capacity >= 1U && capacity <= 7U &&
        fm_value > 0.0f;
    result_out->kadar_score = clamp_unit(health_score / 100.0f);
    result_out->pasangan_found = ft_total >= 0.0f;
    result_out->dominant_known = true;
    result_out->xkn_present = true;
    result_out->xkr_present = true;
    result_out->keperluan_met = health_score >= 50.0f;
    result_out->kapasiti_used = capacity >= 1U && capacity <= 7U;
    result_out->keseimbangan_score =
        (result_out->kadar_score + (ft_total >= 0.0f ? 1.0f : 0.0f)) * 0.5f;
    result_out->all_passed = result_out->pola_identified &&
        result_out->kadar_within && result_out->pasangan_found &&
        result_out->keperluan_met && result_out->kapasiti_used;
    result_out->total_score = (result_out->kadar_score +
        result_out->keseimbangan_score +
        (result_out->pola_identified ? 1.0f : 0.0f) +
        (result_out->pasangan_found ? 1.0f : 0.0f)) / 4.0f;

    const uint32_t before = hz->snap.total_validations;
    hz->snap.total_validations++;
    if (result_out->all_passed) hz->snap.all_passed_count++;
    if (!result_out->pola_identified) hz->snap.pola_failures++;
    if (!result_out->kadar_within) hz->snap.kadar_failures++;
    if (!result_out->pasangan_found) hz->snap.pasangan_failures++;
    if (!result_out->keperluan_met || !result_out->kapasiti_used) {
        hz->snap.keseimbangan_failures++;
    }
    hz->snap.avg_total_score = avg_next(hz->snap.avg_total_score, before,
        result_out->total_score);
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_hukumz_check_all(const QXK24HukumZResult *result) {
    if (result == NULL) return QXK24_ERR_NULL_PARAM;
    return result->all_passed ? QXK24_OK : QXK24_ERR_LAW_VIOLATION;
}

const char *qxk24_adam_hukumz_name(QXK24HukumZ hukum) {
    switch (hukum) {
    case QXK24_HUKUMZ_POLA: return "POLA";
    case QXK24_HUKUMZ_KADAR: return "KADAR";
    case QXK24_HUKUMZ_PASANGAN: return "PASANGAN";
    case QXK24_HUKUMZ_KESEIMBANGAN: return "KESEIMBANGAN";
    default: return "UNKNOWN";
    }
}

qxk24_err_t qxk24_adam_hukumz_snapshot(qxk24_adam_hukumz_t hz,
                                       QXK24HukumZSnapshot *snap_out) {
    if (hz == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = hz->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_hukumz_destroy(qxk24_adam_hukumz_t hz) {
    if (hz == NULL) return QXK24_ERR_NULL_PARAM;
    free(hz);
    return QXK24_OK;
}
