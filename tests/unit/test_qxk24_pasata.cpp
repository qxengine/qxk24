/* ============================================================
 * test_qxk24_pasata.cpp
 * QXK24 Universal Kernel — PASATA Tests
 * ============================================================ */

#include "qxk24/ft/qxk24_pasata.h"
#include <gtest/gtest.h>

class PasataTest : public ::testing::Test {
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

TEST_F(PasataTest, LevelOnePassiveTwoActiveOne) {
    QXK24PasataLevel info{};
    ASSERT_EQ(qxk24_pasata_level_info(1, &info), QXK24_OK);
    EXPECT_EQ(info.passive_count, 2U);
    EXPECT_EQ(info.active_count, 1U);
}
TEST_F(PasataTest, LevelSevenPassiveEightActiveSeven) {
    QXK24PasataLevel info{};
    ASSERT_EQ(qxk24_pasata_level_info(7, &info), QXK24_OK);
    EXPECT_EQ(info.passive_count, 8U);
    EXPECT_EQ(info.active_count, 7U);
}
TEST_F(PasataTest, DeltaAlwaysOne) {
    for (uint8_t i = 1; i <= 7; i++) {
        QXK24PasataLevel info{};
        qxk24_pasata_level_info(i, &info);
        EXPECT_EQ(info.passive_count - info.active_count, QXK24_PASATA_DELTA);
    }
}
TEST_F(PasataTest, WaveSizeEqualsLevel) {
    QXK24PasataLevel info{};
    qxk24_pasata_level_info(5, &info);
    EXPECT_EQ(info.wave_x, 5U);
    EXPECT_EQ(info.wave_y, 5U);
}
TEST_F(PasataTest, EnergyLevelOneIsTwo) {
    EXPECT_FLOAT_EQ(qxk24_pasata_compute_energy(1, 1), 2.0f);
}
TEST_F(PasataTest, EnergyLevelSevenIs56) {
    EXPECT_FLOAT_EQ(qxk24_pasata_compute_energy(7, 1), 56.0f);
}
TEST_F(PasataTest, EnergyMultipleWaves) {
    EXPECT_FLOAT_EQ(qxk24_pasata_compute_energy(3, 4), 48.0f);
}
TEST_F(PasataTest, CreateSucceeds) {
    qxk24_pasata_t p = nullptr;
    EXPECT_EQ(qxk24_pasata_create(ft, 1, &p), QXK24_OK);
    qxk24_pasata_destroy(p);
}
TEST_F(PasataTest, InvalidLowLevelFails) {
    qxk24_pasata_t p = nullptr;
    EXPECT_EQ(qxk24_pasata_create(ft, 0, &p), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(PasataTest, InvalidHighLevelFails) {
    QXK24PasataLevel info{};
    EXPECT_EQ(qxk24_pasata_level_info(8, &info), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(PasataTest, CycleAccumulatesEnergy) {
    qxk24_pasata_t p = nullptr;
    qxk24_pasata_create(ft, 2, &p);
    qxk24_pasata_cycle(p);
    QXK24PasataSnapshot s{};
    qxk24_pasata_snapshot(p, &s);
    EXPECT_FLOAT_EQ(s.energy_total, 6.0f);
    qxk24_pasata_destroy(p);
}
TEST_F(PasataTest, LawSatisfiedAfterCycle) {
    qxk24_pasata_t p = nullptr;
    qxk24_pasata_create(ft, 2, &p);
    qxk24_pasata_cycle(p);
    EXPECT_EQ(qxk24_pasata_check_law(p), QXK24_OK);
    qxk24_pasata_destroy(p);
}
TEST_F(PasataTest, AdvanceIncreasesLevel) {
    qxk24_pasata_t p = nullptr;
    qxk24_pasata_create(ft, 1, &p);
    qxk24_pasata_advance(p);
    QXK24PasataSnapshot s{};
    qxk24_pasata_snapshot(p, &s);
    EXPECT_EQ(s.current_level, 2U);
    qxk24_pasata_destroy(p);
}
TEST_F(PasataTest, AdvanceStopsAtSeven) {
    qxk24_pasata_t p = nullptr;
    qxk24_pasata_create(ft, 7, &p);
    qxk24_pasata_advance(p);
    QXK24PasataSnapshot s{};
    qxk24_pasata_snapshot(p, &s);
    EXPECT_EQ(s.current_level, 7U);
    qxk24_pasata_destroy(p);
}
TEST_F(PasataTest, SnapshotCopiesCounts) {
    qxk24_pasata_t p = nullptr;
    qxk24_pasata_create(ft, 4, &p);
    QXK24PasataSnapshot s{};
    qxk24_pasata_snapshot(p, &s);
    EXPECT_EQ(s.passive_count, 5U);
    EXPECT_EQ(s.active_count, 4U);
    qxk24_pasata_destroy(p);
}
TEST_F(PasataTest, NullCreateFails) {
    qxk24_pasata_t p = nullptr;
    EXPECT_EQ(qxk24_pasata_create(nullptr, 1, &p), QXK24_ERR_NULL_PARAM);
}
TEST_F(PasataTest, NullCycleFails) {
    EXPECT_EQ(qxk24_pasata_cycle(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(PasataTest, NullSnapshotFails) {
    EXPECT_EQ(qxk24_pasata_snapshot(nullptr, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(PasataTest, NullInfoFails) {
    EXPECT_EQ(qxk24_pasata_level_info(1, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(PasataTest, NullLawFails) {
    EXPECT_EQ(qxk24_pasata_check_law(nullptr), QXK24_ERR_NULL_PARAM);
}
