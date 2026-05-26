/* ============================================================
 * test_qxk24_qxengine_health.cpp
 * QXK24 Universal Kernel - QXEngine Health Feed Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/integration/test_qxk24_qxengine_health.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 22 Phase 5 proofs for QXEngine-to-QXK24 health feed.
 * ============================================================ */

#include "qxk24/bridge/qxk24_qxengine_health.h"
#include <gtest/gtest.h>

static qxk24_kernel_t make_started_kernel(void) {
    qxk24_kernel_t kernel = nullptr;
    QXK24Config cfg = qxk24_default_config();
    EXPECT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
    EXPECT_EQ(qxk24_start(kernel), QXK24_OK);
    return kernel;
}

TEST(QXK24HealthFeed_Weights, QXEngineWeightIs040) {
    EXPECT_FLOAT_EQ(QXK24_QXENGINE_HEALTH_WEIGHT, 0.40f);
}
TEST(QXK24HealthFeed_Weights, KernelWeightIs060) {
    EXPECT_FLOAT_EQ(QXK24_KERNEL_HEALTH_WEIGHT, 0.60f);
}
TEST(QXK24HealthFeed_Weights, SumToOne) {
    EXPECT_FLOAT_EQ(QXK24_QXENGINE_HEALTH_WEIGHT + QXK24_KERNEL_HEALTH_WEIGHT, 1.0f);
}
TEST(QXK24HealthFeed_Compute, NullOutReturnsError) {
    EXPECT_EQ(qxk24_health_feed_compute(80.0f, 90.0f, nullptr),
              QXK24_ERR_NULL_PARAM);
}
TEST(QXK24HealthFeed_Compute, PerfectScoreIs100) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(100.0f, 100.0f, &rec);
    EXPECT_NEAR(rec.combined_score, 100.0f, 0.01f);
}
TEST(QXK24HealthFeed_Compute, ZeroScoreIsZero) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(0.0f, 0.0f, &rec);
    EXPECT_NEAR(rec.combined_score, 0.0f, 0.01f);
}
TEST(QXK24HealthFeed_Compute, FormulaKernel60QXEngine40) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(60.0f, 80.0f, &rec);
    EXPECT_NEAR(rec.combined_score, 72.0f, 0.01f);
}
TEST(QXK24HealthFeed_Compute, ProfessionalMetAt80) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(80.0f, 80.0f, &rec);
    EXPECT_TRUE(rec.professional_met);
}
TEST(QXK24HealthFeed_Compute, ProfessionalNotMetBelow80) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(70.0f, 70.0f, &rec);
    EXPECT_FALSE(rec.professional_met);
}
TEST(QXK24HealthFeed_Compute, ClampAbove100) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(150.0f, 150.0f, &rec);
    EXPECT_LE(rec.combined_score, 100.0f);
}
TEST(QXK24HealthFeed_Compute, ClampBelowZero) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(-10.0f, -10.0f, &rec);
    EXPECT_GE(rec.combined_score, 0.0f);
}
TEST(QXK24HealthFeed_Compute, TimestampSet) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(80.0f, 80.0f, &rec);
    EXPECT_GT(rec.timestamp_ms, 0ULL);
}
TEST(QXK24HealthFeed_Compute, ScoresPreserved) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(75.0f, 85.0f, &rec);
    EXPECT_FLOAT_EQ(rec.qxengine_score, 75.0f);
    EXPECT_FLOAT_EQ(rec.kernel_score, 85.0f);
}
TEST(QXK24HealthFeed_FromGovernor, NullGovReturnsError) {
    qxk24_kernel_t kernel = make_started_kernel();
    QXK24HealthFeedRecord rec{};
    EXPECT_EQ(qxk24_health_feed_from_governor(nullptr, kernel, &rec),
              QXK24_ERR_NULL_PARAM);
    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}
TEST(QXK24HealthFeed_FromGovernor, NullKernelReturnsError) {
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    QXK24HealthFeedRecord rec{};
    EXPECT_EQ(qxk24_health_feed_from_governor(gov, nullptr, &rec),
              QXK24_ERR_NULL_PARAM);
    qxk24_qxengine_destroy(gov);
}
TEST(QXK24HealthFeed_FromGovernor, NullOutputReturnsError) {
    qxk24_kernel_t kernel = make_started_kernel();
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    EXPECT_EQ(qxk24_health_feed_from_governor(gov, kernel, nullptr),
              QXK24_ERR_NULL_PARAM);
    qxk24_qxengine_destroy(gov);
    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}
TEST(QXK24HealthFeed_FromGovernor, SucceedsWithLiveHandles) {
    qxk24_kernel_t kernel = make_started_kernel();
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    QXK24HealthFeedRecord rec{};
    qxk24_qxengine_start(gov);
    qxk24_qxengine_cycle(gov);
    EXPECT_EQ(qxk24_health_feed_from_governor(gov, kernel, &rec), QXK24_OK);
    EXPECT_GE(rec.combined_score, 0.0f);
    qxk24_qxengine_destroy(gov);
    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}
TEST(QXK24HealthFeed_FromGovernor, FeedCycleCopiesKernelCycle) {
    qxk24_kernel_t kernel = make_started_kernel();
    QXK24QXEngineConfig cfg = qxk24_qxengine_default_config();
    qxk24_qxengine_t gov = qxk24_qxengine_create(&cfg);
    QXK24HealthFeedRecord rec{};
    qxk24_cycle(kernel, nullptr);
    qxk24_qxengine_start(gov);
    qxk24_qxengine_cycle(gov);
    qxk24_health_feed_from_governor(gov, kernel, &rec);
    EXPECT_EQ(rec.feed_cycle, qxk24_cycle_index(kernel));
    qxk24_qxengine_destroy(gov);
    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}
TEST(QXK24HealthFeed_Professional, MeetsProfessionalTrue) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(85.0f, 85.0f, &rec);
    EXPECT_TRUE(qxk24_health_feed_meets_professional(&rec));
}
TEST(QXK24HealthFeed_Professional, MeetsProfessionalFalse) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(50.0f, 50.0f, &rec);
    EXPECT_FALSE(qxk24_health_feed_meets_professional(&rec));
}
TEST(QXK24HealthFeed_Professional, NullReturnsFalse) {
    EXPECT_FALSE(qxk24_health_feed_meets_professional(nullptr));
}
TEST(QXK24HealthFeed_Professional, BoundaryIsInclusive) {
    QXK24HealthFeedRecord rec{};
    qxk24_health_feed_compute(80.0f, 80.0f, &rec);
    EXPECT_TRUE(qxk24_health_feed_meets_professional(&rec));
}
