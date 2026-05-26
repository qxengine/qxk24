/* ============================================================
 * qxk24_hisal.h
 * QXK24 Universal Kernel — HISAL Mathematics Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/science/qxk24_hisal.h
 * License : Apache 2.0
 *
 * Description:
 * Declares HISAL AIDIL laws, TAJU segmentation, GANDA grids,
 * ASAS growth, and Proses Lerai/Gabung.
 * ============================================================ */

#ifndef QXK24_HISAL_H
#define QXK24_HISAL_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_HISAL_AIDIL_ADD_RESULT 1U
#define QXK24_HISAL_AIDIL_SUB_RESULT 2U
#define QXK24_HISAL_TAJU_MIN 1U
#define QXK24_HISAL_TAJU_MAX 7U
#define QXK24_HISAL_TAJU_COUNT 7U
#define QXK24_HISAL_GANDA_MAX_LEVEL 7U
#define QXK24_HISAL_GANDA_MAX_BOXES 49U
#define QXK24_HISAL_ASAS_ACTIVE_SURFACES 4U
#define QXK24_HISAL_ASAS_PASSIVE_AXES 2U
#define QXK24_HISAL_PL_LEVEL_COUNT 6U
#define QXK24_HISAL_PG_LEVEL_COUNT 6U

#define QXK24_HISAL_TAJU_SA 1U
#define QXK24_HISAL_TAJU_DU 2U
#define QXK24_HISAL_TAJU_GA 3U
#define QXK24_HISAL_TAJU_PA 4U
#define QXK24_HISAL_TAJU_MA 5U
#define QXK24_HISAL_TAJU_NA 6U
#define QXK24_HISAL_TAJU_TU 7U

typedef enum {
    QXK24_SUNOM_XKR = 0,
    QXK24_SUNOM_XKN = 1
} qxk24_sunom_dir_t;

typedef enum {
    QXK24_HISAL_PROCESS_LERAI = 0,
    QXK24_HISAL_PROCESS_GABUNG = 1
} qxk24_hisal_process_t;

typedef struct {
    uint32_t level;
    uint32_t grid_size;
    uint32_t total_boxes;
    uint32_t max_value;
} qxk24_ganda_grid_t;

typedef struct {
    uint32_t input_a;
    uint32_t input_b;
    qxk24_hisal_process_t process;
    uint32_t aidil_result;
    uint32_t conventional_result;
    uint32_t taju_segment;
    int is_valid;
} qxk24_hisal_result_t;

typedef struct {
    uint32_t operations_performed;
    uint32_t unifications;
    uint32_t separations;
    uint32_t taju_overflows;
    float health;
} qxk24_hisal_snapshot_t;

typedef struct qxk24_hisal_impl_t *qxk24_hisal_t;

qxk24_hisal_t qxk24_hisal_create(void);
void qxk24_hisal_destroy(qxk24_hisal_t h);
int qxk24_hisal_add(qxk24_hisal_t h, uint32_t a, uint32_t b,
                    qxk24_hisal_result_t *out);
int qxk24_hisal_sub(qxk24_hisal_t h, uint32_t a, uint32_t b,
                    qxk24_hisal_result_t *out);
uint32_t qxk24_hisal_to_taju(qxk24_hisal_t h, uint32_t value);
int qxk24_hisal_is_taju(uint32_t value);
int qxk24_hisal_ganda_grid(uint32_t level, qxk24_ganda_grid_t *out);
uint32_t qxk24_hisal_asas_boxes(uint32_t level);
int qxk24_hisal_process_lerai(qxk24_hisal_t h, uint32_t value,
                              uint32_t *parts, uint32_t *count);
int qxk24_hisal_process_gabung(qxk24_hisal_t h, uint32_t *parts,
                               uint32_t count, uint32_t *result);
qxk24_hisal_snapshot_t qxk24_hisal_snapshot(qxk24_hisal_t h);
const char *qxk24_hisal_taju_name(uint32_t taju);
const char *qxk24_hisal_process_name(qxk24_hisal_process_t p);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_HISAL_H */
