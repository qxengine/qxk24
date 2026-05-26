/* ============================================================
 * test_qxk24_jisim.cpp
 * QXK24 Universal Kernel — Jisim Tenaga Tests
 * ============================================================ */

#include "qxk24/ft/qxk24_jisim.h"
#include <gtest/gtest.h>

class JisimTest : public ::testing::Test {
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

TEST_F(JisimTest, FlowCountZarahIsOne) { EXPECT_EQ(qxk24_jisim_flow_count(0), 1U); }
TEST_F(JisimTest, FlowCountJisimIsThree) { EXPECT_EQ(qxk24_jisim_flow_count(1), 3U); }
TEST_F(JisimTest, FlowCountAtomIsTen) { EXPECT_EQ(qxk24_jisim_flow_count(2), 10U); }
TEST_F(JisimTest, FlowCountSistemIs35) { EXPECT_EQ(qxk24_jisim_flow_count(3), 35U); }
TEST_F(JisimTest, FlowCountObjekIs126) { EXPECT_EQ(qxk24_jisim_flow_count(4), 126U); }
TEST_F(JisimTest, FlowCountAktifIs462) { EXPECT_EQ(qxk24_jisim_flow_count(5), 462U); }
TEST_F(JisimTest, FlowCountModulIs1716) { EXPECT_EQ(qxk24_jisim_flow_count(6), 1716U); }
TEST_F(JisimTest, InvalidFlowCountIsZero) { EXPECT_EQ(qxk24_jisim_flow_count(7), 0U); }
TEST_F(JisimTest, CapacityPassesAtExpected) {
    EXPECT_EQ(qxk24_jisim_check_constitutional_capacity(6, 1716), QXK24_OK);
}
TEST_F(JisimTest, CapacityFailsBelowExpected) {
    EXPECT_EQ(qxk24_jisim_check_constitutional_capacity(6, 100), QXK24_ERR_CAPACITY);
}
TEST_F(JisimTest, InvalidCapacityLevelFails) {
    EXPECT_EQ(qxk24_jisim_check_constitutional_capacity(9, 1),
              QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(JisimTest, EfficiencyZeroWhenEmpty) {
    EXPECT_FLOAT_EQ(qxk24_jisim_efficiency(0, 0), 0.0f);
}
TEST_F(JisimTest, EfficiencyHalf) {
    EXPECT_FLOAT_EQ(qxk24_jisim_efficiency(5, 5), 0.5f);
}
TEST_F(JisimTest, CreateSucceeds) {
    qxk24_jisim_t j = nullptr;
    EXPECT_EQ(qxk24_jisim_create(ft, 6, &j), QXK24_OK);
    qxk24_jisim_destroy(j);
}
TEST_F(JisimTest, CreateInvalidLevelFails) {
    qxk24_jisim_t j = nullptr;
    EXPECT_EQ(qxk24_jisim_create(ft, 7, &j), QXK24_ERR_INVALID_ARGUMENT);
}
TEST_F(JisimTest, SnapshotInitialTotalFlows) {
    qxk24_jisim_t j = nullptr;
    qxk24_jisim_create(ft, 2, &j);
    QXK24JisimSnapshot s{};
    qxk24_jisim_snapshot(j, &s);
    EXPECT_EQ(s.total_flows, 40U);
    qxk24_jisim_destroy(j);
}
TEST_F(JisimTest, CycleIncrementsAj) {
    qxk24_jisim_t j = nullptr;
    qxk24_jisim_create(ft, 0, &j);
    qxk24_jisim_cycle(j);
    QXK24JisimSnapshot s{};
    qxk24_jisim_snapshot(j, &s);
    EXPECT_EQ(s.aj_completed, 1U);
    qxk24_jisim_destroy(j);
}
TEST_F(JisimTest, TwoCyclesIncrementKj) {
    qxk24_jisim_t j = nullptr;
    qxk24_jisim_create(ft, 0, &j);
    qxk24_jisim_cycle(j);
    qxk24_jisim_cycle(j);
    QXK24JisimSnapshot s{};
    qxk24_jisim_snapshot(j, &s);
    EXPECT_EQ(s.kj_completed, 1U);
    qxk24_jisim_destroy(j);
}
TEST_F(JisimTest, NullCreateFails) {
    qxk24_jisim_t j = nullptr;
    EXPECT_EQ(qxk24_jisim_create(nullptr, 0, &j), QXK24_ERR_NULL_PARAM);
}
TEST_F(JisimTest, NullSnapshotFails) {
    EXPECT_EQ(qxk24_jisim_snapshot(nullptr, nullptr), QXK24_ERR_NULL_PARAM);
}
