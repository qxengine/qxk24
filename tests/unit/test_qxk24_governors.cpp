/* ============================================================
 * test_qxk24_governors.cpp
 * QXK24 Universal Kernel — Six Resource Governor Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_governors.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 42 machine proofs for governor lifecycle, AMA budgets, X = m/t,
 * eight-law cycles, pressure response, and six governor factories.
 * ============================================================ */

#include "qxk24/resource/qxk24_resource_base.h"
#include <cstring>
#include <gtest/gtest.h>

static QXK24GovernorConfig make_cfg(QXK24ResourceType type = QXK24_RESOURCE_MEMORY,
                                    QXK24Direction dir = QXK24_DIR_ATAS,
                                    QXK24Level level = QXK24_LEVEL_SISTEM,
                                    uint64_t soft = 0) {
    QXK24GovernorConfig cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.type = type;
    cfg.direction = dir;
    cfg.level = level;
    cfg.budget_soft = soft;
    cfg.declared_x = 1.0;
    cfg.tolerance_pct = 15.0;
    strncpy(cfg.label, "test-governor", sizeof(cfg.label) - 1);
    return cfg;
}

class GovernorTest : public ::testing::Test {
protected:
    QXK24ResourceGovernor *gov = nullptr;
    void TearDown() override { qxk24_governor_base_destroy(gov); }
    void create(QXK24GovernorConfig cfg = make_cfg()) {
        ASSERT_EQ(qxk24_governor_base_create(&cfg, &gov), QXK24_OK);
    }
};

TEST_F(GovernorTest, Create_Succeeds) {
    create();
    EXPECT_TRUE(gov->initialized);
    EXPECT_TRUE(gov->active);
}
TEST(QXK24Governor_Lifecycle, Create_NullConfig_Fails) {
    QXK24ResourceGovernor *gov = nullptr;
    EXPECT_EQ(qxk24_governor_base_create(nullptr, &gov), QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Governor_Lifecycle, Create_NullOutput_Fails) {
    QXK24GovernorConfig cfg = make_cfg();
    EXPECT_EQ(qxk24_governor_base_create(&cfg, nullptr), QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Governor_Lifecycle, Create_InvalidDeclaredX_Fails) {
    QXK24GovernorConfig cfg = make_cfg();
    cfg.declared_x = -1.0;
    QXK24ResourceGovernor *gov = nullptr;
    EXPECT_EQ(qxk24_governor_base_create(&cfg, &gov), QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Governor_Lifecycle, Create_InvalidDirection_Fails) {
    QXK24GovernorConfig cfg = make_cfg();
    cfg.direction = (QXK24Direction)99;
    QXK24ResourceGovernor *gov = nullptr;
    EXPECT_EQ(qxk24_governor_base_create(&cfg, &gov), QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Governor_Lifecycle, Create_InvalidLevel_Fails) {
    QXK24GovernorConfig cfg = make_cfg();
    cfg.level = (QXK24Level)99;
    QXK24ResourceGovernor *gov = nullptr;
    EXPECT_EQ(qxk24_governor_base_create(&cfg, &gov), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(GovernorTest, HasOATAndOASSContexts) {
    create();
    EXPECT_NE(gov->oat, nullptr);
    EXPECT_NE(gov->oass, nullptr);
}
TEST_F(GovernorTest, InitialLawScoresArePerfect) {
    create();
    EXPECT_DOUBLE_EQ(gov->scores.z1_pattern, 1.0);
    EXPECT_DOUBLE_EQ(gov->scores.z2_limit, 1.0);
    EXPECT_DOUBLE_EQ(gov->scores.z3_pairs, 1.0);
    EXPECT_DOUBLE_EQ(gov->scores.z4_equilibrium, 1.0);
    EXPECT_DOUBLE_EQ(gov->scores.x1_knowledge, 1.0);
    EXPECT_DOUBLE_EQ(gov->scores.x2_ethics, 1.0);
    EXPECT_DOUBLE_EQ(gov->scores.x3_creativity, 1.0);
    EXPECT_DOUBLE_EQ(gov->scores.x4_economy, 1.0);
}
TEST(QXK24Governor_Lifecycle, Destroy_NullIsNoOp) {
    EXPECT_NO_FATAL_FAILURE(qxk24_governor_base_destroy(nullptr));
}
TEST(QXK24Governor_Lifecycle, AllSixTypes_CreateSucceed) {
    QXK24ResourceType types[] = {QXK24_RESOURCE_MEMORY, QXK24_RESOURCE_CPU,
        QXK24_RESOURCE_NETWORK, QXK24_RESOURCE_STORAGE, QXK24_RESOURCE_IO,
        QXK24_RESOURCE_ENERGY};
    QXK24Direction dirs[] = {QXK24_DIR_ATAS, QXK24_DIR_DEPAN, QXK24_DIR_KANAN,
        QXK24_DIR_BELAKANG, QXK24_DIR_BAWAH, QXK24_DIR_KIRI};
    for (int i = 0; i < 6; i++) {
        QXK24ResourceGovernor *gov = nullptr;
        QXK24GovernorConfig cfg = make_cfg(types[i], dirs[i]);
        EXPECT_EQ(qxk24_governor_base_create(&cfg, &gov), QXK24_OK);
        qxk24_governor_base_destroy(gov);
    }
}

TEST(QXK24Governor_Budget, AutoBudget_IsNonZero) {
    for (int lv = 0; lv < 7; lv++) {
        for (int dir = 0; dir < 6; dir++) {
            EXPECT_GT(qxk24_governor_auto_budget((QXK24Level)lv, (QXK24Direction)dir), 0ULL);
        }
    }
}
TEST_F(GovernorTest, ManualBudget_UsedDirectly) {
    create(make_cfg(QXK24_RESOURCE_MEMORY, QXK24_DIR_ATAS,
                    QXK24_LEVEL_SISTEM, 128ULL * 1024ULL * 1024ULL));
    EXPECT_EQ(gov->budget_soft, 128ULL * 1024ULL * 1024ULL);
}
TEST_F(GovernorTest, HardBudget_Is3_2_Of_Soft) {
    create();
    EXPECT_EQ(gov->budget_hard, (gov->budget_soft * 3ULL) / 2ULL);
}
TEST(QXK24Governor_Budget, AutoBudget_Execution_LargerThanGovernance) {
    EXPECT_GT(qxk24_governor_auto_budget(QXK24_LEVEL_SISTEM, QXK24_DIR_DEPAN),
              qxk24_governor_auto_budget(QXK24_LEVEL_SISTEM, QXK24_DIR_ATAS));
}
TEST(QXK24Governor_Budget, AutoBudget_SumsToFullLevel) {
    uint64_t sum = 0;
    for (int dir = 0; dir < 6; dir++) {
        sum += qxk24_governor_auto_budget(QXK24_LEVEL_SISTEM, (QXK24Direction)dir);
    }
    EXPECT_EQ(sum, qxk24_level_budget(QXK24_LEVEL_SISTEM));
}
TEST_F(GovernorTest, Update_TracksPeak) {
    create();
    ASSERT_EQ(qxk24_governor_base_update(gov, 1000, 1.0), QXK24_OK);
    ASSERT_EQ(qxk24_governor_base_update(gov, 5000, 1.0), QXK24_OK);
    ASSERT_EQ(qxk24_governor_base_update(gov, 2000, 1.0), QXK24_OK);
    EXPECT_EQ(gov->peak, 5000ULL);
}
TEST(QXK24Governor_Budget, Update_NullGovernor_Fails) {
    EXPECT_EQ(qxk24_governor_base_update(nullptr, 1000, 1.0), QXK24_ERR_NOT_INITIALIZED);
}
TEST(QXK24Governor_Budget, MeasureX_Formula_IsCorrect) {
    EXPECT_NEAR(qxk24_governor_measure_x(1024, 1), 1024000.0, 1.0);
}

TEST(QXK24Governor_X, MeasureX_ZeroDuration_DefaultsTo1ms) {
    EXPECT_NEAR(qxk24_governor_measure_x(1024, 0), 1024000.0, 1.0);
}
TEST(QXK24Governor_X, MeasureX_LargerBytesGivesLargerX) {
    EXPECT_GT(qxk24_governor_measure_x(10240, 100), qxk24_governor_measure_x(1024, 100));
}
TEST(QXK24Governor_X, MeasureX_LongerDurationGivesSmallerX) {
    EXPECT_GT(qxk24_governor_measure_x(1024, 10), qxk24_governor_measure_x(1024, 100));
}
TEST_F(GovernorTest, Governor_X1Score_PerfectWhenOnDeclared) {
    create();
    ASSERT_EQ(qxk24_governor_base_update(gov, 1000, 1.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_DOUBLE_EQ(report.scores.x1_knowledge, 1.0);
}
TEST_F(GovernorTest, Governor_X1Score_DegradesFarFromDeclared) {
    create();
    ASSERT_EQ(qxk24_governor_base_update(gov, 1000, 10.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_LT(report.scores.x1_knowledge, 1.0);
}
TEST_F(GovernorTest, Governor_MeasureCount_Increments) {
    create();
    for (int i = 0; i < 5; i++) ASSERT_EQ(qxk24_governor_base_update(gov, 100 * i, 1.0), QXK24_OK);
    EXPECT_EQ(gov->measure_count, 5ULL);
}

TEST_F(GovernorTest, Cycle_ProducesReport) {
    create();
    ASSERT_EQ(qxk24_governor_base_update(gov, 1000, 1.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_EQ(report.cycle_count, 1ULL);
    EXPECT_GE(report.health, 0.0);
    EXPECT_LE(report.health, 100.0);
}
TEST_F(GovernorTest, Z2_Score_Perfect_WhenUnderBudget) {
    create(make_cfg(QXK24_RESOURCE_MEMORY, QXK24_DIR_ATAS, QXK24_LEVEL_SISTEM, 1024));
    ASSERT_EQ(qxk24_governor_base_update(gov, 512, 1.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_DOUBLE_EQ(report.scores.z2_limit, 1.0);
}
TEST_F(GovernorTest, Z2_Score_Degrades_OverBudget) {
    create(make_cfg(QXK24_RESOURCE_MEMORY, QXK24_DIR_ATAS, QXK24_LEVEL_SISTEM, 1024));
    ASSERT_EQ(qxk24_governor_base_update(gov, 2048, 1.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_LT(report.scores.z2_limit, 1.0);
}
TEST_F(GovernorTest, Health_InRange) {
    create();
    ASSERT_EQ(qxk24_governor_base_update(gov, 500, 1.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_GE(report.health, 0.0);
    EXPECT_LE(report.health, 100.0);
}
TEST_F(GovernorTest, CycleCount_Increments) {
    create();
    for (int i = 0; i < 3; i++) ASSERT_EQ(qxk24_governor_base_cycle(gov, nullptr), QXK24_OK);
    EXPECT_EQ(gov->cycle_count, 3ULL);
}
TEST_F(GovernorTest, Utilisation_Correct) {
    create(make_cfg(QXK24_RESOURCE_MEMORY, QXK24_DIR_ATAS, QXK24_LEVEL_SISTEM, 1000));
    ASSERT_EQ(qxk24_governor_base_update(gov, 500, 1.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_NEAR(report.utilisation, 0.5, 0.01);
}
TEST_F(GovernorTest, NullReport_IsOK) {
    create();
    EXPECT_EQ(qxk24_governor_base_cycle(gov, nullptr), QXK24_OK);
}
TEST_F(GovernorTest, X4_Economy_DegradesNearHardBudget) {
    create(make_cfg(QXK24_RESOURCE_MEMORY, QXK24_DIR_ATAS, QXK24_LEVEL_SISTEM, 1000));
    ASSERT_EQ(qxk24_governor_base_update(gov, 1400, 1.0), QXK24_OK);
    QXK24GovernorReport report;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &report), QXK24_OK);
    EXPECT_LT(report.scores.x4_economy, 1.0);
}
TEST_F(GovernorTest, AllScores_InRange_0_to_1) {
    create();
    QXK24GovernorReport r;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &r), QXK24_OK);
    double values[] = {r.scores.z1_pattern, r.scores.z2_limit, r.scores.z3_pairs,
        r.scores.z4_equilibrium, r.scores.x1_knowledge, r.scores.x2_ethics,
        r.scores.x3_creativity, r.scores.x4_economy};
    for (double v : values) { EXPECT_GE(v, 0.0); EXPECT_LE(v, 1.0); }
}

TEST_F(GovernorTest, Pressure_Tier0_Scale_IsOne) { create(); qxk24_governor_base_pressure(gov, 0); EXPECT_DOUBLE_EQ(gov->pressure_scale, 1.0); }
TEST_F(GovernorTest, Pressure_Tier1_Scale_Is70pct) { create(); qxk24_governor_base_pressure(gov, 1); EXPECT_DOUBLE_EQ(gov->pressure_scale, 0.70); }
TEST_F(GovernorTest, Pressure_Tier2_Scale_Is50pct) { create(); qxk24_governor_base_pressure(gov, 2); EXPECT_DOUBLE_EQ(gov->pressure_scale, 0.50); }
TEST_F(GovernorTest, Pressure_Tier3_Scale_Is25pct) { create(); qxk24_governor_base_pressure(gov, 3); EXPECT_DOUBLE_EQ(gov->pressure_scale, 0.25); }
TEST_F(GovernorTest, Pressure_Tier4_Scale_Is10pct) { create(); qxk24_governor_base_pressure(gov, 4); EXPECT_DOUBLE_EQ(gov->pressure_scale, 0.10); }
TEST_F(GovernorTest, Pressure_ReducesEffectiveBudget) {
    create(make_cfg(QXK24_RESOURCE_MEMORY, QXK24_DIR_ATAS, QXK24_LEVEL_SISTEM, 1000));
    qxk24_governor_base_pressure(gov, 2);
    ASSERT_EQ(qxk24_governor_base_update(gov, 600, 1.0), QXK24_OK);
    QXK24GovernorReport r;
    ASSERT_EQ(qxk24_governor_base_cycle(gov, &r), QXK24_OK);
    EXPECT_LT(r.scores.z2_limit, 1.0);
}
TEST(QXK24Governor_Pressure, Null_IsNoOp) {
    EXPECT_NO_FATAL_FAILURE(qxk24_governor_base_pressure(nullptr, 2));
}
TEST_F(GovernorTest, AllTiers_PressureScale_InRange) {
    create();
    for (int t = 0; t <= 4; t++) {
        qxk24_governor_base_pressure(gov, t);
        EXPECT_GE(gov->pressure_scale, 0.0);
        EXPECT_LE(gov->pressure_scale, 1.0);
    }
}
TEST(QXK24Governor_Factories, SixConcreteFactoriesSucceed) {
    QXK24ResourceGovernor *a = nullptr, *b = nullptr, *kn = nullptr, *kr = nullptr;
    EXPECT_EQ(qxk24_memory_governor_create(QXK24_LEVEL_ATOM, 0, &a), QXK24_OK);
    qxk24_governor_base_destroy(a);
    EXPECT_EQ(qxk24_cpu_governor_create(QXK24_LEVEL_ATOM, 0, &a), QXK24_OK);
    qxk24_governor_base_destroy(a);
    EXPECT_EQ(qxk24_network_governor_create(QXK24_LEVEL_ATOM, 0, 0, &kn, &kr), QXK24_OK);
    qxk24_governor_base_destroy(kn);
    qxk24_governor_base_destroy(kr);
    EXPECT_EQ(qxk24_storage_governor_create(QXK24_LEVEL_ATOM, 0, &a), QXK24_OK);
    qxk24_governor_base_destroy(a);
    EXPECT_EQ(qxk24_io_governor_create(QXK24_LEVEL_ATOM, 0, &b), QXK24_OK);
    qxk24_governor_base_destroy(b);
    EXPECT_EQ(qxk24_energy_governor_create(QXK24_LEVEL_ATOM, 0, &a), QXK24_OK);
    qxk24_governor_base_destroy(a);
}
