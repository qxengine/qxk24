/* ============================================================
 * qxk24_hisal.c
 * QXK24 Universal Kernel — HISAL Mathematics Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/science/qxk24_hisal.c
 * License : Apache 2.0
 *
 * Description:
 * Implements HISAL AIDIL operations, TAJU segmentation, GANDA
 * grids, ASAS growth, and PL/PG helpers.
 * ============================================================ */

#include "qxk24/science/qxk24_hisal.h"
#include <stdlib.h>
#include <string.h>

struct qxk24_hisal_impl_t {
    uint32_t operations_performed;
    uint32_t unifications;
    uint32_t separations;
    uint32_t taju_overflows;
    float health;
};

static const char *s_taju_names[8] = {
    "zero", "sa", "du", "ga", "pa", "ma", "na", "tu"
};

static void update_health(struct qxk24_hisal_impl_t *h) {
    h->health = 99.5f - (float)(h->operations_performed % 20U) * 0.1f;
    if (h->health < 80.0f) h->health = 80.0f;
}

qxk24_hisal_t qxk24_hisal_create(void) {
    struct qxk24_hisal_impl_t *h = calloc(1, sizeof(*h));
    if (h == NULL) return NULL;
    h->health = 100.0f;
    return h;
}

void qxk24_hisal_destroy(qxk24_hisal_t h) {
    free(h);
}

int qxk24_hisal_add(qxk24_hisal_t h, uint32_t a, uint32_t b,
                    qxk24_hisal_result_t *out) {
    if (h == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    memset(out, 0, sizeof(*out));
    out->input_a = a;
    out->input_b = b;
    out->process = QXK24_HISAL_PROCESS_GABUNG;
    out->aidil_result = QXK24_HISAL_AIDIL_ADD_RESULT;
    out->conventional_result = a + b;
    out->taju_segment = qxk24_hisal_to_taju(h, out->conventional_result);
    out->is_valid = 1;
    h->operations_performed++;
    h->unifications++;
    update_health(h);
    return QXK24_OK;
}

int qxk24_hisal_sub(qxk24_hisal_t h, uint32_t a, uint32_t b,
                    qxk24_hisal_result_t *out) {
    if (h == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    memset(out, 0, sizeof(*out));
    out->input_a = a;
    out->input_b = b;
    out->process = QXK24_HISAL_PROCESS_LERAI;
    out->aidil_result = QXK24_HISAL_AIDIL_SUB_RESULT;
    out->conventional_result = a >= b ? a - b : 0U;
    out->taju_segment = qxk24_hisal_to_taju(h, out->conventional_result);
    out->is_valid = 1;
    h->operations_performed++;
    h->separations++;
    update_health(h);
    return QXK24_OK;
}

uint32_t qxk24_hisal_to_taju(qxk24_hisal_t h, uint32_t value) {
    if (h == NULL || value == 0U) return QXK24_HISAL_TAJU_SA;
    uint32_t t = value;
    while (t > QXK24_HISAL_TAJU_MAX) {
        uint32_t sum = 0U;
        uint32_t tmp = t;
        while (tmp > 0U) {
            sum += (tmp % QXK24_HISAL_TAJU_MAX) + 1U;
            tmp /= QXK24_HISAL_TAJU_MAX;
        }
        t = ((sum - 1U) % QXK24_HISAL_TAJU_MAX) + 1U;
        h->taju_overflows++;
    }
    return t;
}

int qxk24_hisal_is_taju(uint32_t value) {
    return value >= QXK24_HISAL_TAJU_MIN &&
        value <= QXK24_HISAL_TAJU_MAX;
}

int qxk24_hisal_ganda_grid(uint32_t level, qxk24_ganda_grid_t *out) {
    if (out == NULL) return QXK24_ERR_NULL_PARAM;
    if (level < 1U || level > QXK24_HISAL_TAJU_MAX) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    out->level = level;
    out->grid_size = level + 1U;
    if (out->grid_size > 7U) out->grid_size = 7U;
    out->total_boxes = out->grid_size * out->grid_size;
    out->max_value = out->total_boxes;
    return QXK24_OK;
}

uint32_t qxk24_hisal_asas_boxes(uint32_t level) {
    uint32_t result = 1U;
    for (uint32_t i = 1U; i < level; i++) result *= 4U;
    return result;
}

int qxk24_hisal_process_lerai(qxk24_hisal_t h, uint32_t value,
                              uint32_t *parts, uint32_t *count) {
    if (h == NULL || parts == NULL || count == NULL) return QXK24_ERR_NULL_PARAM;
    const uint32_t taju = qxk24_hisal_to_taju(h, value);
    *count = 0U;
    for (uint32_t x = 1U; x <= taju; x++) {
        const uint32_t y = taju - x;
        if (x <= y) parts[(*count)++] = x;
    }
    if (*count == 0U) {
        parts[0] = 1U;
        *count = 1U;
    }
    return QXK24_OK;
}

int qxk24_hisal_process_gabung(qxk24_hisal_t h, uint32_t *parts,
                               uint32_t count, uint32_t *result) {
    if (h == NULL || parts == NULL || result == NULL) return QXK24_ERR_NULL_PARAM;
    (void)count;
    *result = QXK24_HISAL_AIDIL_ADD_RESULT;
    return QXK24_OK;
}

qxk24_hisal_snapshot_t qxk24_hisal_snapshot(qxk24_hisal_t h) {
    qxk24_hisal_snapshot_t s;
    memset(&s, 0, sizeof(s));
    if (h == NULL) return s;
    s.operations_performed = h->operations_performed;
    s.unifications = h->unifications;
    s.separations = h->separations;
    s.taju_overflows = h->taju_overflows;
    s.health = h->health;
    return s;
}

const char *qxk24_hisal_taju_name(uint32_t taju) {
    if (taju < 1U || taju > 7U) return "unknown";
    return s_taju_names[taju];
}

const char *qxk24_hisal_process_name(qxk24_hisal_process_t p) {
    return p == QXK24_HISAL_PROCESS_LERAI ? "Proses Lerai" :
        "Proses Gabung";
}
