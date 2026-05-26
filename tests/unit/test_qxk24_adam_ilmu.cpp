/* ============================================================
 * test_qxk24_adam_ilmu.cpp
 * QXK24 Universal Kernel — ADAM Ilmu Unit Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_adam_ilmu.cpp
 * License : Apache 2.0
 *
 * Description:
 * Tests ADAM Ilmu stage processing, completion checks, snapshot
 * counts, and stage/type names.
 * ============================================================ */

#include <gtest/gtest.h>
#include "qxk24/adam/qxk24_adam_ilmu.h"

class AdamIlmuTest : public ::testing::Test {
protected:
    qxk24_adam_ilmu_t ilmu = nullptr;
    void SetUp() override { ASSERT_EQ(QXK24_OK, qxk24_adam_ilmu_create(&ilmu)); }
    void TearDown() override { (void)qxk24_adam_ilmu_destroy(ilmu); }
};

TEST_F(AdamIlmuTest, CreateSucceeds) { EXPECT_NE(nullptr, ilmu); }
TEST_F(AdamIlmuTest, ProcessCompletesAllStages) {
    QXK24IlmuRecord r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_ilmu_process(ilmu, "baca", &r));
    EXPECT_TRUE(r.baca_done);
    EXPECT_TRUE(r.bukti_done);
    EXPECT_TRUE(r.faham_done);
    EXPECT_TRUE(r.aplikasi_done);
}
TEST_F(AdamIlmuTest, ProcessMarksComplete) {
    QXK24IlmuRecord r;
    qxk24_adam_ilmu_process(ilmu, "input", &r);
    EXPECT_TRUE(r.ilmu_complete);
}
TEST_F(AdamIlmuTest, NonEmptyInputIsAddedValue) {
    QXK24IlmuRecord r;
    qxk24_adam_ilmu_process(ilmu, "input", &r);
    EXPECT_EQ(QXK24_ILMU_AV, r.type);
}
TEST_F(AdamIlmuTest, EmptyInputIsPureValue) {
    QXK24IlmuRecord r;
    qxk24_adam_ilmu_process(ilmu, "", &r);
    EXPECT_EQ(QXK24_ILMU_PV, r.type);
}
TEST_F(AdamIlmuTest, CheckCompleteSucceeds) {
    QXK24IlmuRecord r;
    qxk24_adam_ilmu_process(ilmu, "input", &r);
    EXPECT_EQ(QXK24_OK, qxk24_adam_ilmu_check_complete(&r));
}
TEST_F(AdamIlmuTest, CheckIncompleteFails) {
    QXK24IlmuRecord r = {};
    EXPECT_NE(QXK24_OK, qxk24_adam_ilmu_check_complete(&r));
}
TEST_F(AdamIlmuTest, AdvanceMovesStage) {
    QXK24IlmuRecord r = {};
    r.stage = QXK24_ILMU_STAGE_BACA;
    ASSERT_EQ(QXK24_OK, qxk24_adam_ilmu_advance(ilmu, &r));
    EXPECT_EQ(QXK24_ILMU_STAGE_BUKTI, r.stage);
}
TEST_F(AdamIlmuTest, SnapshotCountsIlmu) {
    QXK24IlmuRecord r;
    qxk24_adam_ilmu_process(ilmu, "input", &r);
    QXK24IlmuSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_ilmu_snapshot(ilmu, &s));
    EXPECT_EQ(1u, s.ilmu_count);
}
TEST_F(AdamIlmuTest, ConfidenceIsPositive) {
    QXK24IlmuRecord r;
    qxk24_adam_ilmu_process(ilmu, "input", &r);
    EXPECT_GT(r.confidence, 0.0f);
}
TEST_F(AdamIlmuTest, ApplicationCountIsOne) {
    QXK24IlmuRecord r;
    qxk24_adam_ilmu_process(ilmu, "input", &r);
    EXPECT_EQ(1u, r.applications);
}
TEST_F(AdamIlmuTest, StageNameBaca) { EXPECT_STREQ("BACA", qxk24_adam_ilmu_stage_name(QXK24_ILMU_STAGE_BACA)); }
TEST_F(AdamIlmuTest, StageNameAplikasi) { EXPECT_STREQ("APLIKASI", qxk24_adam_ilmu_stage_name(QXK24_ILMU_STAGE_APLIKASI)); }
TEST_F(AdamIlmuTest, TypeNamePureValue) { EXPECT_STREQ("pV", qxk24_adam_ilmu_type_name(QXK24_ILMU_PV)); }
TEST_F(AdamIlmuTest, TypeNameAddedValue) { EXPECT_STREQ("aV", qxk24_adam_ilmu_type_name(QXK24_ILMU_AV)); }
