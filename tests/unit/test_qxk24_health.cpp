/* ============================================================
 * test_qxk24_health.cpp
 * QXK24 Universal Kernel — Health Score Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_health.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 36 Phase 3 proofs for weighted eight-law health scoring.
 * ============================================================ */

#include "qxk24/kernel/qxk24_health.h"
#include <gtest/gtest.h>

static QXK24HealthInput all(float v) {
    return QXK24HealthInput{v, v, v, v, v, v, v, v};
}

static QXK24HealthInput standard_input() {
    return QXK24HealthInput{0.8f, 0.8f, 0.7f, 0.7f,
                            0.5f, 0.5f, 0.3f, 0.3f};
}

static QXK24HealthResult compute_all(float v) {
    QXK24HealthInput in = all(v);
    QXK24HealthResult out{};
    EXPECT_EQ(qxk24_health_compute(&in, &out), QXK24_OK);
    return out;
}

TEST(QXK24Health_Weight, Z1) { EXPECT_FLOAT_EQ(qxk24_health_weight(0), 0.20f); }
TEST(QXK24Health_Weight, Z2) { EXPECT_FLOAT_EQ(qxk24_health_weight(1), 0.20f); }
TEST(QXK24Health_Weight, Z3) { EXPECT_FLOAT_EQ(qxk24_health_weight(2), 0.15f); }
TEST(QXK24Health_Weight, Z4) { EXPECT_FLOAT_EQ(qxk24_health_weight(3), 0.15f); }
TEST(QXK24Health_Weight, X1) { EXPECT_FLOAT_EQ(qxk24_health_weight(4), 0.12f); }
TEST(QXK24Health_Weight, X2) { EXPECT_FLOAT_EQ(qxk24_health_weight(5), 0.10f); }
TEST(QXK24Health_Weight, X3) { EXPECT_FLOAT_EQ(qxk24_health_weight(6), 0.05f); }
TEST(QXK24Health_Weight, X4) { EXPECT_FLOAT_EQ(qxk24_health_weight(7), 0.03f); }
TEST(QXK24Health_Weight, SumIsOne) {
    float sum = 0.0f;
    for (uint8_t i = 0; i < 8U; i++) sum += qxk24_health_weight(i);
    EXPECT_NEAR(sum, 1.0f, 0.00001f);
}
TEST(QXK24Health_Weight, OutOfRangeIsZero) {
    EXPECT_FLOAT_EQ(qxk24_health_weight(8), 0.0f);
    EXPECT_FLOAT_EQ(qxk24_health_weight(255), 0.0f);
}

TEST(QXK24Health_Tier, SovereignBoundary) {
    EXPECT_EQ(qxk24_health_tier(95.0f), QXK24_CERT_SOVEREIGN);
}
TEST(QXK24Health_Tier, SovereignMax) {
    EXPECT_EQ(qxk24_health_tier(100.0f), QXK24_CERT_SOVEREIGN);
}
TEST(QXK24Health_Tier, ProfessionalBoundary) {
    EXPECT_EQ(qxk24_health_tier(80.0f), QXK24_CERT_PROFESSIONAL);
}
TEST(QXK24Health_Tier, ProfessionalUpper) {
    EXPECT_EQ(qxk24_health_tier(94.9f), QXK24_CERT_PROFESSIONAL);
}
TEST(QXK24Health_Tier, StandardBoundary) {
    EXPECT_EQ(qxk24_health_tier(60.0f), QXK24_CERT_STANDARD);
}
TEST(QXK24Health_Tier, NoneBelowStandard) {
    EXPECT_EQ(qxk24_health_tier(59.9f), QXK24_CERT_NONE);
}

TEST(QXK24Health_Contribution, Z1Full) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(0, 1.0f), 0.20f);
}
TEST(QXK24Health_Contribution, Z1Half) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(0, 0.5f), 0.10f);
}
TEST(QXK24Health_Contribution, Z4Full) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(3, 1.0f), 0.15f);
}
TEST(QXK24Health_Contribution, X4Full) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(7, 1.0f), 0.03f);
}
TEST(QXK24Health_Contribution, ClampAboveOne) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(0, 3.0f), 0.20f);
}
TEST(QXK24Health_Contribution, ClampBelowZero) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(0, -1.0f), 0.0f);
}
TEST(QXK24Health_Contribution, InvalidLaw) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(9, 1.0f), 0.0f);
}
TEST(QXK24Health_Contribution, ZeroScore) {
    EXPECT_FLOAT_EQ(qxk24_health_law_contribution(4, 0.0f), 0.0f);
}

TEST(QXK24Health_Compute, NullInput) {
    QXK24HealthResult out{};
    EXPECT_EQ(qxk24_health_compute(nullptr, &out), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Health_Compute, NullOutput) {
    QXK24HealthInput in = all(1.0f);
    EXPECT_EQ(qxk24_health_compute(&in, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Health_Compute, PerfectScore) {
    QXK24HealthResult out = compute_all(1.0f);
    EXPECT_NEAR(out.score, 100.0f, 0.01f);
}
TEST(QXK24Health_Compute, ZeroScore) {
    QXK24HealthResult out = compute_all(0.0f);
    EXPECT_NEAR(out.score, 0.0f, 0.01f);
}
TEST(QXK24Health_Compute, PerfectTier) {
    QXK24HealthResult out = compute_all(1.0f);
    EXPECT_EQ(out.tier, QXK24_CERT_SOVEREIGN);
}
TEST(QXK24Health_Compute, ZeroTier) {
    QXK24HealthResult out = compute_all(0.0f);
    EXPECT_EQ(out.tier, QXK24_CERT_NONE);
}
TEST(QXK24Health_Compute, StandardMeetsTarget) {
    QXK24HealthInput in = standard_input();
    QXK24HealthResult out{};
    ASSERT_EQ(qxk24_health_compute(&in, &out), QXK24_OK);
    EXPECT_GE(out.score, QXK24_HEALTH_STANDARD);
}
TEST(QXK24Health_Compute, TimestampSet) {
    QXK24HealthResult out = compute_all(1.0f);
    EXPECT_GT(out.timestamp_ms, 0ULL);
}
TEST(QXK24Health_Compute, ComponentsPreserved) {
    QXK24HealthInput in = standard_input();
    QXK24HealthResult out{};
    ASSERT_EQ(qxk24_health_compute(&in, &out), QXK24_OK);
    EXPECT_FLOAT_EQ(out.components.z1, in.z1);
    EXPECT_FLOAT_EQ(out.components.x4, in.x4);
}
TEST(QXK24Health_Compute, FormulaVerification) {
    QXK24HealthInput in{0.8f, 0.6f, 0.7f, 0.5f, 0.9f, 0.4f, 0.3f, 0.2f};
    QXK24HealthResult out{};
    ASSERT_EQ(qxk24_health_compute(&in, &out), QXK24_OK);
    EXPECT_NEAR(out.score, 62.9f, 0.1f);
}
TEST(QXK24Health_Compute, ClampsHighComponents) {
    QXK24HealthResult out = compute_all(4.0f);
    EXPECT_NEAR(out.score, 100.0f, 0.01f);
}
TEST(QXK24Health_Compute, ClampsLowComponents) {
    QXK24HealthResult out = compute_all(-2.0f);
    EXPECT_NEAR(out.score, 0.0f, 0.01f);
}
