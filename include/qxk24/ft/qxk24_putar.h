/* ============================================================
 * qxk24_putar.h
 * QXK24 Universal Kernel — Putar Tenaga
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/ft/qxk24_putar.h
 * License : Apache 2.0
 *
 * Description:
 * Declares eight-movement constitutional energy rotation.
 * ============================================================ */

#ifndef QXK24_PUTAR_H
#define QXK24_PUTAR_H

#include "qxk24_ft.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_PUTAR_GERAK_INDUK     4
#define QXK24_PUTAR_GERAK_AGEN      4
#define QXK24_PUTAR_TOTAL_MOVEMENTS 8
#define QXK24_PUTAR_LEVELS          7
#define QXK24_PUTAR_ANGLE_STEP      45
#define QXK24_PUTAR_KEY_MOVEMENT    8

typedef enum {
    QXK24_PUTAR_GERAK_INDUK_TYPE = 0,
    QXK24_PUTAR_GERAK_AGEN_TYPE = 1
} QXK24PutarMovementType;

typedef struct QXK24PutarMovement_s {
    uint8_t index;
    QXK24PutarMovementType type;
    uint16_t angle;
    QXK24FtDirection direction;
    bool is_key;
} QXK24PutarMovement;

typedef struct QXK24PutarSnapshot_s {
    uint8_t current_level;
    uint8_t movement_index;
    QXK24FtRotation rotation;
    bool key_reached;
    bool rotation_stable;
    float energy_used;
    uint32_t rotations_done;
} QXK24PutarSnapshot;

typedef struct QXK24PutarImpl *qxk24_putar_t;

qxk24_err_t qxk24_putar_create(qxk24_ft_t ft,
                                QXK24FtRotation rotation,
                                qxk24_putar_t *putar_out);
qxk24_err_t qxk24_putar_cycle(qxk24_putar_t putar);
qxk24_err_t qxk24_putar_advance(qxk24_putar_t putar);
qxk24_err_t qxk24_putar_snapshot(qxk24_putar_t putar,
                                  QXK24PutarSnapshot *snap_out);
qxk24_err_t qxk24_putar_destroy(qxk24_putar_t putar);
float qxk24_putar_compute_energy(float t, uint8_t x_position);
qxk24_err_t qxk24_putar_movement_sequence(QXK24FtRotation rotation,
    QXK24PutarMovement movements_out[QXK24_PUTAR_TOTAL_MOVEMENTS]);
bool qxk24_putar_is_stable(qxk24_putar_t putar);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_PUTAR_H */
