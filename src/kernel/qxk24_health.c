/* ============================================================
 * qxk24_health.c
 * QXK24 Universal Kernel — Constitutional Health Score Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/kernel/qxk24_health.c
 * License : Apache 2.0
 *
 * Description:
 * Computes the Phase 3 weighted health score from eight constitutional laws.
 * ============================================================ */

#include "qxk24/kernel/qxk24_health.h"
#include <time.h>

static const float s_weights[8] = {
    QXK24_HEALTH_W_Z1,
    QXK24_HEALTH_W_Z2,
    QXK24_HEALTH_W_Z3,
    QXK24_HEALTH_W_Z4,
    QXK24_HEALTH_W_X1,
    QXK24_HEALTH_W_X2,
    QXK24_HEALTH_W_X3,
    QXK24_HEALTH_W_X4
};

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

static void input_to_array(const QXK24HealthInput *input, float out[8]) {
    out[0] = input->z1;
    out[1] = input->z2;
    out[2] = input->z3;
    out[3] = input->z4;
    out[4] = input->x1;
    out[5] = input->x2;
    out[6] = input->x3;
    out[7] = input->x4;
}

float qxk24_health_weight(uint8_t law) {
    return law < 8U ? s_weights[law] : 0.0f;
}

float qxk24_health_law_contribution(uint8_t law, float score) {
    return law < 8U ? s_weights[law] * clamp01(score) : 0.0f;
}

QXK24CertTier qxk24_health_tier(float score) {
    if (score >= QXK24_HEALTH_SOVEREIGN) return QXK24_CERT_SOVEREIGN;
    if (score >= QXK24_HEALTH_PROFESSIONAL) return QXK24_CERT_PROFESSIONAL;
    if (score >= QXK24_HEALTH_STANDARD) return QXK24_CERT_STANDARD;
    return QXK24_CERT_NONE;
}

qxk24_err_t qxk24_health_compute(const QXK24HealthInput *input,
                                 QXK24HealthResult *out) {
    if (input == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;

    float raw[8];
    float sum = 0.0f;
    input_to_array(input, raw);
    for (uint8_t i = 0; i < 8U; i++) {
        sum += s_weights[i] * clamp01(raw[i]);
    }

    float score = sum * 100.0f;
    if (score < 0.0f) score = 0.0f;
    if (score > 100.0f) score = 100.0f;

    out->score = score;
    out->tier = qxk24_health_tier(score);
    out->components = *input;
    out->cycle_index = 0ULL;
    out->timestamp_ms = now_ms();
    return QXK24_OK;
}
