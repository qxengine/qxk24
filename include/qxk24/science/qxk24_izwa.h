/* ============================================================
 * qxk24_izwa.h
 * QXK24 Universal Kernel — IZWA Chemistry Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/science/qxk24_izwa.h
 * License : Apache 2.0
 *
 * Description:
 * Declares IZWA Makro/Mikro reactions, 36-element reaction
 * table, same-group combination law, and chemistry snapshots.
 * ============================================================ */

#ifndef QXK24_IZWA_H
#define QXK24_IZWA_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_IZWA_MIKRO_PER_MAKRO 6U
#define QXK24_IZWA_MIKRO_SA_TOTAL 36U
#define QXK24_IZWA_MIKRO_SA_DOMINANT_A 15U
#define QXK24_IZWA_MIKRO_SA_DOMINANT_B 15U
#define QXK24_IZWA_MIKRO_SA_NEUTRAL 6U
#define QXK24_IZWA_MIKRO_DU_TOTAL 14850U
#define QXK24_IZWA_LEVELS 7U
#define QXK24_IZWA_ELEMENTS_BASE 4U

typedef enum {
    QXK24_IZWA_REACTION_EQUAL = 0,
    QXK24_IZWA_REACTION_A_DOM = 1,
    QXK24_IZWA_REACTION_B_DOM = 2
} qxk24_izwa_reaction_t;

typedef enum {
    QXK24_IZWA_MIKRO_CAT_A = 0,
    QXK24_IZWA_MIKRO_CAT_B = 1,
    QXK24_IZWA_MIKRO_CAT_AB = 2
} qxk24_izwa_mikro_cat_t;

typedef enum {
    QXK24_IZWA_ELEMENT_TANAH = 0,
    QXK24_IZWA_ELEMENT_AIR = 1,
    QXK24_IZWA_ELEMENT_API = 2,
    QXK24_IZWA_ELEMENT_ANGIN = 3
} qxk24_izwa_element_t;

typedef struct {
    uint32_t id;
    uint32_t a_level;
    uint32_t b_level;
    qxk24_izwa_reaction_t reaction;
    qxk24_izwa_mikro_cat_t category;
    int is_pincang;
} qxk24_izwa_mikro_t;

typedef struct {
    uint32_t total_mikro;
    uint32_t dominant_a;
    uint32_t dominant_b;
    uint32_t neutral;
    qxk24_izwa_mikro_t mikro[36];
    int is_valid;
} qxk24_izwa_reaction_result_t;

typedef struct {
    qxk24_izwa_mikro_cat_t cat_a;
    qxk24_izwa_mikro_cat_t cat_b;
    int is_allowed;
    char reason[64];
} qxk24_izwa_combination_t;

typedef struct {
    uint32_t reactions_performed;
    uint32_t pincang_detected;
    uint32_t valid_combinations;
    float health;
} qxk24_izwa_snapshot_t;

typedef struct qxk24_izwa_impl_t *qxk24_izwa_t;

qxk24_izwa_t qxk24_izwa_create(void);
void qxk24_izwa_destroy(qxk24_izwa_t z);
int qxk24_izwa_react(qxk24_izwa_t z, uint32_t makro_a,
                     uint32_t makro_b, qxk24_izwa_reaction_result_t *out);
int qxk24_izwa_check_combination(qxk24_izwa_t z,
                                 qxk24_izwa_mikro_cat_t cat_a,
                                 qxk24_izwa_mikro_cat_t cat_b,
                                 qxk24_izwa_combination_t *out);
const char *qxk24_izwa_element_name(qxk24_izwa_element_t e);
const char *qxk24_izwa_reaction_name(qxk24_izwa_reaction_t r);
const char *qxk24_izwa_category_name(qxk24_izwa_mikro_cat_t c);
uint32_t qxk24_izwa_mikro_du_count(void);
qxk24_izwa_snapshot_t qxk24_izwa_snapshot(qxk24_izwa_t z);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_IZWA_H */
