/* ============================================================
 * test_qxk24_report.cpp
 * QXK24 Universal Kernel — Constitutional Report Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_report.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 26 Phase 3 proofs for constitutional report generation.
 * ============================================================ */

#include "qxk24/kernel/qxk24_report.h"
#include <cstring>
#include <gtest/gtest.h>

static QXK24KernelHandle make_kernel(bool start = true) {
    QXK24KernelHandle kernel = nullptr;
    QXK24Config cfg = qxk24_default_config();
    EXPECT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
    if (start) EXPECT_EQ(qxk24_start(kernel), QXK24_OK);
    return kernel;
}

static void destroy_kernel(QXK24KernelHandle kernel) {
    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}

TEST(QXK24Report_Tag, Z1) { EXPECT_STREQ(qxk24_report_violation_tag(0), "Z.1"); }
TEST(QXK24Report_Tag, Z2) { EXPECT_STREQ(qxk24_report_violation_tag(1), "Z.2"); }
TEST(QXK24Report_Tag, Z4) { EXPECT_STREQ(qxk24_report_violation_tag(3), "Z.4"); }
TEST(QXK24Report_Tag, X1) { EXPECT_STREQ(qxk24_report_violation_tag(4), "X.1"); }
TEST(QXK24Report_Tag, X4) { EXPECT_STREQ(qxk24_report_violation_tag(7), "X.4"); }
TEST(QXK24Report_Tag, Unknown) { EXPECT_STREQ(qxk24_report_violation_tag(8), "UNKNOWN"); }

TEST(QXK24Report_Sound, NullReport) {
    EXPECT_FALSE(qxk24_report_is_sound(nullptr));
}
TEST(QXK24Report_Sound, StandardReport) {
    QXK24Report rep{};
    rep.health_score = 60.0;
    rep.cert_tier = QXK24_CERT_STANDARD;
    EXPECT_TRUE(qxk24_report_is_sound(&rep));
}
TEST(QXK24Report_Sound, ProfessionalReport) {
    QXK24Report rep{};
    rep.health_score = 80.0;
    rep.cert_tier = QXK24_CERT_PROFESSIONAL;
    EXPECT_TRUE(qxk24_report_is_sound(&rep));
}
TEST(QXK24Report_Sound, SovereignReport) {
    QXK24Report rep{};
    rep.health_score = 95.0;
    rep.cert_tier = QXK24_CERT_SOVEREIGN;
    EXPECT_TRUE(qxk24_report_is_sound(&rep));
}
TEST(QXK24Report_Sound, LowScoreNotSound) {
    QXK24Report rep{};
    rep.health_score = 59.9;
    rep.cert_tier = QXK24_CERT_NONE;
    EXPECT_FALSE(qxk24_report_is_sound(&rep));
}
TEST(QXK24Report_Sound, LowTierNotSound) {
    QXK24Report rep{};
    rep.health_score = 80.0;
    rep.cert_tier = QXK24_CERT_NONE;
    EXPECT_FALSE(qxk24_report_is_sound(&rep));
}

TEST(QXK24Report_Generate, NullKernel) {
    QXK24Report rep{};
    EXPECT_EQ(qxk24_report_generate(nullptr, &rep), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Report_Generate, NullOutput) {
    QXK24KernelHandle kernel = make_kernel();
    EXPECT_EQ(qxk24_report_generate(kernel, nullptr), QXK24_ERR_NULL_PARAM);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, LiveKernelSucceeds) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    EXPECT_EQ(qxk24_report_generate(kernel, &rep), QXK24_OK);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, TimestampSet) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    qxk24_report_generate(kernel, &rep);
    EXPECT_GT(rep.timestamp_ms, 0ULL);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, HealthRange) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    qxk24_report_generate(kernel, &rep);
    EXPECT_GE(rep.health_score, 0.0);
    EXPECT_LE(rep.health_score, 100.0);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, TierMatchesHealth) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    qxk24_report_generate(kernel, &rep);
    EXPECT_EQ(rep.cert_tier, qxk24_health_tier((float)rep.health_score));
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, CycleIndexCopied) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    QXK24Report cycle_rep{};
    qxk24_cycle(kernel, &cycle_rep);
    qxk24_report_generate(kernel, &rep);
    EXPECT_EQ(rep.cycle_index, 1ULL);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, ResourceCountCopied) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    uint32_t id = 0;
    qxk24_resource_register(kernel, QXK24_RESOURCE_MEMORY, QXK24_DIR_AS,
                            QXK24_LEVEL_SISTEM, 1024, &id);
    qxk24_report_generate(kernel, &rep);
    EXPECT_EQ(rep.active_resources, 1U);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, LogContainsKernelName) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    qxk24_report_generate(kernel, &rep);
    EXPECT_NE(std::strstr(rep.log, "QXK24"), nullptr);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Generate, InitialKernelIsSound) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    qxk24_report_generate(kernel, &rep);
    EXPECT_TRUE(qxk24_report_is_sound(&rep));
    destroy_kernel(kernel);
}

TEST(QXK24Report_Print, NullNoCrash) {
    EXPECT_NO_FATAL_FAILURE(qxk24_report_print(nullptr));
}
TEST(QXK24Report_Print, ValidNoCrash) {
    QXK24Report rep{};
    rep.health_score = 100.0;
    rep.cert_tier = QXK24_CERT_SOVEREIGN;
    EXPECT_NO_FATAL_FAILURE(qxk24_report_print(&rep));
}

TEST(QXK24Report_Fields, LawScoresCopied) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    qxk24_report_generate(kernel, &rep);
    EXPECT_DOUBLE_EQ(rep.law_scores.z1_pattern, 1.0);
    destroy_kernel(kernel);
}
TEST(QXK24Report_Fields, FlowCountStartsZero) {
    QXK24KernelHandle kernel = make_kernel();
    QXK24Report rep{};
    qxk24_report_generate(kernel, &rep);
    EXPECT_EQ(rep.active_flows, 0U);
    destroy_kernel(kernel);
}
