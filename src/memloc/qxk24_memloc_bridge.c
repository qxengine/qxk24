/* ============================================================
 * qxk24_memloc_bridge.c
 * QXK24 Universal Kernel - QXMemloc K24at Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/memloc/qxk24_memloc_bridge.c
 * License : Apache 2.0
 *
 * Description:
 * Implements constitutional memory locate/release at K24at Atom level.
 * ============================================================ */

#include "qxk24/memloc/qxk24_memloc_bridge.h"
#include <stdlib.h>
#include <string.h>

struct QXK24MemlocBridge_s {
    QXK24MemlocConfig config;
    qxk24_qxengine_t gov;
    QXK24MemlocRecord *records;
    uint32_t active_count;
    uint64_t bytes_active;
    uint64_t next_address;
    float health_score;
};

QXK24MemlocConfig qxk24_memloc_default_config(void) {
    QXK24MemlocConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.total_bytes = 256ULL * 1024ULL * 1024ULL;
    cfg.max_records = 1024U;
    cfg.enable_ama_check = true;
    return cfg;
}

qxk24_memloc_t qxk24_memloc_create(const QXK24MemlocConfig *config,
                                   qxk24_qxengine_t gov) {
    if (config == NULL || gov == NULL || config->max_records == 0U) return NULL;

    struct QXK24MemlocBridge_s *ml =
        (struct QXK24MemlocBridge_s *)calloc(1, sizeof(*ml));
    if (ml == NULL) return NULL;

    ml->records = (QXK24MemlocRecord *)calloc(config->max_records,
        sizeof(QXK24MemlocRecord));
    if (ml->records == NULL) {
        free(ml);
        return NULL;
    }

    ml->config = *config;
    ml->gov = gov;
    ml->next_address = 0x1000ULL;
    ml->health_score = 100.0f;
    return ml;
}

uint32_t qxk24_memloc_ama_column(void) {
    return QXK24_MEMLOC_AMA_COLUMN;
}

static float health_from_usage(const struct QXK24MemlocBridge_s *ml) {
    const float record_ratio = (float)ml->active_count /
        (float)QXK24_MEMLOC_AMA_COLUMN;
    const float byte_ratio = ml->config.total_bytes > 0ULL
        ? (float)ml->bytes_active / (float)ml->config.total_bytes
        : 0.0f;
    float score = 100.0f - (record_ratio * 3.0f) - (byte_ratio * 2.0f);
    if (score < 0.0f) score = 0.0f;
    if (score > 100.0f) score = 100.0f;
    return score;
}

qxk24_err_t qxk24_memloc_locate(qxk24_memloc_t ml,
                                uint64_t bytes,
                                QXK24Direction dir,
                                QXK24MemlocRecord *out) {
    if (ml == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    if (bytes == 0ULL || (uint32_t)dir >= QXK24_DIRECTION_COUNT) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    struct QXK24MemlocBridge_s *self = ml;
    if (self->active_count >= self->config.max_records) {
        return QXK24_ERR_CAPACITY;
    }
    if (self->config.enable_ama_check &&
        self->active_count >= QXK24_MEMLOC_AMA_COLUMN) {
        return QXK24_ERR_CAPACITY;
    }
    if (self->bytes_active + bytes > self->config.total_bytes) {
        return QXK24_ERR_BUDGET_EXCEEDED;
    }

    QXK24MemlocRecord *slot = NULL;
    for (uint32_t i = 0; i < self->config.max_records; i++) {
        if (!self->records[i].active) {
            slot = &self->records[i];
            break;
        }
    }
    if (slot == NULL) return QXK24_ERR_CAPACITY;

    slot->address = self->next_address;
    slot->size_bytes = bytes;
    slot->direction = dir;
    slot->ama_column = QXK24_MEMLOC_AMA_COLUMN;
    slot->active = true;
    self->next_address += ((bytes + 7ULL) & ~7ULL) + 8ULL;
    self->active_count++;
    self->bytes_active += bytes;
    self->health_score = health_from_usage(self);

    qxk24_qxengine_observe_memory(self->gov, self->bytes_active,
        self->active_count, 0U);
    *out = *slot;
    return QXK24_OK;
}

qxk24_err_t qxk24_memloc_release(qxk24_memloc_t ml, uint64_t address) {
    if (ml == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24MemlocBridge_s *self = ml;

    for (uint32_t i = 0; i < self->config.max_records; i++) {
        QXK24MemlocRecord *record = &self->records[i];
        if (record->active && record->address == address) {
            record->active = false;
            if (self->active_count > 0U) self->active_count--;
            if (self->bytes_active >= record->size_bytes) {
                self->bytes_active -= record->size_bytes;
            }
            self->health_score = health_from_usage(self);
            qxk24_qxengine_observe_memory(self->gov, self->bytes_active,
                self->active_count, 0U);
            return QXK24_OK;
        }
    }
    return QXK24_ERR_NOT_FOUND;
}

float qxk24_memloc_health_score(qxk24_memloc_t ml) {
    return ml == NULL ? 0.0f : ml->health_score;
}

uint32_t qxk24_memloc_active_count(qxk24_memloc_t ml) {
    return ml == NULL ? 0U : ml->active_count;
}

void qxk24_memloc_destroy(qxk24_memloc_t ml) {
    if (ml == NULL) return;
    free(ml->records);
    memset(ml, 0, sizeof(*ml));
    free(ml);
}
