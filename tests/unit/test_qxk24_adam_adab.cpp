/* ============================================================
 * test_qxk24_adam_adab.cpp
 * QXK24 Universal Kernel — ADAM Adab Unit Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_adam_adab.cpp
 * License : Apache 2.0
 *
 * Description:
 * Tests ADAM Adab scoring, satisfaction checks, snapshots, and
 * constitutional conduct element names.
 * ============================================================ */

#include <gtest/gtest.h>
#include "qxk24/adam/qxk24_adam_adab.h"

class AdamAdabTest : public ::testing::Test {
protected:
    qxk24_adam_adab_t adab = nullptr;
    void SetUp() override { ASSERT_EQ(QXK24_OK, qxk24_adam_adab_create(&adab)); }
    void TearDown() override { (void)qxk24_adam_adab_destroy(adab); }
};

TEST_F(AdamAdabTest, CreateSucceeds) { EXPECT_NE(nullptr, adab); }
TEST_F(AdamAdabTest, HighScoreSatisfiesAdab) {
    QXK24AdabScore s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_adab_score(adab, 90.0f, 7, 7, &s));
    EXPECT_TRUE(s.adab_satisfied);
}
TEST_F(AdamAdabTest, LowScoreViolatesAdab) {
    QXK24AdabScore s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_adab_score(adab, 10.0f, 1, 1, &s));
    EXPECT_FALSE(s.adab_satisfied);
}
TEST_F(AdamAdabTest, CheckSatisfiedPasses) {
    QXK24AdabScore s;
    qxk24_adam_adab_score(adab, 90.0f, 7, 7, &s);
    EXPECT_EQ(QXK24_OK, qxk24_adam_adab_check(&s));
}
TEST_F(AdamAdabTest, CheckViolatedFails) {
    QXK24AdabScore s;
    qxk24_adam_adab_score(adab, 10.0f, 1, 1, &s);
    EXPECT_NE(QXK24_OK, qxk24_adam_adab_check(&s));
}
TEST_F(AdamAdabTest, TotalAdabWithinUnitRange) {
    QXK24AdabScore s;
    qxk24_adam_adab_score(adab, 70.0f, 4, 4, &s);
    EXPECT_GE(s.total_adab, 0.0f);
    EXPECT_LE(s.total_adab, 1.0f);
}
TEST_F(AdamAdabTest, SnapshotCountsSatisfied) {
    QXK24AdabScore s;
    qxk24_adam_adab_score(adab, 90.0f, 7, 7, &s);
    QXK24AdabSnapshot snap;
    ASSERT_EQ(QXK24_OK, qxk24_adam_adab_snapshot(adab, &snap));
    EXPECT_EQ(1u, snap.satisfied_count);
}
TEST_F(AdamAdabTest, ElementNameBenar) { EXPECT_STREQ("BENAR", qxk24_adam_adab_element_name(QXK24_ADAB_BENAR)); }
TEST_F(AdamAdabTest, ElementNameAmanah) { EXPECT_STREQ("AMANAH", qxk24_adam_adab_element_name(QXK24_ADAB_AMANAH)); }
TEST_F(AdamAdabTest, ElementNameBijaksana) { EXPECT_STREQ("BIJAKSANA", qxk24_adam_adab_element_name(QXK24_ADAB_BIJAKSANA)); }
