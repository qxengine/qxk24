/* ============================================================
 * qxk24_oass.h
 * QXK24 Universal Kernel — OASS Cross Flow Header
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/flow/qxk24_oass.h
 * License : Apache 2.0
 *
 * Description:
 * Declares Operasi Aliran Silang Sa (OASS), the cross-direction flow
 * model and ABA tracker for complete constitutional cycles.
 * ============================================================ */

#ifndef QXK24_OASS_H
#define QXK24_OASS_H

#include "qxk24/qxk24.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_OASS_MAX_FLOWS 256U

typedef struct QXK24OASSFlow_s {
    uint64_t flow_id;
    QXK24FlowType type;
    QXK24FlowDir direction;
    QXK24Direction src_segment;
    QXK24Direction dst_segment;
    QXK24Level src_level;
    QXK24Level dst_level;
    uint32_t src_resource_id;
    uint32_t dst_resource_id;
    uint64_t bytes;
    uint64_t timestamp_start_ms;
    uint64_t timestamp_end_ms;
    double x_src;
    double x_dst;
    bool active;
    bool completed;
    bool cross_valid;
    bool z4_compliant;
} QXK24OASSFlow;

typedef struct QXK24OASSContext_s {
    QXK24OASSFlow flows[QXK24_OASS_MAX_FLOWS];
    uint32_t flow_count;
    uint64_t next_flow_id;
    uint64_t bytes_sent[QXK24_DIRECTION_COUNT];
    uint64_t bytes_recv[QXK24_DIRECTION_COUNT];
    uint64_t aba_cycles_complete;
    uint64_t aba_cycles_broken;
    uint32_t violation_count;
    pthread_mutex_t mutex;
    bool initialized;
} QXK24OASSContext;

typedef struct QXK24OASSMeasure_s {
    double x_src;
    double x_dst;
    double cross_ratio;
    bool equilibrium;
    bool pairs_valid;
    bool aba_complete;
    uint64_t duration_ms;
} QXK24OASSMeasure;

typedef enum QXK24ABAState_e {
    QXK24_ABA_IDLE = 0,
    QXK24_ABA_PHASE_1 = 1,
    QXK24_ABA_PHASE_2 = 2,
    QXK24_ABA_PHASE_4 = 3,
    QXK24_ABA_RETURN_2 = 4,
    QXK24_ABA_RETURN_1 = 5,
    QXK24_ABA_COMPLETE = 6
} QXK24ABAState;

typedef struct QXK24ABATracker_s {
    QXK24ABAState state;
    uint64_t flow_id_ikj;
    uint64_t flow_id_lwj;
    uint64_t bytes_out;
    uint64_t bytes_in;
    uint64_t timestamp_start_ms;
    uint64_t timestamp_peak_ms;
    uint64_t timestamp_end_ms;
    bool valid;
} QXK24ABATracker;

int32_t qxk24_oass_context_create(QXK24OASSContext **out_ctx);
void qxk24_oass_context_destroy(QXK24OASSContext *ctx);

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
                         uint64_t *out_flow_id);

int32_t qxk24_oass_complete(QXK24OASSContext *ctx,
                            uint64_t flow_id,
                            QXK24OASSMeasure *out_measure);
int32_t qxk24_oass_cancel(QXK24OASSContext *ctx, uint64_t flow_id);

int32_t qxk24_oass_aba_begin(QXK24OASSContext *ctx,
                             uint64_t ikj_flow_id,
                             QXK24ABATracker *out_tracker);
int32_t qxk24_oass_aba_complete(QXK24OASSContext *ctx,
                                QXK24ABATracker *tracker,
                                uint64_t lwj_flow_id);

double qxk24_oass_z3_score(QXK24OASSContext *ctx);
double qxk24_oass_z4_score(QXK24OASSContext *ctx);
double qxk24_oass_x2_score(QXK24OASSContext *ctx);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_OASS_H */
