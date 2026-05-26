/* ============================================================
 * qxk24_adam_adab.h
 * QXK24 Universal Kernel — ADAM Adab Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/adam/qxk24_adam_adab.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the ADAM Adab engine for Benar, Amanah,
 * Menyampaikan, and Bijaksana conduct scoring.
 * ============================================================ */

#ifndef QXK24_ADAM_ADAB_H
#define QXK24_ADAM_ADAB_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    QXK24_ADAB_BENAR = 0,
    QXK24_ADAB_AMANAH = 1,
    QXK24_ADAB_MENYAMPAIKAN = 2,
    QXK24_ADAB_BIJAKSANA = 3
} QXK24AdabElement;

typedef struct {
    float benar_score;
    float amanah_score;
    float menyampaikan_score;
    float bijaksana_score;
    float total_adab;
    bool adab_satisfied;
} QXK24AdabScore;

typedef struct {
    float avg_benar;
    float avg_amanah;
    float avg_menyampaikan;
    float avg_bijaksana;
    uint32_t satisfied_count;
    uint32_t violated_count;
} QXK24AdabSnapshot;

typedef struct QXK24AdamAdabImpl *qxk24_adam_adab_t;

qxk24_err_t qxk24_adam_adab_create(qxk24_adam_adab_t *adab_out);
qxk24_err_t qxk24_adam_adab_score(qxk24_adam_adab_t adab,
                                  float health_score,
                                  uint8_t capacity,
                                  uint8_t cv_level,
                                  QXK24AdabScore *score_out);
qxk24_err_t qxk24_adam_adab_check(const QXK24AdabScore *score);
qxk24_err_t qxk24_adam_adab_snapshot(qxk24_adam_adab_t adab,
                                     QXK24AdabSnapshot *snap_out);
qxk24_err_t qxk24_adam_adab_destroy(qxk24_adam_adab_t adab);
const char *qxk24_adam_adab_element_name(QXK24AdabElement element);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_ADAB_H */
