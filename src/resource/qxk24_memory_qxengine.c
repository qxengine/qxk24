/* ============================================================
 * qxk24_memory_qxengine.c
 * QXK24 Universal Kernel - QXEngine Memory Governor Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/resource/qxk24_memory_qxengine.c
 * License : Apache 2.0
 *
 * Description:
 * Implements QXEngine as the K24ob memory governor inside QXK24.
 * ============================================================ */

#include "qxk24/resource/qxk24_memory_qxengine.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct QXK24QXEngineGovernor_s {
    QXK24QXEngineConfig config;
    QXK24QXEngineHealthInput health_input;
    QXK24ResourceGovernor *resource_handle;
    uint64_t bytes_used;
    uint64_t bytes_peak;
    uint32_t allocations_active;
    uint32_t allocations_total;
    uint32_t allocations_failed;
    uint32_t cycles_completed;
    uint32_t violations;
    uint64_t last_cycle_ms;
    bool running;
    float health_score;
};

static uint64_t now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static uint64_t canonical_budget(void) {
    return qxk24_ama_core_value((uint32_t)QXK24_QXENGINE_K24_LEVEL + 1U) *
           QXK24_BASE_UNIT_BYTES;
}

QXK24QXEngineConfig qxk24_qxengine_default_config(void) {
    QXK24QXEngineConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.total_memory_bytes = 4ULL * 1024ULL * 1024ULL * 1024ULL;
    cfg.budget_bytes = 256ULL * 1024ULL * 1024ULL;
    cfg.max_allocations = 1024U;
    cfg.pressure_threshold_70 = 0.70f;
    cfg.pressure_threshold_50 = 0.50f;
    cfg.pressure_threshold_25 = 0.25f;
    cfg.enable_health_feed = true;
    cfg.cycle_interval_ms = QXK24_CYCLE_INTERVAL_MS;
    return cfg;
}

static QXK24GovernorConfig resource_config(uint64_t budget) {
    QXK24GovernorConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.type = QXK24_RESOURCE_MEMORY;
    cfg.direction = QXK24_QXENGINE_DIRECTION;
    cfg.level = QXK24_QXENGINE_K24_LEVEL;
    cfg.budget_soft = budget;
    cfg.declared_x = QXK24_DEFAULT_DECLARED_X;
    cfg.tolerance_pct = QXK24_DEFAULT_TOLERANCE_PCT;
    strncpy(cfg.label, QXK24_QXENGINE_NAME, sizeof(cfg.label) - 1U);
    return cfg;
}

qxk24_qxengine_t qxk24_qxengine_create(const QXK24QXEngineConfig *config) {
    if (config == NULL || config->budget_bytes == 0ULL) return NULL;

    struct QXK24QXEngineGovernor_s *gov =
        (struct QXK24QXEngineGovernor_s *)calloc(1, sizeof(*gov));
    if (gov == NULL) return NULL;

    gov->config = *config;
    gov->last_cycle_ms = now_ms();
    gov->health_score = 0.0f;

    QXK24GovernorConfig rcfg = resource_config(canonical_budget());
    if (qxk24_governor_base_create(&rcfg, &gov->resource_handle) != QXK24_OK) {
        free(gov);
        return NULL;
    }
    return gov;
}

qxk24_err_t qxk24_qxengine_start(qxk24_qxengine_t gov) {
    if (gov == NULL) return QXK24_ERR_NULL_PARAM;
    gov->running = true;
    gov->last_cycle_ms = now_ms();
    return QXK24_OK;
}

void qxk24_qxengine_stop(qxk24_qxengine_t gov) {
    if (gov == NULL) return;
    gov->running = false;
}

void qxk24_qxengine_destroy(qxk24_qxengine_t gov) {
    if (gov == NULL) return;
    qxk24_governor_base_destroy(gov->resource_handle);
    memset(gov, 0, sizeof(*gov));
    free(gov);
}

qxk24_err_t qxk24_qxengine_observe_memory(qxk24_qxengine_t gov,
                                          uint64_t bytes_used,
                                          uint32_t active_allocations,
                                          uint32_t failed_allocations) {
    if (gov == NULL) return QXK24_ERR_NULL_PARAM;
    gov->bytes_used = bytes_used;
    if (bytes_used > gov->bytes_peak) gov->bytes_peak = bytes_used;
    gov->allocations_active = active_allocations;
    gov->allocations_total += active_allocations + failed_allocations;
    gov->allocations_failed += failed_allocations;
    if (bytes_used > gov->config.budget_bytes) gov->violations++;
    return QXK24_OK;
}

static void compute_health_input(qxk24_qxengine_t gov, uint64_t dt_ms) {
    const float budget = (float)gov->config.budget_bytes;
    const float used_ratio = budget > 0.0f
        ? (float)gov->bytes_used / budget
        : 0.0f;
    const float alloc_ratio = gov->config.max_allocations > 0U
        ? (float)gov->allocations_active / (float)gov->config.max_allocations
        : 0.0f;
    const float rate = (float)gov->bytes_used / (float)dt_ms;
    const float max_rate = budget / (float)gov->config.cycle_interval_ms;
    const uint32_t total = gov->allocations_total;

    gov->health_input.allocation_score =
        gov->allocations_active > 0U ? 1.0f : 0.0f;
    gov->health_input.pair_score = clamp01(1.0f - (used_ratio * 0.20f));
    gov->health_input.direction_score = used_ratio <= 1.0f ? 1.0f : 0.0f;
    gov->health_input.measurement_score =
        max_rate > 0.0f ? clamp01(1.0f - (rate / max_rate) * 0.10f) : 1.0f;
    gov->health_input.flow_score = total > 0U
        ? clamp01(1.0f - (float)gov->allocations_failed / (float)total)
        : 1.0f;
    gov->health_input.cross_score = clamp01(1.0f - (alloc_ratio * 0.10f));
    gov->health_input.cycle_score = gov->violations == 0U ? 1.0f :
        clamp01(1.0f - (float)gov->violations /
            (float)(gov->cycles_completed + 1U));
    gov->health_input.constitution_score = gov->violations == 0U ? 1.0f : 0.0f;
}

qxk24_err_t qxk24_qxengine_cycle(qxk24_qxengine_t gov) {
    if (gov == NULL) return QXK24_ERR_NULL_PARAM;
    if (!gov->running) return QXK24_ERR_NOT_STARTED;

    uint64_t current = now_ms();
    uint64_t dt_ms = current > gov->last_cycle_ms ? current - gov->last_cycle_ms : 1ULL;
    compute_health_input(gov, dt_ms);

    QXK24HealthInput input = {
        gov->health_input.allocation_score,
        gov->health_input.pair_score,
        gov->health_input.direction_score,
        gov->health_input.measurement_score,
        gov->health_input.flow_score,
        gov->health_input.cross_score,
        gov->health_input.cycle_score,
        gov->health_input.constitution_score
    };
    QXK24HealthResult result;
    qxk24_err_t err = qxk24_health_compute(&input, &result);
    if (err != QXK24_OK) return err;

    gov->health_score = result.score;
    gov->cycles_completed++;
    gov->last_cycle_ms = current;
    return qxk24_governor_base_update(gov->resource_handle,
        gov->bytes_used, QXK24_DEFAULT_DECLARED_X);
}

qxk24_err_t qxk24_qxengine_health_input(qxk24_qxengine_t gov,
                                        QXK24QXEngineHealthInput *out) {
    if (gov == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    *out = gov->health_input;
    return QXK24_OK;
}

float qxk24_qxengine_health_score(qxk24_qxengine_t gov) {
    return gov == NULL ? 0.0f : gov->health_score;
}

qxk24_resource_t qxk24_qxengine_as_resource(qxk24_qxengine_t gov) {
    return gov == NULL ? NULL : gov->resource_handle;
}

uint64_t qxk24_qxengine_bytes_used(qxk24_qxengine_t gov) {
    return gov == NULL ? 0ULL : gov->bytes_used;
}

uint64_t qxk24_qxengine_bytes_budget(qxk24_qxengine_t gov) {
    return gov == NULL ? 0ULL : gov->config.budget_bytes;
}

bool qxk24_qxengine_is_running(qxk24_qxengine_t gov) {
    return gov != NULL && gov->running;
}
