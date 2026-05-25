/* ============================================================
 * qxk24_oat.h
 * QXK24 Universal Kernel — OAT Direct Flow Header
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/flow/qxk24_oat.h
 * License : Apache 2.0
 *
 * Description:
 * Declares Operasi Aliran Terus (OAT), the direct constitutional flow
 * model for one-way and rotational resource movement.
 * ============================================================ */

#ifndef QXK24_OAT_H
#define QXK24_OAT_H

#include "qxk24/qxk24.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_OAT_MAX_FLOWS 256U

typedef struct QXK24OATFlow_s {
    uint64_t flow_id;
    QXK24FlowType type;
    QXK24FlowDir direction;
    QXK24Level level;
    QXK24Direction segment;
    uint32_t resource_id;
    uint64_t bytes;
    uint64_t timestamp_ms;
    uint64_t duration_ms;
    double x_measured;
    bool active;
    bool completed;
    bool z2_compliant;
} QXK24OATFlow;

typedef struct QXK24OATContext_s {
    QXK24OATFlow flows[QXK24_OAT_MAX_FLOWS];
    uint32_t flow_count;
    uint64_t next_flow_id;
    uint64_t total_ikj;
    uint64_t total_lwj;
    uint64_t total_bytes_ikj;
    uint64_t total_bytes_lwj;
    uint32_t violation_count;
    pthread_mutex_t mutex;
    bool initialized;
} QXK24OATContext;

typedef struct QXK24OATMeasure_s {
    double x_value;
    uint64_t m_bytes;
    uint64_t t_ms;
    double budget_ratio;
    bool within_limit;
    bool pattern_valid;
} QXK24OATMeasure;

int32_t qxk24_oat_context_create(QXK24OATContext **out_ctx);
void qxk24_oat_context_destroy(QXK24OATContext *ctx);

int32_t qxk24_oat_begin(QXK24OATContext *ctx,
                        QXK24FlowType type,
                        QXK24FlowDir dir,
                        QXK24Level level,
                        QXK24Direction segment,
                        uint32_t resource_id,
                        uint64_t bytes,
                        uint64_t *out_flow_id);

int32_t qxk24_oat_complete(QXK24OATContext *ctx,
                           uint64_t flow_id,
                           QXK24OATMeasure *out_measure);

int32_t qxk24_oat_cancel(QXK24OATContext *ctx, uint64_t flow_id);

int32_t qxk24_oat_measure(QXK24OATContext *ctx,
                          uint64_t flow_id,
                          uint64_t budget_soft,
                          QXK24OATMeasure *out_measure);

double qxk24_oat_z1_score(QXK24OATContext *ctx);
double qxk24_oat_z2_score(QXK24OATContext *ctx, uint64_t budget_soft);
double qxk24_oat_z3_balance(QXK24OATContext *ctx);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_OAT_H */
