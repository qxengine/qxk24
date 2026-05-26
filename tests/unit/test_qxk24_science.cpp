/* ============================================================
 * test_qxk24_science.cpp
 * QXK24 Universal Kernel — ALAMTOLOGI Science Engine Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_science.cpp
 * License : Apache 2.0
 *
 * Description:
 * Tests Phase 11 HISAL, RINA, IZWA, SIRA, and META-FIZIK
 * engines across 100 constitutional science proofs.
 * ============================================================ */

#include <gtest/gtest.h>
#include "qxk24/science/qxk24_hisal.h"
#include "qxk24/science/qxk24_izwa.h"
#include "qxk24/science/qxk24_meta.h"
#include "qxk24/science/qxk24_rina.h"
#include "qxk24/science/qxk24_sira.h"

class HisalTest : public ::testing::Test {
protected:
    qxk24_hisal_t h = nullptr;
    void SetUp() override { h = qxk24_hisal_create(); }
    void TearDown() override { qxk24_hisal_destroy(h); }
};

TEST_F(HisalTest, CreateNotNull) { EXPECT_NE(h, nullptr); }
TEST_F(HisalTest, AidilAddResultIsOne) { qxk24_hisal_result_t r; ASSERT_EQ(QXK24_OK, qxk24_hisal_add(h, 3, 5, &r)); EXPECT_EQ(r.aidil_result, 1u); }
TEST_F(HisalTest, AidilSubResultIsTwo) { qxk24_hisal_result_t r; ASSERT_EQ(QXK24_OK, qxk24_hisal_sub(h, 5, 3, &r)); EXPECT_EQ(r.aidil_result, 2u); }
TEST_F(HisalTest, AidilAddAlwaysOneRegardlessOfInput) { qxk24_hisal_result_t r; qxk24_hisal_add(h, 100, 200, &r); EXPECT_EQ(r.aidil_result, 1u); }
TEST_F(HisalTest, AidilSubAlwaysTwoRegardlessOfInput) { qxk24_hisal_result_t r; qxk24_hisal_sub(h, 1000, 1, &r); EXPECT_EQ(r.aidil_result, 2u); }
TEST_F(HisalTest, TajuMinIsOne) { EXPECT_EQ(QXK24_HISAL_TAJU_MIN, 1u); }
TEST_F(HisalTest, TajuMaxIsSeven) { EXPECT_EQ(QXK24_HISAL_TAJU_MAX, 7u); }
TEST_F(HisalTest, TajuCountIsSeven) { EXPECT_EQ(QXK24_HISAL_TAJU_COUNT, 7u); }
TEST_F(HisalTest, IsTajuValidRange) { for (uint32_t i = 1; i <= 7; i++) EXPECT_EQ(qxk24_hisal_is_taju(i), 1); }
TEST_F(HisalTest, IsTajuInvalidRange) { EXPECT_EQ(qxk24_hisal_is_taju(0), 0); EXPECT_EQ(qxk24_hisal_is_taju(8), 0); }
TEST_F(HisalTest, ToTajuValueInRange) { EXPECT_EQ(qxk24_hisal_to_taju(h, 5), 5u); }
TEST_F(HisalTest, ToTajuValueOverflowReturnsOneToSeven) { uint32_t t = qxk24_hisal_to_taju(h, 100); EXPECT_GE(t, 1u); EXPECT_LE(t, 7u); }
TEST_F(HisalTest, GandaGridLevelOne) { qxk24_ganda_grid_t g; ASSERT_EQ(QXK24_OK, qxk24_hisal_ganda_grid(1, &g)); EXPECT_EQ(g.grid_size, 2u); EXPECT_EQ(g.total_boxes, 4u); }
TEST_F(HisalTest, GandaGridLevelSeven) { qxk24_ganda_grid_t g; ASSERT_EQ(QXK24_OK, qxk24_hisal_ganda_grid(7, &g)); EXPECT_EQ(g.total_boxes, 49u); }
TEST_F(HisalTest, GandaMaxBoxesIs49) { EXPECT_EQ(QXK24_HISAL_GANDA_MAX_BOXES, 49u); }
TEST_F(HisalTest, AsasActiveSurfacesIsFour) { EXPECT_EQ(QXK24_HISAL_ASAS_ACTIVE_SURFACES, 4u); }
TEST_F(HisalTest, AsasPassiveAxesIsTwo) { EXPECT_EQ(QXK24_HISAL_ASAS_PASSIVE_AXES, 2u); }
TEST_F(HisalTest, AsasBoxesLevelOne) { EXPECT_EQ(qxk24_hisal_asas_boxes(1), 1u); }
TEST_F(HisalTest, AsasBoxesLevelTwo) { EXPECT_EQ(qxk24_hisal_asas_boxes(2), 4u); }
TEST_F(HisalTest, AsasBoxesLevelThree) { EXPECT_EQ(qxk24_hisal_asas_boxes(3), 16u); }
TEST_F(HisalTest, TajuNameSa) { EXPECT_STREQ(qxk24_hisal_taju_name(1), "sa"); }
TEST_F(HisalTest, TajuNameTu) { EXPECT_STREQ(qxk24_hisal_taju_name(7), "tu"); }
TEST_F(HisalTest, ProcessLeraiFillsParts) { uint32_t parts[10]; uint32_t count = 0; ASSERT_EQ(QXK24_OK, qxk24_hisal_process_lerai(h, 4, parts, &count)); EXPECT_GE(count, 1u); }
TEST_F(HisalTest, ProcessGabungReturnsOne) { uint32_t parts[] = {2, 2}; uint32_t result = 0; ASSERT_EQ(QXK24_OK, qxk24_hisal_process_gabung(h, parts, 2, &result)); EXPECT_EQ(result, 1u); }
TEST_F(HisalTest, SnapshotHealthInitial) { qxk24_hisal_snapshot_t s = qxk24_hisal_snapshot(h); EXPECT_GT(s.health, 0.0f); }

class RinaTest : public ::testing::Test {
protected:
    qxk24_rina_t r = nullptr;
    void SetUp() override { r = qxk24_rina_create(); }
    void TearDown() override { qxk24_rina_destroy(r); }
};

TEST_F(RinaTest, CreateNotNull) { EXPECT_NE(r, nullptr); }
TEST_F(RinaTest, ActivePassiveRatioIsSix) { EXPECT_EQ(QXK24_RINA_ACTIVE_PASSIVE_RATIO, 6); }
TEST_F(RinaTest, PassiveEnergyIsOne) { EXPECT_EQ(QXK24_RINA_PASSIVE_ENERGY, 1); }
TEST_F(RinaTest, DayaTenagaRatioIsTwo) { EXPECT_EQ(QXK24_RINA_DAYA_TENAGA_RATIO, 2); }
TEST_F(RinaTest, ForceFormulaCorrect) { qxk24_rina_force_t f; ASSERT_EQ(QXK24_OK, qxk24_rina_compute_force(r, 2.0f, 3.0f, 1.0f, &f)); EXPECT_FLOAT_EQ(f.daya, 8.0f); }
TEST_F(RinaTest, ForceMinTenagaIsHalfDaya) { qxk24_rina_force_t f; qxk24_rina_compute_force(r, 2.0f, 3.0f, 1.0f, &f); EXPECT_FLOAT_EQ(f.tenaga_min, 4.0f); }
TEST_F(RinaTest, LoadVerticalIncludesBt) { qxk24_rina_load_t l; ASSERT_EQ(QXK24_OK, qxk24_rina_compute_load(QXK24_RINA_POSITION_VERTICAL, 10.0f, 5.0f, &l)); EXPECT_FLOAT_EQ(l.nb, 15.0f); }
TEST_F(RinaTest, LoadHorizontalBaOnly) { qxk24_rina_load_t l; ASSERT_EQ(QXK24_OK, qxk24_rina_compute_load(QXK24_RINA_POSITION_HORIZONTAL, 10.0f, 5.0f, &l)); EXPECT_FLOAT_EQ(l.nb, 10.0f); }
TEST_F(RinaTest, GradientComputation) { qxk24_rina_gradient_t g; ASSERT_EQ(QXK24_OK, qxk24_rina_compute_gradient(6.0f, 3.0f, QXK24_RINA_GRADIENT_RISING, &g)); EXPECT_FLOAT_EQ(g.c, 2.0f); }
TEST_F(RinaTest, EnergySplitActiveIs6Parts) { qxk24_rina_energy_split_t e; ASSERT_EQ(QXK24_OK, qxk24_rina_split_energy(r, 7.0f, &e)); EXPECT_FLOAT_EQ(e.active_energy, 6.0f); EXPECT_FLOAT_EQ(e.passive_energy, 1.0f); }
TEST_F(RinaTest, EnergySplitRatioActive) { qxk24_rina_energy_split_t e; qxk24_rina_split_energy(r, 7.0f, &e); EXPECT_FLOAT_EQ(e.ratio_active, 6.0f); }
TEST_F(RinaTest, EnergySplitRatioPassive) { qxk24_rina_energy_split_t e; qxk24_rina_split_energy(r, 7.0f, &e); EXPECT_FLOAT_EQ(e.ratio_passive, 1.0f); }
TEST_F(RinaTest, MinTenagaIsHalfDaya) { EXPECT_FLOAT_EQ(qxk24_rina_min_tenaga(10.0f), 5.0f); }
TEST_F(RinaTest, MetaFizikMetaIsActive) { qxk24_rina_meta_fizik_t mf; qxk24_rina_meta_fizik(3.0f, 3.0f, &mf); EXPECT_EQ(mf.meta_active, 1); }
TEST_F(RinaTest, MetaFizikFizikIsPassive) { qxk24_rina_meta_fizik_t mf; qxk24_rina_meta_fizik(3.0f, 3.0f, &mf); EXPECT_EQ(mf.fizik_passive, 1); }
TEST_F(RinaTest, MetaFizikCompatibleWhenSamePattern) { qxk24_rina_meta_fizik_t mf; qxk24_rina_meta_fizik(3.0f, 3.0f, &mf); EXPECT_FLOAT_EQ(mf.compatibility, 1.0f); }
TEST_F(RinaTest, MetaFizikRejectedWhenDifferent) { qxk24_rina_meta_fizik_t mf; qxk24_rina_meta_fizik(1.0f, 5.0f, &mf); EXPECT_LT(mf.compatibility, 0.5f); }
TEST_F(RinaTest, DistanceLevelsIsSix) { EXPECT_EQ(QXK24_RINA_DISTANCE_LEVELS, 6); }
TEST_F(RinaTest, SnapshotHealthInitial) { qxk24_rina_snapshot_t s = qxk24_rina_snapshot(r); EXPECT_GT(s.health, 0.0f); }
TEST_F(RinaTest, ForceZeroNbIsZero) { qxk24_rina_force_t f; qxk24_rina_compute_force(r, 0.0f, 3.0f, 1.0f, &f); EXPECT_FLOAT_EQ(f.daya, 0.0f); }
TEST_F(RinaTest, ForceIsValid) { qxk24_rina_force_t f; qxk24_rina_compute_force(r, 2.0f, 1.0f, 0.5f, &f); EXPECT_EQ(f.is_valid, 1); }
TEST_F(RinaTest, ForceCountIncrementsOnCompute) { qxk24_rina_force_t f; qxk24_rina_compute_force(r, 1.0f, 1.0f, 1.0f, &f); qxk24_rina_compute_force(r, 2.0f, 2.0f, 2.0f, &f); EXPECT_EQ(qxk24_rina_snapshot(r).force_calculations, 2u); }
TEST_F(RinaTest, GradientRisingDirection) { qxk24_rina_gradient_t g; qxk24_rina_compute_gradient(3.0f, 3.0f, QXK24_RINA_GRADIENT_RISING, &g); EXPECT_EQ(g.direction, QXK24_RINA_GRADIENT_RISING); }
TEST_F(RinaTest, GradientFallingDirection) { qxk24_rina_gradient_t g; qxk24_rina_compute_gradient(3.0f, 3.0f, QXK24_RINA_GRADIENT_FALLING, &g); EXPECT_EQ(g.direction, QXK24_RINA_GRADIENT_FALLING); }
TEST_F(RinaTest, NullForceReturnsError) { EXPECT_NE(QXK24_OK, qxk24_rina_compute_force(r, 1.0f, 1.0f, 1.0f, nullptr)); }

class IzwaTest : public ::testing::Test {
protected:
    qxk24_izwa_t z = nullptr;
    void SetUp() override { z = qxk24_izwa_create(); }
    void TearDown() override { qxk24_izwa_destroy(z); }
};

TEST_F(IzwaTest, CreateNotNull) { EXPECT_NE(z, nullptr); }
TEST_F(IzwaTest, MikroPerMakroIsSix) { EXPECT_EQ(QXK24_IZWA_MIKRO_PER_MAKRO, 6u); }
TEST_F(IzwaTest, MikroSaTotalIs36) { EXPECT_EQ(QXK24_IZWA_MIKRO_SA_TOTAL, 36u); }
TEST_F(IzwaTest, DominantAIs15) { EXPECT_EQ(QXK24_IZWA_MIKRO_SA_DOMINANT_A, 15u); }
TEST_F(IzwaTest, DominantBIs15) { EXPECT_EQ(QXK24_IZWA_MIKRO_SA_DOMINANT_B, 15u); }
TEST_F(IzwaTest, NeutralIs6) { EXPECT_EQ(QXK24_IZWA_MIKRO_SA_NEUTRAL, 6u); }
TEST_F(IzwaTest, ReactProduces36Mikro) { qxk24_izwa_reaction_result_t out; ASSERT_EQ(QXK24_OK, qxk24_izwa_react(z, 1, 2, &out)); EXPECT_EQ(out.total_mikro, 36u); }
TEST_F(IzwaTest, ReactDominantACount) { qxk24_izwa_reaction_result_t out; qxk24_izwa_react(z, 1, 2, &out); EXPECT_EQ(out.dominant_a, 15u); }
TEST_F(IzwaTest, ReactDominantBCount) { qxk24_izwa_reaction_result_t out; qxk24_izwa_react(z, 1, 2, &out); EXPECT_EQ(out.dominant_b, 15u); }
TEST_F(IzwaTest, ReactNeutralCount) { qxk24_izwa_reaction_result_t out; qxk24_izwa_react(z, 1, 2, &out); EXPECT_EQ(out.neutral, 6u); }
TEST_F(IzwaTest, ReactIsValid) { qxk24_izwa_reaction_result_t out; qxk24_izwa_react(z, 1, 2, &out); EXPECT_EQ(out.is_valid, 1); }
TEST_F(IzwaTest, SameGroupAAPincang) { qxk24_izwa_combination_t c; qxk24_izwa_check_combination(z, QXK24_IZWA_MIKRO_CAT_A, QXK24_IZWA_MIKRO_CAT_A, &c); EXPECT_EQ(c.is_allowed, 0); }
TEST_F(IzwaTest, SameGroupBBPincang) { qxk24_izwa_combination_t c; qxk24_izwa_check_combination(z, QXK24_IZWA_MIKRO_CAT_B, QXK24_IZWA_MIKRO_CAT_B, &c); EXPECT_EQ(c.is_allowed, 0); }
TEST_F(IzwaTest, CrossGroupABAllowed) { qxk24_izwa_combination_t c; qxk24_izwa_check_combination(z, QXK24_IZWA_MIKRO_CAT_A, QXK24_IZWA_MIKRO_CAT_B, &c); EXPECT_EQ(c.is_allowed, 1); }
TEST_F(IzwaTest, CrossGroupAAbAllowed) { qxk24_izwa_combination_t c; qxk24_izwa_check_combination(z, QXK24_IZWA_MIKRO_CAT_A, QXK24_IZWA_MIKRO_CAT_AB, &c); EXPECT_EQ(c.is_allowed, 1); }
TEST_F(IzwaTest, ElementNameTanah) { EXPECT_STREQ(qxk24_izwa_element_name(QXK24_IZWA_ELEMENT_TANAH), "Tanah"); }
TEST_F(IzwaTest, MikroDuCount) { EXPECT_EQ(qxk24_izwa_mikro_du_count(), 14850u); }
TEST_F(IzwaTest, ElementsBaseIsFour) { EXPECT_EQ(QXK24_IZWA_ELEMENTS_BASE, 4u); }
TEST_F(IzwaTest, SnapshotAfterReact) { qxk24_izwa_reaction_result_t out; qxk24_izwa_react(z, 1, 2, &out); EXPECT_EQ(qxk24_izwa_snapshot(z).reactions_performed, 1u); }
TEST_F(IzwaTest, PincangDetectedAfterSameGroup) { qxk24_izwa_combination_t c; qxk24_izwa_check_combination(z, QXK24_IZWA_MIKRO_CAT_A, QXK24_IZWA_MIKRO_CAT_A, &c); EXPECT_EQ(qxk24_izwa_snapshot(z).pincang_detected, 1u); }

class SiraTest : public ::testing::Test {
protected:
    qxk24_sira_t s = nullptr;
    void SetUp() override { s = qxk24_sira_create(); }
    void TearDown() override { qxk24_sira_destroy(s); }
};

TEST_F(SiraTest, CreateNotNull) { EXPECT_NE(s, nullptr); }
TEST_F(SiraTest, NuCountSaIsFive) { EXPECT_EQ(qxk24_sira_nu_count(QXK24_SIRA_PPU_SA), 5u); }
TEST_F(SiraTest, NuCountNaIsZero) { EXPECT_EQ(qxk24_sira_nu_count(QXK24_SIRA_PPU_NA), 0u); }
TEST_F(SiraTest, NuCountDecreases) { for (int i = 0; i < (int)QXK24_SIRA_PPU_LEVELS - 1; i++) EXPECT_GT(qxk24_sira_nu_count((qxk24_sira_ppu_t)i), qxk24_sira_nu_count((qxk24_sira_ppu_t)(i + 1))); }
TEST_F(SiraTest, EnergySplitActiveRatio) { float a, p; ASSERT_EQ(QXK24_OK, qxk24_sira_split_energy(7.0f, &a, &p)); EXPECT_FLOAT_EQ(a, 6.0f); EXPECT_FLOAT_EQ(p, 1.0f); }
TEST_F(SiraTest, FormationAllFactorsPerfect) { qxk24_sira_formation_t f; qxk24_sira_check_formation(s, 1, 1, 1, 1, &f); EXPECT_EQ(f.is_perfect, 1); EXPECT_EQ(f.is_pincang, 0); }
TEST_F(SiraTest, FormationMissingNucleusPincang) { qxk24_sira_formation_t f; qxk24_sira_check_formation(s, 0, 1, 1, 1, &f); EXPECT_EQ(f.is_pincang, 1); }
TEST_F(SiraTest, SauModulZarah) { qxk24_sira_sau_structure_t st = qxk24_sira_sau_modul(); EXPECT_EQ(st.k24za, 46656u); }
TEST_F(SiraTest, SauModulJisim) { qxk24_sira_sau_structure_t st = qxk24_sira_sau_modul(); EXPECT_EQ(st.k24ji, 7776u); }
TEST_F(SiraTest, GrowthValidWhenActiveNePassive) { qxk24_sira_growth_t g; qxk24_sira_evaluate_growth(s, QXK24_SIRA_PPU_SA, 6.0f, 1.0f, &g); EXPECT_EQ(g.growth_valid, 1); }
TEST_F(SiraTest, GrowthInvalidWhenActiveEqPassive) { qxk24_sira_growth_t g; qxk24_sira_evaluate_growth(s, QXK24_SIRA_PPU_SA, 5.0f, 5.0f, &g); EXPECT_EQ(g.growth_valid, 0); }
TEST_F(SiraTest, PpuNameSa) { EXPECT_STREQ(qxk24_sira_ppu_name(QXK24_SIRA_PPU_SA), "Sa"); }
TEST_F(SiraTest, ActiveRatioConstant) { EXPECT_EQ(QXK24_SIRA_ACTIVE_RATIO, 6u); }
TEST_F(SiraTest, PassiveRatioConstant) { EXPECT_EQ(QXK24_SIRA_PASSIVE_RATIO, 1u); }
TEST_F(SiraTest, FormationFactorsCount) { EXPECT_EQ(QXK24_SIRA_FORMATION_FACTORS, 4u); }

class MetaTest : public ::testing::Test {
protected:
    qxk24_meta_t m = nullptr;
    void SetUp() override { m = qxk24_meta_create(); }
    void TearDown() override { qxk24_meta_destroy(m); }
};

TEST_F(MetaTest, CreateNotNull) { EXPECT_NE(m, nullptr); }
TEST_F(MetaTest, MetaIsActive) { EXPECT_EQ(QXK24_META_ACTIVE_SIGN, 0); }
TEST_F(MetaTest, FizikIsPassive) { EXPECT_EQ(QXK24_META_PASSIVE_SIGN, 1); }
TEST_F(MetaTest, CompatThreshold) { EXPECT_FLOAT_EQ(QXK24_META_COMPATIBLE_THRESHOLD, 0.80f); }
TEST_F(MetaTest, PartialThreshold) { EXPECT_FLOAT_EQ(QXK24_META_PARTIAL_THRESHOLD, 0.50f); }
TEST_F(MetaTest, FullCompatibleSamePattern) { qxk24_meta_pattern_t meta = {3, 100, 1, 5.0f}; qxk24_meta_pattern_t fizik = {3, 100, 0, 5.0f}; qxk24_meta_check_t c; qxk24_meta_check(m, &meta, &fizik, &c); EXPECT_EQ(c.result, QXK24_META_COMPAT_FULL); }
TEST_F(MetaTest, RejectedDifferentPattern) { qxk24_meta_pattern_t meta = {1, 10, 1, 1.0f}; qxk24_meta_pattern_t fizik = {7, 99, 0, 7.0f}; qxk24_meta_check_t c; qxk24_meta_check(m, &meta, &fizik, &c); EXPECT_EQ(c.result, QXK24_META_COMPAT_REJECTED); }
TEST_F(MetaTest, ValidatePairMetaActive) { qxk24_meta_pattern_t meta = {1, 1, 1, 1.0f}; qxk24_meta_pattern_t fizik = {1, 1, 0, 1.0f}; EXPECT_EQ(qxk24_meta_validate_pair(&meta, &fizik), 1); }
TEST_F(MetaTest, ValidatePairBothActiveInvalid) { qxk24_meta_pattern_t a = {1, 1, 1, 1.0f}; qxk24_meta_pattern_t b = {1, 1, 1, 1.0f}; EXPECT_EQ(qxk24_meta_validate_pair(&a, &b), 0); }
TEST_F(MetaTest, ScoreRangeZeroToOne) { qxk24_meta_pattern_t a = {3, 100, 1, 5.0f}; qxk24_meta_pattern_t b = {3, 100, 0, 5.0f}; float score = qxk24_meta_score(&a, &b); EXPECT_GE(score, 0.0f); EXPECT_LE(score, 1.0f); }
TEST_F(MetaTest, CompatNameFull) { EXPECT_STREQ(qxk24_meta_compat_name(QXK24_META_COMPAT_FULL), "FULL"); }
TEST_F(MetaTest, CompatNameRejected) { EXPECT_STREQ(qxk24_meta_compat_name(QXK24_META_COMPAT_REJECTED), "REJECTED"); }
TEST_F(MetaTest, SnapshotAfterCheck) { qxk24_meta_pattern_t meta = {3, 100, 1, 5.0f}; qxk24_meta_pattern_t fizik = {3, 100, 0, 5.0f}; qxk24_meta_check_t c; qxk24_meta_check(m, &meta, &fizik, &c); EXPECT_EQ(qxk24_meta_snapshot(m).checks_performed, 1u); }
TEST_F(MetaTest, MaxPatternLevelIsSeven) { EXPECT_EQ(QXK24_META_MAX_PATTERN_LEVEL, 7u); }
TEST_F(MetaTest, NullCheckReturnsError) { EXPECT_NE(QXK24_OK, qxk24_meta_check(m, nullptr, nullptr, nullptr)); }
