/**
 * @file test_qxk24_adam.cpp
 * @brief Unit Tests — ADAM Constitutional AI Engine
 * ALAMTOLOGI – Quranic Science | Founder: Masa Bayu
 */

#include <gtest/gtest.h>
#include <cstring>
#include "qxk24/adam/qxk24_adam.h"

class AdamTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_adam_t adam = nullptr;

    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(QXK24_OK, qxk24_create(&cfg, &kernel));
        ASSERT_EQ(QXK24_OK, qxk24_start(kernel));
        ASSERT_EQ(QXK24_OK, qxk24_adam_create(kernel, &adam));
    }

    void TearDown() override {
        (void)qxk24_adam_destroy(adam);
        qxk24_stop(kernel);
        qxk24_destroy(kernel);
    }

    void activate() {
        ASSERT_EQ(QXK24_OK, qxk24_adam_declare(adam));
        ASSERT_EQ(QXK24_OK, qxk24_adam_activate(adam));
    }

    QXK24AdamResponse process(uint8_t capacity = 1U) {
        QXK24AdamRequest req = {"constitutional test", capacity, true,
            QXK24_FT_ELEMENT_TANAH};
        QXK24AdamResponse resp;
        EXPECT_EQ(QXK24_OK, qxk24_adam_process(adam, &req, &resp));
        return resp;
    }
};

TEST_F(AdamTest, ValueIsAlwaysOne) { EXPECT_EQ(1, qxk24_adam_value(adam)); }
TEST_F(AdamTest, ValueConstantMatchesMacro) { EXPECT_EQ(QXK24_ADAM_VALUE, qxk24_adam_value(adam)); }
TEST_F(AdamTest, JourneyCodeIs1001) {
    QXK24AdamSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_snapshot(adam, &s));
    EXPECT_EQ((uint32_t)QXK24_ADAM_JOURNEY_CODE, s.journey_code);
}
TEST_F(AdamTest, InitialStateIsDormant) {
    QXK24AdamSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_snapshot(adam, &s));
    EXPECT_EQ(QXK24_ADAM_STATE_DORMANT, s.state);
}
TEST_F(AdamTest, InitialSifatIsAkur) { EXPECT_EQ(QXK24_ADAM_SIFAT_AKUR, qxk24_adam_sifat(adam)); }
TEST_F(AdamTest, AkalGoverned) { EXPECT_EQ(1, QXK24_ADAM_AKAL_GOVERNED); }
TEST_F(AdamTest, KeperluanOnly) { EXPECT_EQ(1, QXK24_ADAM_KEPERLUAN_ONLY); }
TEST_F(AdamTest, TotalPatternsIs49) { EXPECT_EQ(49, QXK24_ADAM_TOTAL_PATTERNS); }
TEST_F(AdamTest, CapacityMaxIs7) { EXPECT_EQ(7, QXK24_ADAM_CAPACITY_MAX); }
TEST_F(AdamTest, CapacityMinIs1) { EXPECT_EQ(1, QXK24_ADAM_CAPACITY_MIN); }
TEST_F(AdamTest, DeclareSucceeds) {
    EXPECT_EQ(QXK24_OK, qxk24_adam_declare(adam));
    QXK24AdamSnapshot s;
    qxk24_adam_snapshot(adam, &s);
    EXPECT_EQ(QXK24_ADAM_STATE_DECLARED, s.state);
}
TEST_F(AdamTest, CannotDeclareTwice) {
    ASSERT_EQ(QXK24_OK, qxk24_adam_declare(adam));
    EXPECT_NE(QXK24_OK, qxk24_adam_declare(adam));
}
TEST_F(AdamTest, ActivateAfterDeclareSucceeds) {
    ASSERT_EQ(QXK24_OK, qxk24_adam_declare(adam));
    EXPECT_EQ(QXK24_OK, qxk24_adam_activate(adam));
}
TEST_F(AdamTest, CannotActivateWithoutDeclare) { EXPECT_NE(QXK24_OK, qxk24_adam_activate(adam)); }
TEST_F(AdamTest, ReflectAfterActivateSucceeds) {
    activate();
    EXPECT_EQ(QXK24_OK, qxk24_adam_reflect(adam));
}
TEST_F(AdamTest, CompleteAfterReflectSucceeds) {
    activate();
    ASSERT_EQ(QXK24_OK, qxk24_adam_reflect(adam));
    EXPECT_EQ(QXK24_OK, qxk24_adam_complete(adam));
}
TEST_F(AdamTest, SessionsCountAfterComplete) {
    activate();
    ASSERT_EQ(QXK24_OK, qxk24_adam_reflect(adam));
    ASSERT_EQ(QXK24_OK, qxk24_adam_complete(adam));
    QXK24AdamSnapshot s;
    qxk24_adam_snapshot(adam, &s);
    EXPECT_EQ(1u, s.sessions_done);
}
TEST_F(AdamTest, CycleSucceeds) { EXPECT_EQ(QXK24_OK, qxk24_adam_cycle(adam)); }
TEST_F(AdamTest, ProcessBeforeActivateFails) {
    QXK24AdamRequest req = {"test", 1, false, QXK24_FT_ELEMENT_TANAH};
    QXK24AdamResponse resp;
    EXPECT_NE(QXK24_OK, qxk24_adam_process(adam, &req, &resp));
}
TEST_F(AdamTest, ProcessAfterActivateSucceeds) { activate(); (void)process(); }
TEST_F(AdamTest, ResponseJudgmentValid) {
    activate();
    auto resp = process();
    EXPECT_LE(resp.judgment, QXK24_ADAM_JUDGMENT_WAQF);
}
TEST_F(AdamTest, ResponseHealthNonNegative) { activate(); EXPECT_GE(process().health_score, 0.0f); }
TEST_F(AdamTest, ResponseHealthAtMost100) { activate(); EXPECT_LE(process().health_score, 100.0f); }
TEST_F(AdamTest, ResponseFmValueIsTwo) { activate(); EXPECT_FLOAT_EQ(QXK24_FM_VALUE, process().fm_value); }
TEST_F(AdamTest, ResponseFtValueNonNegative) { activate(); EXPECT_GE(process().ft_value, 0.0f); }
TEST_F(AdamTest, ResponseAkalTahapValid) {
    activate();
    auto resp = process(3U);
    EXPECT_GE(resp.akal_tahap, (uint8_t)1);
    EXPECT_LE(resp.akal_tahap, (uint8_t)7);
}
TEST_F(AdamTest, ResponseCvLevelValid) {
    activate();
    auto resp = process();
    EXPECT_GE(resp.cv_level, (uint8_t)1);
    EXPECT_LE(resp.cv_level, (uint8_t)7);
}
TEST_F(AdamTest, RequestsServedIncrement) {
    activate();
    (void)process();
    QXK24AdamSnapshot s;
    qxk24_adam_snapshot(adam, &s);
    EXPECT_EQ(1u, s.requests_served);
}
TEST_F(AdamTest, MultipleRequestsStable) {
    activate();
    for (int i = 0; i < 5; ++i) EXPECT_GE(process().health_score, 0.0f);
}
TEST_F(AdamTest, ResponseNotEmpty) {
    activate();
    auto resp = process(4U);
    EXPECT_GT(std::strlen(resp.response), 0u);
}
TEST_F(AdamTest, JudgmentNameMakmur) { EXPECT_STREQ("MAKMUR", qxk24_adam_judgment_name(QXK24_ADAM_JUDGMENT_MAKMUR)); }
TEST_F(AdamTest, JudgmentNameIslah) { EXPECT_STREQ("ISLAH", qxk24_adam_judgment_name(QXK24_ADAM_JUDGMENT_ISLAH)); }
TEST_F(AdamTest, JudgmentNameWaqf) { EXPECT_STREQ("WAQF", qxk24_adam_judgment_name(QXK24_ADAM_JUDGMENT_WAQF)); }
TEST_F(AdamTest, SifatNameAkur) { EXPECT_STREQ("AKUR", qxk24_adam_sifat_name(QXK24_ADAM_SIFAT_AKUR)); }
TEST_F(AdamTest, SifatNameEngkar) { EXPECT_STREQ("ENGKAR", qxk24_adam_sifat_name(QXK24_ADAM_SIFAT_ENGKAR)); }
TEST_F(AdamTest, CheckAkurInitially) { EXPECT_EQ(QXK24_OK, qxk24_adam_check_akur(adam)); }
TEST_F(AdamTest, AkalPatternCountLevel1Is1) { EXPECT_EQ(1, QXK24_AKAL_PATTERN_COUNT[0]); }
TEST_F(AdamTest, AkalPatternCountLevel7Is22) { EXPECT_EQ(22, QXK24_AKAL_PATTERN_COUNT[6]); }
TEST_F(AdamTest, AkalTotalPatternsIs49) {
    uint32_t total = 0;
    for (int i = 0; i < 7; ++i) total += QXK24_AKAL_PATTERN_COUNT[i];
    EXPECT_EQ(49u, total);
}
TEST_F(AdamTest, CognitiveRequestHasValidCv) { activate(); EXPECT_GE(process(7U).cv_level, (uint8_t)1); }
