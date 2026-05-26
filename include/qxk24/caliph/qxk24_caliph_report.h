/* ============================================================
 * qxk24_caliph_report.h
 * QXK24 Universal Kernel — Caliph Report and Judgment
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/caliph/qxk24_caliph_report.h
 * License : Apache 2.0
 *
 * Description:
 * Declares Caliph report reception and constitutional judgment structures.
 * ============================================================ */

#ifndef QXK24_CALIPH_REPORT_H
#define QXK24_CALIPH_REPORT_H

#include "../qxk24.h"
#include "../kernel/qxk24_health.h"
#include "../kernel/qxk24_report.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef QXK24_CALIPH_HANDLE_DEFINED
#define QXK24_CALIPH_HANDLE_DEFINED
typedef struct QXK24CaliphImpl *qxk24_caliph_t;
#endif

typedef enum {
    QXK24_JUDGMENT_MAKMUR = 0,
    QXK24_JUDGMENT_ISLAH = 1,
    QXK24_JUDGMENT_WAQF = 2
} QXK24CaliphJudgment;

#define QXK24_JUDGMENT_MAKMUR_MIN 80.0f
#define QXK24_JUDGMENT_ISLAH_MIN  50.0f

typedef struct QXK24CaliphReport_s {
    float kernel_health;
    QXK24CertTier certification;
    uint32_t active_violations;
    bool constitutionally_sound;
    uint32_t cycle_index;
    uint32_t usul_masa_position;
    float fm_value;
    QXK24CaliphJudgment judgment;
} QXK24CaliphReport;

qxk24_err_t qxk24_caliph_receive_report(qxk24_caliph_t caliph,
                                         QXK24CaliphReport *report_out);
QXK24CaliphJudgment qxk24_caliph_judge(qxk24_caliph_t caliph,
                                        const QXK24CaliphReport *report);
const char *qxk24_caliph_judgment_name(QXK24CaliphJudgment judgment);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_CALIPH_REPORT_H */
