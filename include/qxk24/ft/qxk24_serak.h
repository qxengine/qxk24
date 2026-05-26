/* ============================================================
 * qxk24_serak.h
 * QXK24 Universal Kernel — Serak Tenaga
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/ft/qxk24_serak.h
 * License : Apache 2.0
 *
 * Description:
 * Declares constitutional energy spiral math and bar generation.
 * ============================================================ */

#ifndef QXK24_SERAK_H
#define QXK24_SERAK_H

#include "qxk24_ft.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_SERAK_MPP           1
#define QXK24_SERAK_MDK_INCREMENT 4
#define QXK24_SERAK_TOTAL_MDK     112
#define QXK24_SERAK_LEVELS        7

extern const uint32_t QXK24_SERAK_MDK[7];
extern const uint32_t QXK24_SERAK_TOTAL_TIME[7];

#define QXK24_SERAK_ANGLE_NE 45
#define QXK24_SERAK_ANGLE_SE 315
#define QXK24_SERAK_ANGLE_SW 225
#define QXK24_SERAK_ANGLE_NW 135

typedef struct QXK24SerakBar_s {
    uint8_t level;
    uint16_t angle;
    int8_t dir_x;
    int8_t dir_y;
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
    uint32_t masa;
    bool is_right;
} QXK24SerakBar;

typedef struct QXK24SerakSnapshot_s {
    uint8_t current_level;
    uint32_t mdk_elapsed;
    uint32_t mpp_count;
    float x_intercept;
    float spiral_radius;
    bool paired;
} QXK24SerakSnapshot;

typedef struct QXK24SerakImpl *qxk24_serak_t;

qxk24_err_t qxk24_serak_create(qxk24_ft_t ft, qxk24_serak_t *serak_out);
qxk24_err_t qxk24_serak_cycle(qxk24_serak_t serak);
qxk24_err_t qxk24_serak_snapshot(qxk24_serak_t serak,
                                  QXK24SerakSnapshot *snap_out);
qxk24_err_t qxk24_serak_destroy(qxk24_serak_t serak);
float qxk24_serak_compute(float m, float x_current);
uint32_t qxk24_serak_mdk(uint8_t level);
uint32_t qxk24_serak_total_time(uint8_t level);
qxk24_err_t qxk24_serak_generate_bars(uint8_t level,
                                       bool is_right,
                                       QXK24SerakBar *bars_out,
                                       uint32_t bars_capacity,
                                       uint32_t *bars_count_out);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_SERAK_H */
