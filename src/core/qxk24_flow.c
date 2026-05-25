/* ============================================================
 * qxk24_flow.c
 * QXK24 Universal Kernel — Flow Registry
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/core/qxk24_flow.c
 * License : Apache 2.0
 *
 * Description:
 * Implements OAT/OASS flow lifecycle operations: begin, complete, and cancel,
 * preserving flow metadata for constitutional cycle evaluation.
 */

#include "core/qxk24_kernel_internal.h"
#include <string.h>

int32_t qxk24_flow_begin(QXK24KernelHandle handle,
                         QXK24FlowType type,
                         QXK24FlowDir dir,
                         uint32_t src_id,
                         uint32_t dst_id,
                         uint64_t bytes,
                         uint64_t *out_flow_id) {
    if (handle == NULL || out_flow_id == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)type > (uint32_t)QXK24_FLOW_OASS_PUSINGAN) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    if ((uint32_t)dir > (uint32_t)QXK24_FLOW_LWJ || bytes == 0ULL) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    if (k->flow_count >= QXK24_MAX_ACTIVE_FLOWS) {
        pthread_mutex_unlock(&k->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    uint32_t slot = QXK24_MAX_ACTIVE_FLOWS;
    for (uint32_t i = 0; i < QXK24_MAX_ACTIVE_FLOWS; i++) {
        if (!k->flows[i].active && !k->flows[i].completed) {
            slot = i;
            break;
        }
    }
    if (slot == QXK24_MAX_ACTIVE_FLOWS) {
        pthread_mutex_unlock(&k->mutex);
        return QXK24_ERR_CAPACITY_FULL;
    }

    QXK24Flow *f = &k->flows[slot];
    memset(f, 0, sizeof(*f));
    f->flow_id = k->next_flow_id++;
    f->type = type;
    f->direction = dir;
    f->source_resource_id = src_id;
    f->dest_resource_id = dst_id;
    f->bytes = bytes;
    f->timestamp_start_ms = qxk24_now_ms();
    f->active = true;
    f->completed = false;

    k->flow_count++;
    *out_flow_id = f->flow_id;
    pthread_mutex_unlock(&k->mutex);
    return QXK24_OK;
}

int32_t qxk24_flow_complete(QXK24KernelHandle handle, uint64_t flow_id) {
    if (handle == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    for (uint32_t i = 0; i < QXK24_MAX_ACTIVE_FLOWS; i++) {
        QXK24Flow *f = &k->flows[i];
        if (!f->active || f->flow_id != flow_id) continue;

        f->active = false;
        f->completed = true;
        f->timestamp_end_ms = qxk24_now_ms();
        if (k->flow_count > 0) k->flow_count--;
        pthread_mutex_unlock(&k->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&k->mutex);
    return QXK24_ERR_NOT_FOUND;
}

int32_t qxk24_flow_cancel(QXK24KernelHandle handle, uint64_t flow_id) {
    if (handle == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24Kernel *k = (QXK24Kernel *)handle;
    pthread_mutex_lock(&k->mutex);
    for (uint32_t i = 0; i < QXK24_MAX_ACTIVE_FLOWS; i++) {
        QXK24Flow *f = &k->flows[i];
        if (!f->active || f->flow_id != flow_id) continue;

        memset(f, 0, sizeof(*f));
        if (k->flow_count > 0) k->flow_count--;
        pthread_mutex_unlock(&k->mutex);
        return QXK24_OK;
    }

    pthread_mutex_unlock(&k->mutex);
    return QXK24_ERR_NOT_FOUND;
}
