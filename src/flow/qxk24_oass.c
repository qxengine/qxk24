/* ============================================================
 * qxk24_oass.c
 * QXK24 Universal Kernel — OASS Cross Flow Implementation
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/flow/qxk24_oass.c
 * License : Apache 2.0
 *
 * Description:
 * Implements OASS cross-direction flows, cross-equilibrium scoring, and
 * ABA 1->2->4->2->1 tracking for constitutional flow cycles.
 * ============================================================ */

#include "qxk24/flow/qxk24_oass.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint64_t oass_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

static bool oass_type_valid(QXK24FlowType type) {
    return type == QXK24_FLOW_OASS_SEHALA ||
           type == QXK24_FLOW_OASS_PUSINGAN;
}

static double flow_x(uint64_t bytes, uint64_t duration_ms) {
    const double sec = (duration_ms > 0)
        ? (double)duration_ms / 1000.0
        : 0.001;
    return (double)bytes / sec;
}

int32_t qxk24_oass_context_create(QXK24OASSContext **out_ctx) {
    if (out_ctx == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24OASSContext *ctx =
        (QXK24OASSContext *)calloc(1, sizeof(QXK24OASSContext));
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

void qxk24_oass_context_destroy(QXK24OASSContext *ctx) {
    if (ctx == NULL) return;
    pthread_mutex_destroy(&ctx->mutex);
    memset(ctx, 0, sizeof(*ctx));
    free(ctx);
}

int32_t qxk24_oass_begin(QXK24OASSContext *ctx,
                         QXK24FlowType type,
                         QXK24FlowDir dir,
                         QXK24Direction src_segment,
                         QXK24Direction dst_segment,
                         QXK24Level src_level,
                         QXK24Level dst_level,
                         uint32_t src_resource_id,
                         uint32_t dst_resource_id,
                         uint64_t bytes,
                         uint64_t *out_flow_id) {
    if (ctx == NULL || out_flow_id == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if (!ctx->initialized) return QXK24_ERR_NOT_INITIALIZED;
    if (bytes == 0ULL || !oass_type_valid(type)) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)dir > (uint32_t)QXK24_FLOW_LWJ) return QXK24_ERR_INVALID_ARGUMENT;
    if (src_segment == dst_segment) return QXK24_ERR_DIRECTION_FAULT;
    if ((uint32_t)src_segment >= QXK24_DIRECTION_COUNT ||
        (uint32_t)dst_segment >= QXK24_DIRECTION_COUNT ||
        (uint32_t)src_level >= QXK24_LEVEL_COUNT ||
        (uint32_t)dst_level >= QXK24_LEVEL_COUNT) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    pthread_mutex_lock(&ctx->mutex);
    if (ctx->flow_count >= QXK24_OASS_MAX_FLOWS) {
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    uint32_t slot = QXK24_OASS_MAX_FLOWS;
    for (uint32_t i = 0; i < QXK24_OASS_MAX_FLOWS; i++) {
        if (!ctx->flows[i].active && !ctx->flows[i].completed) {
            slot = i;
            break;
        }
    }
    if (slot == QXK24_OASS_MAX_FLOWS) {
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    QXK24OASSFlow *f = &ctx->flows[slot];
    memset(f, 0, sizeof(*f));
    f->flow_id = ctx->next_flow_id++;
    f->type = type;
    f->direction = dir;
    f->src_segment = src_segment;
    f->dst_segment = dst_segment;
    f->src_level = src_level;
    f->dst_level = dst_level;
    f->src_resource_id = src_resource_id;
    f->dst_resource_id = dst_resource_id;
    f->bytes = bytes;
    f->timestamp_start_ms = oass_now_ms();
    f->x_src = flow_x(bytes, 1U);
    f->active = true;
    f->cross_valid = true;

    ctx->bytes_sent[(uint32_t)src_segment] += bytes;
    ctx->flow_count++;
    *out_flow_id = f->flow_id;
    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_OK;
}

int32_t qxk24_oass_complete(QXK24OASSContext *ctx,
                            uint64_t flow_id,
                            QXK24OASSMeasure *out_measure) {
    if (ctx == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    pthread_mutex_lock(&ctx->mutex);
    for (uint32_t i = 0; i < QXK24_OASS_MAX_FLOWS; i++) {
        QXK24OASSFlow *f = &ctx->flows[i];
        if (!f->active || f->flow_id != flow_id) continue;

        const uint64_t now = oass_now_ms();
        const uint64_t dur = now - f->timestamp_start_ms;
        f->timestamp_end_ms = now;
        f->x_dst = flow_x(f->bytes, dur);
        ctx->bytes_recv[(uint32_t)f->dst_segment] += f->bytes;

        const uint64_t sent = ctx->bytes_sent[(uint32_t)f->src_segment];
        const uint64_t recv = ctx->bytes_recv[(uint32_t)f->dst_segment];
        const uint64_t total = sent + recv;
        const uint64_t diff = (sent > recv) ? sent - recv : recv - sent;
        f->z4_compliant = total == 0ULL || ((double)diff / (double)total) < 0.30;
        f->active = false;
        f->completed = true;
        if (ctx->flow_count > 0) ctx->flow_count--;

        if (out_measure != NULL) {
            out_measure->x_src = f->x_src;
            out_measure->x_dst = f->x_dst;
            out_measure->cross_ratio = recv > 0ULL ? (double)sent / (double)recv : 1.0;
            out_measure->equilibrium = f->z4_compliant;
            out_measure->pairs_valid = f->cross_valid;
            out_measure->aba_complete = false;
            out_measure->duration_ms = dur;
        }

        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_ERR_NOT_FOUND;
}

int32_t qxk24_oass_cancel(QXK24OASSContext *ctx, uint64_t flow_id) {
    if (ctx == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    pthread_mutex_lock(&ctx->mutex);
    for (uint32_t i = 0; i < QXK24_OASS_MAX_FLOWS; i++) {
        QXK24OASSFlow *f = &ctx->flows[i];
        if (!f->active || f->flow_id != flow_id) continue;

        ctx->violation_count++;
        ctx->aba_cycles_broken++;
        memset(f, 0, sizeof(*f));
        if (ctx->flow_count > 0) ctx->flow_count--;
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_ERR_NOT_FOUND;
}

int32_t qxk24_oass_aba_begin(QXK24OASSContext *ctx,
                             uint64_t ikj_flow_id,
                             QXK24ABATracker *out_tracker) {
    if (ctx == NULL || out_tracker == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    pthread_mutex_lock(&ctx->mutex);
    for (uint32_t i = 0; i < QXK24_OASS_MAX_FLOWS; i++) {
        QXK24OASSFlow *f = &ctx->flows[i];
        if (f->flow_id != ikj_flow_id || f->direction != QXK24_FLOW_IKJ) continue;

        memset(out_tracker, 0, sizeof(*out_tracker));
        out_tracker->state = QXK24_ABA_PHASE_4;
        out_tracker->flow_id_ikj = ikj_flow_id;
        out_tracker->bytes_out = f->bytes;
        out_tracker->timestamp_start_ms = oass_now_ms();
        out_tracker->timestamp_peak_ms = out_tracker->timestamp_start_ms;
        out_tracker->valid = true;
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_ERR_NOT_FOUND;
}

int32_t qxk24_oass_aba_complete(QXK24OASSContext *ctx,
                                QXK24ABATracker *tracker,
                                uint64_t lwj_flow_id) {
    if (ctx == NULL || tracker == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if (!tracker->valid) return QXK24_ERR_INVALID_ARGUMENT;
    if (tracker->state != QXK24_ABA_PHASE_4 &&
        tracker->state != QXK24_ABA_RETURN_2) {
        return QXK24_ERR_FLOW_VIOLATION;
    }

    pthread_mutex_lock(&ctx->mutex);
    for (uint32_t i = 0; i < QXK24_OASS_MAX_FLOWS; i++) {
        QXK24OASSFlow *f = &ctx->flows[i];
        if (f->flow_id != lwj_flow_id || f->direction != QXK24_FLOW_LWJ) continue;

        tracker->flow_id_lwj = lwj_flow_id;
        tracker->bytes_in = f->bytes;
        tracker->timestamp_end_ms = oass_now_ms();
        tracker->state = QXK24_ABA_COMPLETE;
        ctx->aba_cycles_complete++;
        pthread_mutex_unlock(&ctx->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&ctx->mutex);
    return QXK24_ERR_NOT_FOUND;
}

double qxk24_oass_z3_score(QXK24OASSContext *ctx) {
    if (ctx == NULL) return 1.0;

    uint32_t total = 0;
    uint32_t valid = 0;
    for (uint32_t i = 0; i < QXK24_OASS_MAX_FLOWS; i++) {
        if (!ctx->flows[i].completed) continue;
        total++;
        if (ctx->flows[i].cross_valid) valid++;
    }
    return total == 0U ? 1.0 : (double)valid / (double)total;
}

double qxk24_oass_z4_score(QXK24OASSContext *ctx) {
    if (ctx == NULL) return 1.0;

    uint64_t sent = 0;
    uint64_t recv = 0;
    for (uint32_t d = 0; d < QXK24_DIRECTION_COUNT; d++) {
        sent += ctx->bytes_sent[d];
        recv += ctx->bytes_recv[d];
    }
    const uint64_t total = sent + recv;
    if (total == 0ULL) return 1.0;
    const uint64_t diff = (sent > recv) ? sent - recv : recv - sent;
    return 1.0 - ((double)diff / (double)total);
}

double qxk24_oass_x2_score(QXK24OASSContext *ctx) {
    if (ctx == NULL) return 1.0;

    const uint64_t total = ctx->aba_cycles_complete + ctx->aba_cycles_broken;
    if (total == 0ULL) return 1.0;
    return (double)ctx->aba_cycles_complete / (double)total;
}
