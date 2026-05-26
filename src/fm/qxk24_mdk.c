/* ============================================================
 * QXK24 – Universal Kernel
 * src/fm/qxk24_mdk.c
 *
 * Masa Dikawal (MDK) — Controlled Time Implementation
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 * ============================================================ */

#include "qxk24/fm/qxk24_mdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct QXK24MDKEngine {
    qxk24_mp_t parent;
    QXK24MDKSet set;
    float previous_energy;
    bool running;
};

static const QXK24Direction k_mdk_dirs[QXK24_FM_MDK_COUNT] = {
    QXK24_DIR_AS,
    QXK24_DIR_BH,
    QXK24_DIR_DN,
    QXK24_DIR_BG,
    QXK24_DIR_KN,
    QXK24_DIR_KR
};

static float clamp_unit(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static void update_summary(struct QXK24MDKEngine *self) {
    float total_value = 0.0f;
    float total_energy = 0.0f;
    uint32_t active = 0U;

    for (uint32_t i = 0; i < QXK24_FM_MDK_COUNT; i++) {
        const QXK24MDKRecord *record = &self->set.mdk[i];
        if (!record->active) continue;
        active++;
        total_value += record->value;
        total_energy += record->energy;
    }

    self->set.active_count = active;
    self->set.average_value = active > 0U ? total_value / (float)active : 0.0f;
    self->set.average_energy = active > 0U ? total_energy / (float)active : 0.0f;
}

static void init_records(struct QXK24MDKEngine *self) {
    memset(&self->set, 0, sizeof(self->set));
    for (uint8_t i = 0; i < QXK24_FM_MDK_COUNT; i++) {
        QXK24MDKRecord *record = &self->set.mdk[i];
        record->index = i;
        record->direction = k_mdk_dirs[i];
        record->value = 0.0f;
        record->energy = 0.0f;
        record->wave_state = QXK24_MDK_WAVE_TROUGH;
        record->wave_amplitude = 0.0f;
    }
}

qxk24_mdk_t qxk24_mdk_create(qxk24_mp_t mp) {
    if (mp == NULL) return NULL;

    struct QXK24MDKEngine *self =
        (struct QXK24MDKEngine *)calloc(1, sizeof(*self));
    if (self == NULL) return NULL;

    self->parent = mp;
    init_records(self);
    return self;
}

qxk24_err_t qxk24_mdk_start(qxk24_mdk_t mdk) {
    if (mdk == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24MDKEngine *self = mdk;

    for (uint32_t i = 0; i < QXK24_FM_MDK_COUNT; i++) {
        QXK24MDKRecord *record = &self->set.mdk[i];
        record->active = true;
        record->value = 0.5f;
        record->wave_state = QXK24_MDK_WAVE_RISING;
        record->wave_amplitude = 0.5f;
    }
    self->running = true;
    update_summary(self);
    return QXK24_OK;
}

static void cycle_record(QXK24MDKRecord *record, float energy) {
    const float step = 0.05f + (energy * 0.15f);
    record->energy = energy;
    record->wave_amplitude = 0.5f + (energy * 0.5f);

    if (record->wave_state == QXK24_MDK_WAVE_PEAK) {
        record->wave_state = QXK24_MDK_WAVE_FALLING;
    } else if (record->wave_state == QXK24_MDK_WAVE_TROUGH) {
        record->wave_state = QXK24_MDK_WAVE_RISING;
    }

    if (record->wave_state == QXK24_MDK_WAVE_RISING) {
        record->value += step;
        if (record->value >= 1.0f) {
            record->value = 1.0f;
            record->wave_state = QXK24_MDK_WAVE_PEAK;
        }
    } else {
        record->value -= step;
        if (record->value <= 0.0f) {
            record->value = 0.0f;
            record->wave_state = QXK24_MDK_WAVE_TROUGH;
        }
    }

    record->cycles++;
    record->can_be_mpp = record->active && record->cycles > 0ULL &&
        record->energy >= 0.5f;
}

qxk24_err_t qxk24_mdk_cycle(qxk24_mdk_t mdk, float energy) {
    if (mdk == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24MDKEngine *self = mdk;
    if (!self->running) return QXK24_ERR_NOT_STARTED;

    const float clamped = clamp_unit(energy);
    for (uint32_t i = 0; i < QXK24_FM_MDK_COUNT; i++) {
        if (self->set.mdk[i].active) cycle_record(&self->set.mdk[i], clamped);
    }
    self->previous_energy = clamped;
    update_summary(self);
    return QXK24_OK;
}

void qxk24_mdk_stop(qxk24_mdk_t mdk) {
    if (mdk == NULL) return;
    struct QXK24MDKEngine *self = mdk;
    for (uint32_t i = 0; i < QXK24_FM_MDK_COUNT; i++) {
        self->set.mdk[i].active = false;
    }
    self->running = false;
    update_summary(self);
}

void qxk24_mdk_destroy(qxk24_mdk_t mdk) {
    if (mdk == NULL) return;
    memset(mdk, 0, sizeof(*mdk));
    free(mdk);
}

qxk24_err_t qxk24_mdk_set(qxk24_mdk_t mdk, QXK24MDKSet *out) {
    if (mdk == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    update_summary(mdk);
    *out = mdk->set;
    return QXK24_OK;
}

qxk24_err_t qxk24_mdk_record(qxk24_mdk_t mdk,
                             uint8_t index,
                             QXK24MDKRecord *out) {
    if (mdk == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    if (index >= QXK24_FM_MDK_COUNT) return QXK24_ERR_INVALID_ARGUMENT;
    *out = mdk->set.mdk[index];
    return QXK24_OK;
}

QXK24MDKWaveState qxk24_mdk_wave_state(qxk24_mdk_t mdk, uint8_t index) {
    if (mdk == NULL || index >= QXK24_FM_MDK_COUNT) {
        return QXK24_MDK_WAVE_TROUGH;
    }
    return mdk->set.mdk[index].wave_state;
}

qxk24_mp_t qxk24_mdk_promote_to_mpp(qxk24_mdk_t mdk, uint8_t index) {
    if (mdk == NULL || index >= QXK24_FM_MDK_COUNT) return NULL;
    QXK24MDKRecord *record = &mdk->set.mdk[index];
    if (!record->can_be_mpp) return NULL;

    char name[32];
    snprintf(name, sizeof(name), "MDK-%u-MPP", (unsigned)index);
    return qxk24_mp_create_mpp(mdk->parent, name);
}

uint32_t qxk24_mdk_active_count(qxk24_mdk_t mdk) {
    if (mdk == NULL) return 0U;
    update_summary(mdk);
    return mdk->set.active_count;
}
