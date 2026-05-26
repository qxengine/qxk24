/* ============================================================
 * qxk24_padanan.h
 * QXK24 Universal Kernel — Padanan Tenaga
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/ft/qxk24_padanan.h
 * License : Apache 2.0
 *
 * Description:
 * Declares Titik LATI energy pairing across four physical elements.
 * ============================================================ */

#ifndef QXK24_PADANAN_H
#define QXK24_PADANAN_H

#include "qxk24_ft.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_PADANAN_ELEMENTS    4
#define QXK24_PADANAN_COORDINATES 5
#define QXK24_PADANAN_PROCESSES   4
#define QXK24_PADANAN_MOVEMENTS   4
#define QXK24_PADANAN_TOTAL_GL    16
#define QXK24_PADANAN_BRAIN_RATIO 700
#define QXK24_PADANAN_BODY_RATIO  1

typedef enum {
    QXK24_PADANAN_PROC_SA = 0,
    QXK24_PADANAN_PROC_DU = 1,
    QXK24_PADANAN_PROC_GA = 2,
    QXK24_PADANAN_PROC_PA = 3
} QXK24PadananProcess;

typedef enum {
    QXK24_PADANAN_DOM_API = 0,
    QXK24_PADANAN_DOM_TANAH = 1,
    QXK24_PADANAN_DOM_AIR = 2,
    QXK24_PADANAN_DOM_ANGIN = 3
} QXK24PadananDominant;

typedef struct QXK24PadananMovement_s {
    QXK24FtElement element;
    uint8_t from_pos;
    uint8_t to_pos;
    uint32_t gl_index;
    uint32_t gg_index;
} QXK24PadananMovement;

typedef struct QXK24PadananSnapshot_s {
    QXK24PadananDominant dominant;
    QXK24PadananProcess current_process;
    uint32_t gl_count;
    uint32_t gg_count;
    QXK24FtRotation rotation;
    float energy_value;
    bool cycle_complete;
} QXK24PadananSnapshot;

typedef struct QXK24PadananImpl *qxk24_padanan_t;

qxk24_err_t qxk24_padanan_create(qxk24_ft_t ft,
                                  QXK24PadananDominant dominant,
                                  qxk24_padanan_t *padanan_out);
qxk24_err_t qxk24_padanan_cycle(qxk24_padanan_t padanan);
qxk24_err_t qxk24_padanan_snapshot(qxk24_padanan_t padanan,
                                    QXK24PadananSnapshot *snap_out);
qxk24_err_t qxk24_padanan_destroy(qxk24_padanan_t padanan);
qxk24_err_t qxk24_padanan_movements(QXK24PadananDominant dominant,
                                     QXK24PadananProcess process,
                                     QXK24PadananMovement movements_out[4]);
uint32_t qxk24_padanan_energy_ratio(bool is_cognitive);
const char *qxk24_padanan_dominant_name(QXK24PadananDominant dom);
const char *qxk24_padanan_process_name(QXK24PadananProcess proc);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_PADANAN_H */
