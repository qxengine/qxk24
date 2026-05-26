/* ============================================================
 * qxk24_fasa_tenaga.c
 * QXK24 Universal Kernel — Fasa Tenaga Implementation
 * ============================================================ */

#include "qxk24/ft/qxk24_fasa_tenaga.h"
#include <stdlib.h>
#include <string.h>

const uint32_t QXK24_FASA_ABA_COUNT[7] = {1, 6, 19, 44, 85, 146, 231};
const uint32_t QXK24_FASA_LAYER_COUNT[7] = {1, 3, 5, 7, 9, 11, 13};

struct QXK24FasaImpl {
    qxk24_ft_t ft;
    QXK24FasaSnapshot snap;
};

float qxk24_fasa_energy_capacity(QXK24FtPhase phase) {
    if ((uint32_t)phase >= QXK24_FT_PHASES) return 0.0f;
    const float weight = 1.0f + ((float)phase * 0.25f);
    return (float)QXK24_FASA_ABA_COUNT[(uint32_t)phase] *
        QXK24_FM_VALUE * weight;
}

qxk24_err_t qxk24_fasa_descriptor(QXK24FtPhase phase,
                                   QXK24FasaTenagaDescriptor *desc_out) {
    if (desc_out == NULL) return QXK24_ERR_NULL_PARAM;
    if ((uint32_t)phase >= QXK24_FT_PHASES) return QXK24_ERR_INVALID_ARGUMENT;

    const uint32_t idx = (uint32_t)phase;
    desc_out->phase = phase;
    desc_out->aba_count = QXK24_FASA_ABA_COUNT[idx];
    desc_out->layer_count = QXK24_FASA_LAYER_COUNT[idx];
    desc_out->teras_aba = (uint32_t)((idx + 1U) * (idx + 1U));
    desc_out->atas_layers = desc_out->layer_count / 2U;
    desc_out->bawah_layers = desc_out->layer_count / 2U;
    desc_out->energy_capacity = qxk24_fasa_energy_capacity(phase);
    return QXK24_OK;
}

qxk24_err_t qxk24_fasa_create(qxk24_ft_t ft, qxk24_fasa_t *fasa_out) {
    if (ft == NULL || fasa_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24FasaImpl *f = (struct QXK24FasaImpl *)calloc(1, sizeof(*f));
    if (f == NULL) return QXK24_ERR_OUT_OF_MEMORY;

    f->ft = ft;
    f->snap.current_phase = QXK24_FT_PHASE_PESA;
    f->snap.aba_active = QXK24_FASA_ABA_COUNT[0];
    f->snap.energy_capacity = qxk24_fasa_energy_capacity(QXK24_FT_PHASE_PESA);
    f->snap.energy_remaining = f->snap.energy_capacity;
    *fasa_out = f;
    return QXK24_OK;
}

qxk24_err_t qxk24_fasa_cycle(qxk24_fasa_t fasa) {
    if (fasa == NULL) return QXK24_ERR_NULL_PARAM;
    fasa->snap.energy_used += 1.0f;
    if (fasa->snap.energy_used > fasa->snap.energy_capacity) {
        fasa->snap.energy_used = fasa->snap.energy_capacity;
    }
    fasa->snap.energy_remaining =
        fasa->snap.energy_capacity - fasa->snap.energy_used;
    fasa->snap.active_flow =
        (QXK24FasaFlowType)(((uint32_t)fasa->snap.active_flow + 1U) % 3U);
    return QXK24_OK;
}

qxk24_err_t qxk24_fasa_advance_phase(qxk24_fasa_t fasa) {
    if (fasa == NULL) return QXK24_ERR_NULL_PARAM;
    if (fasa->snap.current_phase < QXK24_FT_PHASE_PETU) {
        fasa->snap.current_phase =
            (QXK24FtPhase)((uint32_t)fasa->snap.current_phase + 1U);
    }
    const uint32_t idx = (uint32_t)fasa->snap.current_phase;
    fasa->snap.aba_active = QXK24_FASA_ABA_COUNT[idx];
    fasa->snap.energy_capacity =
        qxk24_fasa_energy_capacity(fasa->snap.current_phase);
    fasa->snap.energy_used = 0.0f;
    fasa->snap.energy_remaining = fasa->snap.energy_capacity;
    return QXK24_OK;
}

qxk24_err_t qxk24_fasa_snapshot(qxk24_fasa_t fasa,
                                 QXK24FasaSnapshot *snap_out) {
    if (fasa == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = fasa->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_fasa_destroy(qxk24_fasa_t fasa) {
    if (fasa == NULL) return QXK24_ERR_NULL_PARAM;
    memset(fasa, 0, sizeof(*fasa));
    free(fasa);
    return QXK24_OK;
}

const char *qxk24_fasa_phase_name(QXK24FtPhase phase) {
    return qxk24_ft_phase_name(phase);
}
