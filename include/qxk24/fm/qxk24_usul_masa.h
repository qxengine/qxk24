/* ============================================================
 * QXK24 – Universal Kernel
 * include/qxk24/fm/qxk24_usul_masa.h
 *
 * Usul Masa — Constitutional Time Constitution Header
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 *
 * Usul Masa defines the constitutional time structure
 * for each process (X). Every process has:
 *   One  Umm  — Usul Masa Makro (master lifetime)
 *   Six  Umk  — Usul Masa Dikawal (six controlled divisions)
 * ============================================================ */

#ifndef QXK24_USUL_MASA_H
#define QXK24_USUL_MASA_H

#include "../qxk24.h"
#include "qxk24_fm.h"
#include "qxk24_mp.h"
#include "qxk24_mdk.h"
#include "qxk24_ahli_masa.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Usul Masa constants ── */
#define QXK24_USUL_UMM_COUNT   1    /* one Umm per process     */
#define QXK24_USUL_UMK_COUNT   6    /* six Umk per Umm         */

/* ── Single Umk record ── */
typedef struct {
    uint8_t         index;          /* 0–5                     */
    QXK24Direction  direction;      /* constitutional direction */
    float           value;          /* Umk current value       */
    float           energy;         /* energy carried by Umk   */
    bool            active;
    uint64_t        cycles;
} QXK24UmkRecord;

/* ── Full Usul Masa record ── */
typedef struct {
    /* Umm — master */
    float       umm_value;          /* master time value = 1.0 */
    uint64_t    umm_start_ms;
    uint64_t    umm_elapsed_ms;
    bool        umm_active;

    /* Six Umk */
    QXK24UmkRecord  umk[QXK24_USUL_UMK_COUNT];
    uint32_t        umk_active_count;

    /* Combined */
    float       total_energy;       /* sum of all Umk energies  */
    float       constitutional_score; /* 0.0 – 1.0              */
    uint64_t    total_cycles;
} QXK24UsulMasaRecord;

/* ── Usul Masa handle ── */
typedef struct QXK24UsulMasa *qxk24_usul_t;

/* ── Public API ── */

/**
 * qxk24_usul_create
 * Create a Usul Masa instance for a process.
 * @param  mp   parent MPi handle
 * @param  mdk  MDK set handle
 * @param  xam  Ahli Masa handle
 */
qxk24_usul_t qxk24_usul_create(
    qxk24_mp_t  mp,
    qxk24_mdk_t mdk,
    qxk24_xam_t xam
);

/**
 * qxk24_usul_start
 * Start the Usul Masa — activates Umm and all six Umk.
 */
qxk24_err_t qxk24_usul_start(qxk24_usul_t usul);

/**
 * qxk24_usul_cycle
 * Run one Usul Masa cycle.
 * Advances Umm, updates all six Umk, moves Ahli Masa.
 * @param  usul    Usul Masa handle
 * @param  energy  current kernel energy (X = m/t)
 */
qxk24_err_t qxk24_usul_cycle(
    qxk24_usul_t usul,
    float        energy
);

/**
 * qxk24_usul_stop
 * Stop the Usul Masa.
 */
void qxk24_usul_stop(qxk24_usul_t usul);

/**
 * qxk24_usul_destroy
 * Destroy the Usul Masa instance.
 */
void qxk24_usul_destroy(qxk24_usul_t usul);

/**
 * qxk24_usul_record
 * Retrieve current Usul Masa state.
 */
qxk24_err_t qxk24_usul_record(
    qxk24_usul_t        usul,
    QXK24UsulMasaRecord *out
);

/**
 * qxk24_usul_constitutional_score
 * Return the constitutional time score (0.0 – 1.0).
 * 1.0 = all six Umk active and energised.
 */
float qxk24_usul_constitutional_score(qxk24_usul_t usul);

/**
 * qxk24_usul_umk_energy
 * Return energy carried by a specific Umk (0–5).
 */
float qxk24_usul_umk_energy(
    qxk24_usul_t usul,
    uint8_t      index
);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_USUL_MASA_H */
