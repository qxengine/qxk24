/* ============================================================
 * QXK24 – Universal Kernel
 * tests/unit/test_qxk24_fm.cpp
 *
 * Faktor Masa Engine — Unit Tests
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * ============================================================ */

#include "qxk24/fm/qxk24_fm.h"
#include <gtest/gtest.h>

static qxk24_kernel_t make_kernel(void) {
    QXK24Config cfg = qxk24_default_config();
    qxk24_kernel_t kernel = nullptr;
    EXPECT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
    EXPECT_EQ(qxk24_start(kernel), QXK24_OK);
    return kernel;
}

static void destroy_kernel(qxk24_kernel_t kernel) {
    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}

TEST(QXK24Fm_Const, MPValueIsOne) { EXPECT_EQ(QXK24_FM_MP_VALUE, 1); }
TEST(QXK24Fm_Const, MDKPairIsTwo) { EXPECT_EQ(QXK24_FM_MDK_PAIR, 2); }
TEST(QXK24Fm_Const, MDKDirectionIsFour) {
    EXPECT_EQ(QXK24_FM_MDK_DIRECTION, 4);
}
TEST(QXK24Fm_Const, MDKCountIsSix) { EXPECT_EQ(QXK24_FM_MDK_COUNT, 6); }
TEST(QXK24Fm_Const, AhliCoordsIsSix) {
    EXPECT_EQ(QXK24_FM_AHLI_COORDINATES, 6);
}
TEST(QXK24Fm_Const, LumanZIsSix) { EXPECT_EQ(QXK24_FM_LUMAN_Z, 6); }
TEST(QXK24Fm_Const, LumanXIsFour) { EXPECT_EQ(QXK24_FM_LUMAN_X, 4); }

TEST(QXK24Fm_Value, FormulaIsTwo) {
    EXPECT_NEAR(qxk24_fm_value(), 2.0f, 0.001f);
}
TEST(QXK24Fm_Value, MacroMatchesFunction) {
    EXPECT_NEAR(QXK24_FM_VALUE, qxk24_fm_value(), 0.001f);
}

TEST(QXK24Fm_Category, MPi) {
    EXPECT_STREQ(qxk24_fm_category_name(QXK24_FM_MPi), "MPi");
}
TEST(QXK24Fm_Category, MPP) {
    EXPECT_STREQ(qxk24_fm_category_name(QXK24_FM_MPP), "MPP");
}
TEST(QXK24Fm_Category, MDK) {
    EXPECT_STREQ(qxk24_fm_category_name(QXK24_FM_MDK), "MDK");
}
TEST(QXK24Fm_Category, UnknownHigh) {
    EXPECT_STREQ(qxk24_fm_category_name((QXK24FmCategory)99), "UNKNOWN");
}
TEST(QXK24Fm_Category, UnknownNegative) {
    EXPECT_STREQ(qxk24_fm_category_name((QXK24FmCategory)-1), "UNKNOWN");
}

TEST(QXK24Fm_FlowLaw, EnergyUp) {
    EXPECT_EQ(qxk24_fm_flow_law(0.5f, 0.8f), QXK24_FM_LAW_ENERGY_UP);
}
TEST(QXK24Fm_FlowLaw, EnergyDown) {
    EXPECT_EQ(qxk24_fm_flow_law(0.8f, 0.5f), QXK24_FM_LAW_ENERGY_DOWN);
}
TEST(QXK24Fm_FlowLaw, EnergyWave) {
    EXPECT_EQ(qxk24_fm_flow_law(0.7f, 0.7f), QXK24_FM_LAW_ENERGY_WAVE);
}
TEST(QXK24Fm_FlowLaw, SmallRiseIsWave) {
    EXPECT_EQ(qxk24_fm_flow_law(0.700f, 0.7005f),
              QXK24_FM_LAW_ENERGY_WAVE);
}
TEST(QXK24Fm_FlowLaw, SmallFallIsWave) {
    EXPECT_EQ(qxk24_fm_flow_law(0.7005f, 0.700f),
              QXK24_FM_LAW_ENERGY_WAVE);
}

TEST(QXK24Fm_Luman, ZLevel1IsOne) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_Z, 1), 1ULL);
}
TEST(QXK24Fm_Luman, ZLevel2IsSix) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_Z, 2), 6ULL);
}
TEST(QXK24Fm_Luman, ZLevel3Is36) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_Z, 3), 36ULL);
}
TEST(QXK24Fm_Luman, ZLevel7Is46656) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_Z, 7), 46656ULL);
}
TEST(QXK24Fm_Luman, XLevel1IsOne) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_X, 1), 1ULL);
}
TEST(QXK24Fm_Luman, XLevel2IsFour) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_X, 2), 4ULL);
}
TEST(QXK24Fm_Luman, XLevel7Is4096) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_X, 7), 4096ULL);
}
TEST(QXK24Fm_Luman, LevelZeroInvalid) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_Z, 0), 0ULL);
}
TEST(QXK24Fm_Luman, LevelEightInvalid) {
    EXPECT_EQ(qxk24_fm_luman_scale(QXK24_FM_LUMAN_X, 8), 0ULL);
}

TEST(QXK24Fm_Lifecycle, CreateNullKernelReturnsNull) {
    EXPECT_EQ(qxk24_fm_create(nullptr), nullptr);
}
TEST(QXK24Fm_Lifecycle, CreateWithKernelSucceeds) {
    qxk24_kernel_t kernel = make_kernel();
    qxk24_fm_t fm = qxk24_fm_create(kernel);
    ASSERT_NE(fm, nullptr);
    qxk24_fm_destroy(fm);
    destroy_kernel(kernel);
}
TEST(QXK24Fm_Lifecycle, StartNullReturnsError) {
    EXPECT_EQ(qxk24_fm_start(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Fm_Lifecycle, StartSucceeds) {
    qxk24_kernel_t kernel = make_kernel();
    qxk24_fm_t fm = qxk24_fm_create(kernel);
    EXPECT_EQ(qxk24_fm_start(fm), QXK24_OK);
    EXPECT_TRUE(qxk24_fm_is_running(fm));
    qxk24_fm_destroy(fm);
    destroy_kernel(kernel);
}
TEST(QXK24Fm_Lifecycle, CycleBeforeStartReturnsError) {
    qxk24_kernel_t kernel = make_kernel();
    qxk24_fm_t fm = qxk24_fm_create(kernel);
    EXPECT_EQ(qxk24_fm_cycle(fm), QXK24_ERR_NOT_STARTED);
    qxk24_fm_destroy(fm);
    destroy_kernel(kernel);
}
TEST(QXK24Fm_Lifecycle, CycleSucceeds) {
    qxk24_kernel_t kernel = make_kernel();
    qxk24_fm_t fm = qxk24_fm_create(kernel);
    ASSERT_EQ(qxk24_fm_start(fm), QXK24_OK);
    EXPECT_EQ(qxk24_fm_cycle(fm), QXK24_OK);
    qxk24_fm_destroy(fm);
    destroy_kernel(kernel);
}
TEST(QXK24Fm_Lifecycle, SnapshotAfterCycleValid) {
    qxk24_kernel_t kernel = make_kernel();
    qxk24_fm_t fm = qxk24_fm_create(kernel);
    ASSERT_EQ(qxk24_fm_start(fm), QXK24_OK);
    ASSERT_EQ(qxk24_fm_cycle(fm), QXK24_OK);
    QXK24FmSnapshot snap{};
    EXPECT_EQ(qxk24_fm_snapshot(fm, &snap), QXK24_OK);
    EXPECT_NEAR(snap.fm_value, 2.0f, 0.001f);
    EXPECT_NEAR(snap.mp_ratio, 1.0f, 0.001f);
    EXPECT_EQ(snap.cycle_index, 1ULL);
    EXPECT_TRUE(snap.constitutionally_timed);
    qxk24_fm_destroy(fm);
    destroy_kernel(kernel);
}
TEST(QXK24Fm_Lifecycle, StopClearsRunning) {
    qxk24_kernel_t kernel = make_kernel();
    qxk24_fm_t fm = qxk24_fm_create(kernel);
    ASSERT_EQ(qxk24_fm_start(fm), QXK24_OK);
    qxk24_fm_stop(fm);
    EXPECT_FALSE(qxk24_fm_is_running(fm));
    qxk24_fm_destroy(fm);
    destroy_kernel(kernel);
}
