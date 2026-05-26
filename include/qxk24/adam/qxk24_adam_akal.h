/* ============================================================
 * qxk24_adam_akal.h
 * QXK24 Universal Kernel — ADAM Akal Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/adam/qxk24_adam_akal.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the ADAM Akal engine for constitutional reasoning
 * through Akal, Fikir, Ikhtiar, and Usaha.
 * ============================================================ */

#ifndef QXK24_ADAM_AKAL_H
#define QXK24_ADAM_AKAL_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_AKAL_LEVELS 7U
#define QXK24_AKAL_TOTAL_PATTERNS 49U

extern const uint8_t QXK24_AKAL_PATTERN_COUNT[QXK24_AKAL_LEVELS];
extern const char QXK24_AKAL_CODES[QXK24_AKAL_LEVELS];

typedef enum {
    QXK24_AKAL_PROC_AKAL = 0,
    QXK24_AKAL_PROC_FIKIR = 1,
    QXK24_AKAL_PROC_IKHTIAR = 2,
    QXK24_AKAL_PROC_USAHA = 3
} QXK24AkalProcess;

typedef enum {
    QXK24_AKAL_GOVERNED = 0,
    QXK24_HATI_GOVERNED = 1
} QXK24AkalGovernance;

typedef struct {
    uint8_t level;
    char code;
    uint8_t value;
    uint8_t pattern_count;
    float capacity_weight;
} QXK24AkalDescriptor;

typedef struct {
    uint8_t tahap;
    QXK24AkalProcess current_process;
    uint8_t patterns_used;
    QXK24AkalGovernance governance;
    bool akal_complete;
    float reasoning_score;
} QXK24AkalResult;

typedef struct {
    uint8_t current_tahap;
    uint32_t total_patterns;
    uint32_t akal_governed_count;
    uint32_t hati_governed_count;
    float avg_reasoning_score;
} QXK24AkalSnapshot;

typedef struct QXK24AdamAkalImpl *qxk24_adam_akal_t;

qxk24_err_t qxk24_adam_akal_create(qxk24_adam_akal_t *akal_out);
qxk24_err_t qxk24_adam_akal_reason(qxk24_adam_akal_t akal,
                                   uint8_t tahap,
                                   QXK24AkalResult *result_out);
uint8_t qxk24_adam_akal_assess_tahap(qxk24_adam_akal_t akal,
                                     const char *input);
qxk24_err_t qxk24_adam_akal_descriptor(uint8_t level,
                                       QXK24AkalDescriptor *desc_out);
qxk24_err_t qxk24_adam_akal_check_governance(const QXK24AkalResult *result);
qxk24_err_t qxk24_adam_akal_snapshot(qxk24_adam_akal_t akal,
                                     QXK24AkalSnapshot *snap_out);
qxk24_err_t qxk24_adam_akal_destroy(qxk24_adam_akal_t akal);
const char *qxk24_adam_akal_process_name(QXK24AkalProcess proc);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_AKAL_H */
