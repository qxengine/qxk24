/* ============================================================
 * qxk24_fasa_tenaga.h
 * QXK24 Universal Kernel — Fasa Tenaga
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/ft/qxk24_fasa_tenaga.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the seven energy phases and ABA cube capacities.
 * ============================================================ */

#ifndef QXK24_FASA_TENAGA_H
#define QXK24_FASA_TENAGA_H

#include "qxk24_ft.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uint32_t QXK24_FASA_ABA_COUNT[7];
extern const uint32_t QXK24_FASA_LAYER_COUNT[7];

typedef struct QXK24FasaTenagaDescriptor_s {
    QXK24FtPhase phase;
    uint32_t aba_count;
    uint32_t layer_count;
    uint32_t teras_aba;
    uint32_t atas_layers;
    uint32_t bawah_layers;
    float energy_capacity;
} QXK24FasaTenagaDescriptor;

typedef enum {
    QXK24_FASA_FLOW_X = 0,
    QXK24_FASA_FLOW_Y_POS = 1,
    QXK24_FASA_FLOW_Y_NEG = 2
} QXK24FasaFlowType;

typedef struct QXK24FasaSnapshot_s {
    QXK24FtPhase current_phase;
    uint32_t aba_active;
    float energy_capacity;
    float energy_used;
    float energy_remaining;
    QXK24FasaFlowType active_flow;
} QXK24FasaSnapshot;

typedef struct QXK24FasaImpl *qxk24_fasa_t;

qxk24_err_t qxk24_fasa_create(qxk24_ft_t ft, qxk24_fasa_t *fasa_out);
qxk24_err_t qxk24_fasa_cycle(qxk24_fasa_t fasa);
qxk24_err_t qxk24_fasa_advance_phase(qxk24_fasa_t fasa);
qxk24_err_t qxk24_fasa_snapshot(qxk24_fasa_t fasa,
                                 QXK24FasaSnapshot *snap_out);
qxk24_err_t qxk24_fasa_destroy(qxk24_fasa_t fasa);
qxk24_err_t qxk24_fasa_descriptor(QXK24FtPhase phase,
                                   QXK24FasaTenagaDescriptor *desc_out);
float qxk24_fasa_energy_capacity(QXK24FtPhase phase);
const char *qxk24_fasa_phase_name(QXK24FtPhase phase);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_FASA_TENAGA_H */
