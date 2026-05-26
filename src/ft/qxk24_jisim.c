/* ============================================================
 * qxk24_jisim.c
 * QXK24 Universal Kernel — Jisim Tenaga Implementation
 * ============================================================ */

#include "qxk24/ft/qxk24_jisim.h"
#include <stdlib.h>
#include <string.h>

const uint32_t QXK24_JISIM_FLOW_COUNT[7] = {1, 3, 10, 35, 126, 462, 1716};

struct QXK24JisimImpl {
    qxk24_ft_t ft;
    QXK24JisimSnapshot snap;
};

uint32_t qxk24_jisim_flow_count(uint32_t k24_level) {
    return k24_level < QXK24_LEVEL_COUNT ? QXK24_JISIM_FLOW_COUNT[k24_level] : 0U;
}

float qxk24_jisim_efficiency(uint32_t aj, uint32_t kj) {
    const uint32_t total = aj + kj;
    return total == 0U ? 0.0f : (float)aj / (float)total;
}

qxk24_err_t qxk24_jisim_check_constitutional_capacity(uint32_t k24_level,
                                                       uint32_t actual) {
    const uint32_t expected = qxk24_jisim_flow_count(k24_level);
    if (expected == 0U) return QXK24_ERR_INVALID_ARGUMENT;
    return actual >= expected ? QXK24_OK : QXK24_ERR_CAPACITY;
}

qxk24_err_t qxk24_jisim_create(qxk24_ft_t ft,
                                uint32_t k24_level,
                                qxk24_jisim_t *jisim_out) {
    if (ft == NULL || jisim_out == NULL) return QXK24_ERR_NULL_PARAM;
    if (k24_level >= QXK24_LEVEL_COUNT) return QXK24_ERR_INVALID_ARGUMENT;

    struct QXK24JisimImpl *j = (struct QXK24JisimImpl *)calloc(1, sizeof(*j));
    if (j == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    j->ft = ft;
    j->snap.level = k24_level;
    j->snap.flows_per_dir = qxk24_jisim_flow_count(k24_level);
    j->snap.total_flows = j->snap.flows_per_dir * 4U;
    *jisim_out = j;
    return QXK24_OK;
}

qxk24_err_t qxk24_jisim_cycle(qxk24_jisim_t jisim) {
    if (jisim == NULL) return QXK24_ERR_NULL_PARAM;
    if (jisim->snap.aj_completed < jisim->snap.total_flows) {
        jisim->snap.aj_completed++;
    }
    if ((jisim->snap.aj_completed % 2U) == 0U) {
        jisim->snap.kj_completed++;
    }
    jisim->snap.flow_efficiency =
        qxk24_jisim_efficiency(jisim->snap.aj_completed,
            jisim->snap.kj_completed);
    return QXK24_OK;
}

qxk24_err_t qxk24_jisim_snapshot(qxk24_jisim_t jisim,
                                  QXK24JisimSnapshot *snap_out) {
    if (jisim == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = jisim->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_jisim_destroy(qxk24_jisim_t jisim) {
    if (jisim == NULL) return QXK24_ERR_NULL_PARAM;
    memset(jisim, 0, sizeof(*jisim));
    free(jisim);
    return QXK24_OK;
}
