/* ============================================================
 * qxk24_qxengine_health.h
 * QXK24 Universal Kernel - QXEngine Health Feed
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/bridge/qxk24_qxengine_health.h
 * License : Apache 2.0
 *
 * Description:
 * Declares weighted health feed computation between QXEngine and QXK24.
 * ============================================================ */

#ifndef QXK24_QXENGINE_HEALTH_H
#define QXK24_QXENGINE_HEALTH_H

#include "qxk24/resource/qxk24_memory_qxengine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_QXENGINE_HEALTH_WEIGHT 0.40f
#define QXK24_KERNEL_HEALTH_WEIGHT 0.60f

typedef struct QXK24HealthFeedRecord_s {
    float qxengine_score;
    float kernel_score;
    float combined_score;
    uint64_t feed_cycle;
    uint64_t timestamp_ms;
    bool professional_met;
} QXK24HealthFeedRecord;

qxk24_err_t qxk24_health_feed_compute(float qxengine_score,
                                      float kernel_score,
                                      QXK24HealthFeedRecord *out);
qxk24_err_t qxk24_health_feed_from_governor(qxk24_qxengine_t gov,
                                            qxk24_kernel_t kernel,
                                            QXK24HealthFeedRecord *out);
bool qxk24_health_feed_meets_professional(
    const QXK24HealthFeedRecord *record);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_QXENGINE_HEALTH_H */
