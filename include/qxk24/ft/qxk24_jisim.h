/* ============================================================
 * qxk24_jisim.h
 * QXK24 Universal Kernel — Jisim Tenaga
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/ft/qxk24_jisim.h
 * License : Apache 2.0
 *
 * Description:
 * Declares K24-level energy mass flow and recycled flow counts.
 * ============================================================ */

#ifndef QXK24_JISIM_H
#define QXK24_JISIM_H

#include "qxk24_ft.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uint32_t QXK24_JISIM_FLOW_COUNT[7];

typedef struct QXK24JisimFlow_s {
    uint32_t flow_index;
    uint32_t from_picu;
    uint32_t to_picu;
    uint32_t kj_from;
    uint32_t kj_to;
    bool is_upper;
} QXK24JisimFlow;

typedef struct QXK24JisimSnapshot_s {
    uint32_t level;
    uint32_t flows_per_dir;
    uint32_t total_flows;
    uint32_t aj_completed;
    uint32_t kj_completed;
    float flow_efficiency;
} QXK24JisimSnapshot;

typedef struct QXK24JisimImpl *qxk24_jisim_t;

qxk24_err_t qxk24_jisim_create(qxk24_ft_t ft,
                                uint32_t k24_level,
                                qxk24_jisim_t *jisim_out);
qxk24_err_t qxk24_jisim_cycle(qxk24_jisim_t jisim);
qxk24_err_t qxk24_jisim_snapshot(qxk24_jisim_t jisim,
                                  QXK24JisimSnapshot *snap_out);
qxk24_err_t qxk24_jisim_destroy(qxk24_jisim_t jisim);
uint32_t qxk24_jisim_flow_count(uint32_t k24_level);
qxk24_err_t qxk24_jisim_check_constitutional_capacity(uint32_t k24_level,
                                                       uint32_t actual);
float qxk24_jisim_efficiency(uint32_t aj, uint32_t kj);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_JISIM_H */
