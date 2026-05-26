/* ============================================================
 * qxk24_rina.c
 * QXK24 Universal Kernel — RINA Physics Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/science/qxk24_rina.c
 * License : Apache 2.0
 *
 * Description:
 * Implements RINA load, gradient, force, energy split, and
 * META-FIZIK physics compatibility helpers.
 * ============================================================ */

#include "qxk24/science/qxk24_rina.h"
#include <stdlib.h>
#include <string.h>

struct qxk24_rina_impl_t {
    uint32_t force_calculations;
    uint32_t energy_splits;
    float total_daya_computed;
    float health;
};

static void update_health(struct qxk24_rina_impl_t *r) {
    r->health = 99.5f - (float)(r->force_calculations % 20U) * 0.1f;
    if (r->health < 80.0f) r->health = 80.0f;
}

qxk24_rina_t qxk24_rina_create(void) {
    struct qxk24_rina_impl_t *r = calloc(1, sizeof(*r));
    if (r == NULL) return NULL;
    r->health = 99.5f;
    return r;
}

void qxk24_rina_destroy(qxk24_rina_t r) {
    free(r);
}

int qxk24_rina_compute_load(int position, float ba, float bt,
                            qxk24_rina_load_t *out) {
    if (out == NULL) return QXK24_ERR_NULL_PARAM;
    out->position = position;
    out->ba = ba;
    out->bt = position == QXK24_RINA_POSITION_VERTICAL ? bt : 0.0f;
    out->nb = out->ba + out->bt;
    return QXK24_OK;
}

int qxk24_rina_compute_gradient(float y_dist, float x_dist, int direction,
                                qxk24_rina_gradient_t *out) {
    if (out == NULL) return QXK24_ERR_NULL_PARAM;
    if (x_dist <= 0.0f) return QXK24_ERR_INVALID_ARGUMENT;
    out->y_distance = y_dist;
    out->x_distance = x_dist;
    out->c = y_dist / x_dist;
    out->direction = direction;
    return QXK24_OK;
}

int qxk24_rina_compute_force(qxk24_rina_t r, float nb, float j, float c,
                             qxk24_rina_force_t *out) {
    if (r == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    out->nb = nb;
    out->j = j;
    out->c = c;
    out->daya = nb * (j + c);
    out->tenaga_min = out->daya / (float)QXK24_RINA_DAYA_TENAGA_RATIO;
    out->is_valid = 1;
    r->force_calculations++;
    r->total_daya_computed += out->daya;
    update_health(r);
    return QXK24_OK;
}

int qxk24_rina_split_energy(qxk24_rina_t r, float total,
                            qxk24_rina_energy_split_t *out) {
    if (r == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    const float unit = total / 7.0f;
    out->total_energy = total;
    out->active_energy = unit * (float)QXK24_RINA_ACTIVE_PASSIVE_RATIO;
    out->passive_energy = unit * (float)QXK24_RINA_PASSIVE_ENERGY;
    out->ratio_active = (float)QXK24_RINA_ACTIVE_PASSIVE_RATIO;
    out->ratio_passive = (float)QXK24_RINA_PASSIVE_ENERGY;
    r->energy_splits++;
    return QXK24_OK;
}

float qxk24_rina_min_tenaga(float daya) {
    return daya / (float)QXK24_RINA_DAYA_TENAGA_RATIO;
}

int qxk24_rina_meta_fizik(float pattern_a, float pattern_b,
                          qxk24_rina_meta_fizik_t *out) {
    if (out == NULL) return QXK24_ERR_NULL_PARAM;
    float diff = pattern_a - pattern_b;
    if (diff < 0.0f) diff = -diff;
    out->meta_active = 1;
    out->fizik_passive = 1;
    out->compatibility = diff < 0.5f ? 1.0f : (diff < 1.5f ? 0.7f : 0.3f);
    return QXK24_OK;
}

qxk24_rina_snapshot_t qxk24_rina_snapshot(qxk24_rina_t r) {
    qxk24_rina_snapshot_t s;
    memset(&s, 0, sizeof(s));
    if (r == NULL) return s;
    s.force_calculations = r->force_calculations;
    s.energy_splits = r->energy_splits;
    s.total_daya_computed = r->total_daya_computed;
    s.health = r->health;
    return s;
}
