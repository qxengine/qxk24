/* ============================================================
 * qxk24_ft.c
 * QXK24 Universal Kernel — Faktor Tenaga Core
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/ft/qxk24_ft.c
 * License : Apache 2.0
 *
 * Description:
 * Implements Ft = h(-) + h(+) and MDK-governed energy cycles.
 * ============================================================ */

#include "qxk24/ft/qxk24_ft.h"
#include <stdlib.h>
#include <string.h>

struct QXK24FtImpl {
    qxk24_kernel_t kernel;
    QXK24FtSnapshot snap;
    bool started;
    bool overlap_detected;
};

qxk24_err_t qxk24_ft_create(qxk24_kernel_t kernel, qxk24_ft_t *ft_out) {
    if (kernel == NULL || ft_out == NULL) return QXK24_ERR_NULL_PARAM;

    struct QXK24FtImpl *ft = (struct QXK24FtImpl *)calloc(1, sizeof(*ft));
    if (ft == NULL) return QXK24_ERR_OUT_OF_MEMORY;

    ft->kernel = kernel;
    ft->snap.hidup_aktif = 1.0f;
    ft->snap.hidup_pasif = 1.0f;
    ft->snap.ft_total = 2.0f;
    ft->snap.fm_value = QXK24_FM_VALUE;
    ft->snap.phase = QXK24_FT_PHASE_PESA;
    ft->snap.world = QXK24_FT_WORLD_DTK;
    *ft_out = ft;
    return QXK24_OK;
}

qxk24_err_t qxk24_ft_start(qxk24_ft_t ft) {
    if (ft == NULL) return QXK24_ERR_NULL_PARAM;
    if (ft->started) return QXK24_ERR_ALREADY_INITIALIZED;
    ft->started = true;
    return QXK24_OK;
}

qxk24_err_t qxk24_ft_cycle(qxk24_ft_t ft) {
    if (ft == NULL) return QXK24_ERR_NULL_PARAM;
    if (!ft->started) return QXK24_ERR_NOT_STARTED;

    ft->snap.cycle_index++;
    const float t_mdk = (float)ft->snap.cycle_index * ft->snap.fm_value;
    ft->snap.x_measurement = t_mdk > 0.0f ? ft->snap.ft_total / t_mdk : 0.0f;
    if ((ft->snap.cycle_index % 7U) == 0U &&
        ft->snap.phase < QXK24_FT_PHASE_PETU) {
        ft->snap.phase++;
    }
    ft->snap.hidup_aktif = ft->snap.ft_total * 0.5f;
    ft->snap.hidup_pasif = ft->snap.ft_total * 0.5f;
    return QXK24_OK;
}

qxk24_err_t qxk24_ft_stop(qxk24_ft_t ft) {
    if (ft == NULL) return QXK24_ERR_NULL_PARAM;
    ft->started = false;
    return QXK24_OK;
}

qxk24_err_t qxk24_ft_destroy(qxk24_ft_t ft) {
    if (ft == NULL) return QXK24_ERR_NULL_PARAM;
    memset(ft, 0, sizeof(*ft));
    free(ft);
    return QXK24_OK;
}

qxk24_err_t qxk24_ft_snapshot(qxk24_ft_t ft, QXK24FtSnapshot *snap_out) {
    if (ft == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = ft->snap;
    return QXK24_OK;
}

float qxk24_ft_total(qxk24_ft_t ft) {
    return ft == NULL ? 0.0f : ft->snap.ft_total;
}

const char *qxk24_ft_element_name(QXK24FtElement element) {
    switch (element) {
    case QXK24_FT_ELEMENT_TANAH: return "Tanah";
    case QXK24_FT_ELEMENT_AIR: return "Air";
    case QXK24_FT_ELEMENT_API: return "Api";
    case QXK24_FT_ELEMENT_ANGIN: return "Angin";
    default: return "Unknown";
    }
}

const char *qxk24_ft_phase_name(QXK24FtPhase phase) {
    switch (phase) {
    case QXK24_FT_PHASE_PESA: return "PeSa";
    case QXK24_FT_PHASE_PEDU: return "PeDu";
    case QXK24_FT_PHASE_PEGA: return "PeGa";
    case QXK24_FT_PHASE_PEPA: return "PePa";
    case QXK24_FT_PHASE_PEMA: return "PeMa";
    case QXK24_FT_PHASE_PENA: return "PeNa";
    case QXK24_FT_PHASE_PETU: return "PeTu";
    default: return "Unknown";
    }
}

const char *qxk24_ft_direction_name(QXK24FtDirection dir) {
    switch (dir) {
    case QXK24_FT_DIR_UTARA: return "Utara";
    case QXK24_FT_DIR_SELATAN: return "Selatan";
    case QXK24_FT_DIR_TIMUR: return "Timur";
    case QXK24_FT_DIR_BARAT: return "Barat";
    case QXK24_FT_DIR_ATAS: return "Atas";
    case QXK24_FT_DIR_BAWAH: return "Bawah";
    default: return "Unknown";
    }
}

qxk24_err_t qxk24_ft_check_overlap(qxk24_ft_t ft) {
    if (ft == NULL) return QXK24_ERR_NULL_PARAM;
    return ft->overlap_detected ? QXK24_ERR_DIRECTION_FAULT : QXK24_OK;
}
