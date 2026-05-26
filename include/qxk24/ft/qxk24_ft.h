/* ============================================================
 * qxk24_ft.h
 * QXK24 Universal Kernel — Faktor Tenaga Core
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/ft/qxk24_ft.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the Faktor Tenaga constitutional energy engine.
 * ============================================================ */

#ifndef QXK24_FT_H
#define QXK24_FT_H

#include "../qxk24.h"
#include "../fm/qxk24_fm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_FT_HIDUP_AKTIF      (-1)
#define QXK24_FT_HIDUP_PASIF      (+1)
#define QXK24_FT_DIRECTIONS       6
#define QXK24_FT_PHASES           7
#define QXK24_FT_ELEMENTS         4
#define QXK24_FT_BRAIN_BODY_RATIO 700
#define QXK24_FT_NO_OVERLAP       1
#define QXK24_FT_FORWARD_ONLY     1
#define QXK24_FT_PASATA_DELTA     1
#define QXK24_FT_WORLD_DTK        0
#define QXK24_FT_WORLD_DK         1

typedef enum {
    QXK24_FT_ELEMENT_TANAH = 0,
    QXK24_FT_ELEMENT_AIR = 1,
    QXK24_FT_ELEMENT_API = 2,
    QXK24_FT_ELEMENT_ANGIN = 3
} QXK24FtElement;

typedef enum {
    QXK24_FT_PHASE_PESA = 0,
    QXK24_FT_PHASE_PEDU = 1,
    QXK24_FT_PHASE_PEGA = 2,
    QXK24_FT_PHASE_PEPA = 3,
    QXK24_FT_PHASE_PEMA = 4,
    QXK24_FT_PHASE_PENA = 5,
    QXK24_FT_PHASE_PETU = 6
} QXK24FtPhase;

typedef enum {
    QXK24_FT_DIR_UTARA = 0,
    QXK24_FT_DIR_SELATAN = 1,
    QXK24_FT_DIR_TIMUR = 2,
    QXK24_FT_DIR_BARAT = 3,
    QXK24_FT_DIR_ATAS = 4,
    QXK24_FT_DIR_BAWAH = 5
} QXK24FtDirection;

typedef enum {
    QXK24_FT_ROTATION_XKN = 0,
    QXK24_FT_ROTATION_XKR = 1
} QXK24FtRotation;

typedef struct QXK24FtSnapshot_s {
    float hidup_aktif;
    float hidup_pasif;
    float ft_total;
    float fm_value;
    float x_measurement;
    uint32_t phase;
    uint32_t cycle_index;
    int world;
} QXK24FtSnapshot;

typedef struct QXK24FtImpl *qxk24_ft_t;

qxk24_err_t qxk24_ft_create(qxk24_kernel_t kernel, qxk24_ft_t *ft_out);
qxk24_err_t qxk24_ft_start(qxk24_ft_t ft);
qxk24_err_t qxk24_ft_cycle(qxk24_ft_t ft);
qxk24_err_t qxk24_ft_stop(qxk24_ft_t ft);
qxk24_err_t qxk24_ft_destroy(qxk24_ft_t ft);
qxk24_err_t qxk24_ft_snapshot(qxk24_ft_t ft, QXK24FtSnapshot *snap_out);
float qxk24_ft_total(qxk24_ft_t ft);
const char *qxk24_ft_element_name(QXK24FtElement element);
const char *qxk24_ft_phase_name(QXK24FtPhase phase);
const char *qxk24_ft_direction_name(QXK24FtDirection dir);
qxk24_err_t qxk24_ft_check_overlap(qxk24_ft_t ft);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_FT_H */
