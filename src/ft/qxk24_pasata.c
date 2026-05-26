/* ============================================================
 * qxk24_pasata.c
 * QXK24 Universal Kernel — PASATA Implementation
 * ============================================================ */

#include "qxk24/ft/qxk24_pasata.h"
#include <stdlib.h>
#include <string.h>

struct QXK24PasataImpl {
    qxk24_ft_t ft;
    QXK24PasataSnapshot snap;
};

static qxk24_err_t fill_level(uint8_t level, QXK24PasataLevel *out) {
    if (out == NULL) return QXK24_ERR_NULL_PARAM;
    if (level < QXK24_PASATA_MIN_LEVEL || level > QXK24_PASATA_MAX_LEVEL) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    out->level = level;
    out->passive_count = (uint8_t)(level + 1U);
    out->active_count = level;
    out->wave_x = level;
    out->wave_y = level;
    out->energy_value = qxk24_pasata_compute_energy(level, 1U);
    return QXK24_OK;
}

qxk24_err_t qxk24_pasata_create(qxk24_ft_t ft,
                                 uint8_t initial_level,
                                 qxk24_pasata_t *pasata_out) {
    if (ft == NULL || pasata_out == NULL) return QXK24_ERR_NULL_PARAM;
    QXK24PasataLevel info;
    qxk24_err_t err = fill_level(initial_level, &info);
    if (err != QXK24_OK) return err;

    struct QXK24PasataImpl *p = (struct QXK24PasataImpl *)calloc(1, sizeof(*p));
    if (p == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    p->ft = ft;
    p->snap.current_level = initial_level;
    p->snap.passive_count = info.passive_count;
    p->snap.active_count = info.active_count;
    p->snap.law_satisfied = true;
    p->snap.wave_amplitude = (float)initial_level;
    *pasata_out = p;
    return QXK24_OK;
}

qxk24_err_t qxk24_pasata_cycle(qxk24_pasata_t pasata) {
    if (pasata == NULL) return QXK24_ERR_NULL_PARAM;
    pasata->snap.passive_count = (uint8_t)(pasata->snap.current_level + 1U);
    pasata->snap.active_count = pasata->snap.current_level;
    pasata->snap.law_satisfied =
        pasata->snap.passive_count ==
        (uint8_t)(pasata->snap.active_count + QXK24_PASATA_DELTA);
    pasata->snap.energy_total +=
        qxk24_pasata_compute_energy(pasata->snap.current_level, 1U);
    return QXK24_OK;
}

qxk24_err_t qxk24_pasata_advance(qxk24_pasata_t pasata) {
    if (pasata == NULL) return QXK24_ERR_NULL_PARAM;
    if (pasata->snap.current_level < QXK24_PASATA_MAX_LEVEL) {
        pasata->snap.current_level++;
        pasata->snap.wave_amplitude = (float)pasata->snap.current_level;
    }
    return qxk24_pasata_cycle(pasata);
}

qxk24_err_t qxk24_pasata_snapshot(qxk24_pasata_t pasata,
                                   QXK24PasataSnapshot *snap_out) {
    if (pasata == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = pasata->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_pasata_destroy(qxk24_pasata_t pasata) {
    if (pasata == NULL) return QXK24_ERR_NULL_PARAM;
    memset(pasata, 0, sizeof(*pasata));
    free(pasata);
    return QXK24_OK;
}

qxk24_err_t qxk24_pasata_level_info(uint8_t level,
                                     QXK24PasataLevel *info_out) {
    return fill_level(level, info_out);
}

qxk24_err_t qxk24_pasata_check_law(qxk24_pasata_t pasata) {
    if (pasata == NULL) return QXK24_ERR_NULL_PARAM;
    return pasata->snap.law_satisfied ? QXK24_OK : QXK24_ERR_LAW_VIOLATION;
}

float qxk24_pasata_compute_energy(uint8_t level, uint32_t wave_count) {
    return (float)wave_count * (float)(level + 1U) * (float)level;
}
