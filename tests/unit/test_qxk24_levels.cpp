/* ============================================================
 * test_qxk24_levels.cpp
 * QXK24 Universal Kernel — Seven-Level Hierarchy Tests
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_levels.cpp
 * License : Apache 2.0
 *
 * Description:
 * Provides 42 machine proofs for the K24 seven-level hierarchy, six
 * directions, Z.3 direction pairs, and kernel lifecycle contract.
 */

#include "qxk24/qxk24.h"
#include <cstring>
#include <gtest/gtest.h>

TEST(QXK24Levels_Scale, Zarah_Is1) {
    EXPECT_EQ(qxk24_level_scale(QXK24_LEVEL_ZARAH), 1ULL);
}
TEST(QXK24Levels_Scale, Jisim_Is6) {
    EXPECT_EQ(qxk24_level_scale(QXK24_LEVEL_JISIM), 6ULL);
}
TEST(QXK24Levels_Scale, Atom_Is36) {
    EXPECT_EQ(qxk24_level_scale(QXK24_LEVEL_ATOM), 36ULL);
}
TEST(QXK24Levels_Scale, Sistem_Is216) {
    EXPECT_EQ(qxk24_level_scale(QXK24_LEVEL_SISTEM), 216ULL);
}
TEST(QXK24Levels_Scale, Objek_Is1296) {
    EXPECT_EQ(qxk24_level_scale(QXK24_LEVEL_OBJEK), 1296ULL);
}
TEST(QXK24Levels_Scale, Aktif_Is7776) {
    EXPECT_EQ(qxk24_level_scale(QXK24_LEVEL_AKTIF), 7776ULL);
}
TEST(QXK24Levels_Scale, Modul_Is46656) {
    EXPECT_EQ(qxk24_level_scale(QXK24_LEVEL_MODUL), 46656ULL);
}

TEST(QXK24Levels_Total, Zarah_Is1) {
    EXPECT_EQ(qxk24_level_total(QXK24_LEVEL_ZARAH), 1ULL);
}
TEST(QXK24Levels_Total, Jisim_Is7) {
    EXPECT_EQ(qxk24_level_total(QXK24_LEVEL_JISIM), 7ULL);
}
TEST(QXK24Levels_Total, Atom_Is43) {
    EXPECT_EQ(qxk24_level_total(QXK24_LEVEL_ATOM), 43ULL);
}
TEST(QXK24Levels_Total, Sistem_Is259) {
    EXPECT_EQ(qxk24_level_total(QXK24_LEVEL_SISTEM), 259ULL);
}
TEST(QXK24Levels_Total, Objek_Is1555) {
    EXPECT_EQ(qxk24_level_total(QXK24_LEVEL_OBJEK), 1555ULL);
}
TEST(QXK24Levels_Total, Aktif_Is9331) {
    EXPECT_EQ(qxk24_level_total(QXK24_LEVEL_AKTIF), 9331ULL);
}
TEST(QXK24Levels_Total, Modul_Is55987) {
    EXPECT_EQ(qxk24_level_total(QXK24_LEVEL_MODUL), 55987ULL);
}

TEST(QXK24Levels_Budget, Zarah_Is1MiB) {
    EXPECT_EQ(qxk24_level_budget(QXK24_LEVEL_ZARAH),
              1ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24Levels_Budget, Jisim_Is7MiB) {
    EXPECT_EQ(qxk24_level_budget(QXK24_LEVEL_JISIM),
              7ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24Levels_Budget, Atom_Is43MiB) {
    EXPECT_EQ(qxk24_level_budget(QXK24_LEVEL_ATOM),
              43ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24Levels_Budget, Sistem_Is259MiB) {
    EXPECT_EQ(qxk24_level_budget(QXK24_LEVEL_SISTEM),
              259ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24Levels_Budget, Objek_Is1555MiB) {
    EXPECT_EQ(qxk24_level_budget(QXK24_LEVEL_OBJEK),
              1555ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24Levels_Budget, Aktif_Is9331MiB) {
    EXPECT_EQ(qxk24_level_budget(QXK24_LEVEL_AKTIF),
              9331ULL * QXK24_BASE_UNIT_BYTES);
}
TEST(QXK24Levels_Budget, Modul_Is55987MiB) {
    EXPECT_EQ(qxk24_level_budget(QXK24_LEVEL_MODUL),
              55987ULL * QXK24_BASE_UNIT_BYTES);
}

TEST(QXK24Levels_Desc, Zarah_CodeIsK24za) {
    QXK24LevelDesc d;
    ASSERT_EQ(qxk24_level_describe(QXK24_LEVEL_ZARAH, &d), QXK24_OK);
    EXPECT_STREQ(d.code, "K24za");
    EXPECT_STREQ(d.name, "Zarah");
    EXPECT_STREQ(d.name_en, "Particle");
}
TEST(QXK24Levels_Desc, Modul_CodeIsK24md) {
    QXK24LevelDesc d;
    ASSERT_EQ(qxk24_level_describe(QXK24_LEVEL_MODUL, &d), QXK24_OK);
    EXPECT_STREQ(d.code, "K24md");
    EXPECT_STREQ(d.name, "Modul");
    EXPECT_STREQ(d.name_en, "Module");
}
TEST(QXK24Levels_Desc, AllLevels_DescribeSucceeds) {
    for (int lv = 0; lv < 7; lv++) {
        QXK24LevelDesc d;
        EXPECT_EQ(qxk24_level_describe((QXK24Level)lv, &d), QXK24_OK);
        EXPECT_NE(d.code, nullptr);
        EXPECT_NE(d.name, nullptr);
        EXPECT_NE(d.digital, nullptr);
    }
}
TEST(QXK24Levels_Desc, InvalidLevel_ReturnsError) {
    QXK24LevelDesc d;
    EXPECT_EQ(qxk24_level_describe((QXK24Level)7, &d),
              QXK24_ERR_INVALID_ARGUMENT);
    EXPECT_EQ(qxk24_level_describe((QXK24Level)99, &d),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Levels_Desc, NullOutput_ReturnsError) {
    EXPECT_EQ(qxk24_level_describe(QXK24_LEVEL_ZARAH, nullptr),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Levels_Desc, ScaleMatchesFormula) {
    for (int lv = 0; lv < 7; lv++) {
        QXK24LevelDesc d;
        ASSERT_EQ(qxk24_level_describe((QXK24Level)lv, &d), QXK24_OK);
        EXPECT_EQ(d.scale, qxk24_pow6((uint32_t)lv));
    }
}
TEST(QXK24Levels_Desc, Scale_Increases_By6_Each_Level) {
    for (int lv = 1; lv < 7; lv++) {
        uint64_t prev = qxk24_level_scale((QXK24Level)(lv - 1));
        uint64_t curr = qxk24_level_scale((QXK24Level)lv);
        EXPECT_EQ(curr, prev * 6ULL);
    }
}

TEST(QXK24Directions, Atas_CodeIsAs) {
    QXK24DirectionDesc d;
    ASSERT_EQ(qxk24_direction_describe(QXK24_DIR_ATAS, &d), QXK24_OK);
    EXPECT_STREQ(d.code, "As");
    EXPECT_STREQ(d.name, "Atas");
    EXPECT_STREQ(d.name_en, "Top");
}
TEST(QXK24Directions, Kiri_CodeIsKr) {
    QXK24DirectionDesc d;
    ASSERT_EQ(qxk24_direction_describe(QXK24_DIR_KIRI, &d), QXK24_OK);
    EXPECT_STREQ(d.code, "Kr");
    EXPECT_STREQ(d.name_en, "Left");
}
TEST(QXK24Directions, AllDirections_DescribeSucceeds) {
    for (int dir = 0; dir < 6; dir++) {
        QXK24DirectionDesc d;
        EXPECT_EQ(qxk24_direction_describe((QXK24Direction)dir, &d), QXK24_OK);
        EXPECT_NE(d.code, nullptr);
        EXPECT_NE(d.name, nullptr);
        EXPECT_NE(d.role, nullptr);
    }
}
TEST(QXK24Directions, PairLaw_Z3_ThreePairsAreMirrors) {
    EXPECT_EQ(qxk24_direction_pair(QXK24_DIR_ATAS), QXK24_DIR_BAWAH);
    EXPECT_EQ(qxk24_direction_pair(QXK24_DIR_BAWAH), QXK24_DIR_ATAS);
    EXPECT_EQ(qxk24_direction_pair(QXK24_DIR_DEPAN), QXK24_DIR_BELAKANG);
    EXPECT_EQ(qxk24_direction_pair(QXK24_DIR_BELAKANG), QXK24_DIR_DEPAN);
    EXPECT_EQ(qxk24_direction_pair(QXK24_DIR_KANAN), QXK24_DIR_KIRI);
    EXPECT_EQ(qxk24_direction_pair(QXK24_DIR_KIRI), QXK24_DIR_KANAN);
}
TEST(QXK24Directions, PairOf_Pair_ReturnsSelf) {
    for (int dir = 0; dir < 6; dir++) {
        QXK24Direction d = (QXK24Direction)dir;
        EXPECT_EQ(qxk24_direction_pair(qxk24_direction_pair(d)), d);
    }
}
TEST(QXK24Directions, InvalidDirection_ReturnsError) {
    QXK24DirectionDesc d;
    EXPECT_EQ(qxk24_direction_describe((QXK24Direction)6, &d),
              QXK24_ERR_INVALID_ARGUMENT);
    EXPECT_EQ(qxk24_direction_describe((QXK24Direction)99, &d),
              QXK24_ERR_INVALID_ARGUMENT);
}

static QXK24Config make_config(void) {
    QXK24Config c;
    memset(&c, 0, sizeof(c));
    c.physical_ram_bytes = 8ULL * 1024ULL * 1024ULL * 1024ULL;
    c.luman_peringkat = 4;
    c.declared_x = 1.0;
    c.tolerance_pct = 15.0;
    strncpy(c.kernel_label, "test-kernel", 63);
    return c;
}

TEST(QXK24Kernel_Lifecycle, Create_Succeeds) {
    QXK24Config c = make_config();
    QXK24KernelHandle h = nullptr;
    EXPECT_EQ(qxk24_create(&c, &h), QXK24_OK);
    EXPECT_NE(h, nullptr);
    qxk24_destroy(h);
}
TEST(QXK24Kernel_Lifecycle, Create_NullConfig_Fails) {
    QXK24KernelHandle h = nullptr;
    EXPECT_EQ(qxk24_create(nullptr, &h), QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Kernel_Lifecycle, Create_NullHandle_Fails) {
    QXK24Config c = make_config();
    EXPECT_EQ(qxk24_create(&c, nullptr), QXK24_ERR_INVALID_ARGUMENT);
}
TEST(QXK24Kernel_Lifecycle, Start_Succeeds) {
    QXK24Config c = make_config();
    QXK24KernelHandle h = nullptr;
    ASSERT_EQ(qxk24_create(&c, &h), QXK24_OK);
    EXPECT_EQ(qxk24_start(h), QXK24_OK);
    qxk24_destroy(h);
}
TEST(QXK24Kernel_Lifecycle, InitialHealthIs100) {
    QXK24Config c = make_config();
    QXK24KernelHandle h = nullptr;
    ASSERT_EQ(qxk24_create(&c, &h), QXK24_OK);
    EXPECT_DOUBLE_EQ(qxk24_health_score(h), 100.0);
    qxk24_destroy(h);
}
TEST(QXK24Kernel_Lifecycle, InitialCertIsSovereign) {
    QXK24Config c = make_config();
    QXK24KernelHandle h = nullptr;
    ASSERT_EQ(qxk24_create(&c, &h), QXK24_OK);
    EXPECT_EQ(qxk24_cert_tier(h), QXK24_CERT_SOVEREIGN);
    qxk24_destroy(h);
}
TEST(QXK24Kernel_Lifecycle, Cycle_ProducesReport) {
    QXK24Config c = make_config();
    QXK24KernelHandle h = nullptr;
    ASSERT_EQ(qxk24_create(&c, &h), QXK24_OK);
    ASSERT_EQ(qxk24_start(h), QXK24_OK);

    QXK24Report report;
    EXPECT_EQ(qxk24_cycle(h, &report), QXK24_OK);
    EXPECT_EQ(report.cycle_index, 1ULL);
    EXPECT_GE(report.health_score, 0.0);
    EXPECT_LE(report.health_score, 100.0);
    EXPECT_GT(report.timestamp_ms, 0ULL);
    qxk24_destroy(h);
}
TEST(QXK24Kernel_Lifecycle, Destroy_NullHandle_IsNoOp) {
    EXPECT_NO_FATAL_FAILURE(qxk24_destroy(nullptr));
}
