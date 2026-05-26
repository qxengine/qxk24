/* ============================================================
 * QXK24 – Universal Kernel
 * src/fm/qxk24_fm.c
 *
 * Faktor Masa (Fm) Engine Implementation
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 * Fm = MP / (MDK) = 1 / (2/4) = 2.0
 * ============================================================ */

#include "qxk24/fm/qxk24_fm.h"
#include "qxk24/fm/qxk24_mp.h"
#include "qxk24/fm/qxk24_mdk.h"
#include "qxk24/fm/qxk24_ahli_masa.h"
#include "qxk24/fm/qxk24_usul_masa.h"
#include <stdlib.h>
#include <time.h>

/* ── Internal Fm engine struct ── */
struct QXK24FmEngine {
    qxk24_kernel_t  kernel;
    qxk24_mp_t      mpi;        /* Masa Pengawal Induk        */
    qxk24_mdk_t     mdk;        /* six Masa Dikawal           */
    qxk24_xam_t     xam;        /* Ahli Masa                  */
    qxk24_usul_t    usul;       /* Usul Masa                  */

    float           energy_prev;
    float           energy_curr;
    QXK24FmFlowLaw  flow_law;
    uint64_t        cycles;
    bool            running;
};

/* ── Category name table ── */
static const char *s_cat_names[3] = {
    "MPi", "MPP", "MDK"
};

/* ── Internal: wall-clock ms ── */
static uint64_t s_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL
         + (uint64_t)(ts.tv_nsec / 1000000ULL);
}

/* ────────────────────────────────────────────────────────────
 * qxk24_fm_value
 * Fm = 1 / (2/4) = 1 / 0.5 = 2.0
 * ──────────────────────────────────────────────────────────── */
float qxk24_fm_value(void) {
    return (float)QXK24_FM_MP_VALUE /
           ((float)QXK24_FM_MDK_PAIR /
            (float)QXK24_FM_MDK_DIRECTION);
}

/* ────────────────────────────────────────────────────────────
 * qxk24_fm_category_name
 * ──────────────────────────────────────────────────────────── */
const char *qxk24_fm_category_name(QXK24FmCategory cat) {
    if ((int)cat < 0 || (int)cat > 2) return "UNKNOWN";
    return s_cat_names[(int)cat];
}

/* ────────────────────────────────────────────────────────────
 * qxk24_fm_flow_law
 * energy↑ → LAW_ENERGY_UP
 * energy↓ → LAW_ENERGY_DOWN
 * equal   → LAW_ENERGY_WAVE
 * ──────────────────────────────────────────────────────────── */
QXK24FmFlowLaw qxk24_fm_flow_law(float energy_prev, float energy_curr) {
    float diff = energy_curr - energy_prev;
    if (diff >  0.001f) return QXK24_FM_LAW_ENERGY_UP;
    if (diff < -0.001f) return QXK24_FM_LAW_ENERGY_DOWN;
    return QXK24_FM_LAW_ENERGY_WAVE;
}

/* ────────────────────────────────────────────────────────────
 * qxk24_fm_luman_scale
 * Z pattern: 6^(level-1)
 * X pattern: 4^(level-1)
 * ──────────────────────────────────────────────────────────── */
uint64_t qxk24_fm_luman_scale(uint8_t pattern, uint8_t level) {
    if (level == 0 || level > 7) return 0;
    uint64_t base = (pattern == QXK24_FM_LUMAN_Z) ? 6ULL : 4ULL;
    uint64_t result = 1ULL;
    for (uint8_t i = 0; i < (uint8_t)(level - 1); i++) result *= base;
    return result;
}

/* ────────────────────────────────────────────────────────────
 * qxk24_fm_create
 * ──────────────────────────────────────────────────────────── */
qxk24_fm_t qxk24_fm_create(qxk24_kernel_t kernel) {
    if (!kernel) return NULL;

    struct QXK24FmEngine *fm =
        (struct QXK24FmEngine *)calloc(1, sizeof(struct QXK24FmEngine));
    if (!fm) return NULL;

    fm->kernel = kernel;
    fm->mpi = qxk24_mp_create_mpi();
    if (!fm->mpi) { free(fm); return NULL; }

    fm->mdk = qxk24_mdk_create(fm->mpi);
    if (!fm->mdk) {
        qxk24_mp_destroy(fm->mpi);
        free(fm);
        return NULL;
    }

    fm->xam = qxk24_xam_create();
    if (!fm->xam) {
        qxk24_mdk_destroy(fm->mdk);
        qxk24_mp_destroy(fm->mpi);
        free(fm);
        return NULL;
    }

    fm->usul = qxk24_usul_create(fm->mpi, fm->mdk, fm->xam);
    if (!fm->usul) {
        qxk24_xam_destroy(fm->xam);
        qxk24_mdk_destroy(fm->mdk);
        qxk24_mp_destroy(fm->mpi);
        free(fm);
        return NULL;
    }

    fm->flow_law = QXK24_FM_LAW_ENERGY_WAVE;
    return (qxk24_fm_t)fm;
}

/* ────────────────────────────────────────────────────────────
 * qxk24_fm_start
 * ──────────────────────────────────────────────────────────── */
qxk24_err_t qxk24_fm_start(qxk24_fm_t fm) {
    if (!fm) return QXK24_ERR_NULL_PARAM;
    struct QXK24FmEngine *self = (struct QXK24FmEngine *)fm;
    if (self->running) return QXK24_OK;

    qxk24_err_t err = qxk24_mp_start(self->mpi);
    if (err != QXK24_OK) return err;

    err = qxk24_mdk_start(self->mdk);
    if (err != QXK24_OK) return err;

    err = qxk24_usul_start(self->usul);
    if (err != QXK24_OK) return err;

    self->running = true;
    return QXK24_OK;
}

/* ────────────────────────────────────────────────────────────
 * qxk24_fm_cycle
 * ──────────────────────────────────────────────────────────── */
qxk24_err_t qxk24_fm_cycle(qxk24_fm_t fm) {
    if (!fm) return QXK24_ERR_NULL_PARAM;
    struct QXK24FmEngine *self = (struct QXK24FmEngine *)fm;
    if (!self->running) return QXK24_ERR_NOT_STARTED;

    qxk24_err_t err = qxk24_mp_cycle(self->mpi);
    if (err != QXK24_OK) return err;

    self->energy_prev = self->energy_curr;
    self->energy_curr = (float)(qxk24_health_score(self->kernel) / 100.0);
    self->flow_law = qxk24_fm_flow_law(self->energy_prev, self->energy_curr);

    err = qxk24_mdk_cycle(self->mdk, self->energy_curr);
    if (err != QXK24_OK) return err;

    err = qxk24_xam_move(self->xam);
    if (err != QXK24_OK) return err;
    if (qxk24_xam_is_cycle_complete(self->xam)) {
        err = qxk24_xam_reset(self->xam);
        if (err != QXK24_OK) return err;
    }

    err = qxk24_usul_cycle(self->usul, self->energy_curr);
    if (err != QXK24_OK) return err;

    self->cycles++;
    return QXK24_OK;
}

void qxk24_fm_stop(qxk24_fm_t fm) {
    if (!fm) return;
    struct QXK24FmEngine *self = (struct QXK24FmEngine *)fm;
    qxk24_mp_stop(self->mpi);
    qxk24_mdk_stop(self->mdk);
    qxk24_usul_stop(self->usul);
    self->running = false;
}

void qxk24_fm_destroy(qxk24_fm_t fm) {
    if (!fm) return;
    struct QXK24FmEngine *self = (struct QXK24FmEngine *)fm;
    qxk24_usul_destroy(self->usul);
    qxk24_xam_destroy(self->xam);
    qxk24_mdk_destroy(self->mdk);
    qxk24_mp_destroy(self->mpi);
    free(self);
}

qxk24_err_t qxk24_fm_snapshot(qxk24_fm_t fm, QXK24FmSnapshot *out) {
    if (!fm || !out) return QXK24_ERR_NULL_PARAM;
    struct QXK24FmEngine *self = (struct QXK24FmEngine *)fm;

    out->fm_value = qxk24_fm_value();
    out->mp_ratio = qxk24_mp_ratio(self->mpi);
    out->mdk_ratio = (float)qxk24_mdk_active_count(self->mdk) /
        (float)QXK24_FM_MDK_COUNT;
    out->energy_factor = self->energy_curr;
    out->flow_law = self->flow_law;
    out->cycle_index = self->cycles;
    out->timestamp_ms = s_now_ms();
    out->constitutionally_timed = (out->mdk_ratio >= 1.0f) && self->running;
    return QXK24_OK;
}

bool qxk24_fm_is_running(qxk24_fm_t fm) {
    if (!fm) return false;
    return ((struct QXK24FmEngine *)fm)->running;
}
