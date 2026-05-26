/* ============================================================
 * qxk24_sira.h
 * QXK24 Universal Kernel — SIRA Biology Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/science/qxk24_sira.h
 * License : Apache 2.0
 *
 * Description:
 * Declares SIRA ppU growth, formation factors, 6:1 energy
 * split, and SAU structural helpers.
 * ============================================================ */

#ifndef QXK24_SIRA_H
#define QXK24_SIRA_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_SIRA_PPU_LEVELS 6U
#define QXK24_SIRA_FORMATION_FACTORS 4U
#define QXK24_SIRA_ACTIVE_RATIO 6U
#define QXK24_SIRA_PASSIVE_RATIO 1U
#define QXK24_SIRA_DIRECTIONS 6U
#define QXK24_SIRA_SAU_LEVELS 7U
#define QXK24_SIRA_NU_SA 5U
#define QXK24_SIRA_NU_DU 4U
#define QXK24_SIRA_NU_GA 3U
#define QXK24_SIRA_NU_PA 2U
#define QXK24_SIRA_NU_MA 1U
#define QXK24_SIRA_NU_NA 0U

typedef enum {
    QXK24_SIRA_PPU_SA = 0,
    QXK24_SIRA_PPU_DU = 1,
    QXK24_SIRA_PPU_GA = 2,
    QXK24_SIRA_PPU_PA = 3,
    QXK24_SIRA_PPU_MA = 4,
    QXK24_SIRA_PPU_NA = 5
} qxk24_sira_ppu_t;

typedef struct {
    int nucleus_complete;
    int position_correct;
    int masa_precise;
    int tenaga_adequate;
    int is_perfect;
    int is_pincang;
} qxk24_sira_formation_t;

typedef struct {
    qxk24_sira_ppu_t level;
    uint32_t nu_count;
    uint32_t nt_size;
    float active_energy;
    float passive_energy;
    float total_energy;
    qxk24_sira_formation_t formation;
    int growth_valid;
} qxk24_sira_growth_t;

typedef struct {
    uint32_t k24md;
    uint32_t k24ak;
    uint32_t k24si;
    uint32_t k24at;
    uint32_t k24ji;
    uint32_t k24za;
} qxk24_sira_sau_structure_t;

typedef struct {
    uint32_t growths_evaluated;
    uint32_t perfect_formations;
    uint32_t pincang_detected;
    float health;
} qxk24_sira_snapshot_t;

typedef struct qxk24_sira_impl_t *qxk24_sira_t;

qxk24_sira_t qxk24_sira_create(void);
void qxk24_sira_destroy(qxk24_sira_t s);
int qxk24_sira_evaluate_growth(qxk24_sira_t s, qxk24_sira_ppu_t level,
                               float active_e, float passive_e,
                               qxk24_sira_growth_t *out);
int qxk24_sira_check_formation(qxk24_sira_t s, int nucleus,
                               int position, int masa, int tenaga,
                               qxk24_sira_formation_t *out);
int qxk24_sira_split_energy(float total, float *active, float *passive);
qxk24_sira_sau_structure_t qxk24_sira_sau_modul(void);
uint32_t qxk24_sira_nu_count(qxk24_sira_ppu_t level);
const char *qxk24_sira_ppu_name(qxk24_sira_ppu_t level);
qxk24_sira_snapshot_t qxk24_sira_snapshot(qxk24_sira_t s);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_SIRA_H */
