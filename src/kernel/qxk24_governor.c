/* ============================================================
 * qxk24_governor.c
 * QXK24 Universal Kernel — Governor Cycle Integration
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/kernel/qxk24_governor.c
 * License : Apache 2.0
 *
 * Description:
 * Integrates six resource governors into one constitutional health input.
 * ============================================================ */

#include "qxk24/kernel/qxk24_governor.h"
#include <stdio.h>
#include <string.h>

static float absf(float value) {
    return value < 0.0f ? -value : value;
}

static float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

qxk24_err_t qxk24_governor_registry_init(QXK24GovernorRegistry *registry) {
    if (registry == NULL) return QXK24_ERR_NULL_PARAM;
    memset(registry, 0, sizeof(*registry));
    return QXK24_OK;
}

qxk24_err_t qxk24_governor_register(QXK24GovernorRegistry *registry,
                                    QXK24Direction direction,
                                    QXK24ResourceGovernor *handle,
                                    const char *name) {
    if (registry == NULL || handle == NULL || name == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    if (registry->count >= QXK24_MAX_GOVERNORS) {
        return QXK24_ERR_CAPACITY;
    }

    QXK24GovernorSlot *slot = &registry->slots[registry->count++];
    slot->direction = direction;
    slot->handle = handle;
    slot->active = true;
    slot->last_law_score = 0.0f;
    snprintf(slot->name, sizeof(slot->name), "%s", name);
    return QXK24_OK;
}

QXK24GovernorSlot *qxk24_governor_find(QXK24GovernorRegistry *registry,
                                       QXK24Direction direction) {
    if (registry == NULL) return NULL;
    for (uint32_t i = 0; i < registry->count; i++) {
        QXK24GovernorSlot *slot = &registry->slots[i];
        if (slot->active && slot->direction == direction) return slot;
    }
    return NULL;
}

uint32_t qxk24_governor_active_count(const QXK24GovernorRegistry *registry) {
    if (registry == NULL) return 0U;
    uint32_t active = 0U;
    for (uint32_t i = 0; i < registry->count; i++) {
        if (registry->slots[i].active) active++;
    }
    return active;
}

static uint32_t count_complete_pairs(QXK24GovernorRegistry *registry) {
    uint32_t pairs = 0U;
    if (qxk24_governor_find(registry, QXK24_DIR_ATAS) &&
        qxk24_governor_find(registry, QXK24_DIR_BAWAH)) pairs++;
    if (qxk24_governor_find(registry, QXK24_DIR_DEPAN) &&
        qxk24_governor_find(registry, QXK24_DIR_BELAKANG)) pairs++;
    if (qxk24_governor_find(registry, QXK24_DIR_KANAN) &&
        qxk24_governor_find(registry, QXK24_DIR_KIRI)) pairs++;
    return pairs;
}

static float score_pair_symmetry(const QXK24GovernorSlot *a,
                                 const QXK24GovernorSlot *b) {
    if (a == NULL || b == NULL) return 0.0f;
    return clamp01(1.0f - absf(a->last_law_score - b->last_law_score));
}

static float cycle_governors(QXK24GovernorRegistry *registry,
                             uint32_t *within_budget) {
    float sum = 0.0f;
    *within_budget = 0U;
    for (uint32_t i = 0; i < registry->count; i++) {
        QXK24GovernorSlot *slot = &registry->slots[i];
        if (!slot->active) continue;

        QXK24GovernorReport report;
        const int32_t rc = qxk24_governor_base_cycle(slot->handle, &report);
        if (rc != QXK24_OK) {
            slot->active = false;
            slot->last_law_score = 0.0f;
            continue;
        }
        slot->last_law_score = (float)(report.health / 100.0);
        sum += slot->last_law_score;
        if (slot->last_law_score >= 0.7f) (*within_budget)++;
    }
    return sum;
}

qxk24_err_t qxk24_governor_cycle_all(QXK24GovernorRegistry *registry,
                                     QXK24HealthInput *input) {
    if (registry == NULL || input == NULL) return QXK24_ERR_NULL_PARAM;

    const uint32_t active = qxk24_governor_active_count(registry);
    const uint32_t pairs = count_complete_pairs(registry);
    uint32_t within_budget = 0U;
    const float law_sum = cycle_governors(registry, &within_budget);

    QXK24GovernorSlot *as = qxk24_governor_find(registry, QXK24_DIR_ATAS);
    QXK24GovernorSlot *bh = qxk24_governor_find(registry, QXK24_DIR_BAWAH);
    QXK24GovernorSlot *dn = qxk24_governor_find(registry, QXK24_DIR_DEPAN);
    QXK24GovernorSlot *bg = qxk24_governor_find(registry, QXK24_DIR_BELAKANG);
    QXK24GovernorSlot *kn = qxk24_governor_find(registry, QXK24_DIR_KANAN);
    QXK24GovernorSlot *kr = qxk24_governor_find(registry, QXK24_DIR_KIRI);

    input->z1 = active > 0U ? 1.0f : 0.0f;
    input->z2 = (float)pairs / 3.0f;
    input->z3 = (float)active / (float)QXK24_MAX_GOVERNORS;
    input->z4 = active > 0U ? law_sum / (float)active : 0.0f;
    input->x1 = active > 0U ? (float)within_budget / (float)active : 0.0f;

    float pair_sym = 0.0f;
    pair_sym += score_pair_symmetry(as, bh);
    pair_sym += score_pair_symmetry(dn, bg);
    pair_sym += score_pair_symmetry(kn, kr);
    input->x2 = pairs > 0U ? pair_sym / (float)pairs : 0.0f;
    input->x3 = (float)active / (float)QXK24_MAX_GOVERNORS;
    input->x4 = (active == registry->count) ? 1.0f : 0.0f;
    return QXK24_OK;
}
