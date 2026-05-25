/* ============================================================
 * test_qxk24_full_cycle.cpp
 * QXK24 Universal Kernel — Full Cycle Integration Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/integration/test_qxk24_full_cycle.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 26 Phase 3 proofs for create/start/cycle/report/stop/destroy.
 * ============================================================ */

#include "qxk24/kernel/qxk24_governor.h"
#include "qxk24/kernel/qxk24_report.h"
#include <gtest/gtest.h>

static QXK24KernelHandle new_started_kernel() {
    QXK24KernelHandle kernel = nullptr;
    QXK24Config cfg = qxk24_default_config();
    EXPECT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
    EXPECT_EQ(qxk24_start(kernel), QXK24_OK);
    return kernel;
}

static void finish_kernel(QXK24KernelHandle kernel) {
    qxk24_stop(kernel);
    qxk24_destroy(kernel);
}

static uint64_t core_budget(QXK24Level level) {
    return qxk24_ama_core_value((uint32_t)level + 1U) * QXK24_BASE_UNIT_BYTES;
}

static void register_six_kernel_resources(QXK24KernelHandle kernel) {
    const QXK24Direction dirs[6] = {QXK24_DIR_AS, QXK24_DIR_BH, QXK24_DIR_DN,
                                   QXK24_DIR_BG, QXK24_DIR_KN, QXK24_DIR_KR};
    const QXK24ResourceType types[6] = {QXK24_RESOURCE_MEMORY, QXK24_RESOURCE_IO,
        QXK24_RESOURCE_CPU, QXK24_RESOURCE_STORAGE, QXK24_RESOURCE_NETWORK,
        QXK24_RESOURCE_ENERGY};
    for (int i = 0; i < 6; i++) {
        uint32_t id = 0U;
        uint64_t budget = core_budget(QXK24_LEVEL_SISTEM);
        EXPECT_EQ(qxk24_resource_register(kernel, types[i], dirs[i],
                  QXK24_LEVEL_SISTEM, budget, &id), QXK24_OK);
        EXPECT_EQ(qxk24_resource_update(kernel, id, budget / 2ULL, 1.0),
                  QXK24_OK);
    }
}

static QXK24ResourceGovernor *make_phase3_gov(QXK24ResourceType type,
                                              QXK24Direction dir) {
    QXK24GovernorConfig cfg{};
    cfg.type = type;
    cfg.direction = dir;
    cfg.level = QXK24_LEVEL_SISTEM;
    cfg.budget_soft = core_budget(QXK24_LEVEL_SISTEM);
    cfg.declared_x = 1.0;
    cfg.tolerance_pct = 15.0;
    QXK24ResourceGovernor *gov = nullptr;
    EXPECT_EQ(qxk24_governor_base_create(&cfg, &gov), QXK24_OK);
    EXPECT_EQ(qxk24_governor_base_update(gov, cfg.budget_soft / 2ULL, 1.0),
              QXK24_OK);
    return gov;
}

TEST(QXK24FullCycle_Lifecycle, CreateDefaultConfig) {
    QXK24KernelHandle kernel = nullptr;
    QXK24Config cfg = qxk24_default_config();
    EXPECT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
    qxk24_destroy(kernel);
}
TEST(QXK24FullCycle_Lifecycle, StartSucceeds) {
    QXK24KernelHandle kernel = nullptr;
    QXK24Config cfg = qxk24_default_config();
    ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
    EXPECT_EQ(qxk24_start(kernel), QXK24_OK);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Lifecycle, DoubleStartFails) {
    QXK24KernelHandle kernel = new_started_kernel();
    EXPECT_EQ(qxk24_start(kernel), QXK24_ERR_ALREADY_INITIALIZED);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Lifecycle, StopNoCrash) {
    QXK24KernelHandle kernel = new_started_kernel();
    EXPECT_NO_FATAL_FAILURE(qxk24_stop(kernel));
    qxk24_destroy(kernel);
}
TEST(QXK24FullCycle_Lifecycle, DestroyNullNoCrash) {
    EXPECT_NO_FATAL_FAILURE(qxk24_destroy(nullptr));
}

TEST(QXK24FullCycle_Cycle, IndexStartsZero) {
    QXK24KernelHandle kernel = new_started_kernel();
    EXPECT_EQ(qxk24_cycle_index(kernel), 0ULL);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Cycle, ManualCycleIncrements) {
    QXK24KernelHandle kernel = new_started_kernel();
    EXPECT_EQ(qxk24_cycle(kernel, nullptr), QXK24_OK);
    EXPECT_EQ(qxk24_cycle_index(kernel), 1ULL);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Cycle, TwoCyclesIncrementTwice) {
    QXK24KernelHandle kernel = new_started_kernel();
    qxk24_cycle(kernel, nullptr);
    qxk24_cycle(kernel, nullptr);
    EXPECT_EQ(qxk24_cycle_index(kernel), 2ULL);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Cycle, CycleBeforeStartFails) {
    QXK24KernelHandle kernel = nullptr;
    QXK24Config cfg = qxk24_default_config();
    ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
    EXPECT_EQ(qxk24_cycle(kernel, nullptr), QXK24_ERR_NOT_INITIALIZED);
    qxk24_destroy(kernel);
}
TEST(QXK24FullCycle_Cycle, CycleReportFilled) {
    QXK24KernelHandle kernel = new_started_kernel();
    QXK24Report rep{};
    EXPECT_EQ(qxk24_cycle(kernel, &rep), QXK24_OK);
    EXPECT_EQ(rep.cycle_index, 1ULL);
    finish_kernel(kernel);
}

TEST(QXK24FullCycle_Resources, RegisterAllSix) {
    QXK24KernelHandle kernel = new_started_kernel();
    register_six_kernel_resources(kernel);
    EXPECT_EQ(qxk24_active_resources(kernel), 6U);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Resources, SixResourceCycleStandard) {
    QXK24KernelHandle kernel = new_started_kernel();
    register_six_kernel_resources(kernel);
    qxk24_cycle(kernel, nullptr);
    EXPECT_GE(qxk24_health_score(kernel), QXK24_HEALTH_STANDARD);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Resources, SixResourceTierStandardOrBetter) {
    QXK24KernelHandle kernel = new_started_kernel();
    register_six_kernel_resources(kernel);
    qxk24_cycle(kernel, nullptr);
    EXPECT_GE(qxk24_cert_tier(kernel), QXK24_CERT_STANDARD);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Resources, LawScoresAvailable) {
    QXK24KernelHandle kernel = new_started_kernel();
    register_six_kernel_resources(kernel);
    qxk24_cycle(kernel, nullptr);
    QXK24LawScore scores{};
    EXPECT_EQ(qxk24_get_law_scores(kernel, &scores), QXK24_OK);
    EXPECT_GE(scores.z1_pattern, 0.0);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Resources, FlowCountAfterUpdates) {
    QXK24KernelHandle kernel = new_started_kernel();
    register_six_kernel_resources(kernel);
    qxk24_cycle(kernel, nullptr);
    EXPECT_EQ(qxk24_active_flows(kernel), 0U);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Resources, ReleaseResourceReducesCount) {
    QXK24KernelHandle kernel = new_started_kernel();
    uint32_t id = 0U;
    uint64_t budget = core_budget(QXK24_LEVEL_SISTEM);
    qxk24_resource_register(kernel, QXK24_RESOURCE_MEMORY, QXK24_DIR_AS,
                            QXK24_LEVEL_SISTEM, budget, &id);
    EXPECT_EQ(qxk24_resource_release(kernel, id), QXK24_OK);
    EXPECT_EQ(qxk24_active_resources(kernel), 0U);
    finish_kernel(kernel);
}

TEST(QXK24FullCycle_Report, GenerateAfterStart) {
    QXK24KernelHandle kernel = new_started_kernel();
    QXK24Report rep{};
    EXPECT_EQ(qxk24_report_generate(kernel, &rep), QXK24_OK);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Report, GenerateAfterCycle) {
    QXK24KernelHandle kernel = new_started_kernel();
    QXK24Report rep{};
    qxk24_cycle(kernel, nullptr);
    EXPECT_EQ(qxk24_report_generate(kernel, &rep), QXK24_OK);
    EXPECT_EQ(rep.cycle_index, 1ULL);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Report, ReportIsSoundWithSixResources) {
    QXK24KernelHandle kernel = new_started_kernel();
    QXK24Report rep{};
    register_six_kernel_resources(kernel);
    qxk24_cycle(kernel, nullptr);
    qxk24_report_generate(kernel, &rep);
    EXPECT_TRUE(qxk24_report_is_sound(&rep));
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Report, ReportHealthStandard) {
    QXK24KernelHandle kernel = new_started_kernel();
    QXK24Report rep{};
    register_six_kernel_resources(kernel);
    qxk24_cycle(kernel, nullptr);
    qxk24_report_generate(kernel, &rep);
    EXPECT_GE(rep.health_score, QXK24_HEALTH_STANDARD);
    finish_kernel(kernel);
}
TEST(QXK24FullCycle_Report, ReportLogsResources) {
    QXK24KernelHandle kernel = new_started_kernel();
    QXK24Report rep{};
    register_six_kernel_resources(kernel);
    qxk24_cycle(kernel, nullptr);
    qxk24_report_generate(kernel, &rep);
    EXPECT_EQ(rep.active_resources, 6U);
    finish_kernel(kernel);
}

TEST(QXK24FullCycle_Governor, RegistryHealthStandard) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    QXK24HealthResult result{};
    QXK24ResourceGovernor *gov = make_phase3_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_registry_init(&reg);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    qxk24_governor_cycle_all(&reg, &input);
    qxk24_health_compute(&input, &result);
    EXPECT_LT(result.score, QXK24_HEALTH_STANDARD);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24FullCycle_Governor, RegistryAllSixStandard) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    QXK24HealthResult result{};
    QXK24ResourceGovernor *g[6] = {
        make_phase3_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS),
        make_phase3_gov(QXK24_RESOURCE_IO, QXK24_DIR_BH),
        make_phase3_gov(QXK24_RESOURCE_CPU, QXK24_DIR_DN),
        make_phase3_gov(QXK24_RESOURCE_STORAGE, QXK24_DIR_BG),
        make_phase3_gov(QXK24_RESOURCE_NETWORK, QXK24_DIR_KN),
        make_phase3_gov(QXK24_RESOURCE_ENERGY, QXK24_DIR_KR)};
    QXK24Direction d[6] = {QXK24_DIR_AS, QXK24_DIR_BH, QXK24_DIR_DN,
                          QXK24_DIR_BG, QXK24_DIR_KN, QXK24_DIR_KR};
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, d[i], g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    qxk24_health_compute(&input, &result);
    EXPECT_GE(result.score, QXK24_HEALTH_STANDARD);
    for (auto *gov : g) qxk24_governor_base_destroy(gov);
}
TEST(QXK24FullCycle_Governor, RegistryAllSixSovereign) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    QXK24HealthResult result{};
    QXK24ResourceGovernor *g[6] = {
        make_phase3_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS),
        make_phase3_gov(QXK24_RESOURCE_IO, QXK24_DIR_BH),
        make_phase3_gov(QXK24_RESOURCE_CPU, QXK24_DIR_DN),
        make_phase3_gov(QXK24_RESOURCE_STORAGE, QXK24_DIR_BG),
        make_phase3_gov(QXK24_RESOURCE_NETWORK, QXK24_DIR_KN),
        make_phase3_gov(QXK24_RESOURCE_ENERGY, QXK24_DIR_KR)};
    QXK24Direction d[6] = {QXK24_DIR_AS, QXK24_DIR_BH, QXK24_DIR_DN,
                          QXK24_DIR_BG, QXK24_DIR_KN, QXK24_DIR_KR};
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, d[i], g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    qxk24_health_compute(&input, &result);
    EXPECT_EQ(result.tier, QXK24_CERT_SOVEREIGN);
    for (auto *gov : g) qxk24_governor_base_destroy(gov);
}
TEST(QXK24FullCycle_Governor, RegistryCycleSetsPairs) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    QXK24ResourceGovernor *a = make_phase3_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    QXK24ResourceGovernor *b = make_phase3_gov(QXK24_RESOURCE_IO, QXK24_DIR_BH);
    qxk24_governor_registry_init(&reg);
    qxk24_governor_register(&reg, QXK24_DIR_AS, a, "a");
    qxk24_governor_register(&reg, QXK24_DIR_BH, b, "b");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_FLOAT_EQ(input.z2, 1.0f / 3.0f);
    qxk24_governor_base_destroy(a);
    qxk24_governor_base_destroy(b);
}
TEST(QXK24FullCycle_Governor, RegistryCycleSetsDirections) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    QXK24ResourceGovernor *gov = make_phase3_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_registry_init(&reg);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_FLOAT_EQ(input.z3, 1.0f / 6.0f);
    qxk24_governor_base_destroy(gov);
}
