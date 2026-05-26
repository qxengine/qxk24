/* ============================================================
 * test_qxk24_qxengine_bridge.cpp
 * QXK24 Universal Kernel - QXEngine Bridge Integration Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/integration/test_qxk24_qxengine_bridge.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 28 Phase 5 proofs for QXEngine-QXK24 integration.
 * ============================================================ */

#include "qxk24/bridge/qxk24_qxengine_bridge.h"
#include <gtest/gtest.h>

class QXK24QXEngineBridgeTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_qxengine_t gov = nullptr;
    qxk24_bridge_t bridge = nullptr;

    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
        ASSERT_EQ(qxk24_start(kernel), QXK24_OK);
        QXK24QXEngineConfig gcfg = qxk24_qxengine_default_config();
        gov = qxk24_qxengine_create(&gcfg);
        ASSERT_NE(gov, nullptr);
        ASSERT_EQ(qxk24_qxengine_start(gov), QXK24_OK);
        bridge = qxk24_bridge_create(kernel, gov);
    }

    void TearDown() override {
        qxk24_bridge_destroy(bridge);
        qxk24_qxengine_destroy(gov);
        qxk24_stop(kernel);
        qxk24_destroy(kernel);
    }
};

TEST_F(QXK24QXEngineBridgeTest, CreateReturnsValidHandle) {
    EXPECT_NE(bridge, nullptr);
}
TEST_F(QXK24QXEngineBridgeTest, CreateNullKernelReturnsNull) {
    EXPECT_EQ(qxk24_bridge_create(nullptr, gov), nullptr);
}
TEST_F(QXK24QXEngineBridgeTest, CreateNullGovReturnsNull) {
    EXPECT_EQ(qxk24_bridge_create(kernel, nullptr), nullptr);
}
TEST_F(QXK24QXEngineBridgeTest, InitialStateIsDetached) {
    EXPECT_EQ(qxk24_bridge_state(bridge), QXK24_BRIDGE_DETACHED);
}
TEST_F(QXK24QXEngineBridgeTest, InitialIsNotBound) {
    EXPECT_FALSE(qxk24_bridge_is_bound(bridge));
}
TEST_F(QXK24QXEngineBridgeTest, AttachSucceeds) {
    EXPECT_EQ(qxk24_bridge_attach(bridge), QXK24_OK);
}
TEST_F(QXK24QXEngineBridgeTest, AttachSetsStateToAttached) {
    qxk24_bridge_attach(bridge);
    EXPECT_EQ(qxk24_bridge_state(bridge), QXK24_BRIDGE_ATTACHED);
}
TEST_F(QXK24QXEngineBridgeTest, AttachNullReturnsError) {
    EXPECT_EQ(qxk24_bridge_attach(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(QXK24QXEngineBridgeTest, AttachTwiceIsIdempotent) {
    ASSERT_EQ(qxk24_bridge_attach(bridge), QXK24_OK);
    EXPECT_EQ(qxk24_bridge_attach(bridge), QXK24_OK);
}
TEST_F(QXK24QXEngineBridgeTest, AttachIncrementsActiveResources) {
    ASSERT_EQ(qxk24_bridge_attach(bridge), QXK24_OK);
    EXPECT_EQ(qxk24_active_resources(kernel), 1U);
}
TEST_F(QXK24QXEngineBridgeTest, CycleSucceeds) {
    EXPECT_EQ(qxk24_bridge_cycle(bridge), QXK24_OK);
}
TEST_F(QXK24QXEngineBridgeTest, CycleSetsStateToActive) {
    qxk24_bridge_cycle(bridge);
    EXPECT_EQ(qxk24_bridge_state(bridge), QXK24_BRIDGE_ACTIVE);
}
TEST_F(QXK24QXEngineBridgeTest, CycleSetsIsBound) {
    qxk24_bridge_cycle(bridge);
    EXPECT_TRUE(qxk24_bridge_is_bound(bridge));
}
TEST_F(QXK24QXEngineBridgeTest, CycleNullReturnsError) {
    EXPECT_EQ(qxk24_bridge_cycle(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(QXK24QXEngineBridgeTest, CycleIncrementsSynced) {
    QXK24BridgeSnapshot snap{};
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_snapshot(bridge, &snap);
    EXPECT_EQ(snap.cycles_synced, 1ULL);
}
TEST_F(QXK24QXEngineBridgeTest, TwoCyclesIncrementsSyncedToTwo) {
    QXK24BridgeSnapshot snap{};
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_snapshot(bridge, &snap);
    EXPECT_EQ(snap.cycles_synced, 2ULL);
}
TEST_F(QXK24QXEngineBridgeTest, CycleIncrementsKernelCycle) {
    qxk24_bridge_cycle(bridge);
    EXPECT_EQ(qxk24_cycle_index(kernel), 1ULL);
}
TEST_F(QXK24QXEngineBridgeTest, SnapshotNullBridgeReturnsError) {
    QXK24BridgeSnapshot snap{};
    EXPECT_EQ(qxk24_bridge_snapshot(nullptr, &snap), QXK24_ERR_NULL_PARAM);
}
TEST_F(QXK24QXEngineBridgeTest, SnapshotNullOutReturnsError) {
    EXPECT_EQ(qxk24_bridge_snapshot(bridge, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(QXK24QXEngineBridgeTest, SnapshotAfterCycleHasValidScores) {
    QXK24BridgeSnapshot snap{};
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_snapshot(bridge, &snap);
    EXPECT_GE(snap.qxengine_health, 0.0f);
    EXPECT_LE(snap.qxengine_health, 100.0f);
    EXPECT_GE(snap.kernel_health, 0.0f);
    EXPECT_LE(snap.kernel_health, 100.0f);
}
TEST_F(QXK24QXEngineBridgeTest, CombinedHealthFormula) {
    QXK24BridgeSnapshot snap{};
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_snapshot(bridge, &snap);
    float expected = snap.kernel_health * 0.60f + snap.qxengine_health * 0.40f;
    EXPECT_NEAR(snap.combined_health, expected, 0.01f);
}
TEST_F(QXK24QXEngineBridgeTest, CombinedHealthProfessional) {
    QXK24BridgeSnapshot snap{};
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_snapshot(bridge, &snap);
    EXPECT_GE(snap.combined_health, QXK24_HEALTH_PROFESSIONAL);
}
TEST_F(QXK24QXEngineBridgeTest, DetachSetsStateToDetached) {
    qxk24_bridge_cycle(bridge);
    qxk24_bridge_detach(bridge);
    EXPECT_EQ(qxk24_bridge_state(bridge), QXK24_BRIDGE_DETACHED);
}
TEST_F(QXK24QXEngineBridgeTest, NullStateIsFaulted) {
    EXPECT_EQ(qxk24_bridge_state(nullptr), QXK24_BRIDGE_FAULTED);
}
TEST_F(QXK24QXEngineBridgeTest, NullBoundIsFalse) {
    EXPECT_FALSE(qxk24_bridge_is_bound(nullptr));
}
TEST_F(QXK24QXEngineBridgeTest, QXEngineOperatesAtK24obLevel) {
    EXPECT_EQ(QXK24_QXENGINE_K24_LEVEL, QXK24_LEVEL_OBJEK);
}
TEST_F(QXK24QXEngineBridgeTest, QXEngineDirectionIsAs) {
    EXPECT_EQ(QXK24_QXENGINE_DIRECTION, QXK24_DIR_AS);
}
TEST_F(QXK24QXEngineBridgeTest, QXEngineName) {
    EXPECT_STREQ(QXK24_QXENGINE_NAME, "QXEngine-K24ob");
}
