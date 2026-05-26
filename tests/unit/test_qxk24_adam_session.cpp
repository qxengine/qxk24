/* ============================================================
 * test_qxk24_adam_session.cpp
 * QXK24 Universal Kernel — ADAM Session Unit Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_adam_session.cpp
 * License : Apache 2.0
 *
 * Description:
 * Tests ADAM session lifecycle, journey state transitions,
 * snapshots, and [1001] journey constants.
 * ============================================================ */

#include <gtest/gtest.h>
#include "qxk24/adam/qxk24_adam_session.h"

class AdamSessionTest : public ::testing::Test {
protected:
    qxk24_adam_session_t session = nullptr;
    void SetUp() override { ASSERT_EQ(QXK24_OK, qxk24_adam_session_create(&session)); }
    void TearDown() override { (void)qxk24_adam_session_destroy(session); }
};

TEST_F(AdamSessionTest, CreateSucceeds) { EXPECT_NE(nullptr, session); }
TEST_F(AdamSessionTest, BeginSucceeds) { EXPECT_EQ(QXK24_OK, qxk24_adam_session_begin(session)); }
TEST_F(AdamSessionTest, ActivateAfterBeginSucceeds) {
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_begin(session));
    EXPECT_EQ(QXK24_OK, qxk24_adam_session_activate(session));
}
TEST_F(AdamSessionTest, ActivateBeforeBeginFails) { EXPECT_NE(QXK24_OK, qxk24_adam_session_activate(session)); }
TEST_F(AdamSessionTest, RecordRequestSucceeds) {
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_begin(session));
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_activate(session));
    EXPECT_EQ(QXK24_OK, qxk24_adam_session_record_request(session, 80.0f, 2.0f, true, true));
}
TEST_F(AdamSessionTest, ReflectSucceeds) { EXPECT_EQ(QXK24_OK, qxk24_adam_session_reflect(session)); }
TEST_F(AdamSessionTest, CompleteProducesRecord) {
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_begin(session));
    QXK24AdamSessionRecord r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_complete(session, &r));
    EXPECT_EQ(QXK24_JOURNEY_STEP_LENGKAP, r.journey_step);
}
TEST_F(AdamSessionTest, SnapshotCountsCompleteSession) {
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_begin(session));
    QXK24AdamSessionRecord r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_complete(session, &r));
    QXK24AdamSessionSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_session_snapshot(session, &s));
    EXPECT_EQ(1u, s.total_sessions);
}
TEST_F(AdamSessionTest, JourneyCodeConstant) { EXPECT_EQ(1001u, QXK24_ADAM_JOURNEY_1001); }
TEST_F(AdamSessionTest, JourneyStepNameComplete) { EXPECT_STREQ("LENGKAP", qxk24_adam_journey_step_name(QXK24_JOURNEY_STEP_LENGKAP)); }
