/* ============================================================
 * qxk24_meta.h
 * QXK24 Universal Kernel — META-FIZIK Compatibility Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/science/qxk24_meta.h
 * License : Apache 2.0
 *
 * Description:
 * Declares META-FIZIK pattern compatibility checks for active
 * META and passive FIZIK constitutional pairing.
 * ============================================================ */

#ifndef QXK24_META_H
#define QXK24_META_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_META_COMPATIBLE_THRESHOLD 0.80f
#define QXK24_META_PARTIAL_THRESHOLD 0.50f
#define QXK24_META_MAX_PATTERN_LEVEL 7U
#define QXK24_META_ACTIVE_SIGN 0
#define QXK24_META_PASSIVE_SIGN 1

typedef enum {
    QXK24_META_COMPAT_FULL = 0,
    QXK24_META_COMPAT_PARTIAL = 1,
    QXK24_META_COMPAT_REJECTED = 2
} qxk24_meta_compat_t;

typedef struct {
    uint32_t level;
    uint32_t structure;
    int is_active;
    float energy;
} qxk24_meta_pattern_t;

typedef struct {
    qxk24_meta_pattern_t meta;
    qxk24_meta_pattern_t fizik;
    float score;
    qxk24_meta_compat_t result;
    int pattern_match;
    int level_match;
    int energy_match;
} qxk24_meta_check_t;

typedef struct {
    uint32_t checks_performed;
    uint32_t full_compatible;
    uint32_t partial_compatible;
    uint32_t rejected;
    float health;
} qxk24_meta_snapshot_t;

typedef struct qxk24_meta_impl_t *qxk24_meta_t;

qxk24_meta_t qxk24_meta_create(void);
void qxk24_meta_destroy(qxk24_meta_t m);
int qxk24_meta_check(qxk24_meta_t m, const qxk24_meta_pattern_t *meta,
                     const qxk24_meta_pattern_t *fizik,
                     qxk24_meta_check_t *out);
int qxk24_meta_validate_pair(const qxk24_meta_pattern_t *meta,
                             const qxk24_meta_pattern_t *fizik);
float qxk24_meta_score(const qxk24_meta_pattern_t *a,
                       const qxk24_meta_pattern_t *b);
const char *qxk24_meta_compat_name(qxk24_meta_compat_t c);
qxk24_meta_snapshot_t qxk24_meta_snapshot(qxk24_meta_t m);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_META_H */
