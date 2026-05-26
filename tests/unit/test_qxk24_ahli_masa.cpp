/* ============================================================
 * QXK24 – Universal Kernel
 * tests/unit/test_qxk24_ahli_masa.cpp
 *
 * Ahli Masa — Unit Tests
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * ============================================================ */

#include "qxk24/fm/qxk24_ahli_masa.h"
#include <gtest/gtest.h>

static qxk24_xam_t make_xam(void) {
    qxk24_xam_t xam = qxk24_xam_create();
    EXPECT_NE(xam, nullptr);
    return xam;
}

TEST(QXK24Xam_Create, NotNull) {
    qxk24_xam_t xam = qxk24_xam_create();
    EXPECT_NE(xam, nullptr);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Create, InitialCoordZero) {
    qxk24_xam_t xam = make_xam();
    EXPECT_EQ(qxk24_xam_current_coord(xam), 0U);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Create, InitialCyclesZero) {
    qxk24_xam_t xam = make_xam();
    EXPECT_EQ(qxk24_xam_total_cycles(xam), 0ULL);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Create, InitiallyIncomplete) {
    qxk24_xam_t xam = make_xam();
    EXPECT_FALSE(qxk24_xam_is_cycle_complete(xam));
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Record, FirstDirectionIsAs) {
    qxk24_xam_t xam = make_xam();
    QXK24XamRecord record{};
    qxk24_xam_record(xam, &record);
    EXPECT_EQ(record.coords[0].direction, QXK24_DIR_AS);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Record, LastDirectionIsKr) {
    qxk24_xam_t xam = make_xam();
    QXK24XamRecord record{};
    qxk24_xam_record(xam, &record);
    EXPECT_EQ(record.coords[5].direction, QXK24_DIR_KR);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Record, InitialPassedZero) {
    qxk24_xam_t xam = make_xam();
    QXK24XamRecord record{};
    qxk24_xam_record(xam, &record);
    EXPECT_EQ(record.coords_passed, 0U);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Move, Succeeds) {
    qxk24_xam_t xam = make_xam();
    EXPECT_EQ(qxk24_xam_move(xam), QXK24_OK);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Move, FirstMoveTimestampSet) {
    qxk24_xam_t xam = make_xam();
    qxk24_xam_move(xam);
    QXK24XamRecord record{};
    qxk24_xam_record(xam, &record);
    EXPECT_GT(record.first_move_ms, 0ULL);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Move, FirstMoveCoordZero) {
    qxk24_xam_t xam = make_xam();
    qxk24_xam_move(xam);
    EXPECT_EQ(qxk24_xam_current_coord(xam), 0U);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Move, SecondMoveCoordOne) {
    qxk24_xam_t xam = make_xam();
    qxk24_xam_move(xam);
    qxk24_xam_move(xam);
    EXPECT_EQ(qxk24_xam_current_coord(xam), 1U);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Move, PreviousCoordPassed) {
    qxk24_xam_t xam = make_xam();
    qxk24_xam_move(xam);
    qxk24_xam_move(xam);
    QXK24XamRecord record{};
    qxk24_xam_record(xam, &record);
    EXPECT_EQ(record.coords[0].state, QXK24_XAM_COORD_PASSED);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_MoveTo, DirectMoveToThree) {
    qxk24_xam_t xam = make_xam();
    EXPECT_EQ(qxk24_xam_move_to(xam, 3), QXK24_OK);
    EXPECT_EQ(qxk24_xam_current_coord(xam), 3U);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_MoveTo, InvalidIndexReturnsError) {
    qxk24_xam_t xam = make_xam();
    EXPECT_EQ(qxk24_xam_move_to(xam, 6), QXK24_ERR_INVALID_ARGUMENT);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Cycle, SixMovesCompleteCycle) {
    qxk24_xam_t xam = make_xam();
    for (int i = 0; i < 6; i++) qxk24_xam_move(xam);
    EXPECT_TRUE(qxk24_xam_is_cycle_complete(xam));
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Cycle, SixMovesAddOneCycle) {
    qxk24_xam_t xam = make_xam();
    for (int i = 0; i < 6; i++) qxk24_xam_move(xam);
    EXPECT_EQ(qxk24_xam_total_cycles(xam), 1ULL);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Reset, ClearsComplete) {
    qxk24_xam_t xam = make_xam();
    for (int i = 0; i < 6; i++) qxk24_xam_move(xam);
    qxk24_xam_reset(xam);
    EXPECT_FALSE(qxk24_xam_is_cycle_complete(xam));
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Reset, PreservesCycleCount) {
    qxk24_xam_t xam = make_xam();
    for (int i = 0; i < 6; i++) qxk24_xam_move(xam);
    qxk24_xam_reset(xam);
    EXPECT_EQ(qxk24_xam_total_cycles(xam), 1ULL);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Null, RecordNullXamReturnsError) {
    QXK24XamRecord record{};
    EXPECT_EQ(qxk24_xam_record(nullptr, &record), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Xam_Null, RecordNullOutReturnsError) {
    qxk24_xam_t xam = make_xam();
    EXPECT_EQ(qxk24_xam_record(xam, nullptr), QXK24_ERR_NULL_PARAM);
    qxk24_xam_destroy(xam);
}
TEST(QXK24Xam_Null, MoveNullReturnsError) {
    EXPECT_EQ(qxk24_xam_move(nullptr), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Xam_Null, MoveToNullReturnsError) {
    EXPECT_EQ(qxk24_xam_move_to(nullptr, 0), QXK24_ERR_NULL_PARAM);
}
TEST(QXK24Xam_Null, CurrentCoordNullIsZero) {
    EXPECT_EQ(qxk24_xam_current_coord(nullptr), 0U);
}
TEST(QXK24Xam_Null, CycleCompleteNullIsFalse) {
    EXPECT_FALSE(qxk24_xam_is_cycle_complete(nullptr));
}
