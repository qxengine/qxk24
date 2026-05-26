/* ============================================================
 * qxk24_meta.c
 * QXK24 Universal Kernel — META-FIZIK Compatibility Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/science/qxk24_meta.c
 * License : Apache 2.0
 *
 * Description:
 * Implements META-FIZIK active/passive pairing validation,
 * compatibility scoring, and snapshots.
 * ============================================================ */

#include "qxk24/science/qxk24_meta.h"
#include <stdlib.h>
#include <string.h>

struct qxk24_meta_impl_t {
    uint32_t checks_performed;
    uint32_t full_compatible;
    uint32_t partial_compatible;
    uint32_t rejected;
    float health;
};

static const char *s_compat_names[] = {"FULL", "PARTIAL", "REJECTED"};

static void update_health(struct qxk24_meta_impl_t *m) {
    m->health = 99.5f - (float)(m->checks_performed % 20U) * 0.1f;
    if (m->health < 80.0f) m->health = 80.0f;
}

qxk24_meta_t qxk24_meta_create(void) {
    struct qxk24_meta_impl_t *m = calloc(1, sizeof(*m));
    if (m == NULL) return NULL;
    m->health = 99.5f;
    return m;
}

void qxk24_meta_destroy(qxk24_meta_t m) {
    free(m);
}

float qxk24_meta_score(const qxk24_meta_pattern_t *a,
                       const qxk24_meta_pattern_t *b) {
    if (a == NULL || b == NULL) return 0.0f;
    float score = 0.0f;
    if (a->level == b->level) score += 0.4f;
    if (a->structure == b->structure) score += 0.4f;
    float diff = a->energy - b->energy;
    if (diff < 0.0f) diff = -diff;
    if (diff < 1.0f) score += 0.2f;
    return score;
}

int qxk24_meta_validate_pair(const qxk24_meta_pattern_t *meta,
                             const qxk24_meta_pattern_t *fizik) {
    if (meta == NULL || fizik == NULL) return 0;
    return meta->is_active == 1 && fizik->is_active == 0;
}

int qxk24_meta_check(qxk24_meta_t m, const qxk24_meta_pattern_t *meta,
                     const qxk24_meta_pattern_t *fizik,
                     qxk24_meta_check_t *out) {
    if (m == NULL || meta == NULL || fizik == NULL || out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    memset(out, 0, sizeof(*out));
    out->meta = *meta;
    out->fizik = *fizik;
    out->score = qxk24_meta_score(meta, fizik);
    out->pattern_match = meta->structure == fizik->structure;
    out->level_match = meta->level == fizik->level;
    float diff = meta->energy - fizik->energy;
    if (diff < 0.0f) diff = -diff;
    out->energy_match = diff < 1.0f;
    if (out->score >= QXK24_META_COMPATIBLE_THRESHOLD) {
        out->result = QXK24_META_COMPAT_FULL;
        m->full_compatible++;
    } else if (out->score >= QXK24_META_PARTIAL_THRESHOLD) {
        out->result = QXK24_META_COMPAT_PARTIAL;
        m->partial_compatible++;
    } else {
        out->result = QXK24_META_COMPAT_REJECTED;
        m->rejected++;
    }
    m->checks_performed++;
    update_health(m);
    return QXK24_OK;
}

const char *qxk24_meta_compat_name(qxk24_meta_compat_t c) {
    return c >= QXK24_META_COMPAT_FULL && c <= QXK24_META_COMPAT_REJECTED
        ? s_compat_names[(int)c] : "unknown";
}

qxk24_meta_snapshot_t qxk24_meta_snapshot(qxk24_meta_t m) {
    qxk24_meta_snapshot_t s;
    memset(&s, 0, sizeof(s));
    if (m == NULL) return s;
    s.checks_performed = m->checks_performed;
    s.full_compatible = m->full_compatible;
    s.partial_compatible = m->partial_compatible;
    s.rejected = m->rejected;
    s.health = m->health;
    return s;
}
