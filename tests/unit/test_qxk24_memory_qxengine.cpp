/* ============================================================
 * test_qxk24_memory_qxengine.cpp
 * QXK24 Universal Kernel - QXEngine Memory Governor Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_memory_qxengine.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 22 Phase 5 proofs for QXEngine as K24ob memory governor.
 * ============================================================ */

#include "qxk24/resource/qxk24_memory_qxengine.h"
#include <gtest/gtest.h>

static qxk24_qxengine_t make_started_qxengine(void) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    EXPECT_NE(gov, nullptr);
    EXPECT_EQ(qxk24_qxengine_start(gov), QXK24_OK);
    return gov;
}

TEST(QXK24MemoryQXEngine_Config, BudgetIs256MB) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    EXPECT_EQ(cfg.budget_bytes, 256ULL * 1024ULL * 1024ULL);
}
TEST(QXK24MemoryQXEngine_Config, TotalMemoryIs4GB) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    EXPECT_EQ(cfg.total_memory_bytes, 4ULL * 1024ULL * 1024ULL * 1024ULL);
}
TEST(QXK24MemoryQXEngine_Config, MaxAllocationsIs1024) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    EXPECT_EQ(cfg.max_allocations, 1024U);
}
TEST(QXK24MemoryQXEngine_Config, HealthFeedEnabled) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    EXPECT_TRUE(cfg.enable_health_feed);
}
TEST(QXK24MemoryQXEngine_Config, CycleIntervalIsKernelDefault) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    EXPECT_EQ(cfg.cycle_interval_ms, QXK24_CYCLE_INTERVAL_MS);
}
TEST(QXK24MemoryQXEngine_Config, PressureThresholdsAreOrdered) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    EXPECT_GT(cfg.pressure_threshold_70, cfg.pressure_threshold_50);
    EXPECT_GT(cfg.pressure_threshold_50, cfg.pressure_threshold_25);
}

TEST(QXK24MemoryQXEngine_Create, ValidHandle) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    ASSERT_NE(gov, nullptr);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Create, NullConfigReturnsNull) {
    EXPECT_EQ(qxk24_qxengine_create(nullptr), nullptr);
}
TEST(QXK24MemoryQXEngine_Create, ZeroBudgetReturnsNull) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    cfg.budget_bytes = 0ULL;
    EXPECT_EQ(qxk24_qxengine_create(&cfg), nullptr);
}
TEST(QXK24MemoryQXEngine_Lifecycle, StartSucceeds) {
    qxk24_qxengine_t gov = make_started_qxengine();
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Lifecycle, StartNullReturnsError) {
    EXPECT_EQ(qxk24_qxengine_start(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24MemoryQXEngine_Lifecycle, IsRunningAfterStart) {
    qxk24_qxengine_t gov = make_started_qxengine();
    EXPECT_TRUE(qxk24_qxengine_is_running(gov));
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Lifecycle, StopClearsRunning) {
    qxk24_qxengine_t gov = make_started_qxengine();
    qxk24_qxengine_stop(gov);
    EXPECT_FALSE(qxk24_qxengine_is_running(gov));
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Cycle, NotStartedReturnsError) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    EXPECT_EQ(qxk24_qxengine_cycle(gov), QXK24_ERR_NOT_STARTED);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Cycle, RunningCycleSucceeds) {
    qxk24_qxengine_t gov = make_started_qxengine();
    EXPECT_EQ(qxk24_qxengine_cycle(gov), QXK24_OK);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Cycle, ObserveMemoryUpdatesBytes) {
    qxk24_qxengine_t gov = make_started_qxengine();
    EXPECT_EQ(qxk24_qxengine_observe_memory(gov, 4096ULL, 1U, 0U), QXK24_OK);
    EXPECT_EQ(qxk24_qxengine_bytes_used(gov), 4096ULL);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Health, ScoreInRangeAfterCycle) {
    qxk24_qxengine_t gov = make_started_qxengine();
    ASSERT_EQ(qxk24_qxengine_cycle(gov), QXK24_OK);
    EXPECT_GE(qxk24_qxengine_health_score(gov), 0.0f);
    EXPECT_LE(qxk24_qxengine_health_score(gov), 100.0f);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Health, DefaultCycleMeetsProfessional) {
    qxk24_qxengine_t gov = make_started_qxengine();
    ASSERT_EQ(qxk24_qxengine_cycle(gov), QXK24_OK);
    EXPECT_GE(qxk24_qxengine_health_score(gov), QXK24_HEALTH_PROFESSIONAL);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Health, HealthInputCopiesOut) {
    qxk24_qxengine_t gov = make_started_qxengine();
    QXK24QXEngineHealthInput input{};
    ASSERT_EQ(qxk24_qxengine_cycle(gov), QXK24_OK);
    EXPECT_EQ(qxk24_qxengine_health_input(gov, &input), QXK24_OK);
    EXPECT_GE(input.measurement_score, 0.0f);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Accessors, BudgetMatchesConfig) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    EXPECT_EQ(qxk24_qxengine_bytes_budget(gov), cfg.budget_bytes);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Accessors, ResourceHandleNotNull) {
    qxk24_qxengine_t gov = make_started_qxengine();
    EXPECT_NE(qxk24_qxengine_as_resource(gov), nullptr);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24MemoryQXEngine_Accessors, NullAccessorsAreSafe) {
    EXPECT_EQ(qxk24_qxengine_bytes_used(nullptr), 0ULL);
    EXPECT_EQ(qxk24_qxengine_bytes_budget(nullptr), 0ULL);
    EXPECT_FALSE(qxk24_qxengine_is_running(nullptr));
}
