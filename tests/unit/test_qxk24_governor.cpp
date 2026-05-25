/* ============================================================
 * test_qxk24_governor.cpp
 * QXK24 Universal Kernel — Governor Integration Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_governor.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 32 Phase 3 proofs for the six-governor registry cycle.
 * ============================================================ */

#include "qxk24/kernel/qxk24_governor.h"
#include <cstring>
#include <gtest/gtest.h>

static QXK24GovernorConfig cfg(QXK24ResourceType type, QXK24Direction dir) {
    QXK24GovernorConfig c{};
    c.type = type;
    c.direction = dir;
    c.level = QXK24_LEVEL_SISTEM;
    c.budget_soft = 1024ULL * 1024ULL;
    c.declared_x = 1.0;
    c.tolerance_pct = 15.0;
    std::strncpy(c.label, "phase3", sizeof(c.label) - 1U);
    return c;
}

static QXK24ResourceGovernor *make_gov(QXK24ResourceType type,
                                       QXK24Direction dir) {
    QXK24ResourceGovernor *gov = nullptr;
    QXK24GovernorConfig c = cfg(type, dir);
    EXPECT_EQ(qxk24_governor_base_create(&c, &gov), QXK24_OK);
    EXPECT_EQ(qxk24_governor_base_update(gov, 512ULL * 1024ULL, 1.0), QXK24_OK);
    return gov;
}

struct GovSet {
    QXK24ResourceGovernor *g[6]{};
    QXK24Direction dirs[6] = {QXK24_DIR_AS, QXK24_DIR_BH, QXK24_DIR_DN,
                              QXK24_DIR_BG, QXK24_DIR_KN, QXK24_DIR_KR};
    QXK24ResourceType types[6] = {QXK24_RESOURCE_MEMORY, QXK24_RESOURCE_IO,
                                  QXK24_RESOURCE_CPU, QXK24_RESOURCE_STORAGE,
                                  QXK24_RESOURCE_NETWORK, QXK24_RESOURCE_ENERGY};
    const char *names[6] = {"Memory", "IO", "CPU", "Storage", "Network", "Energy"};
    void create() {
        for (int i = 0; i < 6; i++) g[i] = make_gov(types[i], dirs[i]);
    }
    void destroy() {
        for (auto *gov : g) qxk24_governor_base_destroy(gov);
    }
};

TEST(QXK24GovernorRegistry_Init, EmptyRegistry) {
    QXK24GovernorRegistry reg{};
    EXPECT_EQ(qxk24_governor_registry_init(&reg), QXK24_OK);
    EXPECT_EQ(reg.count, 0U);
}
TEST(QXK24GovernorRegistry_Init, NullRegistry) {
    EXPECT_EQ(qxk24_governor_registry_init(nullptr), QXK24_ERR_NULL_PARAM);
}

TEST(QXK24GovernorRegistry_Register, OneGovernor) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    EXPECT_EQ(qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory"), QXK24_OK);
    EXPECT_EQ(reg.count, 1U);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Register, NullRegistry) {
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    EXPECT_EQ(qxk24_governor_register(nullptr, QXK24_DIR_AS, gov, "Memory"),
              QXK24_ERR_NULL_PARAM);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Register, NullHandle) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    EXPECT_EQ(qxk24_governor_register(&reg, QXK24_DIR_AS, nullptr, "Memory"),
              QXK24_ERR_NULL_PARAM);
}
TEST(QXK24GovernorRegistry_Register, NullName) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    EXPECT_EQ(qxk24_governor_register(&reg, QXK24_DIR_AS, gov, nullptr),
              QXK24_ERR_NULL_PARAM);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Register, AllSix) {
    QXK24GovernorRegistry reg{};
    GovSet set;
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++)
        EXPECT_EQ(qxk24_governor_register(&reg, set.dirs[i], set.g[i], set.names[i]), QXK24_OK);
    EXPECT_EQ(reg.count, 6U);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Register, CapacityFull) {
    QXK24GovernorRegistry reg{};
    GovSet set;
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    EXPECT_EQ(qxk24_governor_register(&reg, QXK24_DIR_AS, set.g[0], "overflow"),
              QXK24_ERR_CAPACITY);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Register, NameCopied) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    EXPECT_STREQ(reg.slots[0].name, "Memory");
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Register, SlotStartsActive) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    EXPECT_TRUE(reg.slots[0].active);
    qxk24_governor_base_destroy(gov);
}

TEST(QXK24GovernorRegistry_Find, Registered) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    EXPECT_NE(qxk24_governor_find(&reg, QXK24_DIR_AS), nullptr);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Find, Unregistered) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    EXPECT_EQ(qxk24_governor_find(&reg, QXK24_DIR_BH), nullptr);
}
TEST(QXK24GovernorRegistry_Find, NullRegistry) {
    EXPECT_EQ(qxk24_governor_find(nullptr, QXK24_DIR_AS), nullptr);
}
TEST(QXK24GovernorRegistry_Find, IgnoresInactive) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    reg.slots[0].active = false;
    EXPECT_EQ(qxk24_governor_find(&reg, QXK24_DIR_AS), nullptr);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Find, ReturnsSlotDirection) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_IO, QXK24_DIR_BH);
    qxk24_governor_register(&reg, QXK24_DIR_BH, gov, "IO");
    ASSERT_NE(qxk24_governor_find(&reg, QXK24_DIR_BH), nullptr);
    EXPECT_EQ(qxk24_governor_find(&reg, QXK24_DIR_BH)->direction, QXK24_DIR_BH);
    qxk24_governor_base_destroy(gov);
}

TEST(QXK24GovernorRegistry_Active, EmptyIsZero) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    EXPECT_EQ(qxk24_governor_active_count(&reg), 0U);
}
TEST(QXK24GovernorRegistry_Active, NullIsZero) {
    EXPECT_EQ(qxk24_governor_active_count(nullptr), 0U);
}
TEST(QXK24GovernorRegistry_Active, OneActive) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    EXPECT_EQ(qxk24_governor_active_count(&reg), 1U);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Active, InactiveExcluded) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    reg.slots[0].active = false;
    EXPECT_EQ(qxk24_governor_active_count(&reg), 0U);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Active, SixActive) {
    QXK24GovernorRegistry reg{};
    GovSet set;
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    EXPECT_EQ(qxk24_governor_active_count(&reg), 6U);
    set.destroy();
}

TEST(QXK24GovernorRegistry_Cycle, NullRegistry) {
    QXK24HealthInput input{};
    EXPECT_EQ(qxk24_governor_cycle_all(nullptr, &input), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24GovernorRegistry_Cycle, NullInput) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    EXPECT_EQ(qxk24_governor_cycle_all(&reg, nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24GovernorRegistry_Cycle, EmptyScoresZeroExistence) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    qxk24_governor_registry_init(&reg);
    ASSERT_EQ(qxk24_governor_cycle_all(&reg, &input), QXK24_OK);
    EXPECT_FLOAT_EQ(input.z1, 0.0f);
}
TEST(QXK24GovernorRegistry_Cycle, AllSixDirectionsComplete) {
    QXK24GovernorRegistry reg{};
    GovSet set;
    QXK24HealthInput input{};
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], set.names[i]);
    ASSERT_EQ(qxk24_governor_cycle_all(&reg, &input), QXK24_OK);
    EXPECT_FLOAT_EQ(input.z3, 1.0f);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Cycle, AllPairsComplete) {
    QXK24GovernorRegistry reg{};
    GovSet set;
    QXK24HealthInput input{};
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_FLOAT_EQ(input.z2, 1.0f);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Cycle, FullCycleScoreIsStandard) {
    QXK24GovernorRegistry reg{};
    GovSet set;
    QXK24HealthInput input{};
    QXK24HealthResult result{};
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    ASSERT_EQ(qxk24_health_compute(&input, &result), QXK24_OK);
    EXPECT_GE(result.score, QXK24_HEALTH_STANDARD);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Cycle, LastLawScoreStored) {
    QXK24GovernorRegistry reg{};
    qxk24_governor_registry_init(&reg);
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    QXK24HealthInput input{};
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_GT(reg.slots[0].last_law_score, 0.0f);
    qxk24_governor_base_destroy(gov);
}
TEST(QXK24GovernorRegistry_Cycle, ActiveRatioReflected) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    GovSet set;
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 3; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_FLOAT_EQ(input.x3, 0.5f);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Cycle, PartialDirectionsScoreHalf) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    GovSet set;
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 3; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_FLOAT_EQ(input.z3, 0.5f);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Cycle, PairSymmetryPositive) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    GovSet set;
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 2; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_GT(input.x2, 0.0f);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Cycle, ConstitutionScorePerfectWhenActive) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    GovSet set;
    set.create();
    qxk24_governor_registry_init(&reg);
    for (int i = 0; i < 6; i++) qxk24_governor_register(&reg, set.dirs[i], set.g[i], "x");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_FLOAT_EQ(input.x4, 1.0f);
    set.destroy();
}
TEST(QXK24GovernorRegistry_Cycle, MeasurementScorePositive) {
    QXK24GovernorRegistry reg{};
    QXK24HealthInput input{};
    QXK24ResourceGovernor *gov = make_gov(QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
    qxk24_governor_registry_init(&reg);
    qxk24_governor_register(&reg, QXK24_DIR_AS, gov, "Memory");
    qxk24_governor_cycle_all(&reg, &input);
    EXPECT_GT(input.z4, 0.0f);
    qxk24_governor_base_destroy(gov);
}
