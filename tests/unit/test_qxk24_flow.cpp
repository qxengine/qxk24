/* ============================================================
 * test_qxk24_flow.cpp
 * QXK24 Universal Kernel — OAT + OASS Flow Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_flow.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 56 machine proofs for OAT direct flows, OASS cross flows,
 * ABA tracking, and constitutional flow scoring.
 * ============================================================ */

#include "qxk24/flow/qxk24_oass.h"
#include "qxk24/flow/qxk24_oat.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

class OATTest : public ::testing::Test {
protected:
    QXK24OATContext *ctx = nullptr;
    void SetUp() override { ASSERT_EQ(qxk24_oat_context_create(&ctx), QXK24_OK); }
    void TearDown() override { qxk24_oat_context_destroy(ctx); }
    uint64_t begin(QXK24FlowDir dir = QXK24_FLOW_IKJ,
                   QXK24FlowType type = QXK24_FLOW_OAT_SEHALA,
                   uint64_t bytes = 1024) {
        uint64_t id = 0;
        EXPECT_EQ(qxk24_oat_begin(ctx, type, dir, QXK24_LEVEL_ATOM,
                                  QXK24_DIR_ATAS, 1, bytes, &id), QXK24_OK);
        return id;
    }
};

TEST_F(OATTest, Create_Succeeds) { EXPECT_TRUE(ctx->initialized); }
TEST(QXK24OAT_Context, Create_Null_Fails) {
    EXPECT_EQ(qxk24_oat_context_create(nullptr), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OATTest, Begin_IKJ_Succeeds) { EXPECT_GT(begin(QXK24_FLOW_IKJ), 0ULL); }
TEST_F(OATTest, Begin_LWJ_Succeeds) { EXPECT_GT(begin(QXK24_FLOW_LWJ), 0ULL); }
TEST_F(OATTest, Begin_ZeroBytes_Fails) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oat_begin(ctx, QXK24_FLOW_OAT_SEHALA, QXK24_FLOW_IKJ,
                              QXK24_LEVEL_ATOM, QXK24_DIR_ATAS, 1, 0, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OATTest, Begin_NullFlowId_Fails) {
    EXPECT_EQ(qxk24_oat_begin(ctx, QXK24_FLOW_OAT_SEHALA, QXK24_FLOW_IKJ,
                              QXK24_LEVEL_ATOM, QXK24_DIR_ATAS, 1, 1, nullptr),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OATTest, Begin_InvalidLevel_Fails) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oat_begin(ctx, QXK24_FLOW_OAT_SEHALA, QXK24_FLOW_IKJ,
                              (QXK24Level)99, QXK24_DIR_ATAS, 1, 1, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OATTest, Begin_InvalidSegment_Fails) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oat_begin(ctx, QXK24_FLOW_OAT_SEHALA, QXK24_FLOW_IKJ,
                              QXK24_LEVEL_ATOM, (QXK24Direction)99, 1, 1, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OATTest, OATType_OASS_Rejected) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oat_begin(ctx, QXK24_FLOW_OASS_SEHALA, QXK24_FLOW_IKJ,
                              QXK24_LEVEL_ATOM, QXK24_DIR_ATAS, 1, 1, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OATTest, Complete_Succeeds) {
    uint64_t id = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OAT_SEHALA, 4096);
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    QXK24OATMeasure m;
    ASSERT_EQ(qxk24_oat_complete(ctx, id, &m), QXK24_OK);
    EXPECT_GT(m.x_value, 0.0);
    EXPECT_EQ(m.m_bytes, 4096ULL);
}
TEST_F(OATTest, Complete_UnknownFlow_Fails) {
    EXPECT_EQ(qxk24_oat_complete(ctx, 9999ULL, nullptr), QXK24_ERR_NOT_FOUND);
}
TEST_F(OATTest, Cancel_IncrementsViolation) {
    uint64_t id = begin();
    uint32_t before = ctx->violation_count;
    EXPECT_EQ(qxk24_oat_cancel(ctx, id), QXK24_OK);
    EXPECT_EQ(ctx->violation_count, before + 1U);
}
TEST_F(OATTest, Measure_ActiveFlow_Succeeds) {
    uint64_t id = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OAT_SEHALA, 8192);
    QXK24OATMeasure m;
    ASSERT_EQ(qxk24_oat_measure(ctx, id, 16384, &m), QXK24_OK);
    EXPECT_TRUE(m.within_limit);
    EXPECT_NEAR(m.budget_ratio, 0.5, 0.01);
}
TEST_F(OATTest, FlowCount_TracksActiveFlows) {
    uint64_t a = begin(), b = begin(QXK24_FLOW_LWJ), c = begin();
    EXPECT_EQ(ctx->flow_count, 3U);
    EXPECT_EQ(qxk24_oat_complete(ctx, a, nullptr), QXK24_OK);
    EXPECT_EQ(ctx->flow_count, 2U);
    (void)b; (void)c;
}
TEST_F(OATTest, XFormula_IsMonotonicWithBytes) {
    uint64_t a = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OAT_SEHALA, 1024);
    uint64_t b = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OAT_SEHALA, 8192);
    QXK24OATMeasure ma, mb;
    ASSERT_EQ(qxk24_oat_complete(ctx, a, &ma), QXK24_OK);
    ASSERT_EQ(qxk24_oat_complete(ctx, b, &mb), QXK24_OK);
    EXPECT_GT(mb.x_value, ma.x_value);
}

TEST_F(OATTest, Pusingan_IKJ_Then_LWJ_IsComplete) {
    uint64_t a = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OAT_PUSINGAN);
    uint64_t b = begin(QXK24_FLOW_LWJ, QXK24_FLOW_OAT_PUSINGAN);
    ASSERT_EQ(qxk24_oat_complete(ctx, a, nullptr), QXK24_OK);
    ASSERT_EQ(qxk24_oat_complete(ctx, b, nullptr), QXK24_OK);
    EXPECT_EQ(ctx->total_ikj, 1ULL);
    EXPECT_EQ(ctx->total_lwj, 1ULL);
}
TEST_F(OATTest, Z3_Balance_PerfectWhenIKJEqualsLWJ) {
    uint64_t a = begin(QXK24_FLOW_IKJ), b = begin(QXK24_FLOW_LWJ);
    ASSERT_EQ(qxk24_oat_complete(ctx, a, nullptr), QXK24_OK);
    ASSERT_EQ(qxk24_oat_complete(ctx, b, nullptr), QXK24_OK);
    EXPECT_DOUBLE_EQ(qxk24_oat_z3_balance(ctx), 1.0);
}
TEST_F(OATTest, Z3_Balance_ImperfectWhenUnbalanced) {
    for (int i = 0; i < 3; i++) ASSERT_EQ(qxk24_oat_complete(ctx, begin(), nullptr), QXK24_OK);
    ASSERT_EQ(qxk24_oat_complete(ctx, begin(QXK24_FLOW_LWJ), nullptr), QXK24_OK);
    EXPECT_LT(qxk24_oat_z3_balance(ctx), 1.0);
}
TEST_F(OATTest, Z1_Score_PerfectWithNoCancellations) {
    ASSERT_EQ(qxk24_oat_complete(ctx, begin(), nullptr), QXK24_OK);
    EXPECT_DOUBLE_EQ(qxk24_oat_z1_score(ctx), 1.0);
}
TEST_F(OATTest, Z1_Score_DegradesWithCancellations) {
    uint64_t a = begin(), b = begin();
    ASSERT_EQ(qxk24_oat_complete(ctx, a, nullptr), QXK24_OK);
    ASSERT_EQ(qxk24_oat_cancel(ctx, b), QXK24_OK);
    EXPECT_LT(qxk24_oat_z1_score(ctx), 1.0);
}
TEST_F(OATTest, Z2_Score_PerfectWhenWithinBudget) {
    ASSERT_EQ(qxk24_oat_complete(ctx, begin(QXK24_FLOW_IKJ, QXK24_FLOW_OAT_SEHALA, 512), nullptr), QXK24_OK);
    EXPECT_DOUBLE_EQ(qxk24_oat_z2_score(ctx, 1024), 1.0);
}
TEST_F(OATTest, Z2_Score_ZeroWhenExceedsBudget) {
    ASSERT_EQ(qxk24_oat_complete(ctx, begin(QXK24_FLOW_IKJ, QXK24_FLOW_OAT_SEHALA, 4096), nullptr), QXK24_OK);
    EXPECT_DOUBLE_EQ(qxk24_oat_z2_score(ctx, 512), 0.0);
}
TEST_F(OATTest, NullScoreFunctionsAreStable) {
    EXPECT_DOUBLE_EQ(qxk24_oat_z1_score(nullptr), 0.0);
    EXPECT_DOUBLE_EQ(qxk24_oat_z2_score(nullptr, 4096), 1.0);
    EXPECT_DOUBLE_EQ(qxk24_oat_z3_balance(nullptr), 1.0);
}

class OASSTest : public ::testing::Test {
protected:
    QXK24OASSContext *ctx = nullptr;
    void SetUp() override { ASSERT_EQ(qxk24_oass_context_create(&ctx), QXK24_OK); }
    void TearDown() override { qxk24_oass_context_destroy(ctx); }
    uint64_t begin(QXK24FlowDir dir = QXK24_FLOW_IKJ,
                   QXK24FlowType type = QXK24_FLOW_OASS_SEHALA,
                   QXK24Direction s = QXK24_DIR_KIRI,
                   QXK24Direction d = QXK24_DIR_BELAKANG,
                   uint64_t bytes = 1024) {
        uint64_t id = 0;
        EXPECT_EQ(qxk24_oass_begin(ctx, type, dir, s, d, QXK24_LEVEL_ATOM,
                                   QXK24_LEVEL_ATOM, 1, 2, bytes, &id), QXK24_OK);
        return id;
    }
};

TEST_F(OASSTest, Create_Succeeds) { EXPECT_TRUE(ctx->initialized); }
TEST(QXK24OASS_Context, Create_Null_Fails) {
    EXPECT_EQ(qxk24_oass_context_create(nullptr), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, Begin_CrossFlow_Succeeds) { EXPECT_GT(begin(), 0ULL); }
TEST_F(OASSTest, Begin_SameDirection_Fails) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oass_begin(ctx, QXK24_FLOW_OASS_SEHALA, QXK24_FLOW_IKJ,
                               QXK24_DIR_KIRI, QXK24_DIR_KIRI, QXK24_LEVEL_ATOM,
                               QXK24_LEVEL_ATOM, 1, 1, 1, &id),
              QXK24_ERR_DIRECTION_FAULT);
}
TEST_F(OASSTest, Begin_ZeroBytes_Fails) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oass_begin(ctx, QXK24_FLOW_OASS_SEHALA, QXK24_FLOW_IKJ,
                               QXK24_DIR_KIRI, QXK24_DIR_BELAKANG, QXK24_LEVEL_ATOM,
                               QXK24_LEVEL_ATOM, 1, 2, 0, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, Begin_OATType_Rejected) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oass_begin(ctx, QXK24_FLOW_OAT_SEHALA, QXK24_FLOW_IKJ,
                               QXK24_DIR_KIRI, QXK24_DIR_BELAKANG, QXK24_LEVEL_ATOM,
                               QXK24_LEVEL_ATOM, 1, 2, 1, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, Begin_InvalidSrcLevel_Fails) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oass_begin(ctx, QXK24_FLOW_OASS_SEHALA, QXK24_FLOW_IKJ,
                               QXK24_DIR_KIRI, QXK24_DIR_BELAKANG, (QXK24Level)99,
                               QXK24_LEVEL_ATOM, 1, 2, 1, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, Begin_InvalidDstLevel_Fails) {
    uint64_t id = 0;
    EXPECT_EQ(qxk24_oass_begin(ctx, QXK24_FLOW_OASS_SEHALA, QXK24_FLOW_IKJ,
                               QXK24_DIR_KIRI, QXK24_DIR_BELAKANG, QXK24_LEVEL_ATOM,
                               (QXK24Level)99, 1, 2, 1, &id),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, Begin_NullFlowId_Fails) {
    EXPECT_EQ(qxk24_oass_begin(ctx, QXK24_FLOW_OASS_SEHALA, QXK24_FLOW_IKJ,
                               QXK24_DIR_KIRI, QXK24_DIR_BELAKANG, QXK24_LEVEL_ATOM,
                               QXK24_LEVEL_ATOM, 1, 2, 1, nullptr),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, Complete_Succeeds) {
    uint64_t id = begin();
    QXK24OASSMeasure m;
    ASSERT_EQ(qxk24_oass_complete(ctx, id, &m), QXK24_OK);
    EXPECT_TRUE(m.pairs_valid);
    EXPECT_GT(m.x_dst, 0.0);
}
TEST_F(OASSTest, Complete_UnknownFlow_Fails) {
    EXPECT_EQ(qxk24_oass_complete(ctx, 9999ULL, nullptr), QXK24_ERR_NOT_FOUND);
}
TEST_F(OASSTest, Cancel_IncrementsViolationAndBrokenABA) {
    uint64_t id = begin();
    ASSERT_EQ(qxk24_oass_cancel(ctx, id), QXK24_OK);
    EXPECT_EQ(ctx->violation_count, 1U);
    EXPECT_EQ(ctx->aba_cycles_broken, 1ULL);
}
TEST_F(OASSTest, BytesSent_TrackedPerDirection) {
    uint64_t id = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_SEHALA,
                        QXK24_DIR_KIRI, QXK24_DIR_BELAKANG, 32768);
    EXPECT_EQ(ctx->bytes_sent[(uint32_t)QXK24_DIR_KIRI], 32768ULL);
    (void)id;
}
TEST_F(OASSTest, FlowCount_Tracks_Active) {
    uint64_t a = begin(), b = begin(QXK24_FLOW_LWJ, QXK24_FLOW_OASS_SEHALA,
                                    QXK24_DIR_BELAKANG, QXK24_DIR_KIRI);
    EXPECT_EQ(ctx->flow_count, 2U);
    ASSERT_EQ(qxk24_oass_complete(ctx, a, nullptr), QXK24_OK);
    EXPECT_EQ(ctx->flow_count, 1U);
    (void)b;
}

TEST_F(OASSTest, ABA_Begin_IKJ_Succeeds) {
    uint64_t ikj = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
    QXK24ABATracker t;
    ASSERT_EQ(qxk24_oass_aba_begin(ctx, ikj, &t), QXK24_OK);
    EXPECT_EQ(t.state, QXK24_ABA_PHASE_4);
}
TEST_F(OASSTest, ABA_Begin_Unknown_Fails) {
    QXK24ABATracker t;
    EXPECT_EQ(qxk24_oass_aba_begin(ctx, 9999ULL, &t), QXK24_ERR_NOT_FOUND);
}
TEST_F(OASSTest, ABA_Complete_WithLWJ_Succeeds) {
    uint64_t ikj = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
    uint64_t lwj = begin(QXK24_FLOW_LWJ, QXK24_FLOW_OASS_PUSINGAN,
                         QXK24_DIR_BELAKANG, QXK24_DIR_KIRI);
    QXK24ABATracker t;
    ASSERT_EQ(qxk24_oass_aba_begin(ctx, ikj, &t), QXK24_OK);
    ASSERT_EQ(qxk24_oass_aba_complete(ctx, &t, lwj), QXK24_OK);
    EXPECT_EQ(t.state, QXK24_ABA_COMPLETE);
}
TEST_F(OASSTest, ABA_Complete_WrongDirection_Fails) {
    uint64_t ikj = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
    uint64_t ikj2 = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
    QXK24ABATracker t;
    ASSERT_EQ(qxk24_oass_aba_begin(ctx, ikj, &t), QXK24_OK);
    EXPECT_EQ(qxk24_oass_aba_complete(ctx, &t, ikj2), QXK24_ERR_NOT_FOUND);
}
TEST_F(OASSTest, ABA_Complete_NullTracker_Fails) {
    EXPECT_EQ(qxk24_oass_aba_complete(ctx, nullptr, 1), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, ABA_Complete_InvalidTracker_Fails) {
    QXK24ABATracker t{};
    EXPECT_EQ(qxk24_oass_aba_complete(ctx, &t, 1), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(OASSTest, ABA_Multiple_Cycles_Accumulate) {
    for (int i = 0; i < 5; i++) {
        uint64_t ikj = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
        uint64_t lwj = begin(QXK24_FLOW_LWJ, QXK24_FLOW_OASS_PUSINGAN,
                             QXK24_DIR_BELAKANG, QXK24_DIR_KIRI);
        QXK24ABATracker t;
        ASSERT_EQ(qxk24_oass_aba_begin(ctx, ikj, &t), QXK24_OK);
        ASSERT_EQ(qxk24_oass_aba_complete(ctx, &t, lwj), QXK24_OK);
    }
    EXPECT_EQ(ctx->aba_cycles_complete, 5ULL);
}
TEST_F(OASSTest, ABA_BrokenCycle_DegradesX2Score) {
    ASSERT_EQ(qxk24_oass_cancel(ctx, begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN)), QXK24_OK);
    EXPECT_LT(qxk24_oass_x2_score(ctx), 1.0);
}
TEST_F(OASSTest, ABA_Complete_InvalidState_Fails) {
    uint64_t lwj = begin(QXK24_FLOW_LWJ, QXK24_FLOW_OASS_PUSINGAN,
                         QXK24_DIR_BELAKANG, QXK24_DIR_KIRI);
    QXK24ABATracker t{};
    t.valid = true;
    t.state = QXK24_ABA_IDLE;
    EXPECT_EQ(qxk24_oass_aba_complete(ctx, &t, lwj), QXK24_ERR_FLOW_VIOLATION);
}
TEST_F(OASSTest, ABA_Timestamps_AreSet) {
    uint64_t ikj = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
    QXK24ABATracker t;
    ASSERT_EQ(qxk24_oass_aba_begin(ctx, ikj, &t), QXK24_OK);
    EXPECT_GT(t.timestamp_start_ms, 0ULL);
    EXPECT_GE(t.timestamp_peak_ms, t.timestamp_start_ms);
}

TEST_F(OASSTest, Z3_Score_Perfect_ValidCrossFlows) {
    ASSERT_EQ(qxk24_oass_complete(ctx, begin(), nullptr), QXK24_OK);
    EXPECT_DOUBLE_EQ(qxk24_oass_z3_score(ctx), 1.0);
}
TEST_F(OASSTest, Z4_Score_InRange_0_to_1) {
    double score = qxk24_oass_z4_score(ctx);
    EXPECT_GE(score, 0.0);
    EXPECT_LE(score, 1.0);
}
TEST_F(OASSTest, Z4_Score_BalancedFlow_IsHigh) {
    ASSERT_EQ(qxk24_oass_complete(ctx, begin(), nullptr), QXK24_OK);
    ASSERT_EQ(qxk24_oass_complete(ctx, begin(QXK24_FLOW_LWJ, QXK24_FLOW_OASS_SEHALA,
                                             QXK24_DIR_BELAKANG, QXK24_DIR_KIRI), nullptr), QXK24_OK);
    EXPECT_GT(qxk24_oass_z4_score(ctx), 0.5);
}
TEST_F(OASSTest, X2_Score_Is1_WhenNoCycles) { EXPECT_DOUBLE_EQ(qxk24_oass_x2_score(ctx), 1.0); }
TEST_F(OASSTest, X2_Score_Is1_AllCyclesComplete) {
    uint64_t ikj = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
    uint64_t lwj = begin(QXK24_FLOW_LWJ, QXK24_FLOW_OASS_PUSINGAN,
                         QXK24_DIR_BELAKANG, QXK24_DIR_KIRI);
    QXK24ABATracker t;
    ASSERT_EQ(qxk24_oass_aba_begin(ctx, ikj, &t), QXK24_OK);
    ASSERT_EQ(qxk24_oass_aba_complete(ctx, &t, lwj), QXK24_OK);
    EXPECT_DOUBLE_EQ(qxk24_oass_x2_score(ctx), 1.0);
}
TEST_F(OASSTest, Score_Functions_HandleNullCtx) {
    EXPECT_DOUBLE_EQ(qxk24_oass_z3_score(nullptr), 1.0);
    EXPECT_DOUBLE_EQ(qxk24_oass_z4_score(nullptr), 1.0);
    EXPECT_DOUBLE_EQ(qxk24_oass_x2_score(nullptr), 1.0);
}
TEST_F(OASSTest, Z4_OneWayScore_DegradesButInRange) {
    (void)begin();
    double score = qxk24_oass_z4_score(ctx);
    EXPECT_GE(score, 0.0);
    EXPECT_LT(score, 1.0);
}
TEST_F(OASSTest, Z3_NoCompletedFlows_IsPerfect) { EXPECT_DOUBLE_EQ(qxk24_oass_z3_score(ctx), 1.0); }
TEST_F(OASSTest, X2_PartialBrokenAndComplete_IsBetweenZeroAndOne) {
    ASSERT_EQ(qxk24_oass_cancel(ctx, begin()), QXK24_OK);
    uint64_t ikj = begin(QXK24_FLOW_IKJ, QXK24_FLOW_OASS_PUSINGAN);
    uint64_t lwj = begin(QXK24_FLOW_LWJ, QXK24_FLOW_OASS_PUSINGAN,
                         QXK24_DIR_BELAKANG, QXK24_DIR_KIRI);
    QXK24ABATracker t;
    ASSERT_EQ(qxk24_oass_aba_begin(ctx, ikj, &t), QXK24_OK);
    ASSERT_EQ(qxk24_oass_aba_complete(ctx, &t, lwj), QXK24_OK);
    EXPECT_GT(qxk24_oass_x2_score(ctx), 0.0);
    EXPECT_LT(qxk24_oass_x2_score(ctx), 1.0);
}
