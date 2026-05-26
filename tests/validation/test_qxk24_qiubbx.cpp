/* ============================================================
 * test_qxk24_qiubbx.cpp
 * QXK24 Universal Kernel - QIUBBX Validation Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/validation/test_qxk24_qiubbx.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 32 Phase 6 proofs for QIUBBX validation.
 * ============================================================ */

#include "qxk24/validation/qxk24_qiubbx.h"
#include <gtest/gtest.h>

class QIUBBXTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_qxengine_t gov = nullptr;
    qxk24_bridge_t bridge = nullptr;
    qxk24_qiubbx_t validator = nullptr;

    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
        ASSERT_EQ(qxk24_start(kernel), QXK24_OK);

        QXK24QXEngineConfig gcfg = qxk24_qxengine_default_config();
        gov = qxk24_qxengine_create(&gcfg);
        ASSERT_NE(gov, nullptr);
        ASSERT_EQ(qxk24_qxengine_start(gov), QXK24_OK);

        bridge = qxk24_bridge_create(kernel, gov);
        ASSERT_NE(bridge, nullptr);
        validator = qxk24_qiubbx_create(kernel, bridge);
    }

    void TearDown() override {
        qxk24_qiubbx_destroy(validator);
        qxk24_bridge_destroy(bridge);
        qxk24_qxengine_destroy(gov);
        qxk24_stop(kernel);
        qxk24_destroy(kernel);
    }
};

TEST_F(QIUBBXTest, CreateReturnsValidHandle) { EXPECT_NE(validator, nullptr); }
TEST_F(QIUBBXTest, CreateNullKernelReturnsNull) {
    EXPECT_EQ(qxk24_qiubbx_create(nullptr, bridge), nullptr);
}
TEST_F(QIUBBXTest, CreateNullBridgeReturnsNull) {
    EXPECT_EQ(qxk24_qiubbx_create(kernel, nullptr), nullptr);
}
TEST_F(QIUBBXTest, RunSucceeds) {
    QXK24QIUBBXReport rep{};
    EXPECT_EQ(qxk24_qiubbx_run(validator, &rep), QXK24_OK);
}
TEST_F(QIUBBXTest, RunNullValidatorReturnsError) {
    QXK24QIUBBXReport rep{};
    EXPECT_EQ(qxk24_qiubbx_run(nullptr, &rep), QXK24_ERR_NULL_PARAM);
}
TEST_F(QIUBBXTest, RunNullReportReturnsError) {
    EXPECT_EQ(qxk24_qiubbx_run(validator, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(QIUBBXTest, RunSetsFounder) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_STREQ(rep.founder, "Masa Bayu");
}
TEST_F(QIUBBXTest, RunSetsVersion) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_STREQ(rep.version, "1.0.0");
}
TEST_F(QIUBBXTest, RunSetsTimestamp) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_GT(rep.timestamp_ms, 0ULL);
}
TEST_F(QIUBBXTest, RunIncrementsCycle) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.validation_cycle, 1ULL);
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.validation_cycle, 2ULL);
}
TEST_F(QIUBBXTest, LawCountIsEight) {
    EXPECT_EQ(QXK24_QIUBBX_LAWS, 8U);
}
TEST_F(QIUBBXTest, AllLawTagsSet) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_STREQ(rep.laws[0].law_tag, "Z.1");
    EXPECT_STREQ(rep.laws[7].law_tag, "X.4");
}
TEST_F(QIUBBXTest, AllLawScoresInRange) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    for (uint32_t i = 0; i < QXK24_QIUBBX_LAWS; i++) {
        EXPECT_GE(rep.laws[i].score, 0.0f);
        EXPECT_LE(rep.laws[i].score, 1.0f);
    }
}
TEST_F(QIUBBXTest, AllLawsPass) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.laws_passed, QXK24_QIUBBX_LAWS);
}
TEST_F(QIUBBXTest, DirectionCountIsSix) { EXPECT_EQ(QXK24_QIUBBX_DIRECTIONS, 6U); }
TEST_F(QIUBBXTest, AllSixDirectionsActive) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.directions_active, 6U);
}
TEST_F(QIUBBXTest, AllThreePairsActive) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.directions_paired, 3U);
}
TEST_F(QIUBBXTest, DirectionNamesSet) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_STREQ(rep.directions[0].name, "As");
    EXPECT_STREQ(rep.directions[5].name, "Kr");
}
TEST_F(QIUBBXTest, LevelCountIsSeven) { EXPECT_EQ(QXK24_QIUBBX_LEVELS, 7U); }
TEST_F(QIUBBXTest, AllLevelNamesSet) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_STREQ(rep.levels[0].name, "Zarah");
    EXPECT_STREQ(rep.levels[6].name, "Modul");
}
TEST_F(QIUBBXTest, ModulLevelScale) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.levels[6].scale, QXK24_SCALE_MODUL);
}
TEST_F(QIUBBXTest, ZarahLevelScaleIsOne) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.levels[0].scale, 1ULL);
}
TEST_F(QIUBBXTest, AllLevelsPass) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.levels_passed, QXK24_QIUBBX_LEVELS);
}
TEST_F(QIUBBXTest, KernelHealthInRange) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_GE(rep.kernel_health, 0.0f);
    EXPECT_LE(rep.kernel_health, 100.0f);
}
TEST_F(QIUBBXTest, SovereignScoreInRange) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_GE(rep.sovereign_score, 0.0f);
    EXPECT_LE(rep.sovereign_score, 100.0f);
}
TEST_F(QIUBBXTest, SovereignScoreFormula) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    float expected = rep.kernel_health * 0.50f +
        rep.qxengine_health * 0.30f + rep.memloc_health * 0.20f;
    EXPECT_NEAR(rep.sovereign_score, expected, 0.01f);
}
TEST_F(QIUBBXTest, SovereignAchieved) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_TRUE(rep.sovereign_achieved);
}
TEST_F(QIUBBXTest, TierIsSovereign) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.tier, QXK24_CERT_SOVEREIGN);
}
TEST_F(QIUBBXTest, SovereignScoreFunction) {
    QXK24QIUBBXReport rep{};
    rep.kernel_health = 100.0f;
    rep.qxengine_health = 100.0f;
    rep.memloc_health = 100.0f;
    EXPECT_NEAR(qxk24_qiubbx_sovereign_score(&rep), 100.0f, 0.01f);
}
TEST_F(QIUBBXTest, SovereignScoreFunctionNull) {
    EXPECT_FLOAT_EQ(qxk24_qiubbx_sovereign_score(nullptr), 0.0f);
}
TEST_F(QIUBBXTest, ProofTallyIsCorrect) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.total_proofs,
        QXK24_QIUBBX_LAWS + QXK24_QIUBBX_DIRECTIONS + QXK24_QIUBBX_LEVELS);
}
TEST_F(QIUBBXTest, ConstitutionalComplete) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_TRUE(rep.constitutionally_complete);
}
