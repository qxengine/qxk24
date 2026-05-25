/* ============================================================
 * qxk24_report.h
 * QXK24 Universal Kernel — Constitutional Report Header
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/kernel/qxk24_report.h
 * License : Apache 2.0
 *
 * Description:
 * Declares Phase 3 report helpers that generate and inspect constitutional
 * kernel reports using the established QXK24Report ABI.
 * ============================================================ */

#ifndef QXK24_REPORT_H
#define QXK24_REPORT_H

#include "qxk24/kernel/qxk24_health.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_REPORT_MAX_VIOLATIONS 64U
#define QXK24_REPORT_TAG_LEN 32U

typedef struct QXK24Violation_s {
    uint8_t law_index;
    char law_tag[QXK24_REPORT_TAG_LEN];
    char reason[128];
    uint64_t cycle_index;
} QXK24Violation;

qxk24_err_t qxk24_report_generate(QXK24KernelHandle kernel, QXK24Report *out);
bool qxk24_report_is_sound(const QXK24Report *report);
const char *qxk24_report_violation_tag(uint8_t law_index);
void qxk24_report_print(const QXK24Report *report);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_REPORT_H */
