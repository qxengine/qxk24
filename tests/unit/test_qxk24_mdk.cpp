/* ============================================================
 * QXK24 – Universal Kernel
 * tests/unit/test_qxk24_mdk.cpp
 *
 * Masa Dikawal — Unit Tests
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * ============================================================ */

#include "qxk24/fm/qxk24_mdk.h"
#include <gtest/gtest.h>

struct MDKPair {
    qxk24_mp_t mp;
    qxk24_mdk_t mdk;
};

static MDKPair make_mdk(void) {
    MDKPair pair{qxk24_mp_create_mpi(), nullptr};
    EXPECT_NE(pair.mp, nullptr);
    pair.mdk = qxk24_mdk_create(pair.mp);
    EXPECT_NE(pair.mdk, nullptr);
    return pair;
}

static void destroy_mdk(MDKPair pair) {
    qxk24_mdk_destroy(pair.mdk);
    qxk24_mp_destroy(pair.mp);
}

TEST(QXK24MDK_Create, NullParentReturnsNull) {
    EXPECT_EQ(qxk24_mdk_create(nullptr), nullptr);
}
TEST(QXK24MDK_Create, ValidParentSucceeds) {
    MDKPair pair = make_mdk();
    destroy_mdk(pair);
}
TEST(QXK24MDK_Start, StartSucceeds) {
    MDKPair pair = make_mdk();
    EXPECT_EQ(qxk24_mdk_start(pair.mdk), QXK24_OK);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Start, ActiveCountIsSix) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    EXPECT_EQ(qxk24_mdk_active_count(pair.mdk), 6U);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Set, ActiveCountCopied) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    QXK24MDKSet set{};
    qxk24_mdk_set(pair.mdk, &set);
    EXPECT_EQ(set.active_count, 6U);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Set, AverageValueStartsHalf) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    QXK24MDKSet set{};
    qxk24_mdk_set(pair.mdk, &set);
    EXPECT_NEAR(set.average_value, 0.5f, 0.001f);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Record, IndexZeroCopied) {
    MDKPair pair = make_mdk();
    QXK24MDKRecord record{};
    qxk24_mdk_record(pair.mdk, 0, &record);
    EXPECT_EQ(record.index, 0U);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Record, DirectionZeroIsAs) {
    MDKPair pair = make_mdk();
    QXK24MDKRecord record{};
    qxk24_mdk_record(pair.mdk, 0, &record);
    EXPECT_EQ(record.direction, QXK24_DIR_AS);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Record, DirectionFiveIsKr) {
    MDKPair pair = make_mdk();
    QXK24MDKRecord record{};
    qxk24_mdk_record(pair.mdk, 5, &record);
    EXPECT_EQ(record.direction, QXK24_DIR_KR);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Record, InvalidIndexReturnsError) {
    MDKPair pair = make_mdk();
    QXK24MDKRecord record{};
    EXPECT_EQ(qxk24_mdk_record(pair.mdk, 6, &record),
              QXK24_ERR_INVALID_ARGUMENT);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Record, NullOutReturnsError) {
    MDKPair pair = make_mdk();
    EXPECT_EQ(qxk24_mdk_record(pair.mdk, 0, nullptr), QXK24_ERR_NULL_PARAM);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Null, StartNullReturnsError) {
    EXPECT_EQ(qxk24_mdk_start(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24MDK_Null, CycleNullReturnsError) {
    EXPECT_EQ(qxk24_mdk_cycle(nullptr, 1.0f), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24MDK_Cycle, BeforeStartReturnsError) {
    MDKPair pair = make_mdk();
    EXPECT_EQ(qxk24_mdk_cycle(pair.mdk, 1.0f), QXK24_ERR_NOT_STARTED);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Cycle, StoresEnergy) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_cycle(pair.mdk, 0.75f);
    QXK24MDKRecord record{};
    qxk24_mdk_record(pair.mdk, 0, &record);
    EXPECT_NEAR(record.energy, 0.75f, 0.001f);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Cycle, IncrementsCycles) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_cycle(pair.mdk, 0.75f);
    QXK24MDKRecord record{};
    qxk24_mdk_record(pair.mdk, 0, &record);
    EXPECT_EQ(record.cycles, 1ULL);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Cycle, AverageEnergyCopied) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_cycle(pair.mdk, 0.75f);
    QXK24MDKSet set{};
    qxk24_mdk_set(pair.mdk, &set);
    EXPECT_NEAR(set.average_energy, 0.75f, 0.001f);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Cycle, WaveStateAvailable) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_cycle(pair.mdk, 0.75f);
    EXPECT_NE(qxk24_mdk_wave_state(pair.mdk, 0), QXK24_MDK_WAVE_TROUGH);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Cycle, ValueStaysInRange) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    for (int i = 0; i < 16; i++) qxk24_mdk_cycle(pair.mdk, 1.0f);
    QXK24MDKRecord record{};
    qxk24_mdk_record(pair.mdk, 0, &record);
    EXPECT_GE(record.value, 0.0f);
    EXPECT_LE(record.value, 1.0f);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Stop, ActiveCountBecomesZero) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_stop(pair.mdk);
    EXPECT_EQ(qxk24_mdk_active_count(pair.mdk), 0U);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Promote, BeforeCycleReturnsNull) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    EXPECT_EQ(qxk24_mdk_promote_to_mpp(pair.mdk, 0), nullptr);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Promote, HighEnergySucceeds) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_cycle(pair.mdk, 0.75f);
    qxk24_mp_t mpp = qxk24_mdk_promote_to_mpp(pair.mdk, 0);
    EXPECT_NE(mpp, nullptr);
    qxk24_mp_destroy(mpp);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Promote, PromotedTypeIsMPP) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_cycle(pair.mdk, 0.75f);
    qxk24_mp_t mpp = qxk24_mdk_promote_to_mpp(pair.mdk, 0);
    ASSERT_NE(mpp, nullptr);
    EXPECT_EQ(qxk24_mp_type(mpp), QXK24_MP_PROSES);
    qxk24_mp_destroy(mpp);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Promote, InvalidIndexReturnsNull) {
    MDKPair pair = make_mdk();
    EXPECT_EQ(qxk24_mdk_promote_to_mpp(pair.mdk, 6), nullptr);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Null, ActiveCountNullIsZero) {
    EXPECT_EQ(qxk24_mdk_active_count(nullptr), 0U);
}
TEST(QXK24MDK_Null, InvalidWaveStateIsTrough) {
    EXPECT_EQ(qxk24_mdk_wave_state(nullptr, 0), QXK24_MDK_WAVE_TROUGH);
}
TEST(QXK24MDK_Set, NullOutReturnsError) {
    MDKPair pair = make_mdk();
    EXPECT_EQ(qxk24_mdk_set(pair.mdk, nullptr), QXK24_ERR_NULL_PARAM);
    destroy_mdk(pair);
}
TEST(QXK24MDK_Cycle, EnergyClampsToOne) {
    MDKPair pair = make_mdk();
    qxk24_mdk_start(pair.mdk);
    qxk24_mdk_cycle(pair.mdk, 2.0f);
    QXK24MDKRecord record{};
    qxk24_mdk_record(pair.mdk, 0, &record);
    EXPECT_NEAR(record.energy, 1.0f, 0.001f);
    destroy_mdk(pair);
}
