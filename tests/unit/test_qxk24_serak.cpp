/* ============================================================
 * test_qxk24_serak.cpp
 * QXK24 Universal Kernel — Serak Tenaga Tests
 * ============================================================ */

#include "qxk24/ft/qxk24_serak.h"
#include <gtest/gtest.h>

class SerakTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_ft_t ft = nullptr;
    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
        ASSERT_EQ(qxk24_ft_create(kernel, &ft), QXK24_OK);
    }
    void TearDown() override {
        qxk24_ft_destroy(ft);
        qxk24_destroy(kernel);
    }
};

TEST_F(SerakTest, MdkLevel1Is4) { EXPECT_EQ(qxk24_serak_mdk(1), 4U); }
TEST_F(SerakTest, MdkLevel7Is28) { EXPECT_EQ(qxk24_serak_mdk(7), 28U); }
TEST_F(SerakTest, InvalidMdkIsZero) { EXPECT_EQ(qxk24_serak_mdk(8), 0U); }
TEST_F(SerakTest, TotalTimeLevel1Is5) { EXPECT_EQ(qxk24_serak_total_time(1), 5U); }
TEST_F(SerakTest, TotalTimeLevel7Is29) { EXPECT_EQ(qxk24_serak_total_time(7), 29U); }
TEST_F(SerakTest, TotalMdkIs112) {
    uint32_t total = 0U;
    for (uint8_t i = 1; i <= 7; i++) total += qxk24_serak_mdk(i);
    EXPECT_EQ(total, QXK24_SERAK_TOTAL_MDK);
}
TEST_F(SerakTest, FormulaBasic) { EXPECT_FLOAT_EQ(qxk24_serak_compute(1.0f, 0.0f), 1.0f); }
TEST_F(SerakTest, FormulaKeepsCurrentWhenZeroM) {
    EXPECT_FLOAT_EQ(qxk24_serak_compute(0.0f, 3.0f), 3.0f);
}
TEST_F(SerakTest, CreateSucceeds) {
    qxk24_serak_t s = nullptr;
    EXPECT_EQ(qxk24_serak_create(ft, &s), QXK24_OK);
    qxk24_serak_destroy(s);
}
TEST_F(SerakTest, InitialLevelOne) {
    qxk24_serak_t serak = nullptr;
    qxk24_serak_create(ft, &serak);
    QXK24SerakSnapshot s{};
    qxk24_serak_snapshot(serak, &s);
    EXPECT_EQ(s.current_level, 1U);
    qxk24_serak_destroy(serak);
}
TEST_F(SerakTest, InitialPaired) {
    qxk24_serak_t serak = nullptr;
    qxk24_serak_create(ft, &serak);
    QXK24SerakSnapshot s{};
    qxk24_serak_snapshot(serak, &s);
    EXPECT_TRUE(s.paired);
    qxk24_serak_destroy(serak);
}
TEST_F(SerakTest, CycleAdvancesMdk) {
    qxk24_serak_t serak = nullptr;
    qxk24_serak_create(ft, &serak);
    qxk24_serak_cycle(serak);
    QXK24SerakSnapshot s{};
    qxk24_serak_snapshot(serak, &s);
    EXPECT_EQ(s.mdk_elapsed, 1U);
    qxk24_serak_destroy(serak);
}
TEST_F(SerakTest, FourCyclesAdvanceLevel) {
    qxk24_serak_t serak = nullptr;
    qxk24_serak_create(ft, &serak);
    for (int i = 0; i < 4; i++) qxk24_serak_cycle(serak);
    QXK24SerakSnapshot s{};
    qxk24_serak_snapshot(serak, &s);
    EXPECT_EQ(s.current_level, 2U);
    qxk24_serak_destroy(serak);
}
TEST_F(SerakTest, GenerateBarsLevel2CountEight) {
    QXK24SerakBar bars[8]{};
    uint32_t count = 0U;
    EXPECT_EQ(qxk24_serak_generate_bars(2, true, bars, 8, &count), QXK24_OK);
    EXPECT_EQ(count, 8U);
}
TEST_F(SerakTest, GeneratedBarAngleKnown) {
    QXK24SerakBar bars[4]{};
    uint32_t count = 0U;
    qxk24_serak_generate_bars(1, true, bars, 4, &count);
    EXPECT_EQ(bars[0].angle, QXK24_SERAK_ANGLE_NE);
}
TEST_F(SerakTest, GeneratedBarSideCopied) {
    QXK24SerakBar bars[4]{};
    uint32_t count = 0U;
    qxk24_serak_generate_bars(1, false, bars, 4, &count);
    EXPECT_FALSE(bars[0].is_right);
}
TEST_F(SerakTest, GenerateBarsSmallCapacityFails) {
    QXK24SerakBar bars[1]{};
    uint32_t count = 0U;
    EXPECT_EQ(qxk24_serak_generate_bars(2, true, bars, 1, &count),
              QXK24_ERR_CAPACITY);
}
TEST_F(SerakTest, NullCreateFails) {
    qxk24_serak_t s = nullptr;
    EXPECT_EQ(qxk24_serak_create(nullptr, &s), QXK24_ERR_NULL_PARAM);
}
TEST_F(SerakTest, NullSnapshotFails) {
    EXPECT_EQ(qxk24_serak_snapshot(nullptr, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(SerakTest, NullBarsFails) {
    uint32_t count = 0U;
    EXPECT_EQ(qxk24_serak_generate_bars(1, true, nullptr, 0, &count),
              QXK24_ERR_NULL_PARAM);
}
