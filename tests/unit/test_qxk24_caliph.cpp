/* ============================================================
 * test_qxk24_caliph.cpp
 * QXK24 Universal Kernel — Caliph Unit Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_caliph.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 40 Phase 8 proofs for the Caliph constitutional layer.
 * ============================================================ */

#include "qxk24/caliph/qxk24_caliph.h"
#include <gtest/gtest.h>

class CaliphTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_caliph_t caliph = nullptr;

    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
        ASSERT_EQ(qxk24_caliph_declare(kernel, &caliph), QXK24_OK);
    }

    void TearDown() override {
        qxk24_caliph_destroy(caliph);
        qxk24_destroy(kernel);
    }
};

TEST_F(CaliphTest, AuthorityIsAlwaysOne) {
    EXPECT_EQ(qxk24_caliph_authority(caliph), 1);
}
TEST_F(CaliphTest, AuthorityConstantMatchesMacro) {
    EXPECT_EQ(qxk24_caliph_authority(caliph), QXK24_CALIPH_AUTHORITY);
}
TEST_F(CaliphTest, AuthorityIsAboveKernel) {
    EXPECT_EQ(QXK24_CALIPH_ABOVE_KERNEL, 1);
}
TEST_F(CaliphTest, AllCaliphsAreEqual) {
    qxk24_caliph_t c2 = nullptr;
    ASSERT_EQ(qxk24_caliph_declare(kernel, &c2), QXK24_OK);
    EXPECT_EQ(qxk24_caliph_authority(caliph), qxk24_caliph_authority(c2));
    qxk24_caliph_destroy(c2);
}
TEST_F(CaliphTest, FullResponsibilityConstant) {
    EXPECT_EQ(QXK24_CALIPH_FULL_RESPONSIBILITY, 1);
}
TEST_F(CaliphTest, EqualConstant) { EXPECT_EQ(QXK24_CALIPH_EQUAL, 1); }
TEST_F(CaliphTest, LivesInMDKConstant) {
    EXPECT_EQ(QXK24_CALIPH_LIVES_IN_MDK, 1);
}
TEST_F(CaliphTest, NullAuthorityReturnsOne) {
    EXPECT_EQ(qxk24_caliph_authority(nullptr), 1);
}

TEST_F(CaliphTest, DeclareSucceeds) {
    QXK24CaliphSession session{};
    ASSERT_EQ(qxk24_caliph_session(caliph, &session), QXK24_OK);
    EXPECT_EQ(session.state, QXK24_CALIPH_STATE_DECLARED);
}
TEST_F(CaliphTest, ActivateSucceeds) {
    EXPECT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.state, QXK24_CALIPH_STATE_ACTIVE);
}
TEST_F(CaliphTest, CannotActivateTwice) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    EXPECT_NE(qxk24_caliph_activate(caliph), QXK24_OK);
}
TEST_F(CaliphTest, EndSucceeds) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    EXPECT_EQ(qxk24_caliph_end(caliph), QXK24_OK);
}
TEST_F(CaliphTest, SessionIdIsNonZero) {
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_GT(session.session_id, 0U);
}
TEST_F(CaliphTest, SessionAuthorityIsOne) {
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.authority, 1);
}
TEST_F(CaliphTest, SessionEqualIsOne) {
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.equal, 1);
}
TEST_F(CaliphTest, InitialCommandsIssuedZero) {
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.commands_issued, 0U);
}
TEST_F(CaliphTest, DeclareNullKernelFails) {
    qxk24_caliph_t c = nullptr;
    EXPECT_NE(qxk24_caliph_declare(nullptr, &c), QXK24_OK);
}
TEST_F(CaliphTest, DeclareNullOutputFails) {
    EXPECT_NE(qxk24_caliph_declare(kernel, nullptr), QXK24_OK);
}

TEST_F(CaliphTest, CommandBeforeActivateFails) {
    QXK24CommandResult result{};
    EXPECT_NE(qxk24_caliph_command(caliph, QXK24_CMD_START, &result), QXK24_OK);
}
TEST_F(CaliphTest, StartCommandObeyed) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_START, &result), QXK24_OK);
    EXPECT_EQ(result, QXK24_CMD_RESULT_OBEYED);
}
TEST_F(CaliphTest, RegisterGovernorsObeyed) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    ASSERT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_START, &result), QXK24_OK);
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_REGISTER_GOVERNORS,
                                   &result), QXK24_OK);
    EXPECT_EQ(result, QXK24_CMD_RESULT_OBEYED);
}
TEST_F(CaliphTest, CommandsIssuedCountIncrement) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    qxk24_caliph_command(caliph, QXK24_CMD_START, &result);
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.commands_issued, 1U);
}
TEST_F(CaliphTest, UsulMasaPositionAdvances) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    qxk24_caliph_command(caliph, QXK24_CMD_START, &result);
    uint32_t pos = 0U;
    qxk24_caliph_usul_masa_position(caliph, &pos);
    EXPECT_EQ(pos, 1U);
}
TEST_F(CaliphTest, UsulMasaPositionWrapsAtSix) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    qxk24_caliph_command(caliph, QXK24_CMD_START, &result);
    for (int i = 0; i < 5; i++) {
        qxk24_caliph_command(caliph, QXK24_CMD_HEALTH_CHECK, &result);
    }
    uint32_t pos = 1U;
    qxk24_caliph_usul_masa_position(caliph, &pos);
    EXPECT_EQ(pos, 0U);
}
TEST_F(CaliphTest, StartupSequenceObeyed) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    EXPECT_EQ(qxk24_caliph_command_startup(caliph, &result), QXK24_OK);
    EXPECT_EQ(result, QXK24_CMD_RESULT_OBEYED);
}
TEST_F(CaliphTest, FmCycleObeyed) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    ASSERT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_START, &result), QXK24_OK);
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_FM_CYCLE, &result),
              QXK24_OK);
}
TEST_F(CaliphTest, GenerateReportObeyed) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    ASSERT_EQ(qxk24_caliph_command_startup(caliph, &result), QXK24_OK);
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_GENERATE_REPORT,
                                   &result), QXK24_OK);
}
TEST_F(CaliphTest, InvalidCommandFaults) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    EXPECT_NE(qxk24_caliph_command(caliph, (QXK24CaliphCommand)0xFF,
                                   &result), QXK24_OK);
    EXPECT_EQ(result, QXK24_CMD_RESULT_FAULT);
}
TEST_F(CaliphTest, NullCommandResultFails) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    EXPECT_NE(qxk24_caliph_command(caliph, QXK24_CMD_START, nullptr), QXK24_OK);
}
TEST_F(CaliphTest, StopCommandObeyed) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    ASSERT_EQ(qxk24_caliph_command_startup(caliph, &result), QXK24_OK);
    EXPECT_EQ(qxk24_caliph_command(caliph, QXK24_CMD_STOP, &result), QXK24_OK);
}

TEST_F(CaliphTest, JudgmentNameMakmur) {
    EXPECT_STREQ(qxk24_caliph_judgment_name(QXK24_JUDGMENT_MAKMUR), "MAKMUR");
}
TEST_F(CaliphTest, JudgmentNameIslah) {
    EXPECT_STREQ(qxk24_caliph_judgment_name(QXK24_JUDGMENT_ISLAH), "ISLAH");
}
TEST_F(CaliphTest, JudgmentNameWaqf) {
    EXPECT_STREQ(qxk24_caliph_judgment_name(QXK24_JUDGMENT_WAQF), "WAQF");
}
TEST_F(CaliphTest, MakmurThresholdIs80) {
    EXPECT_FLOAT_EQ(QXK24_JUDGMENT_MAKMUR_MIN, 80.0f);
}
TEST_F(CaliphTest, IslahThresholdIs50) {
    EXPECT_FLOAT_EQ(QXK24_JUDGMENT_ISLAH_MIN, 50.0f);
}
TEST_F(CaliphTest, ReportReceiveAfterStartup) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    ASSERT_EQ(qxk24_caliph_command_startup(caliph, &result), QXK24_OK);
    QXK24CaliphReport report{};
    EXPECT_EQ(qxk24_caliph_receive_report(caliph, &report), QXK24_OK);
}
TEST_F(CaliphTest, ReportHealthNonNegative) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    qxk24_caliph_command_startup(caliph, &result);
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_GE(report.kernel_health, 0.0f);
}
TEST_F(CaliphTest, ReportHealthAtMost100) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    qxk24_caliph_command_startup(caliph, &result);
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_LE(report.kernel_health, 100.0f);
}
TEST_F(CaliphTest, ReportFmValueIsTwo) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    qxk24_caliph_command_startup(caliph, &result);
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    EXPECT_FLOAT_EQ(report.fm_value, 2.0f);
}
TEST_F(CaliphTest, ReportsReceivedCountIncrement) {
    ASSERT_EQ(qxk24_caliph_activate(caliph), QXK24_OK);
    QXK24CommandResult result{};
    qxk24_caliph_command_startup(caliph, &result);
    QXK24CaliphReport report{};
    qxk24_caliph_receive_report(caliph, &report);
    QXK24CaliphSession session{};
    qxk24_caliph_session(caliph, &session);
    EXPECT_EQ(session.reports_received, 1U);
}
