/* ============================================================
 * qxk24_adam_hukumz.h
 * QXK24 Universal Kernel — ADAM Hukum Z Validator
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/adam/qxk24_adam_hukumz.h
 * License : Apache 2.0
 *
 * Description:
 * Declares ADAM validation for Pola, Kadar, Pasangan, and
 * Keseimbangan under Hukum Z.
 * ============================================================ */

#ifndef QXK24_ADAM_HUKUMZ_H
#define QXK24_ADAM_HUKUMZ_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    QXK24_HUKUMZ_POLA = 0,
    QXK24_HUKUMZ_KADAR = 1,
    QXK24_HUKUMZ_PASANGAN = 2,
    QXK24_HUKUMZ_KESEIMBANGAN = 3
} QXK24HukumZ;

typedef enum {
    QXK24_POLA_ABA_PASIF = 0,
    QXK24_POLA_ABA_AKTIF = 1,
    QXK24_POLA_GHP = 2,
    QXK24_POLA_GCP = 3
} QXK24PolaType;

typedef struct {
    bool pola_identified;
    QXK24PolaType pola_type;
    bool kadar_within;
    float kadar_score;
    bool pasangan_found;
    bool dominant_known;
    bool xkn_present;
    bool xkr_present;
    bool keperluan_met;
    bool kapasiti_used;
    float keseimbangan_score;
    bool all_passed;
    float total_score;
} QXK24HukumZResult;

typedef struct {
    uint32_t total_validations;
    uint32_t all_passed_count;
    uint32_t pola_failures;
    uint32_t kadar_failures;
    uint32_t pasangan_failures;
    uint32_t keseimbangan_failures;
    float avg_total_score;
} QXK24HukumZSnapshot;

typedef struct QXK24AdamHukumZImpl *qxk24_adam_hukumz_t;

qxk24_err_t qxk24_adam_hukumz_create(qxk24_adam_hukumz_t *hz_out);
qxk24_err_t qxk24_adam_hukumz_validate(qxk24_adam_hukumz_t hz,
                                       float health_score,
                                       uint8_t capacity,
                                       float ft_total,
                                       float fm_value,
                                       QXK24HukumZResult *result_out);
qxk24_err_t qxk24_adam_hukumz_check_all(const QXK24HukumZResult *result);
const char *qxk24_adam_hukumz_name(QXK24HukumZ hukum);
qxk24_err_t qxk24_adam_hukumz_snapshot(qxk24_adam_hukumz_t hz,
                                       QXK24HukumZSnapshot *snap_out);
qxk24_err_t qxk24_adam_hukumz_destroy(qxk24_adam_hukumz_t hz);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_HUKUMZ_H */
