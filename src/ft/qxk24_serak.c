/* ============================================================
 * qxk24_serak.c
 * QXK24 Universal Kernel — Serak Tenaga Implementation
 * ============================================================ */

#include "qxk24/ft/qxk24_serak.h"
#include <stdlib.h>
#include <string.h>

const uint32_t QXK24_SERAK_MDK[7] = {4, 8, 12, 16, 20, 24, 28};
const uint32_t QXK24_SERAK_TOTAL_TIME[7] = {5, 9, 13, 17, 21, 25, 29};

struct QXK24SerakImpl {
    qxk24_ft_t ft;
    QXK24SerakSnapshot snap;
};

float qxk24_serak_compute(float m, float x_current) {
    return m <= 0.0f ? x_current : (1.0f / m) - x_current;
}

uint32_t qxk24_serak_mdk(uint8_t level) {
    return (level >= 1U && level <= QXK24_SERAK_LEVELS) ?
        QXK24_SERAK_MDK[level - 1U] : 0U;
}

uint32_t qxk24_serak_total_time(uint8_t level) {
    return (level >= 1U && level <= QXK24_SERAK_LEVELS) ?
        QXK24_SERAK_TOTAL_TIME[level - 1U] : 0U;
}

qxk24_err_t qxk24_serak_create(qxk24_ft_t ft, qxk24_serak_t *serak_out) {
    if (ft == NULL || serak_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24SerakImpl *s =
        (struct QXK24SerakImpl *)calloc(1, sizeof(*s));
    if (s == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    s->ft = ft;
    s->snap.current_level = 1U;
    s->snap.paired = true;
    *serak_out = s;
    return QXK24_OK;
}

qxk24_err_t qxk24_serak_cycle(qxk24_serak_t serak) {
    if (serak == NULL) return QXK24_ERR_NULL_PARAM;
    serak->snap.mdk_elapsed++;
    serak->snap.x_intercept = qxk24_serak_compute(
        (float)serak->snap.mdk_elapsed, serak->snap.x_intercept);
    if (serak->snap.mdk_elapsed >= qxk24_serak_mdk(serak->snap.current_level)) {
        serak->snap.mpp_count++;
        serak->snap.mdk_elapsed = 0U;
        if (serak->snap.current_level < QXK24_SERAK_LEVELS) {
            serak->snap.current_level++;
        }
    }
    serak->snap.spiral_radius = (float)serak->snap.current_level * 2.0f;
    return QXK24_OK;
}

qxk24_err_t qxk24_serak_snapshot(qxk24_serak_t serak,
                                  QXK24SerakSnapshot *snap_out) {
    if (serak == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = serak->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_serak_destroy(qxk24_serak_t serak) {
    if (serak == NULL) return QXK24_ERR_NULL_PARAM;
    memset(serak, 0, sizeof(*serak));
    free(serak);
    return QXK24_OK;
}

static void fill_bar(QXK24SerakBar *bar,
                     uint8_t level,
                     uint32_t idx,
                     bool is_right) {
    static const uint16_t angles[4] = {
        QXK24_SERAK_ANGLE_NE,
        QXK24_SERAK_ANGLE_SE,
        QXK24_SERAK_ANGLE_SW,
        QXK24_SERAK_ANGLE_NW
    };
    const uint32_t aidx = idx % 4U;
    bar->level = level;
    bar->angle = angles[aidx];
    bar->dir_x = (aidx < 2U) ? 1 : -1;
    bar->dir_y = (aidx == 0U || aidx == 3U) ? 1 : -1;
    bar->x1 = 0;
    bar->y1 = 0;
    bar->x2 = (int32_t)(bar->dir_x * (int8_t)level);
    bar->y2 = (int32_t)(bar->dir_y * (int8_t)level);
    bar->masa = idx + 1U;
    bar->is_right = is_right;
}

qxk24_err_t qxk24_serak_generate_bars(uint8_t level,
                                       bool is_right,
                                       QXK24SerakBar *bars_out,
                                       uint32_t bars_capacity,
                                       uint32_t *bars_count_out) {
    if (bars_out == NULL || bars_count_out == NULL) return QXK24_ERR_NULL_PARAM;
    if (level < 1U || level > QXK24_SERAK_LEVELS) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    const uint32_t needed = (uint32_t)level * 4U;
    if (bars_capacity < needed) return QXK24_ERR_CAPACITY;
    for (uint32_t i = 0U; i < needed; i++) fill_bar(&bars_out[i], level, i, is_right);
    *bars_count_out = needed;
    return QXK24_OK;
}
