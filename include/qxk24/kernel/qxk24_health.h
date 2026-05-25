/* ============================================================
 * qxk24_health.h
 * QXK24 Universal Kernel — Constitutional Health Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/kernel/qxk24_health.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the weighted eight-law health score engine for Phase 3.
 * ============================================================ */

#ifndef QXK24_HEALTH_H
#define QXK24_HEALTH_H

#include "qxk24/qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_HEALTH_W_Z1   0.20f
#define QXK24_HEALTH_W_Z2   0.20f
#define QXK24_HEALTH_W_Z3   0.15f
#define QXK24_HEALTH_W_Z4   0.15f
#define QXK24_HEALTH_W_X1   0.12f
#define QXK24_HEALTH_W_X2   0.10f
#define QXK24_HEALTH_W_X3   0.05f
#define QXK24_HEALTH_W_X4   0.03f

#define QXK24_HEALTH_SOVEREIGN      95.0f
#define QXK24_HEALTH_PROFESSIONAL   80.0f
#define QXK24_HEALTH_STANDARD       60.0f

typedef struct QXK24HealthInput_s {
    float z1;
    float z2;
    float z3;
    float z4;
    float x1;
    float x2;
    float x3;
    float x4;
} QXK24HealthInput;

typedef struct QXK24HealthResult_s {
    float score;
    QXK24CertTier tier;
    QXK24HealthInput components;
    uint64_t cycle_index;
    uint64_t timestamp_ms;
} QXK24HealthResult;

qxk24_err_t qxk24_health_compute(const QXK24HealthInput *input,
                                 QXK24HealthResult *out);
QXK24CertTier qxk24_health_tier(float score);
float qxk24_health_law_contribution(uint8_t law, float score);
float qxk24_health_weight(uint8_t law);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_HEALTH_H */
