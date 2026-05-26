/* ============================================================
 * test_qxk24_adam_hukumz.cpp
 * QXK24 Universal Kernel — ADAM Hukum Z Unit Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_adam_hukumz.cpp
 * License : Apache 2.0
 *
 * Description:
 * Tests ADAM Hukum Z validation, failure accounting, snapshots,
 * and law name strings.
 * ============================================================ */

#include <gtest/gtest.h>
#include "qxk24/adam/qxk24_adam_hukumz.h"

class AdamHukumZTest : public ::testing::Test {
protected:
    qxk24_adam_hukumz_t hz = nullptr;
    void SetUp() override { ASSERT_EQ(QXK24_OK, qxk24_adam_hukumz_create(&hz)); }
    void TearDown() override { (void)qxk24_adam_hukumz_destroy(hz); }
};

TEST_F(AdamHukumZTest, CreateSucceeds) { EXPECT_NE(nullptr, hz); }
TEST_F(AdamHukumZTest, HealthyStatePassesAll) {
    QXK24HukumZResult r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_hukumz_validate(hz, 90.0f, 4, 2.0f, 2.0f, &r));
    EXPECT_TRUE(r.all_passed);
}
TEST_F(AdamHukumZTest, LowHealthFailsAllCheck) {
    QXK24HukumZResult r;
    qxk24_adam_hukumz_validate(hz, 20.0f, 4, 2.0f, 2.0f, &r);
    EXPECT_NE(QXK24_OK, qxk24_adam_hukumz_check_all(&r));
}
TEST_F(AdamHukumZTest, InvalidCapacityFailsKadar) {
    QXK24HukumZResult r;
    qxk24_adam_hukumz_validate(hz, 90.0f, 0, 2.0f, 2.0f, &r);
    EXPECT_FALSE(r.kadar_within);
}
TEST_F(AdamHukumZTest, TotalScoreWithinRange) {
    QXK24HukumZResult r;
    qxk24_adam_hukumz_validate(hz, 90.0f, 4, 2.0f, 2.0f, &r);
    EXPECT_GE(r.total_score, 0.0f);
    EXPECT_LE(r.total_score, 1.0f);
}
TEST_F(AdamHukumZTest, SnapshotCountsValidation) {
    QXK24HukumZResult r;
    qxk24_adam_hukumz_validate(hz, 90.0f, 4, 2.0f, 2.0f, &r);
    QXK24HukumZSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_hukumz_snapshot(hz, &s));
    EXPECT_EQ(1u, s.total_validations);
}
TEST_F(AdamHukumZTest, NamePola) { EXPECT_STREQ("POLA", qxk24_adam_hukumz_name(QXK24_HUKUMZ_POLA)); }
TEST_F(AdamHukumZTest, NameKadar) { EXPECT_STREQ("KADAR", qxk24_adam_hukumz_name(QXK24_HUKUMZ_KADAR)); }
TEST_F(AdamHukumZTest, NamePasangan) { EXPECT_STREQ("PASANGAN", qxk24_adam_hukumz_name(QXK24_HUKUMZ_PASANGAN)); }
TEST_F(AdamHukumZTest, NameKeseimbangan) { EXPECT_STREQ("KESEIMBANGAN", qxk24_adam_hukumz_name(QXK24_HUKUMZ_KESEIMBANGAN)); }
