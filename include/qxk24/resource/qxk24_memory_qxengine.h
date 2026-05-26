/* ============================================================
 * qxk24_memory_qxengine.h
 * QXK24 Universal Kernel - QXEngine Memory Governor Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/resource/qxk24_memory_qxengine.h
 * License : Apache 2.0
 *
 * Description:
 * Declares QXEngine as the K24ob memory governor feeding QXK24 health.
 * ============================================================ */

#ifndef QXK24_MEMORY_QXENGINE_H
#define QXK24_MEMORY_QXENGINE_H

#include "qxk24/kernel/qxk24_health.h"
#include "qxk24/resource/qxk24_resource_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_QXENGINE_K24_LEVEL QXK24_LEVEL_OBJEK
#define QXK24_QXENGINE_DIRECTION QXK24_DIR_AS
#define QXK24_QXENGINE_NAME "QXEngine-K24ob"

typedef QXK24ResourceGovernor *qxk24_resource_t;

typedef struct QXK24QXEngineHealthInput_s {
    float allocation_score;
    float pair_score;
    float direction_score;
    float measurement_score;
    float flow_score;
    float cross_score;
    float cycle_score;
    float constitution_score;
} QXK24QXEngineHealthInput;

typedef struct QXK24QXEngineConfig_s {
    uint64_t total_memory_bytes;
    uint64_t budget_bytes;
    uint32_t max_allocations;
    float pressure_threshold_70;
    float pressure_threshold_50;
    float pressure_threshold_25;
    bool enable_health_feed;
    uint32_t cycle_interval_ms;
} QXK24QXEngineConfig;

typedef struct QXK24QXEngineGovernor_s *qxk24_qxengine_t;

QXK24QXEngineConfig qxk24_qxengine_default_config(void);
qxk24_qxengine_t qxk24_qxengine_create(const QXK24QXEngineConfig *config);
qxk24_err_t qxk24_qxengine_start(qxk24_qxengine_t gov);
void qxk24_qxengine_stop(qxk24_qxengine_t gov);
void qxk24_qxengine_destroy(qxk24_qxengine_t gov);
qxk24_err_t qxk24_qxengine_cycle(qxk24_qxengine_t gov);
qxk24_err_t qxk24_qxengine_health_input(qxk24_qxengine_t gov,
                                        QXK24QXEngineHealthInput *out);
float qxk24_qxengine_health_score(qxk24_qxengine_t gov);
qxk24_resource_t qxk24_qxengine_as_resource(qxk24_qxengine_t gov);
uint64_t qxk24_qxengine_bytes_used(qxk24_qxengine_t gov);
uint64_t qxk24_qxengine_bytes_budget(qxk24_qxengine_t gov);
bool qxk24_qxengine_is_running(qxk24_qxengine_t gov);

/* Test and adapter hook: update observed memory without exposing allocators. */
qxk24_err_t qxk24_qxengine_observe_memory(qxk24_qxengine_t gov,
                                          uint64_t bytes_used,
                                          uint32_t active_allocations,
                                          uint32_t failed_allocations);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_MEMORY_QXENGINE_H */
