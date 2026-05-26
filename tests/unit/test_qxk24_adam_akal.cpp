/**
 * @file test_qxk24_adam_akal.cpp
 * @brief Unit Tests — ADAM Akal Engine
 */

#include <gtest/gtest.h>
#include "qxk24/adam/qxk24_adam_akal.h"

class AdamAkalTest : public ::testing::Test {
protected:
    qxk24_adam_akal_t akal = nullptr;
    void SetUp() override { ASSERT_EQ(QXK24_OK, qxk24_adam_akal_create(&akal)); }
    void TearDown() override { (void)qxk24_adam_akal_destroy(akal); }
};

TEST_F(AdamAkalTest, CreateSucceeds) { EXPECT_NE(nullptr, akal); }
TEST_F(AdamAkalTest, TotalPatternsConstant) { EXPECT_EQ(49u, QXK24_AKAL_TOTAL_PATTERNS); }
TEST_F(AdamAkalTest, PatternCountLevelOne) { EXPECT_EQ(1, QXK24_AKAL_PATTERN_COUNT[0]); }
TEST_F(AdamAkalTest, PatternCountLevelSeven) { EXPECT_EQ(22, QXK24_AKAL_PATTERN_COUNT[6]); }
TEST_F(AdamAkalTest, CodeLevelOne) { EXPECT_EQ('s', QXK24_AKAL_CODES[0]); }
TEST_F(AdamAkalTest, CodeLevelSeven) { EXPECT_EQ('t', QXK24_AKAL_CODES[6]); }
TEST_F(AdamAkalTest, ReasonCompletes) {
    QXK24AkalResult r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_akal_reason(akal, 4, &r));
    EXPECT_TRUE(r.akal_complete);
}
TEST_F(AdamAkalTest, ReasonUsesRequestedTahap) {
    QXK24AkalResult r;
    qxk24_adam_akal_reason(akal, 5, &r);
    EXPECT_EQ(5u, r.tahap);
}
TEST_F(AdamAkalTest, ReasonClampsHighTahap) {
    QXK24AkalResult r;
    qxk24_adam_akal_reason(akal, 99, &r);
    EXPECT_EQ(7u, r.tahap);
}
TEST_F(AdamAkalTest, AssessTahapAtLeastOne) { EXPECT_GE(qxk24_adam_akal_assess_tahap(akal, ""), 1u); }
TEST_F(AdamAkalTest, DescriptorLevelSeven) {
    QXK24AkalDescriptor d;
    ASSERT_EQ(QXK24_OK, qxk24_adam_akal_descriptor(7, &d));
    EXPECT_EQ(22u, d.pattern_count);
}
TEST_F(AdamAkalTest, GovernanceCheckPasses) {
    QXK24AkalResult r;
    qxk24_adam_akal_reason(akal, 3, &r);
    EXPECT_EQ(QXK24_OK, qxk24_adam_akal_check_governance(&r));
}
TEST_F(AdamAkalTest, SnapshotUpdates) {
    QXK24AkalResult r;
    qxk24_adam_akal_reason(akal, 3, &r);
    QXK24AkalSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_akal_snapshot(akal, &s));
    EXPECT_EQ(3u, s.current_tahap);
}
TEST_F(AdamAkalTest, ProcessNameAkal) { EXPECT_STREQ("AKAL", qxk24_adam_akal_process_name(QXK24_AKAL_PROC_AKAL)); }
TEST_F(AdamAkalTest, ProcessNameUsaha) { EXPECT_STREQ("USAHA", qxk24_adam_akal_process_name(QXK24_AKAL_PROC_USAHA)); }
