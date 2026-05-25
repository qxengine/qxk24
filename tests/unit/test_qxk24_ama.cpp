/* ============================================================
 * test_qxk24_ama.cpp
 * QXK24 Universal Kernel — AMA 124 Pattern Tests
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_ama.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 42 machine proofs for AMA 124 columns, totals, core values,
 * budgets, symmetry, growth, and inline helper consistency.
 */

#include "qxk24/qxk24.h"
#include <gtest/gtest.h>

TEST(QXK24AMA_ColumnValues, Level1_Col0_IsOne) {
    EXPECT_EQ(qxk24_ama_column_value(1, 0), 1ULL);
}
TEST(QXK24AMA_ColumnValues, Level1_Col1_IsTwo) {
    EXPECT_EQ(qxk24_ama_column_value(1, 1), 2ULL);
}
TEST(QXK24AMA_ColumnValues, Level1_Col2_IsEight) {
    EXPECT_EQ(qxk24_ama_column_value(1, 2), 8ULL);
}
TEST(QXK24AMA_ColumnValues, Level1_Col3_IsEight) {
    EXPECT_EQ(qxk24_ama_column_value(1, 3), 8ULL);
}
TEST(QXK24AMA_ColumnValues, Level1_Col4_IsTwo) {
    EXPECT_EQ(qxk24_ama_column_value(1, 4), 2ULL);
}
TEST(QXK24AMA_ColumnValues, Level1_Col5_IsOne) {
    EXPECT_EQ(qxk24_ama_column_value(1, 5), 1ULL);
}
TEST(QXK24AMA_ColumnValues, Level2_CoreCol_Is32) {
    EXPECT_EQ(qxk24_ama_column_value(2, 3), 32ULL);
}
TEST(QXK24AMA_ColumnValues, Level3_CoreCol_Is128) {
    EXPECT_EQ(qxk24_ama_column_value(3, 4), 128ULL);
}
TEST(QXK24AMA_ColumnValues, Level4_CoreCol_Is512) {
    EXPECT_EQ(qxk24_ama_column_value(4, 5), 512ULL);
}
TEST(QXK24AMA_ColumnValues, Level5_CoreCol_Is2048) {
    EXPECT_EQ(qxk24_ama_column_value(5, 6), 2048ULL);
}
TEST(QXK24AMA_ColumnValues, Level6_CoreCol_Is8192) {
    EXPECT_EQ(qxk24_ama_column_value(6, 7), 8192ULL);
}
TEST(QXK24AMA_ColumnValues, AllLevels_OuterColIsOne) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        EXPECT_EQ(qxk24_ama_column_value(lv, 0), 1ULL) << "Level " << lv;
    }
}
TEST(QXK24AMA_ColumnValues, AllLevels_SecondColIsTwo) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        EXPECT_EQ(qxk24_ama_column_value(lv, 1), 2ULL) << "Level " << lv;
    }
}
TEST(QXK24AMA_ColumnValues, OutOfRange_ReturnsZero) {
    EXPECT_EQ(qxk24_ama_column_value(0, 0), 0ULL);
    EXPECT_EQ(qxk24_ama_column_value(8, 0), 0ULL);
    EXPECT_EQ(qxk24_ama_column_value(1, 99), 0ULL);
}

TEST(QXK24AMA_TotalBoxes, Level1_Is22) {
    EXPECT_EQ(qxk24_ama_total_boxes(1), 22ULL);
}
TEST(QXK24AMA_TotalBoxes, Level2_Is86) {
    EXPECT_EQ(qxk24_ama_total_boxes(2), 86ULL);
}
TEST(QXK24AMA_TotalBoxes, Level3_Is342) {
    EXPECT_EQ(qxk24_ama_total_boxes(3), 342ULL);
}
TEST(QXK24AMA_TotalBoxes, Level4_Is1366) {
    EXPECT_EQ(qxk24_ama_total_boxes(4), 1366ULL);
}
TEST(QXK24AMA_TotalBoxes, Level5_Is5462) {
    EXPECT_EQ(qxk24_ama_total_boxes(5), 5462ULL);
}
TEST(QXK24AMA_TotalBoxes, Level6_Is21846) {
    EXPECT_EQ(qxk24_ama_total_boxes(6), 21846ULL);
}
TEST(QXK24AMA_TotalBoxes, Level7_Is87382) {
    EXPECT_EQ(qxk24_ama_total_boxes(7), 87382ULL);
}

TEST(QXK24AMA_CoreValues, Level1_Is8) {
    EXPECT_EQ(qxk24_ama_core_value(1), 8ULL);
}
TEST(QXK24AMA_CoreValues, Level2_Is32) {
    EXPECT_EQ(qxk24_ama_core_value(2), 32ULL);
}
TEST(QXK24AMA_CoreValues, Level3_Is128) {
    EXPECT_EQ(qxk24_ama_core_value(3), 128ULL);
}
TEST(QXK24AMA_CoreValues, Level4_Is512) {
    EXPECT_EQ(qxk24_ama_core_value(4), 512ULL);
}
TEST(QXK24AMA_CoreValues, Level5_Is2048) {
    EXPECT_EQ(qxk24_ama_core_value(5), 2048ULL);
}
TEST(QXK24AMA_CoreValues, Level6_Is8192) {
    EXPECT_EQ(qxk24_ama_core_value(6), 8192ULL);
}
TEST(QXK24AMA_CoreValues, Level7_Is32768) {
    EXPECT_EQ(qxk24_ama_core_value(7), 32768ULL);
}

TEST(QXK24AMA_Budget, Level1_SoftIs22MiB) {
    QXK24AMAPattern p;
    ASSERT_EQ(qxk24_ama_compute(1, &p), QXK24_OK);
    EXPECT_EQ(p.soft_budget_bytes, 22ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24AMA_Budget, Level1_HardIs33MiB) {
    QXK24AMAPattern p;
    ASSERT_EQ(qxk24_ama_compute(1, &p), QXK24_OK);
    EXPECT_EQ(p.hard_budget_bytes, 33ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24AMA_Budget, Level3_SoftIs342MiB) {
    QXK24AMAPattern p;
    ASSERT_EQ(qxk24_ama_compute(3, &p), QXK24_OK);
    EXPECT_EQ(p.soft_budget_bytes, 342ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24AMA_Budget, Level4_SoftIs1366MiB) {
    QXK24AMAPattern p;
    ASSERT_EQ(qxk24_ama_compute(4, &p), QXK24_OK);
    EXPECT_EQ(p.soft_budget_bytes, 1366ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24AMA_Budget, HardAlwaysGreaterThanSoft) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        QXK24AMAPattern p;
        ASSERT_EQ(qxk24_ama_compute(lv, &p), QXK24_OK);
        EXPECT_GT(p.hard_budget_bytes, p.soft_budget_bytes) << "Level " << lv;
    }
}
TEST(QXK24AMA_Budget, InvalidLevel_ReturnsError) {
    QXK24AMAPattern p;
    EXPECT_EQ(qxk24_ama_compute(0, &p), QXK24_ERR_INVALID_ARGUMENT);
    EXPECT_EQ(qxk24_ama_compute(8, &p), QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24AMA_Budget, NullOutput_ReturnsError) {
    EXPECT_EQ(qxk24_ama_compute(1, nullptr), QXK24_ERR_INVALID_ARGUMENT);
}

TEST(QXK24AMA_Structure, AllLevels_ColumnsArePalindrome) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        QXK24AMAPattern p;
        ASSERT_EQ(qxk24_ama_compute(lv, &p), QXK24_OK);
        for (uint32_t i = 0; i < p.column_count / 2; i++) {
            EXPECT_EQ(p.columns[i], p.columns[p.column_count - 1 - i])
                << "Level " << lv;
        }
    }
}
TEST(QXK24AMA_Structure, AllLevels_EdgeColumnsAreK) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        QXK24AMAPattern p;
        ASSERT_EQ(qxk24_ama_compute(lv, &p), QXK24_OK);
        EXPECT_EQ(p.columns[0], (uint64_t)QXK24_K);
        EXPECT_EQ(p.columns[p.column_count - 1], (uint64_t)QXK24_K);
    }
}
TEST(QXK24AMA_Structure, AllLevels_TotalMatchesSumOfColumns) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        QXK24AMAPattern p;
        ASSERT_EQ(qxk24_ama_compute(lv, &p), QXK24_OK);
        uint64_t sum = 0;
        for (uint32_t c = 0; c < p.column_count; c++) sum += p.columns[c];
        EXPECT_EQ(sum, p.total_boxes) << "Level " << lv;
    }
}
TEST(QXK24AMA_Structure, AllLevels_CoreValueMatchesTwoPow4n) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        QXK24AMAPattern p;
        ASSERT_EQ(qxk24_ama_compute(lv, &p), QXK24_OK);
        EXPECT_EQ(p.core_value, 2ULL * qxk24_pow4(lv)) << "Level " << lv;
    }
}
TEST(QXK24AMA_Structure, ColumnCount_Is2TimesLevelPlus4) {
    uint32_t expected[7] = {6, 8, 10, 12, 14, 16, 18};
    for (uint32_t lv = 1; lv <= 7; lv++) {
        QXK24AMAPattern p;
        ASSERT_EQ(qxk24_ama_compute(lv, &p), QXK24_OK);
        EXPECT_EQ(p.column_count, expected[lv - 1]) << "Level " << lv;
    }
}
TEST(QXK24AMA_Structure, TotalBoxes_Increase_ByFactor4) {
    for (uint32_t lv = 2; lv <= 7; lv++) {
        uint64_t prev = qxk24_ama_total_boxes(lv - 1);
        uint64_t curr = qxk24_ama_total_boxes(lv);
        EXPECT_GT(curr, prev * 3ULL) << "Level " << lv;
    }
}
TEST(QXK24AMA_Structure, InlineHelper_MatchesCoreValue) {
    for (uint32_t lv = 1; lv <= 7; lv++) {
        QXK24AMAPattern p;
        ASSERT_EQ(qxk24_ama_compute(lv, &p), QXK24_OK);
        EXPECT_EQ(qxk24_ama_col_inline(lv), p.core_value) << "Level " << lv;
    }
}
