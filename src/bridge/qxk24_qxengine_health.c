/* ============================================================
 * qxk24_qxengine_health.c
 * QXK24 Universal Kernel - QXEngine Health Feed
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/bridge/qxk24_qxengine_health.c
 * License : Apache 2.0
 *
 * Description:
 * Computes combined PROFESSIONAL health from QXEngine and QXK24 scores.
 * ============================================================ */

#include "qxk24/bridge/qxk24_qxengine_health.h"
#include <time.h>

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

static float clamp_score(float score) {
    if (score < 0.0f) return 0.0f;
    if (score > 100.0f) return 100.0f;
    return score;
}

qxk24_err_t qxk24_health_feed_compute(float qxengine_score,
                                      float kernel_score,
                                      QXK24HealthFeedRecord *out) {
    if (out == NULL) return QXK24_ERR_NULL_PARAM;

    qxengine_score = clamp_score(qxengine_score);
    kernel_score = clamp_score(kernel_score);
    out->qxengine_score = qxengine_score;
    out->kernel_score = kernel_score;
    out->combined_score =
        kernel_score * QXK24_KERNEL_HEALTH_WEIGHT +
        qxengine_score * QXK24_QXENGINE_HEALTH_WEIGHT;
    out->feed_cycle = 0ULL;
    out->timestamp_ms = now_ms();
    out->professional_met =
        out->combined_score >= QXK24_HEALTH_PROFESSIONAL;
    return QXK24_OK;
}

qxk24_err_t qxk24_health_feed_from_governor(qxk24_qxengine_t gov,
                                            qxk24_kernel_t kernel,
                                            QXK24HealthFeedRecord *out) {
    if (gov == NULL || kernel == NULL || out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    qxk24_err_t err = qxk24_health_feed_compute(
        qxk24_qxengine_health_score(gov),
        (float)qxk24_health_score(kernel),
        out);
    if (err == QXK24_OK) out->feed_cycle = qxk24_cycle_index(kernel);
    return err;
}

bool qxk24_health_feed_meets_professional(
    const QXK24HealthFeedRecord *record) {
    return record != NULL && record->professional_met;
}
