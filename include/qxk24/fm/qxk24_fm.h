/* ============================================================
 * QXK24 – Universal Kernel
 * include/qxk24/fm/qxk24_fm.h
 *
 * Faktor Masa (Fm) — Constitutional Time Engine
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 *
 * Faktor Masa (Fm) defines time as a constitutional
 * structure — not merely a measurement unit.
 *
 * Fm Formula : Fm = MP / (MDK) = 1 / (2/4)
 * K24 Formula: K24 = K(1) x Leraian2 x Leraian4
 *
 * Fm and K24 are the same constitutional structure.
 * ============================================================ */

#ifndef QXK24_FM_H
#define QXK24_FM_H

#include "../qxk24.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Fm constants ── */
#define QXK24_FM_VERSION         "1.1.0"
#define QXK24_FM_MP_VALUE        1       /* MPi = 1, absolute    */
#define QXK24_FM_MDK_PAIR        2       /* every MDK has pair   */
#define QXK24_FM_MDK_DIRECTION   4       /* four directions      */
#define QXK24_FM_MDK_COUNT       6       /* six MDK per MP       */
#define QXK24_FM_LUMAN_Z         6       /* universe pattern     */
#define QXK24_FM_LUMAN_X         4       /* human pattern        */
#define QXK24_FM_AHLI_COORDINATES 6      /* six Ahli coordinates */

/* ── Fm formula: Fm = 1 / (2/4) = 2.0 ── */
#define QXK24_FM_VALUE  \
    ((float)QXK24_FM_MP_VALUE / \
     ((float)QXK24_FM_MDK_PAIR / \
      (float)QXK24_FM_MDK_DIRECTION))

/* ── Time category ── */
typedef enum {
    QXK24_FM_MPi  = 0,  /* Masa Pengawal Induk — absolute master */
    QXK24_FM_MPP  = 1,  /* Masa Pengawal Proses — process master */
    QXK24_FM_MDK  = 2   /* Masa Dikawal — controlled time        */
} QXK24FmCategory;

/* ── Time flow direction ── */
typedef enum {
    QXK24_FM_FLOW_STRAIGHT = 0,  /* MP  — straight line forward  */
    QXK24_FM_FLOW_WAVE     = 1   /* MDK — wave up/down           */
} QXK24FmFlowType;

/* ── Time flow law ── */
typedef enum {
    QXK24_FM_LAW_ENERGY_UP   = 0, /* energy↑ → time rise↓        */
    QXK24_FM_LAW_ENERGY_DOWN = 1, /* energy↓ → time rise↑        */
    QXK24_FM_LAW_ENERGY_WAVE = 2  /* energy↑↓ → time descent=K   */
} QXK24FmFlowLaw;

/* ── Fm snapshot ── */
typedef struct {
    float       fm_value;           /* Fm = 1/(2/4) = 2.0         */
    float       mp_ratio;           /* MPi ratio = 1.0 constant   */
    float       mdk_ratio;          /* MDK ratio = current/max    */
    float       energy_factor;      /* current X = m/t            */
    QXK24FmFlowLaw  flow_law;       /* active flow law            */
    uint64_t    cycle_index;        /* kernel cycle               */
    uint64_t    timestamp_ms;       /* wall-clock ms              */
    bool        constitutionally_timed; /* Fm within bounds       */
} QXK24FmSnapshot;

/* ── Fm handle ── */
typedef struct QXK24FmEngine *qxk24_fm_t;

/* ── Public API ── */

/**
 * qxk24_fm_create
 * Create the Faktor Masa engine.
 * @param  kernel  running QXK24 kernel handle
 * @return handle or NULL
 */
qxk24_fm_t qxk24_fm_create(qxk24_kernel_t kernel);

/**
 * qxk24_fm_start
 * Start the Fm engine — activates MPi clock.
 * @param  fm  Fm handle
 * @return QXK24_OK on success
 */
qxk24_err_t qxk24_fm_start(qxk24_fm_t fm);

/**
 * qxk24_fm_cycle
 * Run one Fm constitutional cycle.
 * Updates MDK values, applies flow laws, feeds Fm into kernel.
 * @param  fm  Fm handle
 * @return QXK24_OK on success
 */
qxk24_err_t qxk24_fm_cycle(qxk24_fm_t fm);

/**
 * qxk24_fm_stop
 * Stop the Fm engine.
 */
void qxk24_fm_stop(qxk24_fm_t fm);

/**
 * qxk24_fm_destroy
 * Destroy the Fm engine.
 */
void qxk24_fm_destroy(qxk24_fm_t fm);

/**
 * qxk24_fm_snapshot
 * Retrieve current Fm state snapshot.
 * @param  fm   Fm handle
 * @param  out  pointer to snapshot struct
 * @return QXK24_OK on success
 */
qxk24_err_t qxk24_fm_snapshot(
    qxk24_fm_t      fm,
    QXK24FmSnapshot *out
);

/**
 * qxk24_fm_value
 * Return the constitutional Fm value (= 2.0).
 */
float qxk24_fm_value(void);

/**
 * qxk24_fm_category_name
 * Return string name for a time category.
 */
const char *qxk24_fm_category_name(QXK24FmCategory cat);

/**
 * qxk24_fm_flow_law
 * Determine the active flow law from energy factor.
 * @param  energy_prev  previous X = m/t
 * @param  energy_curr  current  X = m/t
 * @return QXK24FmFlowLaw
 */
QXK24FmFlowLaw qxk24_fm_flow_law(
    float energy_prev,
    float energy_curr
);

/**
 * qxk24_fm_luman_scale
 * Return LUMAN scale for a given pattern (Z=6, X=4).
 * @param  pattern  QXK24_FM_LUMAN_Z or QXK24_FM_LUMAN_X
 * @param  level    1–7
 * @return scale value
 */
uint64_t qxk24_fm_luman_scale(uint8_t pattern, uint8_t level);

/**
 * qxk24_fm_is_running
 * Return true if Fm engine is running.
 */
bool qxk24_fm_is_running(qxk24_fm_t fm);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_FM_H */
