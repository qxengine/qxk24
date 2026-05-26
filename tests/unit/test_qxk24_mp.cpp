/* ============================================================
 * QXK24 – Universal Kernel
 * tests/unit/test_qxk24_mp.cpp
 *
 * Masa Pengawal — Unit Tests
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * ============================================================ */

#include "qxk24/fm/qxk24_mp.h"
#include <gtest/gtest.h>

static qxk24_mp_t make_mpi(void) {
    qxk24_mp_t mp = qxk24_mp_create_mpi();
    EXPECT_NE(mp, nullptr);
    return mp;
}

TEST(QXK24MP_Create, MPiNotNull) {
    qxk24_mp_t mp = qxk24_mp_create_mpi();
    EXPECT_NE(mp, nullptr);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Create, MPiTypeIsInduk) {
    qxk24_mp_t mp = make_mpi();
    EXPECT_EQ(qxk24_mp_type(mp), QXK24_MP_INDUK);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Create, InitiallyInactive) {
    qxk24_mp_t mp = make_mpi();
    EXPECT_FALSE(qxk24_mp_is_active(mp));
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Create, InitialRatioIsOne) {
    qxk24_mp_t mp = make_mpi();
    EXPECT_NEAR(qxk24_mp_ratio(mp), 1.0f, 0.001f);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Create, InitialElapsedIsZero) {
    qxk24_mp_t mp = make_mpi();
    EXPECT_EQ(qxk24_mp_elapsed_ms(mp), 0ULL);
    qxk24_mp_destroy(mp);
}

TEST(QXK24MP_Lifecycle, StartSucceeds) {
    qxk24_mp_t mp = make_mpi();
    EXPECT_EQ(qxk24_mp_start(mp), QXK24_OK);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Lifecycle, ActiveAfterStart) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    EXPECT_TRUE(qxk24_mp_is_active(mp));
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Lifecycle, CycleSucceeds) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    EXPECT_EQ(qxk24_mp_cycle(mp), QXK24_OK);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Lifecycle, CycleAdvancesElapsed) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    qxk24_mp_cycle(mp);
    EXPECT_GT(qxk24_mp_elapsed_ms(mp), 0ULL);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Lifecycle, StopClearsActive) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    qxk24_mp_stop(mp);
    EXPECT_FALSE(qxk24_mp_is_active(mp));
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Lifecycle, RestartSucceeds) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    qxk24_mp_stop(mp);
    EXPECT_EQ(qxk24_mp_start(mp), QXK24_OK);
    EXPECT_TRUE(qxk24_mp_is_active(mp));
    qxk24_mp_destroy(mp);
}

TEST(QXK24MP_Record, CopiesType) {
    qxk24_mp_t mp = make_mpi();
    QXK24MPRecord record{};
    qxk24_mp_record(mp, &record);
    EXPECT_EQ(record.type, QXK24_MP_INDUK);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Record, CopiesActive) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    QXK24MPRecord record{};
    qxk24_mp_record(mp, &record);
    EXPECT_TRUE(record.active);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Record, CopiesMDKCount) {
    qxk24_mp_t mp = make_mpi();
    QXK24MPRecord record{};
    qxk24_mp_record(mp, &record);
    EXPECT_EQ(record.mdk_count, QXK24_FM_MDK_COUNT);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Record, CopiesName) {
    qxk24_mp_t mp = make_mpi();
    QXK24MPRecord record{};
    qxk24_mp_record(mp, &record);
    EXPECT_STREQ(record.name, "MPi");
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Record, StartTimestampSetAfterStart) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    QXK24MPRecord record{};
    qxk24_mp_record(mp, &record);
    EXPECT_GT(record.start_ms, 0ULL);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Record, RatioIsOne) {
    qxk24_mp_t mp = make_mpi();
    QXK24MPRecord record{};
    qxk24_mp_record(mp, &record);
    EXPECT_NEAR(record.ratio, 1.0f, 0.001f);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Record, ElapsedAfterCycle) {
    qxk24_mp_t mp = make_mpi();
    qxk24_mp_start(mp);
    qxk24_mp_cycle(mp);
    QXK24MPRecord record{};
    qxk24_mp_record(mp, &record);
    EXPECT_GT(record.elapsed_ms, 0ULL);
    qxk24_mp_destroy(mp);
}

TEST(QXK24MP_MPP, CreateWithParentSucceeds) {
    qxk24_mp_t parent = make_mpi();
    qxk24_mp_t child = qxk24_mp_create_mpp(parent, "worker");
    EXPECT_NE(child, nullptr);
    qxk24_mp_destroy(child);
    qxk24_mp_destroy(parent);
}
TEST(QXK24MP_MPP, TypeIsProses) {
    qxk24_mp_t parent = make_mpi();
    qxk24_mp_t child = qxk24_mp_create_mpp(parent, "worker");
    EXPECT_EQ(qxk24_mp_type(child), QXK24_MP_PROSES);
    qxk24_mp_destroy(child);
    qxk24_mp_destroy(parent);
}
TEST(QXK24MP_MPP, NameCopied) {
    qxk24_mp_t parent = make_mpi();
    qxk24_mp_t child = qxk24_mp_create_mpp(parent, "worker");
    QXK24MPRecord record{};
    qxk24_mp_record(child, &record);
    EXPECT_STREQ(record.name, "worker");
    qxk24_mp_destroy(child);
    qxk24_mp_destroy(parent);
}
TEST(QXK24MP_MPP, DefaultNameWhenNull) {
    qxk24_mp_t parent = make_mpi();
    qxk24_mp_t child = qxk24_mp_create_mpp(parent, nullptr);
    QXK24MPRecord record{};
    qxk24_mp_record(child, &record);
    EXPECT_STREQ(record.name, "MPP");
    qxk24_mp_destroy(child);
    qxk24_mp_destroy(parent);
}
TEST(QXK24MP_MPP, NullParentReturnsNull) {
    EXPECT_EQ(qxk24_mp_create_mpp(nullptr, "worker"), nullptr);
}

TEST(QXK24MP_Null, StartNullReturnsError) {
    EXPECT_EQ(qxk24_mp_start(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24MP_Null, CycleNullReturnsError) {
    EXPECT_EQ(qxk24_mp_cycle(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24MP_Null, RecordNullMPReturnsError) {
    QXK24MPRecord record{};
    EXPECT_EQ(qxk24_mp_record(nullptr, &record), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24MP_Null, RecordNullOutReturnsError) {
    qxk24_mp_t mp = make_mpi();
    EXPECT_EQ(qxk24_mp_record(mp, nullptr), QXK24_ERR_NULL_PARAM);
    qxk24_mp_destroy(mp);
}
TEST(QXK24MP_Null, RatioNullIsZero) {
    EXPECT_NEAR(qxk24_mp_ratio(nullptr), 0.0f, 0.001f);
}
