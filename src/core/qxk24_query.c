/* ============================================================
 * qxk24_query.c
 * QXK24 Universal Kernel — Signals and Query API
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/core/qxk24_query.c
 * License : Apache 2.0
 *
 * Description:
 * Implements OS signal forwarding, health and certification queries, active
 * resource/flow counters, and law-score snapshot accessors.
 */

#include "core/qxk24_kernel_internal.h"

uint32_t qxk24_count_active_flows(const QXK24Kernel *kernel) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < QXK24_MAX_ACTIVE_FLOWS; i++) {
        if (kernel->flows[i].active) count++;
    }
    return count;
}

void qxk24_notify_pressure(QXK24KernelHandle handle, int32_t tier) {
    if (handle == NULL) return;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    k->pressure_tier = tier;
    pthread_mutex_unlock(&k->mutex);
}

void qxk24_notify_lifecycle(QXK24KernelHandle handle, int32_t foreground) {
    if (handle == NULL) return;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    k->foreground = (foreground != 0);
    pthread_mutex_unlock(&k->mutex);
}

void qxk24_notify_screen(QXK24KernelHandle handle,
                         int32_t width_px,
                         int32_t height_px,
                         float scale) {
    (void)width_px;
    (void)height_px;
    (void)scale;
    if (handle == NULL) return;
}

double qxk24_health_score(QXK24KernelHandle handle) {
    if (handle == NULL) return 0.0;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    const double score = k->health_score;
    pthread_mutex_unlock(&k->mutex);
    return score;
}

QXK24CertTier qxk24_cert_tier(QXK24KernelHandle handle) {
    if (handle == NULL) return QXK24_CERT_NONE;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    const QXK24CertTier tier = k->cert_tier;
    pthread_mutex_unlock(&k->mutex);
    return tier;
}

uint64_t qxk24_cycle_index(QXK24KernelHandle handle) {
    if (handle == NULL) return 0ULL;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    const uint64_t cycle = k->cycle_index;
    pthread_mutex_unlock(&k->mutex);
    return cycle;
}

uint32_t qxk24_active_resources(QXK24KernelHandle handle) {
    if (handle == NULL) return 0U;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    const uint32_t count = k->resource_count;
    pthread_mutex_unlock(&k->mutex);
    return count;
}

uint32_t qxk24_active_flows(QXK24KernelHandle handle) {
    if (handle == NULL) return 0U;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    const uint32_t count = qxk24_count_active_flows(k);
    pthread_mutex_unlock(&k->mutex);
    return count;
}

int32_t qxk24_get_law_scores(QXK24KernelHandle handle,
                             QXK24LawScore *out) {
    if (handle == NULL || out == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    *out = k->law_scores;
    pthread_mutex_unlock(&k->mutex);
    return QXK24_OK;
}
