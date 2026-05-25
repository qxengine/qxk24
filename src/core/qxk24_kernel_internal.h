/* ============================================================
 * qxk24_kernel_internal.h
 * QXK24 Universal Kernel — Internal Kernel State
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/core/qxk24_kernel_internal.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the private kernel structure and shared internal helpers used
 * across lifecycle, evaluation, resource, flow, and query modules.
 */

#ifndef QXK24_KERNEL_INTERNAL_H
#define QXK24_KERNEL_INTERNAL_H

#include "qxk24/qxk24.h"
#include <pthread.h>

typedef struct QXK24Kernel_s {
    char label[QXK24_MAX_LABEL_LEN];
    uint32_t version;
    QXK24Config config;
    QXK24Resource resources[QXK24_MAX_RESOURCES];
    uint32_t resource_count;
    uint32_t next_resource_id;
    QXK24Flow flows[QXK24_MAX_ACTIVE_FLOWS];
    uint32_t flow_count;
    uint64_t next_flow_id;
    QXK24LawScore law_scores;
    double health_score;
    QXK24CertTier cert_tier;
    uint64_t cycle_index;
    int32_t pressure_tier;
    bool foreground;
    pthread_mutex_t mutex;
    bool initialized;
    bool started;
} QXK24Kernel;

uint64_t qxk24_now_ms(void);
void qxk24_law_scores_init(QXK24LawScore *scores);
void qxk24_evaluate_laws(QXK24Kernel *kernel);
double qxk24_compute_health(const QXK24LawScore *scores);
QXK24CertTier qxk24_cert_from_score(double score);
uint32_t qxk24_count_active_flows(const QXK24Kernel *kernel);

#endif /* QXK24_KERNEL_INTERNAL_H */
