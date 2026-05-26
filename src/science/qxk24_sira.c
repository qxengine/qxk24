/* ============================================================
 * qxk24_sira.c
 * QXK24 Universal Kernel — SIRA Biology Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/science/qxk24_sira.c
 * License : Apache 2.0
 *
 * Description:
 * Implements SIRA ppU growth evaluation, formation factors,
 * 6:1 energy split, and SAU structure helpers.
 * ============================================================ */

#include "qxk24/science/qxk24_sira.h"
#include <stdlib.h>
#include <string.h>

struct qxk24_sira_impl_t {
    uint32_t growths_evaluated;
    uint32_t perfect_formations;
    uint32_t pincang_detected;
    float health;
};

static const uint32_t s_nu_counts[] = {5U, 4U, 3U, 2U, 1U, 0U};
static const char *s_ppu_names[] = {"Sa", "Du", "Ga", "Pa", "Ma", "Na"};

static void update_health(struct qxk24_sira_impl_t *s) {
    s->health = 99.5f - (float)(s->growths_evaluated % 20U) * 0.1f;
    if (s->health < 80.0f) s->health = 80.0f;
}

qxk24_sira_t qxk24_sira_create(void) {
    struct qxk24_sira_impl_t *s = calloc(1, sizeof(*s));
    if (s == NULL) return NULL;
    s->health = 99.5f;
    return s;
}

void qxk24_sira_destroy(qxk24_sira_t s) {
    free(s);
}

int qxk24_sira_split_energy(float total, float *active, float *passive) {
    if (active == NULL || passive == NULL) return QXK24_ERR_NULL_PARAM;
    const float unit = total / 7.0f;
    *active = unit * (float)QXK24_SIRA_ACTIVE_RATIO;
    *passive = unit * (float)QXK24_SIRA_PASSIVE_RATIO;
    return QXK24_OK;
}

int qxk24_sira_check_formation(qxk24_sira_t s, int nucleus,
                               int position, int masa, int tenaga,
                               qxk24_sira_formation_t *out) {
    if (s == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    out->nucleus_complete = nucleus;
    out->position_correct = position;
    out->masa_precise = masa;
    out->tenaga_adequate = tenaga;
    out->is_perfect = nucleus && position && masa && tenaga;
    out->is_pincang = !out->is_perfect;
    if (out->is_perfect) s->perfect_formations++;
    else s->pincang_detected++;
    return QXK24_OK;
}

int qxk24_sira_evaluate_growth(qxk24_sira_t s, qxk24_sira_ppu_t level,
                               float active_e, float passive_e,
                               qxk24_sira_growth_t *out) {
    if (s == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    if (level < QXK24_SIRA_PPU_SA || level > QXK24_SIRA_PPU_NA) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    memset(out, 0, sizeof(*out));
    out->level = level;
    out->nu_count = s_nu_counts[(int)level];
    out->nt_size = (uint32_t)level + 1U;
    out->active_energy = active_e;
    out->passive_energy = passive_e;
    out->total_energy = active_e + passive_e;
    out->growth_valid = active_e != passive_e;
    s->growths_evaluated++;
    update_health(s);
    return QXK24_OK;
}

qxk24_sira_sau_structure_t qxk24_sira_sau_modul(void) {
    qxk24_sira_sau_structure_t st;
    st.k24md = 6U;
    st.k24ak = 36U;
    st.k24si = 216U;
    st.k24at = 1296U;
    st.k24ji = 7776U;
    st.k24za = 46656U;
    return st;
}

uint32_t qxk24_sira_nu_count(qxk24_sira_ppu_t level) {
    return level >= QXK24_SIRA_PPU_SA && level <= QXK24_SIRA_PPU_NA
        ? s_nu_counts[(int)level] : 0U;
}

const char *qxk24_sira_ppu_name(qxk24_sira_ppu_t level) {
    return level >= QXK24_SIRA_PPU_SA && level <= QXK24_SIRA_PPU_NA
        ? s_ppu_names[(int)level] : "unknown";
}

qxk24_sira_snapshot_t qxk24_sira_snapshot(qxk24_sira_t s) {
    qxk24_sira_snapshot_t snap;
    memset(&snap, 0, sizeof(snap));
    if (s == NULL) return snap;
    snap.growths_evaluated = s->growths_evaluated;
    snap.perfect_formations = s->perfect_formations;
    snap.pincang_detected = s->pincang_detected;
    snap.health = s->health;
    return snap;
}
