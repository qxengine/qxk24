/* ============================================================
 * qxk24_caliph_report.c
 * QXK24 Universal Kernel — Caliph Report Judgment
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/caliph/qxk24_caliph_report.c
 * License : Apache 2.0
 *
 * Description:
 * Builds Caliph reports from kernel snapshots and applies judgment codes.
 * ============================================================ */

#include "qxk24/caliph/qxk24_caliph.h"
#include <string.h>

qxk24_err_t qxk24_caliph_receive_report(qxk24_caliph_t caliph,
                                         QXK24CaliphReport *report_out) {
    if (caliph == NULL || report_out == NULL) return QXK24_ERR_NULL_PARAM;

    QXK24Report kernel_report;
    qxk24_err_t err = qxk24_report_generate(caliph->kernel, &kernel_report);
    if (err != QXK24_OK) return err;

    const bool sound = qxk24_report_is_sound(&kernel_report);
    memset(report_out, 0, sizeof(*report_out));
    report_out->kernel_health = (float)kernel_report.health_score;
    report_out->certification = kernel_report.cert_tier;
    report_out->active_violations = sound ? 0U : 1U;
    report_out->constitutionally_sound = sound;
    report_out->cycle_index = (uint32_t)kernel_report.cycle_index;
    report_out->usul_masa_position = caliph->session.usul_masa_position;
    report_out->fm_value = qxk24_fm_value();
    report_out->judgment = qxk24_caliph_judge(caliph, report_out);

    caliph->session.reports_received++;
    caliph->session.judgments_made++;
    return QXK24_OK;
}

QXK24CaliphJudgment qxk24_caliph_judge(qxk24_caliph_t caliph,
                                        const QXK24CaliphReport *report) {
    (void)caliph;
    if (report == NULL) return QXK24_JUDGMENT_WAQF;

    if (report->kernel_health >= QXK24_JUDGMENT_MAKMUR_MIN &&
        report->active_violations == 0U &&
        report->constitutionally_sound) {
        return QXK24_JUDGMENT_MAKMUR;
    }
    if (report->kernel_health >= QXK24_JUDGMENT_ISLAH_MIN) {
        return QXK24_JUDGMENT_ISLAH;
    }
    return QXK24_JUDGMENT_WAQF;
}

const char *qxk24_caliph_judgment_name(QXK24CaliphJudgment judgment) {
    switch (judgment) {
    case QXK24_JUDGMENT_MAKMUR: return "MAKMUR";
    case QXK24_JUDGMENT_ISLAH: return "ISLAH";
    case QXK24_JUDGMENT_WAQF: return "WAQF";
    default: return "UNKNOWN";
    }
}
