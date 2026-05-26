/* ============================================================
 * QXK24 – Universal Kernel
 * include/qxk24/fm/qxk24_mdk.h
 *
 * Masa Dikawal (MDK) — Controlled Time Header
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 *
 * MDK is the slave to MP. Active, measurable, controllable.
 * MDK moves as a wave — up and down — within MP boundary.
 * Six MDK exist per MP — one per constitutional direction.
 * MDK can become MPP for the next level.
 * ============================================================ */

#ifndef QXK24_MDK_H
#define QXK24_MDK_H

#include "../qxk24.h"
#include "qxk24_fm.h"
#include "qxk24_mp.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── MDK wave state ── */
typedef enum {
    QXK24_MDK_WAVE_RISING  = 0,  /* moving upward   */
    QXK24_MDK_WAVE_FALLING = 1,  /* moving downward */
    QXK24_MDK_WAVE_PEAK    = 2,  /* at upper bound  */
    QXK24_MDK_WAVE_TROUGH  = 3   /* at lower bound  */
} QXK24MDKWaveState;

/* ── MDK record ── */
typedef struct {
    uint8_t             index;          /* 0–5 (six MDK)        */
    QXK24Direction      direction;      /* constitutional dir    */
    float               value;          /* current MDK value    */
    float               energy;         /* X = m/t at this MDK  */
    QXK24MDKWaveState   wave_state;     /* current wave state   */
    float               wave_amplitude; /* current amplitude    */
    bool                active;
    bool                can_be_mpp;     /* MDK → MPP transition */
    uint64_t            cycles;         /* cycles completed     */
} QXK24MDKRecord;

/* ── MDK set — six MDK per MP ── */
typedef struct {
    QXK24MDKRecord  mdk[QXK24_FM_MDK_COUNT];
    uint32_t        active_count;
    float           average_value;
    float           average_energy;
} QXK24MDKSet;

/* ── MDK handle ── */
typedef struct QXK24MDKEngine *qxk24_mdk_t;

/* ── Public API ── */

/**
 * qxk24_mdk_create
 * Create a set of six MDK governed by the given MP.
 * @param  mp  parent MP handle
 */
qxk24_mdk_t qxk24_mdk_create(qxk24_mp_t mp);

/**
 * qxk24_mdk_start
 * Activate all six MDK.
 */
qxk24_err_t qxk24_mdk_start(qxk24_mdk_t mdk);

/**
 * qxk24_mdk_cycle
 * Advance one MDK wave cycle.
 * Applies Fm flow law — energy governs wave movement.
 * @param  mdk       MDK handle
 * @param  energy    current X = m/t from kernel
 */
qxk24_err_t qxk24_mdk_cycle(
    qxk24_mdk_t mdk,
    float       energy
);

/**
 * qxk24_mdk_stop
 * Deactivate all six MDK.
 */
void qxk24_mdk_stop(qxk24_mdk_t mdk);

/**
 * qxk24_mdk_destroy
 * Destroy the MDK engine.
 */
void qxk24_mdk_destroy(qxk24_mdk_t mdk);

/**
 * qxk24_mdk_set
 * Retrieve the full MDK set snapshot.
 */
qxk24_err_t qxk24_mdk_set(
    qxk24_mdk_t  mdk,
    QXK24MDKSet  *out
);

/**
 * qxk24_mdk_record
 * Retrieve record for a single MDK by index (0–5).
 */
qxk24_err_t qxk24_mdk_record(
    qxk24_mdk_t   mdk,
    uint8_t       index,
    QXK24MDKRecord *out
);

/**
 * qxk24_mdk_wave_state
 * Return current wave state for MDK at index.
 */
QXK24MDKWaveState qxk24_mdk_wave_state(
    qxk24_mdk_t mdk,
    uint8_t     index
);

/**
 * qxk24_mdk_promote_to_mpp
 * Promote an MDK to become an MPP for the next level.
 * @param  mdk    MDK handle
 * @param  index  MDK index 0–5 to promote
 * @return new MPP handle
 */
qxk24_mp_t qxk24_mdk_promote_to_mpp(
    qxk24_mdk_t mdk,
    uint8_t     index
);

/**
 * qxk24_mdk_active_count
 * Return count of active MDK.
 */
uint32_t qxk24_mdk_active_count(qxk24_mdk_t mdk);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_MDK_H */
