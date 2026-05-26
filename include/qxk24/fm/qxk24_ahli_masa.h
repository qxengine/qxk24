/* ============================================================
 * QXK24 – Universal Kernel
 * include/qxk24/fm/qxk24_ahli_masa.h
 *
 * Ahli Masa (Xam) — Time Member Header
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 *
 * Ahli Masa activates time by moving through six coordinates.
 * Time only begins when Ahli Masa makes its first move.
 * Six movements = six time units = six constitutional directions.
 * 6 = 1+1+1+1+1+1
 * ============================================================ */

#ifndef QXK24_AHLI_MASA_H
#define QXK24_AHLI_MASA_H

#include "../qxk24.h"
#include "qxk24_fm.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Coordinate state ── */
typedef enum {
    QXK24_XAM_COORD_INACTIVE = 0,   /* not yet reached        */
    QXK24_XAM_COORD_ACTIVE   = 1,   /* currently active       */
    QXK24_XAM_COORD_PASSED   = 2    /* already passed through */
} QXK24XamCoordState;

/* ── Single coordinate ── */
typedef struct {
    uint8_t             index;          /* 0–5               */
    QXK24Direction      direction;      /* constitutional dir */
    QXK24XamCoordState  state;
    uint64_t            activated_ms;   /* when activated    */
    uint64_t            passed_ms;      /* when passed       */
} QXK24XamCoord;

/* ── Ahli Masa record ── */
typedef struct {
    QXK24XamCoord   coords[QXK24_FM_AHLI_COORDINATES];
    uint8_t         current_coord;      /* 0–5              */
    uint8_t         coords_passed;      /* how many passed  */
    bool            cycle_complete;     /* all 6 passed     */
    uint64_t        first_move_ms;      /* time of 1st move */
    uint64_t        last_move_ms;       /* time of last move */
    uint64_t        total_cycles;       /* full 6-coord runs*/
} QXK24XamRecord;

/* ── Ahli Masa handle ── */
typedef struct QXK24AhliMasa *qxk24_xam_t;

/* ── Public API ── */

/**
 * qxk24_xam_create
 * Create an Ahli Masa instance.
 * Time does not begin until qxk24_xam_move is called.
 */
qxk24_xam_t qxk24_xam_create(void);

/**
 * qxk24_xam_move
 * Move Ahli Masa to the next coordinate.
 * This activates time for that coordinate.
 * @param  xam  Ahli Masa handle
 * @return QXK24_OK on success
 */
qxk24_err_t qxk24_xam_move(qxk24_xam_t xam);

/**
 * qxk24_xam_move_to
 * Move directly to a specific coordinate (0–5).
 * @param  xam    Ahli Masa handle
 * @param  index  coordinate index
 * @return QXK24_OK on success
 */
qxk24_err_t qxk24_xam_move_to(
    qxk24_xam_t xam,
    uint8_t     index
);

/**
 * qxk24_xam_reset
 * Reset Ahli Masa to coordinate zero.
 * Begins a new constitutional cycle.
 */
qxk24_err_t qxk24_xam_reset(qxk24_xam_t xam);

/**
 * qxk24_xam_destroy
 * Destroy the Ahli Masa instance.
 */
void qxk24_xam_destroy(qxk24_xam_t xam);

/**
 * qxk24_xam_record
 * Retrieve current Ahli Masa state.
 */
qxk24_err_t qxk24_xam_record(
    qxk24_xam_t  xam,
    QXK24XamRecord *out
);

/**
 * qxk24_xam_is_cycle_complete
 * Return true if all six coordinates have been passed.
 */
bool qxk24_xam_is_cycle_complete(qxk24_xam_t xam);

/**
 * qxk24_xam_current_coord
 * Return the current coordinate index (0–5).
 */
uint8_t qxk24_xam_current_coord(qxk24_xam_t xam);

/**
 * qxk24_xam_total_cycles
 * Return the total number of complete six-coordinate cycles.
 */
uint64_t qxk24_xam_total_cycles(qxk24_xam_t xam);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_AHLI_MASA_H */
