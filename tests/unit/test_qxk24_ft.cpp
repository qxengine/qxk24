/* ============================================================
 * test_qxk24_ft.cpp
 * QXK24 Universal Kernel — Faktor Tenaga Core Tests
 * ============================================================ */

#include "qxk24/ft/qxk24_ft.h"
#include <gtest/gtest.h>

class FtCoreTest : public ::testing::Test {
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

TEST_F(FtCoreTest, TotalEqualsActivePlusPassive) {
    QXK24FtSnapshot s{};
    qxk24_ft_snapshot(ft, &s);
    EXPECT_FLOAT_EQ(s.ft_total, s.hidup_aktif + s.hidup_pasif);
}
TEST_F(FtCoreTest, InitialTotalIsTwo) { EXPECT_FLOAT_EQ(qxk24_ft_total(ft), 2.0f); }
TEST_F(FtCoreTest, FmValueIsTwo) {
    QXK24FtSnapshot s{};
    qxk24_ft_snapshot(ft, &s);
    EXPECT_FLOAT_EQ(s.fm_value, QXK24_FM_VALUE);
}
TEST_F(FtCoreTest, InitialPhaseIsPeSa) {
    QXK24FtSnapshot s{};
    qxk24_ft_snapshot(ft, &s);
    EXPECT_EQ((QXK24FtPhase)s.phase, QXK24_FT_PHASE_PESA);
}
TEST_F(FtCoreTest, WorldIsDtk) {
    QXK24FtSnapshot s{};
    qxk24_ft_snapshot(ft, &s);
    EXPECT_EQ(s.world, QXK24_FT_WORLD_DTK);
}
TEST_F(FtCoreTest, StartSucceeds) { EXPECT_EQ(qxk24_ft_start(ft), QXK24_OK); }
TEST_F(FtCoreTest, CannotStartTwice) {
    ASSERT_EQ(qxk24_ft_start(ft), QXK24_OK);
    EXPECT_EQ(qxk24_ft_start(ft), QXK24_ERR_ALREADY_INITIALIZED);
}
TEST_F(FtCoreTest, CycleBeforeStartFails) {
    EXPECT_EQ(qxk24_ft_cycle(ft), QXK24_ERR_NOT_STARTED);
}
TEST_F(FtCoreTest, CycleAfterStartSucceeds) {
    ASSERT_EQ(qxk24_ft_start(ft), QXK24_OK);
    EXPECT_EQ(qxk24_ft_cycle(ft), QXK24_OK);
}
TEST_F(FtCoreTest, CycleIndexIncrements) {
    qxk24_ft_start(ft);
    qxk24_ft_cycle(ft);
    QXK24FtSnapshot s{};
    qxk24_ft_snapshot(ft, &s);
    EXPECT_EQ(s.cycle_index, 1U);
}
TEST_F(FtCoreTest, XMeasurementAfterCycleIsOne) {
    qxk24_ft_start(ft);
    qxk24_ft_cycle(ft);
    QXK24FtSnapshot s{};
    qxk24_ft_snapshot(ft, &s);
    EXPECT_FLOAT_EQ(s.x_measurement, 1.0f);
}
TEST_F(FtCoreTest, SevenCyclesAdvancePhase) {
    qxk24_ft_start(ft);
    for (int i = 0; i < 7; i++) qxk24_ft_cycle(ft);
    QXK24FtSnapshot s{};
    qxk24_ft_snapshot(ft, &s);
    EXPECT_EQ((QXK24FtPhase)s.phase, QXK24_FT_PHASE_PEDU);
}
TEST_F(FtCoreTest, StopSucceeds) {
    qxk24_ft_start(ft);
    EXPECT_EQ(qxk24_ft_stop(ft), QXK24_OK);
}
TEST_F(FtCoreTest, NoOverlapInitially) {
    EXPECT_EQ(qxk24_ft_check_overlap(ft), QXK24_OK);
}
TEST_F(FtCoreTest, ElementNames) {
    EXPECT_STREQ(qxk24_ft_element_name(QXK24_FT_ELEMENT_TANAH), "Tanah");
    EXPECT_STREQ(qxk24_ft_element_name(QXK24_FT_ELEMENT_ANGIN), "Angin");
}
TEST_F(FtCoreTest, PhaseNames) {
    EXPECT_STREQ(qxk24_ft_phase_name(QXK24_FT_PHASE_PESA), "PeSa");
    EXPECT_STREQ(qxk24_ft_phase_name(QXK24_FT_PHASE_PETU), "PeTu");
}
TEST_F(FtCoreTest, DirectionNames) {
    EXPECT_STREQ(qxk24_ft_direction_name(QXK24_FT_DIR_UTARA), "Utara");
    EXPECT_STREQ(qxk24_ft_direction_name(QXK24_FT_DIR_BAWAH), "Bawah");
}
TEST_F(FtCoreTest, NullCreateFails) {
    qxk24_ft_t out = nullptr;
    EXPECT_EQ(qxk24_ft_create(nullptr, &out), QXK24_ERR_NULL_PARAM);
}
TEST_F(FtCoreTest, NullSnapshotFails) {
    EXPECT_EQ(qxk24_ft_snapshot(ft, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(FtCoreTest, NullTotalIsZero) {
    EXPECT_FLOAT_EQ(qxk24_ft_total(nullptr), 0.0f);
}
