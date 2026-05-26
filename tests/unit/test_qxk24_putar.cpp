/* ============================================================
 * test_qxk24_putar.cpp
 * QXK24 Universal Kernel — Putar Tenaga Tests
 * ============================================================ */

#include "qxk24/ft/qxk24_putar.h"
#include <gtest/gtest.h>

class PutarTest : public ::testing::Test {
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

TEST_F(PutarTest, TotalMovementsIsEight) { EXPECT_EQ(QXK24_PUTAR_TOTAL_MOVEMENTS, 8); }
TEST_F(PutarTest, KeyMovementIsEight) { EXPECT_EQ(QXK24_PUTAR_KEY_MOVEMENT, 8); }
TEST_F(PutarTest, CreateXknSucceeds) {
    qxk24_putar_t p = nullptr;
    EXPECT_EQ(qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p), QXK24_OK);
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, InitialLevelOne) {
    qxk24_putar_t p = nullptr;
    qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p);
    QXK24PutarSnapshot s{};
    qxk24_putar_snapshot(p, &s);
    EXPECT_EQ(s.current_level, 1U);
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, NotStableInitially) {
    qxk24_putar_t p = nullptr;
    qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p);
    EXPECT_FALSE(qxk24_putar_is_stable(p));
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, CycleAdvancesMovement) {
    qxk24_putar_t p = nullptr;
    qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p);
    qxk24_putar_cycle(p);
    QXK24PutarSnapshot s{};
    qxk24_putar_snapshot(p, &s);
    EXPECT_EQ(s.movement_index, 1U);
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, StableAfterEightCycles) {
    qxk24_putar_t p = nullptr;
    qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p);
    for (int i = 0; i < 8; i++) qxk24_putar_cycle(p);
    EXPECT_TRUE(qxk24_putar_is_stable(p));
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, RotationCountAfterEightCycles) {
    qxk24_putar_t p = nullptr;
    qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p);
    for (int i = 0; i < 8; i++) qxk24_putar_cycle(p);
    QXK24PutarSnapshot s{};
    qxk24_putar_snapshot(p, &s);
    EXPECT_EQ(s.rotations_done, 1U);
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, LevelAdvancesAfterRotation) {
    qxk24_putar_t p = nullptr;
    qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p);
    for (int i = 0; i < 8; i++) qxk24_putar_cycle(p);
    QXK24PutarSnapshot s{};
    qxk24_putar_snapshot(p, &s);
    EXPECT_EQ(s.current_level, 2U);
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, EnergyFormula) {
    EXPECT_FLOAT_EQ(qxk24_putar_compute_energy(2.0f, 3), 6.0f);
}
TEST_F(PutarTest, MovementSequenceXknFirstEast) {
    QXK24PutarMovement seq[8]{};
    qxk24_putar_movement_sequence(QXK24_FT_ROTATION_XKN, seq);
    EXPECT_EQ(seq[0].direction, QXK24_FT_DIR_TIMUR);
}
TEST_F(PutarTest, MovementSequenceXkrFirstWest) {
    QXK24PutarMovement seq[8]{};
    qxk24_putar_movement_sequence(QXK24_FT_ROTATION_XKR, seq);
    EXPECT_EQ(seq[0].direction, QXK24_FT_DIR_BARAT);
}
TEST_F(PutarTest, MovementSequenceLastIsKey) {
    QXK24PutarMovement seq[8]{};
    qxk24_putar_movement_sequence(QXK24_FT_ROTATION_XKN, seq);
    EXPECT_TRUE(seq[7].is_key);
}
TEST_F(PutarTest, MovementSequenceLastIndexEight) {
    QXK24PutarMovement seq[8]{};
    qxk24_putar_movement_sequence(QXK24_FT_ROTATION_XKN, seq);
    EXPECT_EQ(seq[7].index, 8U);
}
TEST_F(PutarTest, AdvanceCallsCycle) {
    qxk24_putar_t p = nullptr;
    qxk24_putar_create(ft, QXK24_FT_ROTATION_XKN, &p);
    qxk24_putar_advance(p);
    QXK24PutarSnapshot s{};
    qxk24_putar_snapshot(p, &s);
    EXPECT_EQ(s.movement_index, 1U);
    qxk24_putar_destroy(p);
}
TEST_F(PutarTest, NullCreateFails) {
    qxk24_putar_t p = nullptr;
    EXPECT_EQ(qxk24_putar_create(nullptr, QXK24_FT_ROTATION_XKN, &p),
              QXK24_ERR_NULL_PARAM);
}
TEST_F(PutarTest, NullCycleFails) {
    EXPECT_EQ(qxk24_putar_cycle(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(PutarTest, NullSnapshotFails) {
    EXPECT_EQ(qxk24_putar_snapshot(nullptr, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(PutarTest, NullSequenceFails) {
    EXPECT_EQ(qxk24_putar_movement_sequence(QXK24_FT_ROTATION_XKN, nullptr),
              QXK24_ERR_NULL_PARAM);
}
TEST_F(PutarTest, NullStableFalse) {
    EXPECT_FALSE(qxk24_putar_is_stable(nullptr));
}
