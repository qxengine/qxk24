/* ============================================================
 * qxk24_qiubbx.h
 * QXK24 Universal Kernel - QIUBBX Validator
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/validation/qxk24_qiubbx.h
 * License : Apache 2.0
 *
 * Description:
 * Declares QIUBBX validation for SOVEREIGN certification.
 * ============================================================ */

#ifndef QXK24_QIUBBX_H
#define QXK24_QIUBBX_H

#include "qxk24/bridge/qxk24_qxengine_bridge.h"
#include "qxk24/memloc/qxk24_memloc_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_QIUBBX_VERSION "1.0.0"
#define QXK24_QIUBBX_CYCLES 7U
#define QXK24_QIUBBX_LAWS 8U
#define QXK24_QIUBBX_DIRECTIONS 6U
#define QXK24_QIUBBX_LEVELS 7U
#define QXK24_QIUBBX_SOVEREIGN 95.0f

typedef struct QXK24QIUBBXLawResult_s {
    uint8_t law_index;
    char law_tag[8];
    float score;
    float weighted;
    bool passed;
} QXK24QIUBBXLawResult;

typedef struct QXK24QIUBBXDirectionResult_s {
    QXK24Direction direction;
    char name[16];
    float score;
    bool active;
    bool paired;
} QXK24QIUBBXDirectionResult;

typedef struct QXK24QIUBBXLevelResult_s {
    QXK24Level level;
    char name[16];
    uint64_t scale;
    float score;
    bool passed;
} QXK24QIUBBXLevelResult;

typedef struct QXK24QIUBBXReport_s {
    char version[16];
    char founder[32];
    uint64_t timestamp_ms;
    uint64_t validation_cycle;
    QXK24QIUBBXLawResult laws[QXK24_QIUBBX_LAWS];
    uint32_t laws_passed;
    QXK24QIUBBXDirectionResult directions[QXK24_QIUBBX_DIRECTIONS];
    uint32_t directions_active;
    uint32_t directions_paired;
    QXK24QIUBBXLevelResult levels[QXK24_QIUBBX_LEVELS];
    uint32_t levels_passed;
    float kernel_health;
    float qxengine_health;
    float memloc_health;
    float sovereign_score;
    QXK24CertTier tier;
    bool sovereign_achieved;
    bool constitutionally_complete;
    uint32_t total_proofs;
    uint32_t proofs_passed;
    uint32_t proofs_failed;
} QXK24QIUBBXReport;

typedef struct QXK24QIUBBX_s *qxk24_qiubbx_t;

qxk24_qiubbx_t qxk24_qiubbx_create(qxk24_kernel_t kernel,
                                   qxk24_bridge_t bridge);
qxk24_err_t qxk24_qiubbx_run(qxk24_qiubbx_t v,
                             QXK24QIUBBXReport *out);
qxk24_err_t qxk24_qiubbx_validate_laws(qxk24_qiubbx_t v,
                                       QXK24QIUBBXReport *out);
qxk24_err_t qxk24_qiubbx_validate_directions(qxk24_qiubbx_t v,
                                             QXK24QIUBBXReport *out);
qxk24_err_t qxk24_qiubbx_validate_levels(qxk24_qiubbx_t v,
                                         QXK24QIUBBXReport *out);
float qxk24_qiubbx_sovereign_score(const QXK24QIUBBXReport *report);
void qxk24_qiubbx_destroy(qxk24_qiubbx_t v);
void qxk24_qiubbx_print(const QXK24QIUBBXReport *report);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_QIUBBX_H */
