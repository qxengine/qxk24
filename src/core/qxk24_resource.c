/* ============================================================
 * qxk24_resource.c
 * QXK24 Universal Kernel — Resource Registry
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/core/qxk24_resource.c
 * License : Apache 2.0
 *
 * Description:
 * Implements resource registration, resource measurement updates, and
 * resource release for governed memory, CPU, network, storage, I/O, energy.
 */

#include "core/qxk24_kernel_internal.h"
#include <string.h>

int32_t qxk24_resource_register(QXK24KernelHandle handle,
                                QXK24ResourceType type,
                                QXK24Direction direction,
                                QXK24Level level,
                                uint64_t budget_soft,
                                uint32_t *out_id) {
    if (handle == NULL || out_id == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)type > (uint32_t)QXK24_RESOURCE_ENERGY) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    if ((uint32_t)direction >= QXK24_DIRECTION_COUNT) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    if ((uint32_t)level >= QXK24_LEVEL_COUNT || budget_soft == 0ULL) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    if (k->resource_count >= QXK24_MAX_RESOURCES) {
        pthread_mutex_unlock(&k->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    uint32_t slot = QXK24_MAX_RESOURCES;
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        if (!k->resources[i].active) {
            slot = i;
            break;
        }
    }
    if (slot == QXK24_MAX_RESOURCES) {
        pthread_mutex_unlock(&k->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    QXK24Resource *r = &k->resources[slot];
    memset(r, 0, sizeof(*r));
    r->resource_id = k->next_resource_id++;
    r->type = type;
    r->direction = direction;
    r->level = level;
    r->budget_soft = budget_soft;
    r->budget_hard = qxk24_hard_budget(budget_soft);
    r->x_declared = k->config.declared_x;
    r->tolerance_pct = k->config.tolerance_pct;
    r->timestamp_registered_ms = qxk24_now_ms();
    r->timestamp_last_update_ms = r->timestamp_registered_ms;
    r->active = true;

    k->resource_count++;
    *out_id = r->resource_id;
    pthread_mutex_unlock(&k->mutex);
    return QXK24_OK;
}

int32_t qxk24_resource_update(QXK24KernelHandle handle,
                              uint32_t resource_id,
                              uint64_t used_bytes,
                              double x_measured) {
    if (handle == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (!r->active || r->resource_id != resource_id) continue;

        r->used_bytes = used_bytes;
        r->x_measured = x_measured;
        r->timestamp_last_update_ms = qxk24_now_ms();
        r->flow_count++;
        if (used_bytes > r->budget_hard) {
            r->violation_count++;
        }

        pthread_mutex_unlock(&k->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&k->mutex);
    return QXK24_ERR_NOT_FOUND;
}

int32_t qxk24_resource_release(QXK24KernelHandle handle,
                               uint32_t resource_id) {
    if (handle == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    for (uint32_t i = 0; i < QXK24_MAX_RESOURCES; i++) {
        QXK24Resource *r = &k->resources[i];
        if (!r->active || r->resource_id != resource_id) continue;

        memset(r, 0, sizeof(*r));
        if (k->resource_count > 0) k->resource_count--;
        pthread_mutex_unlock(&k->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&k->mutex);
    return QXK24_ERR_NOT_FOUND;
}
