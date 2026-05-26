/* ============================================================
 * qxk24_qxengine_bridge.h
 * QXK24 Universal Kernel - QXEngine Integration Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/bridge/qxk24_qxengine_bridge.h
 * License : Apache 2.0
 *
 * Description:
 * Wires QXEngine K24ob into QXK24 K24md as the memory governor.
 * ============================================================ */

#ifndef QXK24_QXENGINE_BRIDGE_H
#define QXK24_QXENGINE_BRIDGE_H

#include "qxk24/bridge/qxk24_qxengine_health.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum QXK24BridgeState_e {
    QXK24_BRIDGE_DETACHED = 0,
    QXK24_BRIDGE_ATTACHED = 1,
    QXK24_BRIDGE_ACTIVE = 2,
    QXK24_BRIDGE_FAULTED = 3
} QXK24BridgeState;

typedef struct QXK24BridgeSnapshot_s {
    QXK24BridgeState state;
    float qxengine_health;
    float kernel_health;
    float combined_health;
    uint64_t cycles_synced;
    bool constitutionally_bound;
} QXK24BridgeSnapshot;

typedef struct QXK24QXEngineBridge_s *qxk24_bridge_t;

qxk24_bridge_t qxk24_bridge_create(qxk24_kernel_t kernel,
                                   qxk24_qxengine_t gov);
qxk24_err_t qxk24_bridge_attach(qxk24_bridge_t bridge);
qxk24_err_t qxk24_bridge_cycle(qxk24_bridge_t bridge);
void qxk24_bridge_detach(qxk24_bridge_t bridge);
void qxk24_bridge_destroy(qxk24_bridge_t bridge);
qxk24_err_t qxk24_bridge_snapshot(qxk24_bridge_t bridge,
                                  QXK24BridgeSnapshot *out);
bool qxk24_bridge_is_bound(qxk24_bridge_t bridge);
QXK24BridgeState qxk24_bridge_state(qxk24_bridge_t bridge);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_QXENGINE_BRIDGE_H */
