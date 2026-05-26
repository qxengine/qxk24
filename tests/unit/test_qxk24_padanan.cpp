/* ============================================================
 * test_qxk24_padanan.cpp
 * QXK24 Universal Kernel — Padanan Tenaga Tests
 * ============================================================ */

#include "qxk24/ft/qxk24_padanan.h"
#include <gtest/gtest.h>

class PadananTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_ft_t ft = nullptr;
    qxk24_padanan_t padanan = nullptr;
    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
        ASSERT_EQ(qxk24_ft_create(kernel, &ft), QXK24_OK);
        ASSERT_EQ(qxk24_padanan_create(ft, QXK24_PADANAN_DOM_API, &padanan),
                  QXK24_OK);
    }
    void TearDown() override {
        qxk24_padanan_destroy(padanan);
        qxk24_ft_destroy(ft);
        qxk24_destroy(kernel);
    }
};

TEST_F(PadananTest, ConstantsMatch) {
    EXPECT_EQ(QXK24_PADANAN_ELEMENTS, 4);
    EXPECT_EQ(QXK24_PADANAN_COORDINATES, 5);
}
TEST_F(PadananTest, BrainRatioIs700) {
    EXPECT_EQ(qxk24_padanan_energy_ratio(true), 700U);
}
TEST_F(PadananTest, BodyRatioIsOne) {
    EXPECT_EQ(qxk24_padanan_energy_ratio(false), 1U);
}
TEST_F(PadananTest, DominantNameApi) {
    EXPECT_STREQ(qxk24_padanan_dominant_name(QXK24_PADANAN_DOM_API), "Ai2ThArAn");
}
TEST_F(PadananTest, DominantNameTanah) {
    EXPECT_STREQ(qxk24_padanan_dominant_name(QXK24_PADANAN_DOM_TANAH), "Th2ArAnAi");
}
TEST_F(PadananTest, DominantNameAir) {
    EXPECT_STREQ(qxk24_padanan_dominant_name(QXK24_PADANAN_DOM_AIR), "Ar2AnThAi");
}
TEST_F(PadananTest, DominantNameAngin) {
    EXPECT_STREQ(qxk24_padanan_dominant_name(QXK24_PADANAN_DOM_ANGIN), "An2AiThAr");
}
TEST_F(PadananTest, ProcessNames) {
    EXPECT_STREQ(qxk24_padanan_process_name(QXK24_PADANAN_PROC_SA), "Sa");
    EXPECT_STREQ(qxk24_padanan_process_name(QXK24_PADANAN_PROC_PA), "Pa");
}
TEST_F(PadananTest, SnapshotInitialDominant) {
    QXK24PadananSnapshot s{};
    qxk24_padanan_snapshot(padanan, &s);
    EXPECT_EQ(s.dominant, QXK24_PADANAN_DOM_API);
}
TEST_F(PadananTest, SnapshotInitialEnergy) {
    QXK24PadananSnapshot s{};
    qxk24_padanan_snapshot(padanan, &s);
    EXPECT_FLOAT_EQ(s.energy_value, 1.0f);
}
TEST_F(PadananTest, CycleIncrementsGL) {
    qxk24_padanan_cycle(padanan);
    QXK24PadananSnapshot s{};
    qxk24_padanan_snapshot(padanan, &s);
    EXPECT_EQ(s.gl_count, 1U);
}
TEST_F(PadananTest, CycleIncrementsGG) {
    qxk24_padanan_cycle(padanan);
    QXK24PadananSnapshot s{};
    qxk24_padanan_snapshot(padanan, &s);
    EXPECT_EQ(s.gg_count, 1U);
}
TEST_F(PadananTest, SixteenCyclesComplete) {
    for (int i = 0; i < 16; i++) qxk24_padanan_cycle(padanan);
    QXK24PadananSnapshot s{};
    qxk24_padanan_snapshot(padanan, &s);
    EXPECT_TRUE(s.cycle_complete);
}
TEST_F(PadananTest, MovementsReturnFour) {
    QXK24PadananMovement moves[4]{};
    EXPECT_EQ(qxk24_padanan_movements(QXK24_PADANAN_DOM_API,
        QXK24_PADANAN_PROC_SA, moves), QXK24_OK);
    EXPECT_EQ(moves[3].gl_index, 4U);
}
TEST_F(PadananTest, MovementGGIndexUsesProcess) {
    QXK24PadananMovement moves[4]{};
    qxk24_padanan_movements(QXK24_PADANAN_DOM_API, QXK24_PADANAN_PROC_DU, moves);
    EXPECT_EQ(moves[0].gg_index, 5U);
}
TEST_F(PadananTest, DominantMovementIsApi) {
    QXK24PadananMovement moves[4]{};
    qxk24_padanan_movements(QXK24_PADANAN_DOM_API, QXK24_PADANAN_PROC_SA, moves);
    EXPECT_EQ(moves[0].element, QXK24_FT_ELEMENT_API);
}
TEST_F(PadananTest, InvalidDominantFails) {
    qxk24_padanan_t out = nullptr;
    EXPECT_EQ(qxk24_padanan_create(ft, (QXK24PadananDominant)9, &out),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(PadananTest, NullCreateFails) {
    EXPECT_EQ(qxk24_padanan_create(nullptr, QXK24_PADANAN_DOM_API, &padanan),
              QXK24_ERR_NULL_PARAM);
}
TEST_F(PadananTest, NullSnapshotFails) {
    EXPECT_EQ(qxk24_padanan_snapshot(padanan, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(PadananTest, NullMovementsFails) {
    EXPECT_EQ(qxk24_padanan_movements(QXK24_PADANAN_DOM_API,
        QXK24_PADANAN_PROC_SA, nullptr), QXK24_ERR_NULL_PARAM);
}
