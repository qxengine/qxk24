/* ============================================================
 * qxk24_governor.h
 * QXK24 Universal Kernel — Governor Integration Header
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/kernel/qxk24_governor.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the registry that wires six resource governors into a single
 * Phase 3 constitutional health input.
 * ============================================================ */

#ifndef QXK24_GOVERNOR_H
#define QXK24_GOVERNOR_H

#include "qxk24/kernel/qxk24_health.h"
#include "qxk24/resource/qxk24_resource_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_MAX_GOVERNORS 6U

typedef struct QXK24GovernorSlot_s {
    QXK24Direction direction;
    QXK24ResourceGovernor *handle;
    bool active;
    char name[32];
    float last_law_score;
} QXK24GovernorSlot;

typedef struct QXK24GovernorRegistry_s {
    QXK24GovernorSlot slots[QXK24_MAX_GOVERNORS];
    uint32_t count;
} QXK24GovernorRegistry;

qxk24_err_t qxk24_governor_registry_init(QXK24GovernorRegistry *registry);
qxk24_err_t qxk24_governor_register(QXK24GovernorRegistry *registry,
                                    QXK24Direction direction,
                                    QXK24ResourceGovernor *handle,
                                    const char *name);
qxk24_err_t qxk24_governor_cycle_all(QXK24GovernorRegistry *registry,
                                     QXK24HealthInput *input);
QXK24GovernorSlot *qxk24_governor_find(QXK24GovernorRegistry *registry,
                                       QXK24Direction direction);
uint32_t qxk24_governor_active_count(const QXK24GovernorRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_GOVERNOR_H */
