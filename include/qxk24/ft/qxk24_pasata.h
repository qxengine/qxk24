/* ============================================================
 * qxk24_pasata.h
 * QXK24 Universal Kernel — PASATA Energy Unit
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/ft/qxk24_pasata.h
 * License : Apache 2.0
 *
 * Description:
 * Declares PASATA levels where passive energy always exceeds active by one.
 * ============================================================ */

#ifndef QXK24_PASATA_H
#define QXK24_PASATA_H

#include "qxk24_ft.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_PASATA_LEVELS    7
#define QXK24_PASATA_DELTA     1
#define QXK24_PASATA_MAX_LEVEL 7
#define QXK24_PASATA_MIN_LEVEL 1

typedef struct QXK24PasataLevel_s {
    uint8_t level;
    uint8_t passive_count;
    uint8_t active_count;
    uint8_t wave_x;
    uint8_t wave_y;
    float energy_value;
} QXK24PasataLevel;

typedef struct QXK24PasataSnapshot_s {
    uint8_t current_level;
    uint8_t passive_count;
    uint8_t active_count;
    bool law_satisfied;
    float wave_amplitude;
    float energy_total;
} QXK24PasataSnapshot;

typedef struct QXK24PasataImpl *qxk24_pasata_t;

qxk24_err_t qxk24_pasata_create(qxk24_ft_t ft,
                                 uint8_t initial_level,
                                 qxk24_pasata_t *pasata_out);
qxk24_err_t qxk24_pasata_cycle(qxk24_pasata_t pasata);
qxk24_err_t qxk24_pasata_advance(qxk24_pasata_t pasata);
qxk24_err_t qxk24_pasata_snapshot(qxk24_pasata_t pasata,
                                   QXK24PasataSnapshot *snap_out);
qxk24_err_t qxk24_pasata_destroy(qxk24_pasata_t pasata);
qxk24_err_t qxk24_pasata_level_info(uint8_t level,
                                     QXK24PasataLevel *info_out);
qxk24_err_t qxk24_pasata_check_law(qxk24_pasata_t pasata);
float qxk24_pasata_compute_energy(uint8_t level, uint32_t wave_count);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_PASATA_H */
