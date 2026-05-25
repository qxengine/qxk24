/* ============================================================
 * qxk24_resource_base.h
 * QXK24 Universal Kernel — Resource Governor Base
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/resource/qxk24_resource_base.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the shared constitutional base for all six resource governors:
 * memory, CPU, network, storage, I/O, and energy.
 * ============================================================ */

#ifndef QXK24_RESOURCE_BASE_H
#define QXK24_RESOURCE_BASE_H

#include "qxk24/flow/qxk24_oass.h"
#include "qxk24/flow/qxk24_oat.h"
#include "qxk24/qxk24.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QXK24ResourceGovernor_s {
    QXK24ResourceType type;
    QXK24Direction direction;
    QXK24Level level;
    char label[64];
    uint64_t budget_soft;
    uint64_t budget_hard;
    uint64_t used;
    uint64_t peak;
    double x_declared;
    double x_measured;
    double x_tolerance;
    uint64_t measure_count;
    QXK24OATContext *oat;
    QXK24OASSContext *oass;
    QXK24LawScore scores;
    int32_t pressure_tier;
    double pressure_scale;
    uint64_t timestamp_created_ms;
    uint64_t timestamp_last_cycle_ms;
    uint64_t cycle_count;
    bool active;
    pthread_mutex_t mutex;
    bool initialized;
} QXK24ResourceGovernor;

typedef struct QXK24GovernorConfig_s {
    QXK24ResourceType type;
    QXK24Direction direction;
    QXK24Level level;
    uint64_t budget_soft;
    double declared_x;
    double tolerance_pct;
    char label[64];
} QXK24GovernorConfig;

typedef struct QXK24GovernorReport_s {
    QXK24ResourceType type;
    QXK24Direction direction;
    QXK24Level level;
    uint64_t used;
    uint64_t budget_soft;
    uint64_t budget_hard;
    double utilisation;
    double x_measured;
    double x_declared;
    QXK24LawScore scores;
    double health;
    uint64_t cycle_count;
    uint32_t active_oat_flows;
    uint32_t active_oass_flows;
} QXK24GovernorReport;

int32_t qxk24_governor_base_create(const QXK24GovernorConfig *config,
                                   QXK24ResourceGovernor **out_gov);
void qxk24_governor_base_destroy(QXK24ResourceGovernor *gov);
int32_t qxk24_governor_base_update(QXK24ResourceGovernor *gov,
                                   uint64_t used,
                                   double x_measured);
int32_t qxk24_governor_base_cycle(QXK24ResourceGovernor *gov,
                                  QXK24GovernorReport *out_report);
void qxk24_governor_base_pressure(QXK24ResourceGovernor *gov, int32_t tier);

double qxk24_governor_measure_x(uint64_t m_bytes, uint64_t t_ms);
uint64_t qxk24_governor_auto_budget(QXK24Level level,
                                    QXK24Direction direction);

int32_t qxk24_memory_governor_create(QXK24Level level,
                                     uint64_t budget_soft,
                                     QXK24ResourceGovernor **out);
int32_t qxk24_cpu_governor_create(QXK24Level level,
                                  uint64_t budget_ms,
                                  QXK24ResourceGovernor **out);
int32_t qxk24_network_governor_create(QXK24Level level,
                                      uint64_t egress_budget,
                                      uint64_t ingress_budget,
                                      QXK24ResourceGovernor **out_kn,
                                      QXK24ResourceGovernor **out_kr);
int32_t qxk24_storage_governor_create(QXK24Level level,
                                      uint64_t budget_bytes,
                                      QXK24ResourceGovernor **out);
int32_t qxk24_io_governor_create(QXK24Level level,
                                 uint64_t budget_ops,
                                 QXK24ResourceGovernor **out);
int32_t qxk24_energy_governor_create(QXK24Level level,
                                     uint64_t budget_mj,
                                     QXK24ResourceGovernor **out);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_RESOURCE_BASE_H */
