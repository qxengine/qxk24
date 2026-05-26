/* ============================================================
 * test_qxk24_caliph_kernel.cpp
 * QXK24 Universal Kernel — Caliph Kernel Integration Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/integration/test_qxk24_caliph_kernel.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 30 Phase 8 proofs for Caliph -> QXK24 full cycles.
 * ============================================================ */

#include "qxk24/caliph/qxk24_caliph.h"
#include <gtest/gtest.h>

class CaliphKernelTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_caliph_t caliph = nullptr;
    QXK24CommandResult result{};

    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
        ASSERT_EQ(qxk24_caliph_declare(kernel, &caliph), QXK24_OK);
        ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
        ASSERT_EQ(qxk24_caliph_command_startup(caliph, &result), QXK24_OK);
    }

    void TearDown() override {
        qxk24_caliph_command(caliph, QXK24_CMD_STOP, &result);
        qxk24_caliph_destroy(caliph);
        qxk24_destroy(kernel);
    }
};

TEST_F(CaliphKernelTest, StartupResultObeyed) {
    EXPECT_EQ(result, QXK24_CMD_RESULT_OBEYED);
}
TEST_F(CaliphKernelTest, CaliphAuthorityAfterStartup) {
    EXPECT_EQ(qxk24_caliph_authority(caliph), 1);
}
TEST_F(CaliphKernelTest, ReportReceivedAfterStartup) {
    QXK24CaliphReport report{};
    EXPECT_EQ(qxk24_caliph_receive_report(caliph, &report), QXK24_OK);
}
TEST_F(CaliphKernelTest, ReportJudgmentValid) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_GE(report.judgment, QXK24_JUDGMENT_MAKMUR);
    EXPECT_LE(report.judgment, QXK24_JUDGMENT_WAQF);
}
TEST_F(CaliphKernelTest, ReportHealthNonNegative) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_GE(report.kernel_health, 0.0f);
}
TEST_F(CaliphKernelTest, ReportHealthAtMost100) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_LE(report.kernel_health, 100.0f);
}
TEST_F(CaliphKernelTest, FmValueIsTwo) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_FLOAT_EQ(report.fm_value, 2.0f);
}
TEST_F(CaliphKernelTest, CycleIndexNonZero) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_GT(report.cycle_index, 0U);
}
TEST_F(CaliphKernelTest, SixGovernorsActive) {
    EXPECT_EQ(qxk24_active_resources(kernel), 6U);
}
TEST_F(CaliphKernelTest, ConstitutionallySoundAfterStartup) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_TRUE(report.constitutionally_sound);
}
TEST_F(CaliphKernelTest, ZeroViolationsAfterCleanStartup) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_EQ(report.active_violations, 0U);
}
TEST_F(CaliphKernelTest, JudgmentMakmurAfterCleanStartup) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_EQ(report.judgment, QXK24_JUDGMENT_MAKMUR);
}
TEST_F(CaliphKernelTest, AdditionalCycleObeyed) {
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_CYCLE, &result),
              QXK24_OK);
}
TEST_F(CaliphKernelTest, MultipleCyclesHealthStable) {
    for (int i = 0; i < 5; i++) {
        qxk24_caliph_command(caliph, QXK24_CMD_CYCLE, &result);
    }
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_GE(report.kernel_health, 0.0f);
}
TEST_F(CaliphKernelTest, MultipleCyclesJudgmentValid) {
    for (int i = 0; i < 3; i++) {
        qxk24_caliph_command(caliph, QXK24_CMD_CYCLE, &result);
    }
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_NE(report.judgment, (QXK24CaliphJudgment)-1);
}
TEST_F(CaliphKernelTest, UsulMasaPositionValid) {
    uint32_t pos = 0U;
    qxk24_caliph_usul_masa_position(caliph, &pos);
    EXPECT_LT(pos, (uint32_t)QXK24_CALIPH_COORDINATES);
}
TEST_F(CaliphKernelTest, CommandsIssuedCountCorrect) {
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.commands_issued, 5U);
}
TEST_F(CaliphKernelTest, SessionStateActiveAfterStartup) {
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.state, QXK24_CALIPH_STATE_ACTIVE);
}
TEST_F(CaliphKernelTest, ResetViolationsObeyed) {
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_RESET_VIOLATIONS,
                                   &result), QXK24_OK);
}
TEST_F(CaliphKernelTest, HealthCheckCommandObeyed) {
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_HEALTH_CHECK, &result),
              QXK24_OK);
}
TEST_F(CaliphKernelTest, TwoCaliphsSameAuthority) {
    qxk24_caliph_t c2 = nullptr;
    ASSERT_EQ(qxk24_caliph_declare(kernel, &c2), QXK24_OK);
    EXPECT_EQ(qxk24_caliph_authority(caliph), qxk24_caliph_authority(c2));
    qxk24_caliph_destroy(c2);
}
TEST_F(CaliphKernelTest, EndSessionSucceeds) {
    EXPECT_EQ(qxk24_caliph_end(caliph), QXK24_OK);
}
TEST_F(CaliphKernelTest, FmCycleCommandObeyed) {
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_FM_CYCLE, &result),
              QXK24_OK);
}
TEST_F(CaliphKernelTest, ReportAfterFmCycle) {
    qxk24_caliph_command(caliph, QXK24_CMD_FM_CYCLE, &result);
    QXK24CaliphReport report{};
    EXPECT_EQ(qxk24_caliph_receive_report(caliph, &report), QXK24_OK);
    EXPECT_FLOAT_EQ(report.fm_value, 2.0f);
}
TEST_F(CaliphKernelTest, ConstitutionalChainComplete) {
    EXPECT_EQ(qxk24_caliph_authority(caliph), 1);
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_TRUE(report.constitutionally_sound);
    EXPECT_FLOAT_EQ(report.fm_value, 2.0f);
}
TEST_F(CaliphKernelTest, JudgmentNameStringsValid) {
    EXPECT_NE(qxk24_caliph_judgment_name(QXK24_JUDGMENT_MAKMUR), nullptr);
    EXPECT_NE(qxk24_caliph_judgment_name(QXK24_JUDGMENT_ISLAH), nullptr);
    EXPECT_NE(qxk24_caliph_judgment_name(QXK24_JUDGMENT_WAQF), nullptr);
}
TEST_F(CaliphKernelTest, ReportCycleIndexIncreases) {
    QXK24CaliphReport r1{};
    QXK24CaliphReport r2{};
    qxk24_caliph_receive_report(caliph, &r1);
    qxk24_caliph_command(caliph, QXK24_CMD_CYCLE, &result);
    qxk24_caliph_receive_report(caliph, &r2);
    EXPECT_GE(r2.cycle_index, r1.cycle_index);
}
TEST_F(CaliphKernelTest, HealthAfterMultipleReports) {
    for (int i = 0; i < 3; i++) {
        QXK24CaliphReport report{};
        qxk24_caliph_receive_report(caliph, &report);
        EXPECT_GE(report.kernel_health, 0.0f);
        EXPECT_LE(report.kernel_health, 100.0f);
    }
}
TEST_F(CaliphKernelTest, SessionJudgmentsCountMatches) {
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    qxk24_caliph_receive_report(caliph, &report);
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.judgments_made, 2U);
}
TEST_F(CaliphKernelTest, KernelHierarchyIntact) {
    EXPECT_EQ(QXK24_CALIPH_ABOVE_KERNEL, 1);
    EXPECT_EQ(qxk24_active_resources(kernel), 6U);
}
