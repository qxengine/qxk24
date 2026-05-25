/* ============================================================
 * qxk24_report.c
 * QXK24 Universal Kernel — Constitutional Report Generator
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/kernel/qxk24_report.c
 * License : Apache 2.0
 *
 * Description:
 * Generates Phase 3 constitutional report snapshots from live kernel state.
 * ============================================================ */

#include "qxk24/kernel/qxk24_report.h"
#include <stdio.h>

static const char *s_law_tags[8] = {
    "Z.1", "Z.2", "Z.3", "Z.4",
    "X.1", "X.2", "X.3", "X.4"
};

static const char *tier_label(QXK24CertTier tier) {
    switch (tier) {
    case QXK24_CERT_SOVEREIGN: return "SOVEREIGN";
    case QXK24_CERT_PROFESSIONAL: return "PROFESSIONAL";
    case QXK24_CERT_STANDARD: return "STANDARD";
    case QXK24_CERT_NONE:
    default: return "NONE";
    }
}

static QXK24HealthInput law_scores_to_input(const QXK24LawScore *scores) {
    QXK24HealthInput input;
    input.z1 = (float)scores->z1_pattern;
    input.z2 = (float)scores->z2_limit;
    input.z3 = (float)scores->z3_pairs;
    input.z4 = (float)scores->z4_equilibrium;
    input.x1 = (float)scores->x1_knowledge;
    input.x2 = (float)scores->x2_ethics;
    input.x3 = (float)scores->x3_creativity;
    input.x4 = (float)scores->x4_economy;
    return input;
}

const char *qxk24_report_violation_tag(uint8_t law_index) {
    return law_index < 8U ? s_law_tags[law_index] : "UNKNOWN";
}

bool qxk24_report_is_sound(const QXK24Report *report) {
    if (report == NULL) return false;
    return report->health_score >= QXK24_HEALTH_STANDARD &&
           report->cert_tier >= QXK24_CERT_STANDARD;
}

qxk24_err_t qxk24_report_generate(QXK24KernelHandle kernel, QXK24Report *out) {
    if (kernel == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;

    QXK24LawScore scores;
    qxk24_err_t rc = qxk24_get_law_scores(kernel, &scores);
    if (rc != QXK24_OK) return rc;

    QXK24HealthInput input = law_scores_to_input(&scores);
    QXK24HealthResult health;
    rc = qxk24_health_compute(&input, &health);
    if (rc != QXK24_OK) return rc;

    out->cycle_index = qxk24_cycle_index(kernel);
    out->timestamp_ms = health.timestamp_ms;
    out->law_scores = scores;
    out->health_score = health.score;
    out->cert_tier = health.tier;
    out->active_resources = qxk24_active_resources(kernel);
    out->active_flows = qxk24_active_flows(kernel);
    out->total_bytes_governed = 0ULL;
    out->global_utilisation = 0.0;
    snprintf(out->log, QXK24_MAX_LOG_LEN,
        "QXK24 Constitutional Report | Cycle %llu | Health %.1f | %s | "
        "Z1:%.2f Z2:%.2f Z3:%.2f Z4:%.2f | "
        "X1:%.2f X2:%.2f X3:%.2f X4:%.2f | Resources:%u Flows:%u",
        (unsigned long long)out->cycle_index,
        out->health_score,
        tier_label(out->cert_tier),
        out->law_scores.z1_pattern,
        out->law_scores.z2_limit,
        out->law_scores.z3_pairs,
        out->law_scores.z4_equilibrium,
        out->law_scores.x1_knowledge,
        out->law_scores.x2_ethics,
        out->law_scores.x3_creativity,
        out->law_scores.x4_economy,
        out->active_resources,
        out->active_flows);
    return QXK24_OK;
}

void qxk24_report_print(const QXK24Report *report) {
    if (report == NULL) return;
    printf("\n=== QXK24 Constitutional Report ===\n");
    printf("Cycle       : %llu\n", (unsigned long long)report->cycle_index);
    printf("Health      : %.2f (%s)\n",
           report->health_score, tier_label(report->cert_tier));
    printf("Resources   : %u active\n", report->active_resources);
    printf("Flows       : %u active\n", report->active_flows);
    printf("Sound       : %s\n", qxk24_report_is_sound(report) ? "YES" : "NO");
    printf("===================================\n\n");
}
