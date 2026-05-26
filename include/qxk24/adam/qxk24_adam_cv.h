/* ============================================================
 * qxk24_adam_cv.h
 * QXK24 Universal Kernel — ADAM Contribution Value Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/adam/qxk24_adam_cv.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the ADAM cV engine for seven-tier constitutional
 * contribution value assessment.
 * ============================================================ */

#ifndef QXK24_ADAM_CV_H
#define QXK24_ADAM_CV_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    QXK24_CV_DIRI_SENDIRI = 1,
    QXK24_CV_KELUARGA = 2,
    QXK24_CV_MASYARAKAT = 3,
    QXK24_CV_BANGSA = 4,
    QXK24_CV_WILAYAH = 5,
    QXK24_CV_DUNIA = 6,
    QXK24_CV_ALAM_SEMESTA = 7
} QXK24CvLevel;

typedef struct {
    QXK24CvLevel level;
    float value;
    bool self_met;
    bool family_met;
    bool community_met;
    bool universal_met;
} QXK24CvRecord;

typedef struct {
    float avg_cv_level;
    uint32_t level_counts[7];
    float total_cv_value;
} QXK24CvSnapshot;

typedef struct QXK24AdamCvImpl *qxk24_adam_cv_t;

qxk24_err_t qxk24_adam_cv_create(qxk24_adam_cv_t *cv_out);
qxk24_err_t qxk24_adam_cv_assess(qxk24_adam_cv_t cv,
                                 float health_score,
                                 uint8_t capacity,
                                 bool hukumz_passed,
                                 QXK24CvRecord *record_out);
const char *qxk24_adam_cv_level_name(QXK24CvLevel level);
float qxk24_adam_cv_compute(uint8_t capacity,
                            float health_score,
                            float fm_value);
qxk24_err_t qxk24_adam_cv_snapshot(qxk24_adam_cv_t cv,
                                   QXK24CvSnapshot *snap_out);
qxk24_err_t qxk24_adam_cv_destroy(qxk24_adam_cv_t cv);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_CV_H */
