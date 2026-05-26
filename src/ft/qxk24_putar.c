/* ============================================================
 * qxk24_putar.c
 * QXK24 Universal Kernel — Putar Tenaga Implementation
 * ============================================================ */

#include "qxk24/ft/qxk24_putar.h"
#include <stdlib.h>
#include <string.h>

struct QXK24PutarImpl {
    qxk24_ft_t ft;
    QXK24PutarSnapshot snap;
};

static const QXK24PutarMovement k_xkn[8] = {
    {1, QXK24_PUTAR_GERAK_INDUK_TYPE, 0, QXK24_FT_DIR_TIMUR, false},
    {2, QXK24_PUTAR_GERAK_AGEN_TYPE, 45, QXK24_FT_DIR_ATAS, false},
    {3, QXK24_PUTAR_GERAK_INDUK_TYPE, 90, QXK24_FT_DIR_UTARA, false},
    {4, QXK24_PUTAR_GERAK_AGEN_TYPE, 135, QXK24_FT_DIR_ATAS, false},
    {5, QXK24_PUTAR_GERAK_INDUK_TYPE, 180, QXK24_FT_DIR_BARAT, false},
    {6, QXK24_PUTAR_GERAK_AGEN_TYPE, 225, QXK24_FT_DIR_BAWAH, false},
    {7, QXK24_PUTAR_GERAK_INDUK_TYPE, 270, QXK24_FT_DIR_SELATAN, false},
    {8, QXK24_PUTAR_GERAK_AGEN_TYPE, 315, QXK24_FT_DIR_BAWAH, true}
};

static const QXK24PutarMovement k_xkr[8] = {
    {1, QXK24_PUTAR_GERAK_INDUK_TYPE, 180, QXK24_FT_DIR_BARAT, false},
    {2, QXK24_PUTAR_GERAK_AGEN_TYPE, 135, QXK24_FT_DIR_ATAS, false},
    {3, QXK24_PUTAR_GERAK_INDUK_TYPE, 90, QXK24_FT_DIR_UTARA, false},
    {4, QXK24_PUTAR_GERAK_AGEN_TYPE, 45, QXK24_FT_DIR_ATAS, false},
    {5, QXK24_PUTAR_GERAK_INDUK_TYPE, 0, QXK24_FT_DIR_TIMUR, false},
    {6, QXK24_PUTAR_GERAK_AGEN_TYPE, 315, QXK24_FT_DIR_BAWAH, false},
    {7, QXK24_PUTAR_GERAK_INDUK_TYPE, 270, QXK24_FT_DIR_SELATAN, false},
    {8, QXK24_PUTAR_GERAK_AGEN_TYPE, 225, QXK24_FT_DIR_BAWAH, true}
};

qxk24_err_t qxk24_putar_create(qxk24_ft_t ft,
                                QXK24FtRotation rotation,
                                qxk24_putar_t *putar_out) {
    if (ft == NULL || putar_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24PutarImpl *p =
        (struct QXK24PutarImpl *)calloc(1, sizeof(*p));
    if (p == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    p->ft = ft;
    p->snap.current_level = 1U;
    p->snap.rotation = rotation;
    *putar_out = p;
    return QXK24_OK;
}

float qxk24_putar_compute_energy(float t, uint8_t x_position) {
    return t * (float)x_position;
}

qxk24_err_t qxk24_putar_movement_sequence(QXK24FtRotation rotation,
    QXK24PutarMovement movements_out[QXK24_PUTAR_TOTAL_MOVEMENTS]) {
    if (movements_out == NULL) return QXK24_ERR_NULL_PARAM;
    const QXK24PutarMovement *src =
        rotation == QXK24_FT_ROTATION_XKN ? k_xkn : k_xkr;
    for (uint32_t i = 0; i < QXK24_PUTAR_TOTAL_MOVEMENTS; i++) {
        movements_out[i] = src[i];
    }
    return QXK24_OK;
}

qxk24_err_t qxk24_putar_cycle(qxk24_putar_t putar) {
    if (putar == NULL) return QXK24_ERR_NULL_PARAM;
    putar->snap.movement_index++;
    putar->snap.energy_used =
        qxk24_putar_compute_energy(1.0f, putar->snap.current_level);
    if (putar->snap.movement_index >= QXK24_PUTAR_KEY_MOVEMENT) {
        putar->snap.key_reached = true;
        putar->snap.rotation_stable = true;
    }
    if (putar->snap.movement_index >= QXK24_PUTAR_TOTAL_MOVEMENTS) {
        putar->snap.rotations_done++;
        putar->snap.movement_index = 0U;
        if (putar->snap.current_level < QXK24_PUTAR_LEVELS) {
            putar->snap.current_level++;
        }
    }
    return QXK24_OK;
}

qxk24_err_t qxk24_putar_advance(qxk24_putar_t putar) {
    return qxk24_putar_cycle(putar);
}

qxk24_err_t qxk24_putar_snapshot(qxk24_putar_t putar,
                                  QXK24PutarSnapshot *snap_out) {
    if (putar == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = putar->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_putar_destroy(qxk24_putar_t putar) {
    if (putar == NULL) return QXK24_ERR_NULL_PARAM;
    memset(putar, 0, sizeof(*putar));
    free(putar);
    return QXK24_OK;
}

bool qxk24_putar_is_stable(qxk24_putar_t putar) {
    return putar != NULL && putar->snap.rotation_stable;
}
