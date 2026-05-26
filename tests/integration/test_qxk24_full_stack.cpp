/* ============================================================
 * test_qxk24_full_stack.cpp
 * QXK24 Universal Kernel - Full Stack Integration Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/integration/test_qxk24_full_stack.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 26 Phase 6 proofs for K24md -> K24ob -> K24at.
 * ============================================================ */

#include "qxk24/validation/qxk24_qiubbx.h"
#include <gtest/gtest.h>

class FullStackTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_qxengine_t gov = nullptr;
    qxk24_bridge_t bridge = nullptr;
    qxk24_memloc_t memloc = nullptr;
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
        QXK24MemlocConfig mcfg = qxk24_memloc_default_config();
        memloc = qxk24_memloc_create(&mcfg, gov);
        ASSERT_NE(memloc, nullptr);
        validator = qxk24_qiubbx_create(kernel, bridge);
        ASSERT_NE(validator, nullptr);
    }

    void TearDown() override {
        qxk24_qiubbx_destroy(validator);
        qxk24_memloc_destroy(memloc);
        qxk24_bridge_destroy(bridge);
        qxk24_qxengine_destroy(gov);
        qxk24_stop(kernel);
        qxk24_destroy(kernel);
    }
};

TEST_F(FullStackTest, AllLayersCreated) {
    EXPECT_NE(kernel, nullptr);
    EXPECT_NE(gov, nullptr);
    EXPECT_NE(bridge, nullptr);
    EXPECT_NE(memloc, nullptr);
    EXPECT_NE(validator, nullptr);
}
TEST_F(FullStackTest, K24mdIsModulLevel) { EXPECT_EQ(QXK24_LEVEL_MODUL, (QXK24Level)6); }
TEST_F(FullStackTest, K24obIsObjekLevel) {
    EXPECT_EQ(QXK24_QXENGINE_K24_LEVEL, QXK24_LEVEL_OBJEK);
}
TEST_F(FullStackTest, K24atIsAtomLevel) {
    EXPECT_EQ(QXK24_MEMLOC_K24_LEVEL, QXK24_LEVEL_ATOM);
}
TEST_F(FullStackTest, MemlocAMAColumnIs32) {
    EXPECT_EQ(qxk24_memloc_ama_column(), 32U);
}
TEST_F(FullStackTest, MemlocLocateSucceeds) {
    QXK24MemlocRecord rec{};
    EXPECT_EQ(qxk24_memloc_locate(memloc, 1024, QXK24_DIR_AS, &rec), QXK24_OK);
    EXPECT_GT(rec.address, 0ULL);
}
TEST_F(FullStackTest, MemlocRecordFieldsSet) {
    QXK24MemlocRecord rec{};
    qxk24_memloc_locate(memloc, 1024, QXK24_DIR_DN, &rec);
    EXPECT_EQ(rec.size_bytes, 1024ULL);
    EXPECT_EQ(rec.direction, QXK24_DIR_DN);
    EXPECT_TRUE(rec.active);
}
TEST_F(FullStackTest, MemlocActiveCountIncrementsOnLocate) {
    QXK24MemlocRecord rec{};
    EXPECT_EQ(qxk24_memloc_active_count(memloc), 0U);
    qxk24_memloc_locate(memloc, 1024, QXK24_DIR_AS, &rec);
    EXPECT_EQ(qxk24_memloc_active_count(memloc), 1U);
}
TEST_F(FullStackTest, MemlocReleaseDecrementsActiveCount) {
    QXK24MemlocRecord rec{};
    qxk24_memloc_locate(memloc, 1024, QXK24_DIR_AS, &rec);
    EXPECT_EQ(qxk24_memloc_release(memloc, rec.address), QXK24_OK);
    EXPECT_EQ(qxk24_memloc_active_count(memloc), 0U);
}
TEST_F(FullStackTest, MemlocHealthStartsAt100) {
    EXPECT_NEAR(qxk24_memloc_health_score(memloc), 100.0f, 0.01f);
}
TEST_F(FullStackTest, MemlocHealthDecreasesWithAllocations) {
    QXK24MemlocRecord rec{};
    float initial = qxk24_memloc_health_score(memloc);
    qxk24_memloc_locate(memloc, 4096, QXK24_DIR_AS, &rec);
    EXPECT_LT(qxk24_memloc_health_score(memloc), initial);
}
TEST_F(FullStackTest, MemlocReleaseInvalidAddressReturnsError) {
    EXPECT_EQ(qxk24_memloc_release(memloc, 0xDEADBEEFULL), QXK24_ERR_NOT_FOUND);
}
TEST_F(FullStackTest, BridgeCycleSucceeds) {
    EXPECT_EQ(qxk24_bridge_cycle(bridge), QXK24_OK);
}
TEST_F(FullStackTest, BridgeIsBoundAfterCycle) {
    qxk24_bridge_cycle(bridge);
    EXPECT_TRUE(qxk24_bridge_is_bound(bridge));
}
TEST_F(FullStackTest, QIUBBXRunSucceeds) {
    QXK24QIUBBXReport rep{};
    EXPECT_EQ(qxk24_qiubbx_run(validator, &rep), QXK24_OK);
}
TEST_F(FullStackTest, QIUBBXSovereignScoreInRange) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_GE(rep.sovereign_score, 0.0f);
    EXPECT_LE(rep.sovereign_score, 100.0f);
}
TEST_F(FullStackTest, QIUBBXSovereignAchieved) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_TRUE(rep.sovereign_achieved);
}
TEST_F(FullStackTest, QIUBBXAllSixDirectionsActive) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.directions_active, 6U);
}
TEST_F(FullStackTest, QIUBBXAllThreePairsBound) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.directions_paired, 3U);
}
TEST_F(FullStackTest, QIUBBXEightLawsValidated) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.laws_passed, QXK24_QIUBBX_LAWS);
}
TEST_F(FullStackTest, QIUBBXSevenLevelsValidated) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.levels[6].scale, QXK24_SCALE_MODUL);
}
TEST_F(FullStackTest, QIUBBXProofTallyIs21) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.total_proofs, 21U);
}
TEST_F(FullStackTest, HealthFeedFromAllLayers) {
    QXK24BridgeSnapshot snap{};
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_snapshot(bridge, &snap);
    EXPECT_GE(snap.combined_health, QXK24_HEALTH_PROFESSIONAL);
}
TEST_F(FullStackTest, MemlocUpdatesQXEngineObservedBytes) {
    QXK24MemlocRecord rec{};
    qxk24_memloc_locate(memloc, 2048, QXK24_DIR_AS, &rec);
    EXPECT_EQ(qxk24_qxengine_bytes_used(gov), 2048ULL);
}
TEST_F(FullStackTest, QIUBBXTierIsSovereign) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_EQ(rep.tier, QXK24_CERT_SOVEREIGN);
}
TEST_F(FullStackTest, QIUBBXPrintNoCrash) {
    QXK24QIUBBXReport rep{};
    qxk24_qiubbx_run(validator, &rep);
    EXPECT_NO_FATAL_FAILURE(qxk24_qiubbx_print(&rep));
}
