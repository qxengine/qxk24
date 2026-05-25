/* ============================================================
 * qxk24_oat.c
 * QXK24 Universal Kernel — OAT Direct Flow Implementation
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/flow/qxk24_oat.c
 * License : Apache 2.0
 *
 * Description:
 * Implements OAT direct flows, X = m/t measurement, cancellation, and
 * Law Z.1/Z.2/Z.3 scoring for IKJ/LWJ direct constitutional paths.
 * ============================================================ */

#include "qxk24/flow/qxk24_oat.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint64_t oat_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

static bool oat_type_valid(QXK24FlowType type) {
    return type == QXK24_FLOW_OAT_SEHALA ||
           type == QXK24_FLOW_OAT_PUSINGAN;
}

static double flow_x(uint64_t bytes, uint64_t duration_ms) {
    const double sec = (duration_ms > 0)
        ? (double)duration_ms / 1000.0
        : 0.001;
    return (double)bytes / sec;
}

int32_t qxk24_oat_context_create(QXK24OATContext **out_ctx) {
    if (out_ctx == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24OATContext *ctx =
        (QXK24OATContext *)calloc(1, sizeof(QXK24OATContext));
    if (ctx == NULL) return QXK24_ERR_OUT_OF_MEMORY;

    if (pthread_mutex_init(&ctx->mutex, NULL) != 0) {
        free(ctx);
        return QXK24_ERR_OUT_OF_MEMORY;
    }

    ctx->next_flow_id = 1ULL;
    ctx->initialized = true;
    *out_ctx = ctx;
    return QXK24_OK;
}

void qxk24_oat_context_destroy(QXK24OATContext *ctx) {
    if (ctx == NULL) return;
    pthread_mutex_destroy(&ctx->mutex);
    memset(ctx, 0, sizeof(*ctx));
    free(ctx);
}

int32_t qxk24_oat_begin(QXK24OATContext *ctx,
                        QXK24FlowType type,
                        QXK24FlowDir dir,
                        QXK24Level level,
                        QXK24Direction segment,
                        uint32_t resource_id,
                        uint64_t bytes,
                        uint64_t *out_flow_id) {
    if (ctx == NULL || out_flow_id == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if (!ctx->initialized) return QXK24_ERR_NOT_INITIALIZED;
    if (bytes == 0ULL || !oat_type_valid(type)) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)dir > (uint32_t)QXK24_FLOW_LWJ) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)level >= QXK24_LEVEL_COUNT) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)segment >= QXK24_DIRECTION_COUNT) return QXK24_ERR_INVALID_ARGUMENT;

    pthread_mutex_lock(&ctx->mutex);
    if (ctx->flow_count >= QXK24_OAT_MAX_FLOWS) {
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    uint32_t slot = QXK24_OAT_MAX_FLOWS;
    for (uint32_t i = 0; i < QXK24_OAT_MAX_FLOWS; i++) {
        if (!ctx->flows[i].active && !ctx->flows[i].completed) {
            slot = i;
            break;
        }
    }
    if (slot == QXK24_OAT_MAX_FLOWS) {
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    QXK24OATFlow *f = &ctx->flows[slot];
    memset(f, 0, sizeof(*f));
    f->flow_id = ctx->next_flow_id++;
    f->type = type;
    f->direction = dir;
    f->level = level;
    f->segment = segment;
    f->resource_id = resource_id;
    f->bytes = bytes;
    f->timestamp_ms = oat_now_ms();
    f->active = true;
    f->z2_compliant = true;

    ctx->flow_count++;
    *out_flow_id = f->flow_id;
    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_OK;
}

int32_t qxk24_oat_complete(QXK24OATContext *ctx,
                           uint64_t flow_id,
                           QXK24OATMeasure *out_measure) {
    if (ctx == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    pthread_mutex_lock(&ctx->mutex);
    for (uint32_t i = 0; i < QXK24_OAT_MAX_FLOWS; i++) {
        QXK24OATFlow *f = &ctx->flows[i];
        if (!f->active || f->flow_id != flow_id) continue;

        const uint64_t now = oat_now_ms();
        f->duration_ms = now - f->timestamp_ms;
        f->x_measured = flow_x(f->bytes, f->duration_ms);
        if (f->direction == QXK24_FLOW_IKJ) {
            ctx->total_ikj++;
            ctx->total_bytes_ikj += f->bytes;
        } else {
            ctx->total_lwj++;
            ctx->total_bytes_lwj += f->bytes;
        }
        f->active = false;
        f->completed = true;
        if (ctx->flow_count > 0) ctx->flow_count--;

        if (out_measure != NULL) {
            out_measure->x_value = f->x_measured;
            out_measure->m_bytes = f->bytes;
            out_measure->t_ms = f->duration_ms;
            out_measure->budget_ratio = 0.0;
            out_measure->within_limit = f->z2_compliant;
            out_measure->pattern_valid = true;
        }

        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_ERR_NOT_FOUND;
}

int32_t qxk24_oat_cancel(QXK24OATContext *ctx, uint64_t flow_id) {
    if (ctx == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    pthread_mutex_lock(&ctx->mutex);
    for (uint32_t i = 0; i < QXK24_OAT_MAX_FLOWS; i++) {
        QXK24OATFlow *f = &ctx->flows[i];
        if (!f->active || f->flow_id != flow_id) continue;

        ctx->violation_count++;
        memset(f, 0, sizeof(*f));
        if (ctx->flow_count > 0) ctx->flow_count--;
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_ERR_NOT_FOUND;
}

int32_t qxk24_oat_measure(QXK24OATContext *ctx,
                          uint64_t flow_id,
                          uint64_t budget_soft,
                          QXK24OATMeasure *out_measure) {
    if (ctx == NULL || out_measure == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    pthread_mutex_lock(&ctx->mutex);
    for (uint32_t i = 0; i < QXK24_OAT_MAX_FLOWS; i++) {
        QXK24OATFlow *f = &ctx->flows[i];
        if (!f->active && !f->completed) continue;
        if (f->flow_id != flow_id) continue;

        const uint64_t now = oat_now_ms();
        const uint64_t dur = f->completed ? f->duration_ms : now - f->timestamp_ms;
        out_measure->x_value = flow_x(f->bytes, dur);
        out_measure->m_bytes = f->bytes;
        out_measure->t_ms = dur;
        out_measure->budget_ratio = (budget_soft > 0)
            ? (double)f->bytes / (double)budget_soft
            : 0.0;
        out_measure->within_limit = budget_soft == 0 || f->bytes <= budget_soft;
        out_measure->pattern_valid = oat_type_valid(f->type);
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_ERR_NOT_FOUND;
}

double qxk24_oat_z1_score(QXK24OATContext *ctx) {
    if (ctx == NULL) return 0.0;
    const uint64_t good = ctx->total_ikj + ctx->total_lwj;
    if (good == 0ULL) return 1.0;
    return (double)good / (double)(good + (uint64_t)ctx->violation_count);
}

double qxk24_oat_z2_score(QXK24OATContext *ctx, uint64_t budget_soft) {
    if (ctx == NULL || budget_soft == 0ULL) return 1.0;

    uint32_t total = 0;
    uint32_t within = 0;
    for (uint32_t i = 0; i < QXK24_OAT_MAX_FLOWS; i++) {
        if (!ctx->flows[i].completed) continue;
        total++;
        if (ctx->flows[i].bytes <= budget_soft) within++;
    }
    return total == 0 ? 1.0 : (double)within / (double)total;
}

double qxk24_oat_z3_balance(QXK24OATContext *ctx) {
    if (ctx == NULL) return 1.0;

    const uint64_t total = ctx->total_ikj + ctx->total_lwj;
    if (total == 0ULL) return 1.0;
    const uint64_t diff = (ctx->total_ikj > ctx->total_lwj)
        ? ctx->total_ikj - ctx->total_lwj
        : ctx->total_lwj - ctx->total_ikj;
    return 1.0 - ((double)diff / (double)total);
}
