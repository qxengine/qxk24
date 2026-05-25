/* ============================================================
 * qxk24_kernel.c
 * QXK24 Universal Kernel — Kernel Lifecycle & Governor
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/core/qxk24_kernel.c
 * License : Apache 2.0
 *
 * Description:
 * Implements kernel creation, start, cycle execution, stop, destruction,
 * and constitutional report generation for the public lifecycle API.
 */

#include "core/qxk24_kernel_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *tier_label(QXK24CertTier tier) {
    switch (tier) {
    case QXK24_CERT_SOVEREIGN: return "SOVEREIGN";
    case QXK24_CERT_PROFESSIONAL: return "PROFESSIONAL";
    case QXK24_CERT_STANDARD: return "STANDARD";
    case QXK24_CERT_NONE:
    default: return "NONE";
    }
}

static void fill_report(QXK24Kernel *k, QXK24Report *out_report) {
    uint64_t total_bytes = 0;
    uint64_t total_soft = 0;
    uint32_t active_r = 0;

    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (!r->active) continue;
        total_bytes += r->used_bytes;
        total_soft += r->budget_soft;
        active_r++;
    }

    out_report->cycle_index = k->cycle_index;
    out_report->timestamp_ms = qxk24_now_ms();
    out_report->law_scores = k->law_scores;
    out_report->health_score = k->health_score;
    out_report->cert_tier = k->cert_tier;
    out_report->active_resources = active_r;
    out_report->active_flows = qxk24_count_active_flows(k);
    out_report->total_bytes_governed = total_bytes;
    out_report->global_utilisation = (total_soft > 0)
        ? (double)total_bytes / (double)total_soft
        : 0.0;

    snprintf(out_report->log, QXK24_MAX_LOG_LEN,
        "QXK24 Cycle %llu | Health %.1f | %s | "
        "Z1:%.2f Z2:%.2f Z3:%.2f Z4:%.2f | "
        "X1:%.2f X2:%.2f X3:%.2f X4:%.2f | Resources:%u Flows:%u",
        (unsigned long long)k->cycle_index,
        k->health_score,
        tier_label(k->cert_tier),
        k->law_scores.z1_pattern,
        k->law_scores.z2_limit,
        k->law_scores.z3_pairs,
        k->law_scores.z4_equilibrium,
        k->law_scores.x1_knowledge,
        k->law_scores.x2_ethics,
        k->law_scores.x3_creativity,
        k->law_scores.x4_economy,
        active_r,
        out_report->active_flows);
}

int32_t qxk24_create(const QXK24Config *config,
                     QXK24KernelHandle *out_handle) {
    if (config == NULL || out_handle == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if (config->declared_x <= 0.0) return QXK24_ERR_INVALID_ARGUMENT;
    if (config->tolerance_pct < 0.0 || config->tolerance_pct > 100.0) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    if (config->luman_peringkat > 6U) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)calloc(1, sizeof(QXK24Kernel));
    if (k == NULL) return QXK24_ERR_OUT_OF_MEMORY;

    k->config = *config;
    k->health_score = 100.0;
    k->cert_tier = QXK24_CERT_SOVEREIGN;
    k->foreground = true;
    qxk24_law_scores_init(&k->law_scores);
    strncpy(k->label, config->kernel_label, QXK24_MAX_LABEL_LEN - 1U);

    if (pthread_mutex_init(&k->mutex, NULL) != 0) {
        free(k);
        return QXK24_ERR_OUT_OF_MEMORY;
    }

    k->initialized = true;
    *out_handle = k;
    return QXK24_OK;
}

int32_t qxk24_start(QXK24KernelHandle handle) {
    if (handle == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    if (!k->initialized) {
        pthread_mutex_unlock(&k->mutex);
        return QXK24_ERR_NOT_INITIALIZED;
    }
    if (k->started) {
        pthread_mutex_unlock(&k->mutex);
        return QXK24_ERR_ALREADY_INITIALIZED;
    }

    k->started = true;
    pthread_mutex_unlock(&k->mutex);
    return QXK24_OK;
}

int32_t qxk24_cycle(QXK24KernelHandle handle, QXK24Report *out_report) {
    if (handle == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    if (!k->started) {
        pthread_mutex_unlock(&k->mutex);
        return QXK24_ERR_NOT_INITIALIZED;
    }

    k->cycle_index++;
    qxk24_evaluate_laws(k);
    k->health_score = qxk24_compute_health(&k->law_scores);
    k->cert_tier = qxk24_cert_from_score(k->health_score);

    if (out_report != NULL) {
        fill_report(k, out_report);
    }

    pthread_mutex_unlock(&k->mutex);
    return QXK24_OK;
}

void qxk24_stop(QXK24KernelHandle handle) {
    if (handle == NULL) return;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    k->started = false;
    pthread_mutex_unlock(&k->mutex);
}

void qxk24_destroy(QXK24KernelHandle handle) {
    if (handle == NULL) return;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    qxk24_stop(handle);
    pthread_mutex_destroy(&k->mutex);
    memset(k, 0, sizeof(QXK24Kernel));
    free(k);
}
