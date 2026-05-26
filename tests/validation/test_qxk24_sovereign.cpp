/* ============================================================
 * test_qxk24_sovereign.cpp
 * QXK24 Universal Kernel - SOVEREIGN Health Proofs
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/validation/test_qxk24_sovereign.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 24 Phase 6 proofs for SOVEREIGN certification math.
 * ============================================================ */

#include "qxk24/validation/qxk24_qiubbx.h"
#include <gtest/gtest.h>

TEST(QXK24Sovereign, ThresholdIs95) { EXPECT_FLOAT_EQ(QXK24_QIUBBX_SOVEREIGN, 95.0f); }
TEST(QXK24Sovereign, ProfessionalThresholdIs80) { EXPECT_FLOAT_EQ(QXK24_HEALTH_PROFESSIONAL, 80.0f); }
TEST(QXK24Sovereign, StandardThresholdIs60) { EXPECT_FLOAT_EQ(QXK24_HEALTH_STANDARD, 60.0f); }
TEST(QXK24Sovereign, PerfectScoreIsSovereign) {
    QXK24QIUBBXReport rep{};
    rep.kernel_health = 100.0f;
    rep.qxengine_health = 100.0f;
    rep.memloc_health = 100.0f;
    EXPECT_EQ(qxk24_health_tier(qxk24_qiubbx_sovereign_score(&rep)),
              QXK24_CERT_SOVEREIGN);
}
TEST(QXK24Sovereign, Kernel100QXEngine80Memloc80IsProfessional) {
    QXK24QIUBBXReport rep{};
    rep.kernel_health = 100.0f;
    rep.qxengine_health = 80.0f;
    rep.memloc_health = 80.0f;
    EXPECT_NEAR(qxk24_qiubbx_sovereign_score(&rep), 90.0f, 0.01f);
}
TEST(QXK24Sovereign, AllAt95IsSovereign) {
    QXK24QIUBBXReport rep{};
    rep.kernel_health = 95.0f;
    rep.qxengine_health = 95.0f;
    rep.memloc_health = 95.0f;
    EXPECT_NEAR(qxk24_qiubbx_sovereign_score(&rep), 95.0f, 0.01f);
}
TEST(QXK24Sovereign, ZeroScoreIsUncertified) {
    QXK24QIUBBXReport rep{};
    EXPECT_EQ(qxk24_health_tier(qxk24_qiubbx_sovereign_score(&rep)),
              QXK24_CERT_UNCERTIFIED);
}
TEST(QXK24Sovereign, KernelWeightIs050) {
    QXK24QIUBBXReport rep{};
    rep.kernel_health = 100.0f;
    EXPECT_NEAR(qxk24_qiubbx_sovereign_score(&rep), 50.0f, 0.01f);
}
TEST(QXK24Sovereign, QXEngineWeightIs030) {
    QXK24QIUBBXReport rep{};
    rep.qxengine_health = 100.0f;
    EXPECT_NEAR(qxk24_qiubbx_sovereign_score(&rep), 30.0f, 0.01f);
}
TEST(QXK24Sovereign, MemlocWeightIs020) {
    QXK24QIUBBXReport rep{};
    rep.memloc_health = 100.0f;
    EXPECT_NEAR(qxk24_qiubbx_sovereign_score(&rep), 20.0f, 0.01f);
}
TEST(QXK24Sovereign, WeightsSumTo100) {
    QXK24QIUBBXReport rep{};
    rep.kernel_health = 100.0f;
    rep.qxengine_health = 100.0f;
    rep.memloc_health = 100.0f;
    EXPECT_NEAR(qxk24_qiubbx_sovereign_score(&rep), 100.0f, 0.01f);
}
TEST(QXK24Sovereign, QXK24LevelIsModul) { EXPECT_EQ(QXK24_LEVEL_MODUL, (QXK24Level)6); }
TEST(QXK24Sovereign, QXEngineK24LevelIsObjek) {
    EXPECT_EQ(QXK24_QXENGINE_K24_LEVEL, QXK24_LEVEL_OBJEK);
}
TEST(QXK24Sovereign, QXMemlocK24LevelIsAtom) {
    EXPECT_EQ(QXK24_MEMLOC_K24_LEVEL, QXK24_LEVEL_ATOM);
}
TEST(QXK24Sovereign, ModulAMAColumn) {
    EXPECT_EQ(qxk24_ama_col_inline(6), 8192ULL);
}
TEST(QXK24Sovereign, AtomAMAColumn) {
    EXPECT_EQ(qxk24_memloc_ama_column(), 32U);
}
TEST(QXK24Sovereign, ObjekAMAColumn) {
    EXPECT_EQ(qxk24_ama_col_inline(4), 512ULL);
}
TEST(QXK24Sovereign, EightLawWeightSumIsOne) {
    float sum = 0.0f;
    for (uint8_t i = 0; i < 8U; i++) sum += qxk24_health_weight(i);
    EXPECT_NEAR(sum, 1.0f, 0.00001f);
}
TEST(QXK24Sovereign, SevenLevelsConfirmed) { EXPECT_EQ(QXK24_QIUBBX_LEVELS, 7U); }
TEST(QXK24Sovereign, SixDirectionsConfirmed) { EXPECT_EQ(QXK24_QIUBBX_DIRECTIONS, 6U); }
TEST(QXK24Sovereign, EightLawsConfirmed) { EXPECT_EQ(QXK24_QIUBBX_LAWS, 8U); }
TEST(QXK24Sovereign, ThreePairsConfirmed) {
    EXPECT_EQ(QXK24_QIUBBX_DIRECTIONS / 2U, 3U);
}
TEST(QXK24Sovereign, QIUBBXVersionIsFinal) {
    EXPECT_STREQ(QXK24_QIUBBX_VERSION, "1.0.0");
}
TEST(QXK24Sovereign, MemlocNameIsK24at) {
    EXPECT_STREQ(QXK24_MEMLOC_NAME, "QXMemloc-K24at");
}
