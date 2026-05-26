/* ============================================================
 * qxk24_qxengine_bridge.c
 * QXK24 Universal Kernel - QXEngine Integration Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/bridge/qxk24_qxengine_bridge.c
 * License : Apache 2.0
 *
 * Description:
 * Synchronizes QXEngine K24ob memory health into the QXK24 kernel cycle.
 * ============================================================ */

#include "qxk24/bridge/qxk24_qxengine_bridge.h"
#include <stdlib.h>
#include <string.h>

struct QXK24QXEngineBridge_s {
    qxk24_kernel_t kernel;
    qxk24_qxengine_t gov;
    QXK24BridgeState state;
    uint32_t kernel_resource_id;
    uint64_t cycles_synced;
    float last_qxengine_health;
    float last_kernel_health;
};

static uint64_t kernel_budget(qxk24_bridge_t bridge) {
    qxk24_resource_t res = qxk24_qxengine_as_resource(bridge->gov);
    return res != NULL ? res->budget_soft : qxk24_level_budget(QXK24_LEVEL_OBJEK);
}

static uint64_t kernel_used(qxk24_bridge_t bridge) {
    const uint64_t observed = qxk24_qxengine_bytes_used(bridge->gov);
    return observed > 0ULL ? observed : kernel_budget(bridge) / 2ULL;
}

qxk24_bridge_t qxk24_bridge_create(qxk24_kernel_t kernel,
                                   qxk24_qxengine_t gov) {
    if (kernel == NULL || gov == NULL) return NULL;

    struct QXK24QXEngineBridge_s *bridge =
        (struct QXK24QXEngineBridge_s *)calloc(1, sizeof(*bridge));
    if (bridge == NULL) return NULL;
    bridge->kernel = kernel;
    bridge->gov = gov;
    bridge->state = QXK24_BRIDGE_DETACHED;
    return bridge;
}

qxk24_err_t qxk24_bridge_attach(qxk24_bridge_t bridge) {
    if (bridge == NULL) return QXK24_ERR_NULL_PARAM;
    if (bridge->state == QXK24_BRIDGE_ATTACHED ||
        bridge->state == QXK24_BRIDGE_ACTIVE) {
        return QXK24_OK;
    }

    qxk24_resource_t res = qxk24_qxengine_as_resource(bridge->gov);
    if (res == NULL) {
        bridge->state = QXK24_BRIDGE_FAULTED;
        return QXK24_ERR_NULL_PARAM;
    }

    qxk24_err_t err = qxk24_resource_register(bridge->kernel,
        QXK24_RESOURCE_MEMORY, QXK24_QXENGINE_DIRECTION,
        QXK24_QXENGINE_K24_LEVEL, res->budget_soft,
        &bridge->kernel_resource_id);
    if (err != QXK24_OK) {
        bridge->state = QXK24_BRIDGE_FAULTED;
        return err;
    }
    bridge->state = QXK24_BRIDGE_ATTACHED;
    return QXK24_OK;
}

qxk24_err_t qxk24_bridge_cycle(qxk24_bridge_t bridge) {
    if (bridge == NULL) return QXK24_ERR_NULL_PARAM;
    if (bridge->state == QXK24_BRIDGE_FAULTED) return QXK24_ERR_FAULT;

    qxk24_err_t err = qxk24_bridge_attach(bridge);
    if (err != QXK24_OK) return err;

    err = qxk24_qxengine_cycle(bridge->gov);
    if (err != QXK24_OK) {
        bridge->state = QXK24_BRIDGE_FAULTED;
        return err;
    }
    err = qxk24_resource_update(bridge->kernel, bridge->kernel_resource_id,
        kernel_used(bridge), QXK24_DEFAULT_DECLARED_X);
    if (err != QXK24_OK) {
        bridge->state = QXK24_BRIDGE_FAULTED;
        return err;
    }
    err = qxk24_cycle(bridge->kernel, NULL);
    if (err != QXK24_OK) {
        bridge->state = QXK24_BRIDGE_FAULTED;
        return err;
    }

    bridge->last_qxengine_health = qxk24_qxengine_health_score(bridge->gov);
    bridge->last_kernel_health = (float)qxk24_health_score(bridge->kernel);
    bridge->cycles_synced++;
    bridge->state = QXK24_BRIDGE_ACTIVE;
    return QXK24_OK;
}

void qxk24_bridge_detach(qxk24_bridge_t bridge) {
    if (bridge == NULL) return;
    bridge->state = QXK24_BRIDGE_DETACHED;
}

void qxk24_bridge_destroy(qxk24_bridge_t bridge) {
    if (bridge == NULL) return;
    memset(bridge, 0, sizeof(*bridge));
    free(bridge);
}

qxk24_err_t qxk24_bridge_snapshot(qxk24_bridge_t bridge,
                                  QXK24BridgeSnapshot *out) {
    if (bridge == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;

    out->state = bridge->state;
    out->qxengine_health = bridge->last_qxengine_health;
    out->kernel_health = bridge->last_kernel_health;
    out->combined_health =
        bridge->last_kernel_health * QXK24_KERNEL_HEALTH_WEIGHT +
        bridge->last_qxengine_health * QXK24_QXENGINE_HEALTH_WEIGHT;
    out->cycles_synced = bridge->cycles_synced;
    out->constitutionally_bound =
        bridge->state == QXK24_BRIDGE_ACTIVE &&
        out->combined_health >= QXK24_HEALTH_PROFESSIONAL;
    return QXK24_OK;
}

bool qxk24_bridge_is_bound(qxk24_bridge_t bridge) {
    if (bridge == NULL) return false;
    QXK24BridgeSnapshot snapshot;
    if (qxk24_bridge_snapshot(bridge, &snapshot) != QXK24_OK) return false;
    return snapshot.constitutionally_bound;
}

QXK24BridgeState qxk24_bridge_state(qxk24_bridge_t bridge) {
    return bridge == NULL ? QXK24_BRIDGE_FAULTED : bridge->state;
}
