/* ============================================================
 * test_qxk24_adam_knowledge.cpp
 * QXK24 Universal Kernel — ADAM Knowledge Pipeline Tests
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : tests/unit/test_qxk24_adam_knowledge.cpp
 * License : Apache 2.0
 *
 * Description:
 * Tests Phase 12 ADAM knowledge approval, runtime monitoring,
 * learning requests, and governed reply judgment.
 * ============================================================ */

#include <cstring>
#include <gtest/gtest.h>
#include "qxk24/adam/qxk24_adam_knowledge.h"

static QXK24AdamKnowledgeEntry make_entry(
    QXK24AdamKnowledgeStatus status = QXK24_ADAM_KNOWLEDGE_DRAFT) {
    QXK24AdamKnowledgeEntry e;
    std::memset(&e, 0, sizeof(e));
    std::snprintf(e.id, sizeof(e.id), "rina-energy");
    std::snprintf(e.title, sizeof(e.title), "RINA Energy");
    e.source_type = QXK24_ADAM_SOURCE_FOUNDER_NOTE;
    e.domain = QXK24_ADAM_DOMAIN_RINA;
    std::snprintf(e.principle, sizeof(e.principle),
                  "Energy has active and passive constitutional pair");
    std::snprintf(e.definition, sizeof(e.definition),
                  "Energy is governed by active and passive balance");
    std::snprintf(e.application, sizeof(e.application),
                  "Use this to enrich conventional energy answers");
    e.status = status;
    e.version_major = 1;
    e.version_minor = 6;
    return e;
}

class AdamKnowledgeTest : public ::testing::Test {
protected:
    qxk24_adam_knowledge_index_t index = nullptr;
    void SetUp() override {
        ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_index_create(&index));
    }
    void TearDown() override {
        EXPECT_EQ(QXK24_OK, qxk24_adam_knowledge_index_destroy(index));
    }
};

TEST_F(AdamKnowledgeTest, CreateIndexNotNull) {
    EXPECT_NE(index, nullptr);
}

TEST_F(AdamKnowledgeTest, AddDraftUpdatesSnapshot) {
    QXK24AdamKnowledgeEntry e = make_entry();
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    QXK24AdamKnowledgeSnapshot s;
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_snapshot(index, &s));
    EXPECT_EQ(s.total_entries, 1u);
    EXPECT_EQ(s.draft_entries, 1u);
}

TEST_F(AdamKnowledgeTest, DraftDoesNotGovernDomainQuery) {
    QXK24AdamKnowledgeEntry e = make_entry();
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    uint32_t count = 99;
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_query_domain(
        index, QXK24_ADAM_DOMAIN_RINA, nullptr, 0, &count));
    EXPECT_EQ(count, 0u);
}

TEST_F(AdamKnowledgeTest, FounderApprovalEnablesDomainQuery) {
    QXK24AdamKnowledgeEntry e = make_entry();
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_approve(
        index, "rina-energy", 20260526));
    QXK24AdamKnowledgeEntry out[2];
    uint32_t count = 0;
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_query_domain(
        index, QXK24_ADAM_DOMAIN_RINA, out, 2, &count));
    EXPECT_EQ(count, 1u);
    EXPECT_STREQ(out[0].id, "rina-energy");
}

TEST_F(AdamKnowledgeTest, FindReturnsStoredEntry) {
    QXK24AdamKnowledgeEntry e = make_entry();
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    QXK24AdamKnowledgeEntry out;
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_find(index, "rina-energy", &out));
    EXPECT_STREQ(out.title, "RINA Energy");
}

TEST_F(AdamKnowledgeTest, DuplicateEntryRejected) {
    QXK24AdamKnowledgeEntry e = make_entry();
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    EXPECT_NE(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
}

TEST_F(AdamKnowledgeTest, MonitorWaqfWithoutFounderApproval) {
    QXK24AdamClaim c = {};
    std::snprintf(c.id, sizeof(c.id), "claim-1");
    std::snprintf(c.claim_text, sizeof(c.claim_text), "Energy does work");
    c.domain = QXK24_ADAM_DOMAIN_RINA;
    QXK24AdamMonitorDecision d;
    ASSERT_EQ(QXK24_OK, qxk24_adam_monitor_decide(index, &c, &d));
    EXPECT_EQ(d.action, QXK24_ADAM_MONITOR_WAQF);
    EXPECT_LT(d.score_total, 0.50f);
}

TEST_F(AdamKnowledgeTest, WaqfCreatesLearningRequest) {
    QXK24AdamMonitorDecision d = {};
    d.action = QXK24_ADAM_MONITOR_WAQF;
    d.domain = QXK24_ADAM_DOMAIN_RINA;
    std::snprintf(d.claim_id, sizeof(d.claim_id), "claim-1");
    std::snprintf(d.reason, sizeof(d.reason), "Founder knowledge required");
    QXK24AdamLearningRequest r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_learning_request_create(&d, &r));
    EXPECT_EQ(r.missing_domain, QXK24_ADAM_DOMAIN_RINA);
    EXPECT_FALSE(r.resolved);
}

TEST_F(AdamKnowledgeTest, NonWaqfLearningRequestRejected) {
    QXK24AdamMonitorDecision d = {};
    d.action = QXK24_ADAM_MONITOR_CONFIRM;
    QXK24AdamLearningRequest r;
    EXPECT_NE(QXK24_OK, qxk24_adam_learning_request_create(&d, &r));
}

TEST_F(AdamKnowledgeTest, MonitorConfirmWhenClaimMatchesTruth) {
    QXK24AdamKnowledgeEntry e = make_entry(QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED);
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    QXK24AdamClaim c = {};
    std::snprintf(c.id, sizeof(c.id), "claim-2");
    std::snprintf(c.claim_text, sizeof(c.claim_text), "%s", e.definition);
    c.domain = QXK24_ADAM_DOMAIN_RINA;
    QXK24AdamMonitorDecision d;
    ASSERT_EQ(QXK24_OK, qxk24_adam_monitor_decide(index, &c, &d));
    EXPECT_EQ(d.action, QXK24_ADAM_MONITOR_CONFIRM);
    EXPECT_FLOAT_EQ(d.score_total, 1.0f);
}

TEST_F(AdamKnowledgeTest, MonitorEnrichWhenClaimIsIncomplete) {
    QXK24AdamKnowledgeEntry e = make_entry(QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED);
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    QXK24AdamClaim c = {};
    std::snprintf(c.id, sizeof(c.id), "claim-3");
    std::snprintf(c.claim_text, sizeof(c.claim_text), "Energy is capacity");
    c.domain = QXK24_ADAM_DOMAIN_RINA;
    QXK24AdamMonitorDecision d;
    ASSERT_EQ(QXK24_OK, qxk24_adam_monitor_decide(index, &c, &d));
    EXPECT_EQ(d.action, QXK24_ADAM_MONITOR_ENRICH);
    EXPECT_STREQ(d.replacement_text, e.definition);
}

TEST_F(AdamKnowledgeTest, MonitorCorrectWhenClaimContradicts) {
    QXK24AdamKnowledgeEntry e = make_entry(QXK24_ADAM_KNOWLEDGE_FOUNDER_APPROVED);
    ASSERT_EQ(QXK24_OK, qxk24_adam_knowledge_add(index, &e));
    QXK24AdamClaim c = {};
    std::snprintf(c.id, sizeof(c.id), "claim-4");
    std::snprintf(c.claim_text, sizeof(c.claim_text),
                  "Energy is not active passive");
    c.domain = QXK24_ADAM_DOMAIN_RINA;
    QXK24AdamMonitorDecision d;
    ASSERT_EQ(QXK24_OK, qxk24_adam_monitor_decide(index, &c, &d));
    EXPECT_EQ(d.action, QXK24_ADAM_MONITOR_CORRECT);
    EXPECT_STREQ(d.replacement_text, e.definition);
}

TEST_F(AdamKnowledgeTest, GovernReplyMakmur) {
    QXK24AdamMonitorDecision d = {};
    d.action = QXK24_ADAM_MONITOR_CONFIRM;
    d.score_total = 1.0f;
    QXK24AdamGovernedReply r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_govern_reply("answer", &d, 1, &r));
    EXPECT_EQ(r.judgment, QXK24_ADAM_JUDGMENT_MAKMUR);
    EXPECT_EQ(r.confirmed_count, 1u);
}

TEST_F(AdamKnowledgeTest, GovernReplyIslah) {
    QXK24AdamMonitorDecision d = {};
    d.action = QXK24_ADAM_MONITOR_CORRECT;
    d.score_total = 0.75f;
    QXK24AdamGovernedReply r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_govern_reply("answer", &d, 1, &r));
    EXPECT_EQ(r.judgment, QXK24_ADAM_JUDGMENT_ISLAH);
    EXPECT_EQ(r.corrected_count, 1u);
}

TEST_F(AdamKnowledgeTest, GovernReplyWaqf) {
    QXK24AdamMonitorDecision d = {};
    d.action = QXK24_ADAM_MONITOR_WAQF;
    d.score_total = 0.25f;
    QXK24AdamGovernedReply r;
    ASSERT_EQ(QXK24_OK, qxk24_adam_govern_reply("answer", &d, 1, &r));
    EXPECT_EQ(r.judgment, QXK24_ADAM_JUDGMENT_WAQF);
    EXPECT_EQ(r.waqf_count, 1u);
}

TEST_F(AdamKnowledgeTest, NamesAreStable) {
    EXPECT_STREQ(qxk24_adam_domain_name(QXK24_ADAM_DOMAIN_RINA), "RINA");
    EXPECT_STREQ(qxk24_adam_monitor_action_name(QXK24_ADAM_MONITOR_ENRICH),
                 "ENRICH");
}

TEST_F(AdamKnowledgeTest, NullParametersRejected) {
    EXPECT_NE(QXK24_OK, qxk24_adam_knowledge_add(index, nullptr));
    EXPECT_NE(QXK24_OK, qxk24_adam_monitor_decide(index, nullptr, nullptr));
    EXPECT_NE(QXK24_OK, qxk24_adam_govern_reply(nullptr, nullptr, 0, nullptr));
}
