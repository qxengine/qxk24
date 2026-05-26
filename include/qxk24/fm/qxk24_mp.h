/* ============================================================
 * QXK24 – Universal Kernel
 * include/qxk24/fm/qxk24_mp.h
 *
 * Masa Pengawal (MP) — Master Time Header
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 *
 * MP is the master time — passive, dominant, absolute.
 * MPi = Masa Pengawal Induk — one, uncontrollable.
 * MPP = Masa Pengawal Proses — many, controllable.
 * ============================================================ */

#ifndef QXK24_MP_H
#define QXK24_MP_H

#include "../qxk24.h"
#include "qxk24_fm.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── MP type ── */
typedef enum {
    QXK24_MP_INDUK   = 0,   /* MPi — absolute, one only         */
    QXK24_MP_PROSES  = 1    /* MPP — process, many allowed      */
} QXK24MPType;

/* ── MP record ── */
typedef struct {
    QXK24MPType type;
    uint64_t    start_ms;       /* constitutional start time      */
    uint64_t    elapsed_ms;     /* time elapsed since start       */
    float       ratio;          /* elapsed / total = constant 1.0 */
    bool        active;
    uint32_t    mdk_count;      /* how many MDK this MP governs   */
    char        name[32];
} QXK24MPRecord;

/* ── MP handle ── */
typedef struct QXK24MP *qxk24_mp_t;

/* ── Public API ── */

/**
 * qxk24_mp_create_mpi
 * Create the Masa Pengawal Induk (MPi) — absolute master.
 * Only one MPi should exist per kernel.
 */
qxk24_mp_t qxk24_mp_create_mpi(void);

/**
 * qxk24_mp_create_mpp
 * Create a Masa Pengawal Proses (MPP).
 * Many MPPs can exist — each governs its own MDK set.
 * @param  parent  parent MPi handle
 * @param  name    process name
 */
qxk24_mp_t qxk24_mp_create_mpp(
    qxk24_mp_t  parent,
    const char *name
);

/**
 * qxk24_mp_start
 * Start the MP clock.
 */
qxk24_err_t qxk24_mp_start(qxk24_mp_t mp);

/**
 * qxk24_mp_cycle
 * Advance one MP cycle — updates elapsed time.
 */
qxk24_err_t qxk24_mp_cycle(qxk24_mp_t mp);

/**
 * qxk24_mp_stop
 * Stop the MP clock.
 */
void qxk24_mp_stop(qxk24_mp_t mp);

/**
 * qxk24_mp_destroy
 * Destroy the MP instance.
 */
void qxk24_mp_destroy(qxk24_mp_t mp);

/**
 * qxk24_mp_record
 * Retrieve current MP state record.
 */
qxk24_err_t qxk24_mp_record(
    qxk24_mp_t   mp,
    QXK24MPRecord *out
);

/**
 * qxk24_mp_ratio
 * Return the MP ratio (always 1.0 for MPi — constant).
 */
float qxk24_mp_ratio(qxk24_mp_t mp);

/**
 * qxk24_mp_elapsed_ms
 * Return elapsed milliseconds since MP start.
 */
uint64_t qxk24_mp_elapsed_ms(qxk24_mp_t mp);

/**
 * qxk24_mp_type
 * Return MP type (MPi or MPP).
 */
QXK24MPType qxk24_mp_type(qxk24_mp_t mp);

/**
 * qxk24_mp_is_active
 * Return true if MP is active.
 */
bool qxk24_mp_is_active(qxk24_mp_t mp);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_MP_H */
