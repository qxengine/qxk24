/* ============================================================
 * qxk24_rina.h
 * QXK24 Universal Kernel — RINA Physics Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/science/qxk24_rina.h
 * License : Apache 2.0
 *
 * Description:
 * Declares RINA force, load, gradient, energy ratio, and
 * META-FIZIK physics compatibility helpers.
 * ============================================================ */

#ifndef QXK24_RINA_H
#define QXK24_RINA_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_RINA_ACTIVE_PASSIVE_RATIO 6
#define QXK24_RINA_PASSIVE_ENERGY 1
#define QXK24_RINA_DAYA_TENAGA_RATIO 2
#define QXK24_RINA_DISTANCE_LEVELS 6
#define QXK24_RINA_GRADIENT_RISING 0
#define QXK24_RINA_GRADIENT_FALLING 1
#define QXK24_RINA_POSITION_VERTICAL 0
#define QXK24_RINA_POSITION_HORIZONTAL 1

typedef struct {
    float ba;
    float bt;
    float nb;
    int position;
} qxk24_rina_load_t;

typedef struct {
    float y_distance;
    float x_distance;
    float c;
    int direction;
} qxk24_rina_gradient_t;

typedef struct {
    float nb;
    float j;
    float c;
    float daya;
    float tenaga_min;
    int is_valid;
} qxk24_rina_force_t;

typedef struct {
    float total_energy;
    float active_energy;
    float passive_energy;
    float ratio_active;
    float ratio_passive;
} qxk24_rina_energy_split_t;

typedef struct {
    int meta_active;
    int fizik_passive;
    float compatibility;
} qxk24_rina_meta_fizik_t;

typedef struct {
    uint32_t force_calculations;
    uint32_t energy_splits;
    float total_daya_computed;
    float health;
} qxk24_rina_snapshot_t;

typedef struct qxk24_rina_impl_t *qxk24_rina_t;

qxk24_rina_t qxk24_rina_create(void);
void qxk24_rina_destroy(qxk24_rina_t r);
int qxk24_rina_compute_load(int position, float ba, float bt,
                            qxk24_rina_load_t *out);
int qxk24_rina_compute_gradient(float y_dist, float x_dist, int direction,
                                qxk24_rina_gradient_t *out);
int qxk24_rina_compute_force(qxk24_rina_t r, float nb, float j, float c,
                             qxk24_rina_force_t *out);
int qxk24_rina_split_energy(qxk24_rina_t r, float total,
                            qxk24_rina_energy_split_t *out);
float qxk24_rina_min_tenaga(float daya);
int qxk24_rina_meta_fizik(float pattern_a, float pattern_b,
                          qxk24_rina_meta_fizik_t *out);
qxk24_rina_snapshot_t qxk24_rina_snapshot(qxk24_rina_t r);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_RINA_H */
