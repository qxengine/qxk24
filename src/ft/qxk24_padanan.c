/* ============================================================
 * qxk24_padanan.c
 * QXK24 Universal Kernel — Padanan Tenaga Implementation
 * ============================================================ */

#include "qxk24/ft/qxk24_padanan.h"
#include <stdlib.h>
#include <string.h>

struct QXK24PadananImpl {
    qxk24_ft_t ft;
    QXK24PadananSnapshot snap;
};

static const QXK24FtElement k_dom_elements[4] = {
    QXK24_FT_ELEMENT_API,
    QXK24_FT_ELEMENT_TANAH,
    QXK24_FT_ELEMENT_AIR,
    QXK24_FT_ELEMENT_ANGIN
};

qxk24_err_t qxk24_padanan_create(qxk24_ft_t ft,
                                  QXK24PadananDominant dominant,
                                  qxk24_padanan_t *padanan_out) {
    if (ft == NULL || padanan_out == NULL) return QXK24_ERR_NULL_PARAM;
    if ((uint32_t)dominant >= QXK24_PADANAN_ELEMENTS) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    struct QXK24PadananImpl *p =
        (struct QXK24PadananImpl *)calloc(1, sizeof(*p));
    if (p == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    p->ft = ft;
    p->snap.dominant = dominant;
    p->snap.rotation = QXK24_FT_ROTATION_XKR;
    p->snap.energy_value = 1.0f;
    *padanan_out = p;
    return QXK24_OK;
}

qxk24_err_t qxk24_padanan_cycle(qxk24_padanan_t padanan) {
    if (padanan == NULL) return QXK24_ERR_NULL_PARAM;
    padanan->snap.gl_count++;
    padanan->snap.gg_count++;
    padanan->snap.current_process =
        (QXK24PadananProcess)(padanan->snap.gg_count /
            QXK24_PADANAN_MOVEMENTS);
    if (padanan->snap.current_process >= QXK24_PADANAN_PROCESSES) {
        padanan->snap.current_process = QXK24_PADANAN_PROC_PA;
        padanan->snap.cycle_complete = true;
    }
    padanan->snap.energy_value = 1.0f +
        ((float)padanan->snap.gg_count / (float)QXK24_PADANAN_TOTAL_GL);
    padanan->snap.rotation = (padanan->snap.gg_count % 2U) == 0U
        ? QXK24_FT_ROTATION_XKR
        : QXK24_FT_ROTATION_XKN;
    return QXK24_OK;
}

qxk24_err_t qxk24_padanan_snapshot(qxk24_padanan_t padanan,
                                    QXK24PadananSnapshot *snap_out) {
    if (padanan == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = padanan->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_padanan_destroy(qxk24_padanan_t padanan) {
    if (padanan == NULL) return QXK24_ERR_NULL_PARAM;
    memset(padanan, 0, sizeof(*padanan));
    free(padanan);
    return QXK24_OK;
}

qxk24_err_t qxk24_padanan_movements(QXK24PadananDominant dominant,
                                     QXK24PadananProcess process,
                                     QXK24PadananMovement movements_out[4]) {
    if (movements_out == NULL) return QXK24_ERR_NULL_PARAM;
    if ((uint32_t)dominant >= QXK24_PADANAN_ELEMENTS ||
        (uint32_t)process >= QXK24_PADANAN_PROCESSES) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    for (uint8_t i = 0; i < QXK24_PADANAN_MOVEMENTS; i++) {
        movements_out[i].element = (i == 0U) ? k_dom_elements[dominant] :
            (QXK24FtElement)((dominant + i) % QXK24_PADANAN_ELEMENTS);
        movements_out[i].from_pos = (uint8_t)(i + 1U);
        movements_out[i].to_pos = (uint8_t)(((i + 1U) %
            QXK24_PADANAN_COORDINATES) + 1U);
        movements_out[i].gl_index = (uint32_t)(i + 1U);
        movements_out[i].gg_index =
            ((uint32_t)process * QXK24_PADANAN_MOVEMENTS) + i + 1U;
    }
    return QXK24_OK;
}

uint32_t qxk24_padanan_energy_ratio(bool is_cognitive) {
    return is_cognitive ? QXK24_PADANAN_BRAIN_RATIO : QXK24_PADANAN_BODY_RATIO;
}

const char *qxk24_padanan_dominant_name(QXK24PadananDominant dom) {
    switch (dom) {
    case QXK24_PADANAN_DOM_API: return "Ai2ThArAn";
    case QXK24_PADANAN_DOM_TANAH: return "Th2ArAnAi";
    case QXK24_PADANAN_DOM_AIR: return "Ar2AnThAi";
    case QXK24_PADANAN_DOM_ANGIN: return "An2AiThAr";
    default: return "Unknown";
    }
}

const char *qxk24_padanan_process_name(QXK24PadananProcess proc) {
    switch (proc) {
    case QXK24_PADANAN_PROC_SA: return "Sa";
    case QXK24_PADANAN_PROC_DU: return "Du";
    case QXK24_PADANAN_PROC_GA: return "Ga";
    case QXK24_PADANAN_PROC_PA: return "Pa";
    default: return "Unknown";
    }
}
