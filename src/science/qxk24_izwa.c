/* ============================================================
 * qxk24_izwa.c
 * QXK24 Universal Kernel — IZWA Chemistry Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/science/qxk24_izwa.c
 * License : Apache 2.0
 *
 * Description:
 * Implements IZWA Makro reactions, Mikro table generation,
 * combination law checks, and chemistry snapshots.
 * ============================================================ */

#include "qxk24/science/qxk24_izwa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct qxk24_izwa_impl_t {
    uint32_t reactions_performed;
    uint32_t pincang_detected;
    uint32_t valid_combinations;
    float health;
};

static const char *s_element_names[] = {"Tanah", "Air", "Api", "Angin"};
static const char *s_reaction_names[] = {
    "A=B (Neutral)", "A>B (A Dominant)", "A<B (B Dominant)"
};
static const char *s_cat_names[] = {
    "Mikro Sa:A", "Mikro Sa:B", "Mikro Sa:AB"
};

static void update_health(struct qxk24_izwa_impl_t *z) {
    z->health = 99.5f - (float)(z->reactions_performed % 20U) * 0.1f;
    if (z->health < 80.0f) z->health = 80.0f;
}

qxk24_izwa_t qxk24_izwa_create(void) {
    struct qxk24_izwa_impl_t *z = calloc(1, sizeof(*z));
    if (z == NULL) return NULL;
    z->health = 99.5f;
    return z;
}

void qxk24_izwa_destroy(qxk24_izwa_t z) {
    free(z);
}

int qxk24_izwa_react(qxk24_izwa_t z, uint32_t makro_a,
                     uint32_t makro_b, qxk24_izwa_reaction_result_t *out) {
    if (z == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    (void)makro_a;
    (void)makro_b;
    memset(out, 0, sizeof(*out));
    uint32_t idx = 0U;
    for (uint32_t a = 1U; a <= QXK24_IZWA_MIKRO_PER_MAKRO; a++) {
        for (uint32_t b = 1U; b <= QXK24_IZWA_MIKRO_PER_MAKRO; b++) {
            qxk24_izwa_mikro_t *m = &out->mikro[idx];
            m->id = idx + 1U;
            m->a_level = a;
            m->b_level = b;
            if (a == b) {
                m->reaction = QXK24_IZWA_REACTION_EQUAL;
                m->category = QXK24_IZWA_MIKRO_CAT_AB;
            } else if (a > b) {
                m->reaction = QXK24_IZWA_REACTION_A_DOM;
                m->category = QXK24_IZWA_MIKRO_CAT_A;
            } else {
                m->reaction = QXK24_IZWA_REACTION_B_DOM;
                m->category = QXK24_IZWA_MIKRO_CAT_B;
            }
            idx++;
        }
    }
    out->total_mikro = QXK24_IZWA_MIKRO_SA_TOTAL;
    out->dominant_a = QXK24_IZWA_MIKRO_SA_DOMINANT_A;
    out->dominant_b = QXK24_IZWA_MIKRO_SA_DOMINANT_B;
    out->neutral = QXK24_IZWA_MIKRO_SA_NEUTRAL;
    out->is_valid = 1;
    z->reactions_performed++;
    update_health(z);
    return QXK24_OK;
}

int qxk24_izwa_check_combination(qxk24_izwa_t z,
                                 qxk24_izwa_mikro_cat_t cat_a,
                                 qxk24_izwa_mikro_cat_t cat_b,
                                 qxk24_izwa_combination_t *out) {
    if (z == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    out->cat_a = cat_a;
    out->cat_b = cat_b;
    if ((cat_a == QXK24_IZWA_MIKRO_CAT_A &&
         cat_b == QXK24_IZWA_MIKRO_CAT_A) ||
        (cat_a == QXK24_IZWA_MIKRO_CAT_B &&
         cat_b == QXK24_IZWA_MIKRO_CAT_B)) {
        out->is_allowed = 0;
        snprintf(out->reason, sizeof(out->reason),
                 "Same-group combination: pincang");
        z->pincang_detected++;
    } else {
        out->is_allowed = 1;
        snprintf(out->reason, sizeof(out->reason),
                 "Valid constitutional combination");
        z->valid_combinations++;
    }
    return QXK24_OK;
}

const char *qxk24_izwa_element_name(qxk24_izwa_element_t e) {
    return e >= QXK24_IZWA_ELEMENT_TANAH && e <= QXK24_IZWA_ELEMENT_ANGIN
        ? s_element_names[(int)e] : "unknown";
}

const char *qxk24_izwa_reaction_name(qxk24_izwa_reaction_t r) {
    return r >= QXK24_IZWA_REACTION_EQUAL && r <= QXK24_IZWA_REACTION_B_DOM
        ? s_reaction_names[(int)r] : "unknown";
}

const char *qxk24_izwa_category_name(qxk24_izwa_mikro_cat_t c) {
    return c >= QXK24_IZWA_MIKRO_CAT_A && c <= QXK24_IZWA_MIKRO_CAT_AB
        ? s_cat_names[(int)c] : "unknown";
}

uint32_t qxk24_izwa_mikro_du_count(void) {
    return QXK24_IZWA_MIKRO_DU_TOTAL;
}

qxk24_izwa_snapshot_t qxk24_izwa_snapshot(qxk24_izwa_t z) {
    qxk24_izwa_snapshot_t s;
    memset(&s, 0, sizeof(s));
    if (z == NULL) return s;
    s.reactions_performed = z->reactions_performed;
    s.pincang_detected = z->pincang_detected;
    s.valid_combinations = z->valid_combinations;
    s.health = z->health;
    return s;
}
