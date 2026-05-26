/**
 * @file test_qxk24_adam_cv.cpp
 * @brief Unit Tests — ADAM Contribution Value Engine
 */

#include <gtest/gtest.h>
#include "qxk24/adam/qxk24_adam_cv.h"

class AdamCvTest : public ::testing::Test {
protected:
    qxk24_adam_cv_t cv = nullptr;
    void SetUp() override { ASSERT_EQ(QXK24_OK, qxk24_adam_cv_create(&cv)); }
    void TearDown() override { (void)qxk24_adam_cv_destroy(cv); }
};

TEST_F(AdamCvTest, CreateSucceeds) { EXPECT_NE(nullptr, cv); }
TEST_F(AdamCvTest, ComputePositive) { EXPECT_GT(qxk24_adam_cv_compute(3, 80.0f, 2.0f), 0.0f); }
TEST_F(AdamCvTest, AssessAtLeastSelf) {
    QXK24CvRecord r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_cv_assess(cv, 80.0f, 3, true, &r));
    EXPECT_GE(r.level, QXK24_CV_DIRI_SENDIRI);
}
TEST_F(AdamCvTest, FailedHukumZFallsToSelf) {
    QXK24CvRecord r;
    qxk24_adam_cv_assess(cv, 95.0f, 7, false, &r);
    EXPECT_EQ(QXK24_CV_DIRI_SENDIRI, r.level);
}
TEST_F(AdamCvTest, UniversalLevelCanBeReached) {
    QXK24CvRecord r;
    qxk24_adam_cv_assess(cv, 96.0f, 7, true, &r);
    EXPECT_EQ(QXK24_CV_ALAM_SEMESTA, r.level);
}
TEST_F(AdamCvTest, FamilyFlagFollowsLevel) {
    QXK24CvRecord r;
    qxk24_adam_cv_assess(cv, 80.0f, 2, true, &r);
    EXPECT_TRUE(r.family_met);
}
TEST_F(AdamCvTest, SnapshotCountsLevels) {
    QXK24CvRecord r;
    qxk24_adam_cv_assess(cv, 80.0f, 2, true, &r);
    QXK24CvSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_cv_snapshot(cv, &s));
    EXPECT_GT(s.total_cv_value, 0.0f);
}
TEST_F(AdamCvTest, NameSelf) { EXPECT_STREQ("DIRI_SENDIRI", qxk24_adam_cv_level_name(QXK24_CV_DIRI_SENDIRI)); }
TEST_F(AdamCvTest, NameWorld) { EXPECT_STREQ("DUNIA", qxk24_adam_cv_level_name(QXK24_CV_DUNIA)); }
TEST_F(AdamCvTest, NameUniverse) { EXPECT_STREQ("ALAM_SEMESTA", qxk24_adam_cv_level_name(QXK24_CV_ALAM_SEMESTA)); }
