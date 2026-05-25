/* ============================================================
 * qxk24_ama_pattern.c
 * QXK24 Universal Kernel — AMA 124 Pattern Engine
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/ama/qxk24_ama_pattern.c
 * License : Apache 2.0
 *
 * Description:
 * Implements the AMA 124 pattern table, budget derivation, column lookup,
 * total box lookup, and core value lookup for levels 1 through 7.
 */

#include "qxk24/qxk24.h"
#include <string.h>

static const uint64_t s_ama_columns[7][QXK24_AMA_MAX_COLUMNS] = {
    {1, 2, 8, 8, 2, 1},
    {1, 2, 8, 32, 32, 8, 2, 1},
    {1, 2, 8, 32, 128, 128, 32, 8, 2, 1},
    {1, 2, 8, 32, 128, 512, 512, 128, 32, 8, 2, 1},
    {1, 2, 8, 32, 128, 512, 2048, 2048, 512, 128, 32, 8, 2, 1},
    {1, 2, 8, 32, 128, 512, 2048, 8192, 8192, 2048, 512, 128, 32, 8, 2, 1},
    {1, 2, 8, 32, 128, 512, 2048, 8192, 32768, 32768,
     8192, 2048, 512, 128, 32, 8, 2, 1}
};

static const uint32_t s_ama_col_counts[7] = {6, 8, 10, 12, 14, 16, 18};

static const uint64_t s_ama_totals[7] = {
    QXK24_AMA_TOTAL_1,
    QXK24_AMA_TOTAL_2,
    QXK24_AMA_TOTAL_3,
    QXK24_AMA_TOTAL_4,
    QXK24_AMA_TOTAL_5,
    QXK24_AMA_TOTAL_6,
    QXK24_AMA_TOTAL_7
};

static const uint64_t s_ama_cores[7] = {
    QXK24_AMA_CORE_1,
    QXK24_AMA_CORE_2,
    QXK24_AMA_CORE_3,
    QXK24_AMA_CORE_4,
    QXK24_AMA_CORE_5,
    QXK24_AMA_CORE_6,
    QXK24_AMA_CORE_7
};

int32_t qxk24_ama_compute(uint32_t level, QXK24AMAPattern *out) {
    if (out == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if (level < 1 || level > 7) return QXK24_ERR_INVALID_ARGUMENT;

    const uint32_t idx = level - 1;
    memset(out, 0, sizeof(*out));
    out->level = level;
    out->column_count = s_ama_col_counts[idx];
    out->core_value = s_ama_cores[idx];
    out->total_boxes = s_ama_totals[idx];

    for (uint32_t c = 0; c < out->column_count; c++) {
        out->columns[c] = s_ama_columns[idx][c];
    }

    out->soft_budget_bytes = out->total_boxes * QXK24_BASE_UNIT_BYTES;
    out->hard_budget_bytes = qxk24_hard_budget(out->soft_budget_bytes);
    return QXK24_OK;
}

uint64_t qxk24_ama_column_value(uint32_t level, uint32_t col_index) {
    if (level < 1 || level > 7) return 0ULL;

    const uint32_t idx = level - 1;
    if (col_index >= s_ama_col_counts[idx]) return 0ULL;
    return s_ama_columns[idx][col_index];
}

uint64_t qxk24_ama_total_boxes(uint32_t level) {
    if (level < 1 || level > 7) return 0ULL;
    return s_ama_totals[level - 1];
}

uint64_t qxk24_ama_core_value(uint32_t level) {
    if (level < 1 || level > 7) return 0ULL;
    return s_ama_cores[level - 1];
}
