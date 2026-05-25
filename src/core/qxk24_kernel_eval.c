/* ============================================================
 * qxk24_kernel_eval.c
 * QXK24 Universal Kernel — Eight-Law Evaluation
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/core/qxk24_kernel_eval.c
 * License : Apache 2.0
 *
 * Description:
 * Evaluates the eight constitutional laws, computes health score, maps
 * certification tiers, and provides monotonic timing helpers.
 */

#include "core/qxk24_kernel_internal.h"
#include <time.h>

uint64_t qxk24_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
           (uint64_t)ts.tv_nsec / 1000000ULL;
}

void qxk24_law_scores_init(QXK24LawScore *scores) {
    scores->z1_pattern = 1.0;
    scores->z2_limit = 1.0;
    scores->z3_pairs = 1.0;
    scores->z4_equilibrium = 1.0;
    scores->x1_knowledge = 1.0;
    scores->x2_ethics = 1.0;
    scores->x3_creativity = 1.0;
    scores->x4_economy = 1.0;
}

double qxk24_compute_health(const QXK24LawScore *s) {
    return (s->z1_pattern * QXK24_WEIGHT_Z1 +
            s->z2_limit * QXK24_WEIGHT_Z2 +
            s->z3_pairs * QXK24_WEIGHT_Z3 +
            s->z4_equilibrium * QXK24_WEIGHT_Z4 +
            s->x1_knowledge * QXK24_WEIGHT_X1 +
            s->x2_ethics * QXK24_WEIGHT_X2 +
            s->x3_creativity * QXK24_WEIGHT_X3 +
            s->x4_economy * QXK24_WEIGHT_X4) * 100.0;
}

QXK24CertTier qxk24_cert_from_score(double score) {
    if (score >= QXK24_SCORE_SOVEREIGN) return QXK24_CERT_SOVEREIGN;
    if (score >= QXK24_SCORE_PROFESSIONAL) return QXK24_CERT_PROFESSIONAL;
    if (score >= QXK24_SCORE_STANDARD) return QXK24_CERT_STANDARD;
    return QXK24_CERT_NONE;
}

static double eval_z1_pattern(QXK24Kernel *k) {
    if (k->resource_count == 0) return 1.0;

    uint32_t compliant = 0;
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (!r->active) continue;

        const uint32_t level = (uint32_t)r->level + 1U;
        const uint64_t expected = qxk24_ama_core_value(level) *
                                  QXK24_BASE_UNIT_BYTES;
        const uint64_t lo = expected / 2ULL;
        const uint64_t hi = expected * 2ULL;
        if (r->budget_soft >= lo && r->budget_soft <= hi) compliant++;
    }
    return (double)compliant / (double)k->resource_count;
}

static double eval_z2_limit(QXK24Kernel *k) {
    if (k->resource_count == 0) return 1.0;

    uint32_t within = 0;
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (r->active && r->used_bytes <= r->budget_soft) within++;
    }
    return (double)within / (double)k->resource_count;
}

static double eval_z3_pairs(QXK24Kernel *k) {
    uint64_t dir_used[QXK24_DIRECTION_COUNT] = {0};
    const QXK24Direction pairs[3][2] = {
        {QXK24_DIR_ATAS, QXK24_DIR_BAWAH},
        {QXK24_DIR_DEPAN, QXK24_DIR_BELAKANG},
        {QXK24_DIR_KANAN, QXK24_DIR_KIRI}
    };
    double score_sum = 0.0;

    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (r->active) dir_used[(uint32_t)r->direction] += r->used_bytes;
    }

    for (uint32_t p = 0; p < 3; p++) {
        const uint64_t a = dir_used[(uint32_t)pairs[p][0]];
        const uint64_t b = dir_used[(uint32_t)pairs[p][1]];
        const uint64_t total = a + b;
        const uint64_t diff = (a > b) ? (a - b) : (b - a);
        score_sum += (total == 0) ? 1.0 : 1.0 - ((double)diff / (double)total);
    }
    return score_sum / 3.0;
}

static double eval_z4_equilibrium(QXK24Kernel *k) {
    uint64_t total_soft = 0;
    uint64_t total_used = 0;

    if (k->resource_count == 0) return 1.0;
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (!r->active) continue;
        total_soft += r->budget_soft;
        total_used += r->used_bytes;
    }
    if (total_soft == 0) return 1.0;

    const double util = (double)total_used / (double)total_soft;
    if (util <= 0.40) return util / 0.40;
    if (util <= 0.70) return 1.0;
    if (util >= 1.0) return 0.0;
    return 1.0 - ((util - 0.70) / 0.30);
}

static double eval_x1_knowledge(QXK24Kernel *k) {
    if (k->resource_count == 0) return 1.0;

    const uint64_t now = qxk24_now_ms();
    const uint64_t window = QXK24_CYCLE_INTERVAL_MS * 2ULL;
    uint32_t aware = 0;
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (r->active && now - r->timestamp_last_update_ms <= window) aware++;
    }
    return (double)aware / (double)k->resource_count;
}

static double eval_x2_ethics(QXK24Kernel *k) {
    uint64_t total_flows = 0;
    uint64_t total_violations = 0;

    if (k->resource_count == 0) return 1.0;
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (!r->active) continue;
        total_flows += r->flow_count;
        total_violations += r->violation_count;
    }
    if (total_flows == 0) return 1.0;

    const double rate = (double)total_violations / (double)total_flows;
    return (rate >= 1.0) ? 0.0 : 1.0 - rate;
}

static double eval_x3_creativity(QXK24Kernel *k) {
    bool types_seen[4] = {false, false, false, false};
    uint32_t distinct = 0;

    for (uint32_t i = 0; i < QXK24_MAX_ACTIVE_FLOWS; i++) {
        QXK24Flow *f = &k->flows[i];
        if (!f->active && !f->completed) continue;
        if ((uint32_t)f->type < 4U) types_seen[(uint32_t)f->type] = true;
    }
    for (uint32_t t = 0; t < 4U; t++) {
        if (types_seen[t]) distinct++;
    }
    return (k->flow_count == 0) ? 0.5 : (double)distinct / 4.0;
}

static double eval_x4_economy(QXK24Kernel *k) {
    uint64_t total_hard = 0;
    uint64_t total_used = 0;

    if (k->resource_count == 0) return 1.0;
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (!r->active) continue;
        total_hard += r->budget_hard;
        total_used += r->used_bytes;
    }
    if (total_hard == 0) return 1.0;

    const double util = (double)total_used / (double)total_hard;
    if (util <= 0.85) return 1.0;
    if (util >= 1.0) return 0.0;
    return 1.0 - ((util - 0.85) / 0.15);
}

void qxk24_evaluate_laws(QXK24Kernel *k) {
    k->law_scores.z1_pattern = eval_z1_pattern(k);
    k->law_scores.z2_limit = eval_z2_limit(k);
    k->law_scores.z3_pairs = eval_z3_pairs(k);
    k->law_scores.z4_equilibrium = eval_z4_equilibrium(k);
    k->law_scores.x1_knowledge = eval_x1_knowledge(k);
    k->law_scores.x2_ethics = eval_x2_ethics(k);
    k->law_scores.x3_creativity = eval_x3_creativity(k);
    k->law_scores.x4_economy = eval_x4_economy(k);
}
