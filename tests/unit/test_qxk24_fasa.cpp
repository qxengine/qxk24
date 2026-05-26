/* ============================================================
 * test_qxk24_fasa.cpp
 * QXK24 Universal Kernel — Fasa Tenaga Tests
 * ============================================================ */

#include "qxk24/ft/qxk24_fasa_tenaga.h"
#include <gtest/gtest.h>

class FasaTest : public ::testing::Test {
protected:
    qxk24_kernel_t kernel = nullptr;
    qxk24_ft_t ft = nullptr;
    void SetUp() override {
        QXK24Config cfg = qxk24_default_config();
        ASSERT_EQ(qxk24_create(&cfg, &kernel), QXK24_OK);
        ASSERT_EQ(qxk24_ft_create(kernel, &ft), QXK24_OK);
    }
    void TearDown() override {
        qxk24_ft_destroy(ft);
        qxk24_destroy(kernel);
    }
};

TEST_F(FasaTest, AbaCountsKnown) {
    EXPECT_EQ(QXK24_FASA_ABA_COUNT[0], 1U);
    EXPECT_EQ(QXK24_FASA_ABA_COUNT[6], 231U);
}
TEST_F(FasaTest, LayerCountsKnown) {
    EXPECT_EQ(QXK24_FASA_LAYER_COUNT[0], 1U);
    EXPECT_EQ(QXK24_FASA_LAYER_COUNT[6], 13U);
}
TEST_F(FasaTest, DescriptorPeSa) {
    QXK24FasaTenagaDescriptor d{};
    ASSERT_EQ(qxk24_fasa_descriptor(QXK24_FT_PHASE_PESA, &d), QXK24_OK);
    EXPECT_EQ(d.aba_count, 1U);
}
TEST_F(FasaTest, DescriptorPeTu) {
    QXK24FasaTenagaDescriptor d{};
    ASSERT_EQ(qxk24_fasa_descriptor(QXK24_FT_PHASE_PETU, &d), QXK24_OK);
    EXPECT_EQ(d.layer_count, 13U);
}
TEST_F(FasaTest, DescriptorLayersSplit) {
    QXK24FasaTenagaDescriptor d{};
    qxk24_fasa_descriptor(QXK24_FT_PHASE_PEPA, &d);
    EXPECT_EQ(d.atas_layers, 3U);
    EXPECT_EQ(d.bawah_layers, 3U);
}
TEST_F(FasaTest, CapacityPositive) {
    EXPECT_GT(qxk24_fasa_energy_capacity(QXK24_FT_PHASE_PEGA), 0.0f);
}
TEST_F(FasaTest, CapacityIncreases) {
    EXPECT_GT(qxk24_fasa_energy_capacity(QXK24_FT_PHASE_PETU),
              qxk24_fasa_energy_capacity(QXK24_FT_PHASE_PESA));
}
TEST_F(FasaTest, CreateSucceeds) {
    qxk24_fasa_t f = nullptr;
    EXPECT_EQ(qxk24_fasa_create(ft, &f), QXK24_OK);
    qxk24_fasa_destroy(f);
}
TEST_F(FasaTest, InitialPhasePeSa) {
    qxk24_fasa_t f = nullptr;
    qxk24_fasa_create(ft, &f);
    QXK24FasaSnapshot s{};
    qxk24_fasa_snapshot(f, &s);
    EXPECT_EQ(s.current_phase, QXK24_FT_PHASE_PESA);
    qxk24_fasa_destroy(f);
}
TEST_F(FasaTest, InitialRemainingEqualsCapacity) {
    qxk24_fasa_t f = nullptr;
    qxk24_fasa_create(ft, &f);
    QXK24FasaSnapshot s{};
    qxk24_fasa_snapshot(f, &s);
    EXPECT_FLOAT_EQ(s.energy_remaining, s.energy_capacity);
    qxk24_fasa_destroy(f);
}
TEST_F(FasaTest, CycleUsesEnergy) {
    qxk24_fasa_t f = nullptr;
    qxk24_fasa_create(ft, &f);
    qxk24_fasa_cycle(f);
    QXK24FasaSnapshot s{};
    qxk24_fasa_snapshot(f, &s);
    EXPECT_GT(s.energy_used, 0.0f);
    qxk24_fasa_destroy(f);
}
TEST_F(FasaTest, CycleChangesFlow) {
    qxk24_fasa_t f = nullptr;
    qxk24_fasa_create(ft, &f);
    qxk24_fasa_cycle(f);
    QXK24FasaSnapshot s{};
    qxk24_fasa_snapshot(f, &s);
    EXPECT_EQ(s.active_flow, QXK24_FASA_FLOW_Y_POS);
    qxk24_fasa_destroy(f);
}
TEST_F(FasaTest, AdvancePhaseMovesToPeDu) {
    qxk24_fasa_t f = nullptr;
    qxk24_fasa_create(ft, &f);
    qxk24_fasa_advance_phase(f);
    QXK24FasaSnapshot s{};
    qxk24_fasa_snapshot(f, &s);
    EXPECT_EQ(s.current_phase, QXK24_FT_PHASE_PEDU);
    qxk24_fasa_destroy(f);
}
TEST_F(FasaTest, AdvanceUpdatesAba) {
    qxk24_fasa_t f = nullptr;
    qxk24_fasa_create(ft, &f);
    qxk24_fasa_advance_phase(f);
    QXK24FasaSnapshot s{};
    qxk24_fasa_snapshot(f, &s);
    EXPECT_EQ(s.aba_active, 6U);
    qxk24_fasa_destroy(f);
}
TEST_F(FasaTest, PhaseNameDelegates) {
    EXPECT_STREQ(qxk24_fasa_phase_name(QXK24_FT_PHASE_PEMA), "PeMa");
}
TEST_F(FasaTest, InvalidDescriptorFails) {
    QXK24FasaTenagaDescriptor d{};
    EXPECT_EQ(qxk24_fasa_descriptor((QXK24FtPhase)9, &d),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(FasaTest, NullDescriptorFails) {
    EXPECT_EQ(qxk24_fasa_descriptor(QXK24_FT_PHASE_PESA, nullptr),
              QXK24_ERR_NULL_PARAM);
}
TEST_F(FasaTest, NullCreateFails) {
    qxk24_fasa_t f = nullptr;
    EXPECT_EQ(qxk24_fasa_create(nullptr, &f), QXK24_ERR_NULL_PARAM);
}
TEST_F(FasaTest, NullCycleFails) {
    EXPECT_EQ(qxk24_fasa_cycle(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST_F(FasaTest, NullSnapshotFails) {
    EXPECT_EQ(qxk24_fasa_snapshot(nullptr, nullptr), QXK24_ERR_NULL_PARAM);
}
