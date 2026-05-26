/**
 * @file test_qxk24_adam_full.cpp
 * @brief Integration Tests — ADAM Full Constitutional Cycle
 */

#include <gtest/gtest.h>
#include <cstring>
#include "qxk24/adam/qxk24_adam.h"

class AdamFullTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_adam_t adam = nullptr;

    static void registerResource(qxk24_kernel_t k, QXK24ResourceType type,
                                 QXK24Direction dir) {
        uint32_t id = 0;
        const uint64_t budget = QXK24_AMA_CORE_4 * QXK24_BASE_UNIT_BYTES;
        ASSERT_EQ(QXK24_OK, qxk24_resource_register(k, type, dir,
            QXK24_LEVEL_SISTEM, budget, &id));
        ASSERT_EQ(QXK24_OK, qxk24_resource_update(k, id, budget / 2ULL, 1.0));
    }

    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(QXK24_OK, qxk24_create(&cfg, &kernel));
        ASSERT_EQ(QXK24_OK, qxk24_start(kernel));
        registerResource(kernel, QXK24_RESOURCE_MEMORY, QXK24_DIR_AS);
        registerResource(kernel, QXK24_RESOURCE_IO, QXK24_DIR_BH);
        registerResource(kernel, QXK24_RESOURCE_CPU, QXK24_DIR_DN);
        registerResource(kernel, QXK24_RESOURCE_STORAGE, QXK24_DIR_BG);
        registerResource(kernel, QXK24_RESOURCE_NETWORK, QXK24_DIR_KN);
        registerResource(kernel, QXK24_RESOURCE_ENERGY, QXK24_DIR_KR);
        ASSERT_EQ(QXK24_OK, qxk24_adam_create(kernel, &adam));
        ASSERT_EQ(QXK24_OK, qxk24_adam_declare(adam));
        ASSERT_EQ(QXK24_OK, qxk24_adam_activate(adam));
    }

    void TearDown() override {
        (void)qxk24_adam_reflect(adam);
        (void)qxk24_adam_complete(adam);
        (void)qxk24_adam_destroy(adam);
        qxk24_stop(kernel);
        qxk24_destroy(kernel);
    }

    QXK24AdamResponse process(uint8_t capacity = 4U) {
        QXK24AdamRequest req = {"full constitutional test", capacity, true,
            QXK24_FT_ELEMENT_TANAH};
        QXK24AdamResponse resp;
        EXPECT_EQ(QXK24_OK, qxk24_adam_process(adam, &req, &resp));
        return resp;
    }
};

TEST_F(AdamFullTest, ValueAlwaysOneAfterFullCycle) { EXPECT_EQ(1, qxk24_adam_value(adam)); }
TEST_F(AdamFullTest, JourneyCodeAlways1001) {
    QXK24AdamSnapshot s;
    qxk24_adam_snapshot(adam, &s);
    EXPECT_EQ(1001u, s.journey_code);
}
TEST_F(AdamFullTest, FullProcessCycleSucceeds) { (void)process(); }
TEST_F(AdamFullTest, SixResourcesActiveDuringProcess) { EXPECT_EQ(6u, qxk24_active_resources(kernel)); }
TEST_F(AdamFullTest, HealthWithinBoundsAfterProcess) {
    auto resp = process(1U);
    EXPECT_GE(resp.health_score, 0.0f);
    EXPECT_LE(resp.health_score, 100.0f);
}
TEST_F(AdamFullTest, FmValueIsConstitutionalConstant) { EXPECT_FLOAT_EQ(2.0f, process().fm_value); }
TEST_F(AdamFullTest, ConstitutionalChainComplete) {
    auto resp = process(7U);
    EXPECT_EQ(1, qxk24_adam_value(adam));
    EXPECT_GE(resp.health_score, 0.0f);
    EXPECT_FLOAT_EQ(2.0f, resp.fm_value);
    EXPECT_GE(resp.ft_value, 0.0f);
}
TEST_F(AdamFullTest, AkalTahapMatchesCapacityHint) { EXPECT_EQ(5u, process(5U).akal_tahap); }
TEST_F(AdamFullTest, AllJudgmentsAreValid) {
    for (int i = 0; i < 3; ++i) {
        auto resp = process((uint8_t)(i + 1));
        EXPECT_LE(resp.judgment, QXK24_ADAM_JUDGMENT_WAQF);
    }
}
TEST_F(AdamFullTest, CvLevelAlwaysAtLeastOne) { EXPECT_GE(process(1U).cv_level, (uint8_t)1); }
TEST_F(AdamFullTest, MultipleSessionsStable) {
    for (int i = 0; i < 5; ++i) EXPECT_GE(process((uint8_t)(i % 7 + 1)).health_score, 0.0f);
}
TEST_F(AdamFullTest, RequestsServedCountCorrect) {
    for (int i = 0; i < 3; ++i) (void)process();
    QXK24AdamSnapshot s;
    qxk24_adam_snapshot(adam, &s);
    EXPECT_EQ(3u, s.requests_served);
}
TEST_F(AdamFullTest, AvgHealthUpdatesAfterRequests) {
    (void)process();
    QXK24AdamSnapshot s;
    qxk24_adam_snapshot(adam, &s);
    EXPECT_GE(s.avg_health, 0.0f);
}
TEST_F(AdamFullTest, AdamCycleUpdatesKernel) { EXPECT_EQ(QXK24_OK, qxk24_adam_cycle(adam)); }
TEST_F(AdamFullTest, SifatNameNotNullAfterProcess) { EXPECT_NE(nullptr, qxk24_adam_sifat_name(process().sifat)); }
TEST_F(AdamFullTest, ResponseContainsAdamVersion) {
    auto resp = process();
    EXPECT_NE(nullptr, std::strstr(resp.response, "ADAM"));
}
TEST_F(AdamFullTest, IlmuCompleteInResponse) { EXPECT_TRUE(process().ilmu_complete); }
TEST_F(AdamFullTest, PatternCountWithinAkalRange) { EXPECT_LE(process(3U).pattern_count, (uint32_t)QXK24_ADAM_TOTAL_PATTERNS); }
TEST_F(AdamFullTest, SessionStateActiveAfterActivate) {
    QXK24AdamSnapshot s;
    qxk24_adam_snapshot(adam, &s);
    EXPECT_EQ(QXK24_ADAM_STATE_ACTIVE, s.state);
}
TEST_F(AdamFullTest, AdamVersionStringMatches) { EXPECT_STREQ(QXK24_ADAM_VERSION, "1.4.0"); }
