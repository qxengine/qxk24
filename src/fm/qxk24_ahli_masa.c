/* ============================================================
 * QXK24 – Universal Kernel
 * src/fm/qxk24_ahli_masa.c
 *
 * Ahli Masa (Xam) — Time Member Implementation
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 * ============================================================ */

#include "qxk24/fm/qxk24_ahli_masa.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct QXK24AhliMasa {
    QXK24XamRecord record;
    bool has_moved;
};

static const QXK24Direction k_xam_dirs[QXK24_FM_AHLI_COORDINATES] = {
    QXK24_DIR_AS,
    QXK24_DIR_BH,
    QXK24_DIR_DN,
    QXK24_DIR_BG,
    QXK24_DIR_KN,
    QXK24_DIR_KR
};

static uint64_t xam_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
        (uint64_t)(ts.tv_nsec / 1000000ULL);
}

static void init_coords(struct QXK24AhliMasa *xam, bool keep_cycles) {
    const uint64_t total = keep_cycles ? xam->record.total_cycles : 0ULL;
    memset(&xam->record, 0, sizeof(xam->record));

    for (uint8_t i = 0; i < QXK24_FM_AHLI_COORDINATES; i++) {
        xam->record.coords[i].index = i;
        xam->record.coords[i].direction = k_xam_dirs[i];
        xam->record.coords[i].state = QXK24_XAM_COORD_INACTIVE;
    }
    xam->record.total_cycles = total;
    xam->has_moved = false;
}

qxk24_xam_t qxk24_xam_create(void) {
    struct QXK24AhliMasa *xam =
        (struct QXK24AhliMasa *)calloc(1, sizeof(*xam));
    if (xam == NULL) return NULL;
    init_coords(xam, false);
    return xam;
}

qxk24_err_t qxk24_xam_move_to(qxk24_xam_t xam, uint8_t index) {
    if (xam == NULL) return QXK24_ERR_NULL_PARAM;
    if (index >= QXK24_FM_AHLI_COORDINATES) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    struct QXK24AhliMasa *self = xam;
    const uint64_t now = xam_now_ms();
    if (!self->has_moved) self->record.first_move_ms = now;

    for (uint8_t i = 0; i < QXK24_FM_AHLI_COORDINATES; i++) {
        if (self->record.coords[i].state == QXK24_XAM_COORD_ACTIVE) {
            self->record.coords[i].state = QXK24_XAM_COORD_PASSED;
            self->record.coords[i].passed_ms = now;
        }
    }

    QXK24XamCoord *coord = &self->record.coords[index];
    coord->state = QXK24_XAM_COORD_ACTIVE;
    coord->activated_ms = now;
    self->record.current_coord = index;
    self->record.last_move_ms = now;
    self->record.coords_passed = (uint8_t)(index + 1U);
    self->has_moved = true;

    if (index == (uint8_t)(QXK24_FM_AHLI_COORDINATES - 1U) &&
        !self->record.cycle_complete) {
        self->record.cycle_complete = true;
        self->record.total_cycles++;
    }
    return QXK24_OK;
}

qxk24_err_t qxk24_xam_move(qxk24_xam_t xam) {
    if (xam == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AhliMasa *self = xam;

    if (!self->has_moved) return qxk24_xam_move_to(xam, 0U);
    if (self->record.cycle_complete) return QXK24_OK;
    return qxk24_xam_move_to(xam, (uint8_t)(self->record.current_coord + 1U));
}

qxk24_err_t qxk24_xam_reset(qxk24_xam_t xam) {
    if (xam == NULL) return QXK24_ERR_NULL_PARAM;
    init_coords(xam, true);
    return QXK24_OK;
}

void qxk24_xam_destroy(qxk24_xam_t xam) {
    if (xam == NULL) return;
    memset(xam, 0, sizeof(*xam));
    free(xam);
}

qxk24_err_t qxk24_xam_record(qxk24_xam_t xam, QXK24XamRecord *out) {
    if (xam == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    *out = xam->record;
    return QXK24_OK;
}

bool qxk24_xam_is_cycle_complete(qxk24_xam_t xam) {
    return xam != NULL && xam->record.cycle_complete;
}

uint8_t qxk24_xam_current_coord(qxk24_xam_t xam) {
    return xam == NULL ? 0U : xam->record.current_coord;
}

uint64_t qxk24_xam_total_cycles(qxk24_xam_t xam) {
    return xam == NULL ? 0ULL : xam->record.total_cycles;
}
