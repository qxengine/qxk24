/* ============================================================
 * qxk24_resource_base.c
 * QXK24 Universal Kernel — Resource Governor Base Implementation
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/resource/qxk24_resource_base.c
 * License : Apache 2.0
 *
 * Description:
 * Implements the shared governor lifecycle, AMA budget selection, X = m/t,
 * pressure scaling, and eight-law cycle scoring.
 * ============================================================ */

#include "qxk24/resource/qxk24_resource_base.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint64_t gov_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

static void init_scores(QXK24LawScore *scores) {
    scores->z1_pattern = 1.0;
    scores->z2_limit = 1.0;
    scores->z3_pairs = 1.0;
    scores->z4_equilibrium = 1.0;
    scores->x1_knowledge = 1.0;
    scores->x2_ethics = 1.0;
    scores->x3_creativity = 1.0;
    scores->x4_economy = 1.0;
}

static double clamp01(double value) {
    if (value < 0.0) return 0.0;
    if (value > 1.0) return 1.0;
    return value;
}

static double governor_health(const QXK24LawScore *s) {
    return (s->z1_pattern * QXK24_WEIGHT_Z1 +
            s->z2_limit * QXK24_WEIGHT_Z2 +
            s->z3_pairs * QXK24_WEIGHT_Z3 +
            s->z4_equilibrium * QXK24_WEIGHT_Z4 +
            s->x1_knowledge * QXK24_WEIGHT_X1 +
            s->x2_ethics * QXK24_WEIGHT_X2 +
            s->x3_creativity * QXK24_WEIGHT_X3 +
            s->x4_economy * QXK24_WEIGHT_X4) * 100.0;
}

double qxk24_governor_measure_x(uint64_t m_bytes, uint64_t t_ms) {
    if (t_ms == 0ULL) t_ms = 1ULL;
    return (double)m_bytes / ((double)t_ms / 1000.0);
}

uint64_t qxk24_governor_auto_budget(QXK24Level level,
                                    QXK24Direction direction) {
    static const uint32_t weights[QXK24_DIRECTION_COUNT] = {10, 15, 25, 25, 15, 10};
    const uint64_t base = qxk24_level_budget(level);

    if ((uint32_t)direction >= QXK24_DIRECTION_COUNT) return base;
    if (direction == QXK24_DIR_KIRI) {
        uint64_t first_five = 0;
        for (uint32_t i = 0; i < 5U; i++) {
            first_five += (base * weights[i]) / 100ULL;
        }
        return base - first_five;
    }
    return (base * weights[(uint32_t)direction]) / 100ULL;
}

int32_t qxk24_governor_base_create(const QXK24GovernorConfig *cfg,
                                   QXK24ResourceGovernor **out_gov) {
    if (cfg == NULL || out_gov == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)cfg->type > (uint32_t)QXK24_RESOURCE_ENERGY) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    if ((uint32_t)cfg->direction >= QXK24_DIRECTION_COUNT ||
        (uint32_t)cfg->level >= QXK24_LEVEL_COUNT ||
        cfg->declared_x <= 0.0) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    QXK24ResourceGovernor *gov =
        (QXK24ResourceGovernor *)calloc(1, sizeof(QXK24ResourceGovernor));
    if (gov == NULL) return QXK24_ERR_OUT_OF_MEMORY;

    gov->type = cfg->type;
    gov->direction = cfg->direction;
    gov->level = cfg->level;
    gov->x_declared = cfg->declared_x;
    gov->x_tolerance = cfg->tolerance_pct / 100.0;
    gov->pressure_scale = 1.0;
    gov->budget_soft = cfg->budget_soft > 0ULL
        ? cfg->budget_soft
        : qxk24_governor_auto_budget(cfg->level, cfg->direction);
    gov->budget_hard = qxk24_hard_budget(gov->budget_soft);
    strncpy(gov->label, cfg->label, sizeof(gov->label) - 1U);
    init_scores(&gov->scores);

    int32_t rc = qxk24_oat_context_create(&gov->oat);
    if (rc != QXK24_OK) {
        free(gov);
        return rc;
    }
    rc = qxk24_oass_context_create(&gov->oass);
    if (rc != QXK24_OK) {
        qxk24_oat_context_destroy(gov->oat);
        free(gov);
        return rc;
    }
    if (pthread_mutex_init(&gov->mutex, NULL) != 0) {
        qxk24_oat_context_destroy(gov->oat);
        qxk24_oass_context_destroy(gov->oass);
        free(gov);
        return QXK24_ERR_OUT_OF_MEMORY;
    }

    gov->timestamp_created_ms = gov_now_ms();
    gov->initialized = true;
    gov->active = true;
    *out_gov = gov;
    return QXK24_OK;
}

void qxk24_governor_base_destroy(QXK24ResourceGovernor *gov) {
    if (gov == NULL) return;
    qxk24_oat_context_destroy(gov->oat);
    qxk24_oass_context_destroy(gov->oass);
    pthread_mutex_destroy(&gov->mutex);
    memset(gov, 0, sizeof(*gov));
    free(gov);
}

int32_t qxk24_governor_base_update(QXK24ResourceGovernor *gov,
                                   uint64_t used,
                                   double x_measured) {
    if (gov == NULL || !gov->initialized) return QXK24_ERR_NOT_INITIALIZED;

    pthread_mutex_lock(&gov->mutex);
    gov->used = used;
    gov->x_measured = x_measured;
    gov->measure_count++;
    if (used > gov->peak) gov->peak = used;
    pthread_mutex_unlock(&gov->mutex);
    return QXK24_OK;
}

static void evaluate_governor(QXK24ResourceGovernor *gov) {
    const uint64_t soft = (uint64_t)((double)gov->budget_soft * gov->pressure_scale);
    gov->scores.z1_pattern = qxk24_oat_z1_score(gov->oat);
    gov->scores.z2_limit = soft == 0ULL || gov->used <= soft
        ? 1.0
        : 1.0 - ((double)(gov->used - soft) / (double)soft);
    gov->scores.z3_pairs = 0.5 * qxk24_oat_z3_balance(gov->oat) +
                           0.5 * qxk24_oass_z3_score(gov->oass);
    gov->scores.z4_equilibrium = qxk24_oass_z4_score(gov->oass);

    if (gov->x_declared > 0.0) {
        const double tolerance = gov->x_tolerance > 0.0 ? gov->x_tolerance : 0.01;
        const double drift = fabs(gov->x_measured - gov->x_declared) /
                             gov->x_declared;
        gov->scores.x1_knowledge = drift <= tolerance
            ? 1.0
            : 1.0 - ((drift - tolerance) / tolerance);
    }
    gov->scores.x2_ethics = qxk24_oass_x2_score(gov->oass);
    gov->scores.x3_creativity = qxk24_oat_z2_score(gov->oat, gov->budget_soft);

    if (gov->budget_hard > 0ULL) {
        const double util = (double)gov->used / (double)gov->budget_hard;
        gov->scores.x4_economy = util <= 0.85 ? 1.0 : 1.0 - ((util - 0.85) / 0.15);
    }

    gov->scores.z2_limit = clamp01(gov->scores.z2_limit);
    gov->scores.x1_knowledge = clamp01(gov->scores.x1_knowledge);
    gov->scores.x4_economy = clamp01(gov->scores.x4_economy);
}

int32_t qxk24_governor_base_cycle(QXK24ResourceGovernor *gov,
                                  QXK24GovernorReport *out_report) {
    if (gov == NULL || !gov->initialized) return QXK24_ERR_NOT_INITIALIZED;

    pthread_mutex_lock(&gov->mutex);
    gov->cycle_count++;
    gov->timestamp_last_cycle_ms = gov_now_ms();
    evaluate_governor(gov);

    if (out_report != NULL) {
        out_report->type = gov->type;
        out_report->direction = gov->direction;
        out_report->level = gov->level;
        out_report->used = gov->used;
        out_report->budget_soft = gov->budget_soft;
        out_report->budget_hard = gov->budget_hard;
        out_report->utilisation = gov->budget_soft > 0ULL
            ? (double)gov->used / (double)gov->budget_soft
            : 0.0;
        out_report->x_measured = gov->x_measured;
        out_report->x_declared = gov->x_declared;
        out_report->scores = gov->scores;
        out_report->health = governor_health(&gov->scores);
        out_report->cycle_count = gov->cycle_count;
        out_report->active_oat_flows = gov->oat->flow_count;
        out_report->active_oass_flows = gov->oass->flow_count;
    }

    pthread_mutex_unlock(&gov->mutex);
    return QXK24_OK;
}

void qxk24_governor_base_pressure(QXK24ResourceGovernor *gov, int32_t tier) {
    if (gov == NULL) return;

    pthread_mutex_lock(&gov->mutex);
    gov->pressure_tier = tier;
    switch (tier) {
    case 1: gov->pressure_scale = 0.70; break;
    case 2: gov->pressure_scale = 0.50; break;
    case 3: gov->pressure_scale = 0.25; break;
    case 4: gov->pressure_scale = 0.10; break;
    default: gov->pressure_scale = 1.00; break;
    }
    pthread_mutex_unlock(&gov->mutex);
}
