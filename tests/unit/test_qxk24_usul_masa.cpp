/* ============================================================
 * QXK24 – Universal Kernel
 * tests/unit/test_qxk24_usul_masa.cpp
 *
 * Usul Masa — Unit Tests
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * ============================================================ */

#include "qxk24/fm/qxk24_usul_masa.h"
#include <gtest/gtest.h>

struct UsulParts {
    qxk24_mp_t mp;
    qxk24_mdk_t mdk;
    qxk24_xam_t xam;
    qxk24_usul_t usul;
};

static UsulParts make_usul(void) {
    UsulParts parts{qxk24_mp_create_mpi(), nullptr, nullptr, nullptr};
    EXPECT_NE(parts.mp, nullptr);
    parts.mdk = qxk24_mdk_create(parts.mp);
    EXPECT_NE(parts.mdk, nullptr);
    parts.xam = qxk24_xam_create();
    EXPECT_NE(parts.xam, nullptr);
    parts.usul = qxk24_usul_create(parts.mp, parts.mdk, parts.xam);
    EXPECT_NE(parts.usul, nullptr);
    return parts;
}

static void destroy_usul(UsulParts parts) {
    qxk24_usul_destroy(parts.usul);
    qxk24_xam_destroy(parts.xam);
    qxk24_mdk_destroy(parts.mdk);
    qxk24_mp_destroy(parts.mp);
}

TEST(QXK24Usul_Create, ValidInputsSucceed) {
    UsulParts parts = make_usul();
    destroy_usul(parts);
}
TEST(QXK24Usul_Create, NullMPReturnsNull) {
    qxk24_xam_t xam = qxk24_xam_create();
    EXPECT_EQ(qxk24_usul_create(nullptr, nullptr, xam), nullptr);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Usul_Create, NullMDKReturnsNull) {
    qxk24_mp_t mp = qxk24_mp_create_mpi();
    qxk24_xam_t xam = qxk24_xam_create();
    EXPECT_EQ(qxk24_usul_create(mp, nullptr, xam), nullptr);
    qxk24_xam_destroy(xam);
    qxk24_mp_destroy(mp);
}
TEST(QXK24Usul_Create, NullXamReturnsNull) {
    qxk24_mp_t mp = qxk24_mp_create_mpi();
    qxk24_mdk_t mdk = qxk24_mdk_create(mp);
    EXPECT_EQ(qxk24_usul_create(mp, mdk, nullptr), nullptr);
    qxk24_mdk_destroy(mdk);
    qxk24_mp_destroy(mp);
}
TEST(QXK24Usul_Start, NullReturnsError) {
    EXPECT_EQ(qxk24_usul_start(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Usul_Start, Succeeds) {
    UsulParts parts = make_usul();
    EXPECT_EQ(qxk24_usul_start(parts.usul), QXK24_OK);
    destroy_usul(parts);
}
TEST(QXK24Usul_Record, UmmActiveAfterStart) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    QXK24UsulMasaRecord record{};
    qxk24_usul_record(parts.usul, &record);
    EXPECT_TRUE(record.umm_active);
    destroy_usul(parts);
}
TEST(QXK24Usul_Record, UmkActiveCountIsSix) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    QXK24UsulMasaRecord record{};
    qxk24_usul_record(parts.usul, &record);
    EXPECT_EQ(record.umk_active_count, 6U);
    destroy_usul(parts);
}
TEST(QXK24Usul_Record, UmmValueIsOne) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    QXK24UsulMasaRecord record{};
    qxk24_usul_record(parts.usul, &record);
    EXPECT_NEAR(record.umm_value, 1.0f, 0.001f);
    destroy_usul(parts);
}
TEST(QXK24Usul_Record, StartScoreIsZero) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    EXPECT_NEAR(qxk24_usul_constitutional_score(parts.usul), 0.0f, 0.001f);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, NullReturnsError) {
    EXPECT_EQ(qxk24_usul_cycle(nullptr, 1.0f), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Usul_Cycle, BeforeStartReturnsError) {
    UsulParts parts = make_usul();
    EXPECT_EQ(qxk24_usul_cycle(parts.usul, 1.0f), QXK24_ERR_NOT_STARTED);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, Succeeds) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    EXPECT_EQ(qxk24_usul_cycle(parts.usul, 0.75f), QXK24_OK);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, TotalCyclesIncrements) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    QXK24UsulMasaRecord record{};
    qxk24_usul_record(parts.usul, &record);
    EXPECT_EQ(record.total_cycles, 1ULL);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, ScoreFollowsEnergy) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    EXPECT_NEAR(qxk24_usul_constitutional_score(parts.usul), 0.75f, 0.001f);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, TotalEnergyIsSixTimesEnergy) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    QXK24UsulMasaRecord record{};
    qxk24_usul_record(parts.usul, &record);
    EXPECT_NEAR(record.total_energy, 4.5f, 0.001f);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, UmkZeroEnergyCopied) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    EXPECT_NEAR(qxk24_usul_umk_energy(parts.usul, 0), 0.75f, 0.001f);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, UmkFiveEnergyCopied) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    EXPECT_NEAR(qxk24_usul_umk_energy(parts.usul, 5), 0.75f, 0.001f);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, UmkCyclesIncrement) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    QXK24UsulMasaRecord record{};
    qxk24_usul_record(parts.usul, &record);
    EXPECT_EQ(record.umk[0].cycles, 1ULL);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, MovesAhliMasa) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    QXK24XamRecord record{};
    qxk24_xam_record(parts.xam, &record);
    EXPECT_EQ(record.coords_passed, 1U);
    destroy_usul(parts);
}
TEST(QXK24Usul_Cycle, UpdatesMDKEnergy) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_cycle(parts.usul, 0.75f);
    QXK24MDKRecord record{};
    qxk24_mdk_record(parts.mdk, 0, &record);
    EXPECT_NEAR(record.energy, 0.75f, 0.001f);
    destroy_usul(parts);
}
TEST(QXK24Usul_Stop, ClearsUmmActive) {
    UsulParts parts = make_usul();
    qxk24_usul_start(parts.usul);
    qxk24_usul_stop(parts.usul);
    QXK24UsulMasaRecord record{};
    qxk24_usul_record(parts.usul, &record);
    EXPECT_FALSE(record.umm_active);
    destroy_usul(parts);
}
TEST(QXK24Usul_Record, NullOutReturnsError) {
    UsulParts parts = make_usul();
    EXPECT_EQ(qxk24_usul_record(parts.usul, nullptr), QXK24_ERR_NULL_PARAM);
    destroy_usul(parts);
}
TEST(QXK24Usul_Energy, InvalidUmkReturnsZero) {
    UsulParts parts = make_usul();
    EXPECT_NEAR(qxk24_usul_umk_energy(parts.usul, 6), 0.0f, 0.001f);
    destroy_usul(parts);
}
