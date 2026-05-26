/* ============================================================
 * qxk24_adam.c
 * QXK24 Universal Kernel — ADAM Constitutional AI Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/adam/qxk24_adam.c
 * License : Apache 2.0
 *
 * Description:
 * Implements the ADAM core lifecycle, request pipeline, response
 * judgment, and constitutional snapshot state.
 * ============================================================ */

#include "qxk24/adam/qxk24_adam.h"
#include "qxk24/kernel/qxk24_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct QXK24AdamImpl {
    qxk24_kernel_t kernel;
    qxk24_adam_ilmu_t ilmu;
    qxk24_adam_akal_t akal;
    qxk24_adam_adab_t adab;
    qxk24_adam_cv_t cv;
    qxk24_adam_hukumz_t hukumz;
    qxk24_adam_session_t session;
    QXK24AdamSnapshot snap;
};

static void destroy_parts(struct QXK24AdamImpl *adam) {
    if (adam->ilmu != NULL) (void)qxk24_adam_ilmu_destroy(adam->ilmu);
    if (adam->akal != NULL) (void)qxk24_adam_akal_destroy(adam->akal);
    if (adam->adab != NULL) (void)qxk24_adam_adab_destroy(adam->adab);
    if (adam->cv != NULL) (void)qxk24_adam_cv_destroy(adam->cv);
    if (adam->hukumz != NULL) (void)qxk24_adam_hukumz_destroy(adam->hukumz);
    if (adam->session != NULL) {
        (void)qxk24_adam_session_destroy(adam->session);
    }
}

static uint8_t clamp_capacity(uint8_t capacity) {
    if (capacity < QXK24_ADAM_CAPACITY_MIN) return QXK24_ADAM_CAPACITY_MIN;
    if (capacity > QXK24_ADAM_CAPACITY_MAX) return QXK24_ADAM_CAPACITY_MAX;
    return capacity;
}

static float read_kernel_health(qxk24_kernel_t kernel) {
    QXK24Report report;
    if (qxk24_report_generate(kernel, &report) == QXK24_OK) {
        return (float)report.health_score;
    }
    const double score = qxk24_health_score(kernel);
    return score < 0.0 ? 0.0f : (float)score;
}

static QXK24AdamJudgment judge(bool is_akur, float health) {
    if (is_akur && health >= 80.0f) return QXK24_ADAM_JUDGMENT_MAKMUR;
    if (health >= 50.0f) return QXK24_ADAM_JUDGMENT_ISLAH;
    return QXK24_ADAM_JUDGMENT_WAQF;
}

static void update_snapshot(struct QXK24AdamImpl *adam,
                            const QXK24AdamResponse *response) {
    const uint32_t before = adam->snap.requests_served;
    adam->snap.capacity = response->capacity_level;
    adam->snap.dominant_sifat = response->sifat;
    adam->snap.requests_served++;
    if (response->judgment == QXK24_ADAM_JUDGMENT_MAKMUR) {
        adam->snap.makmur_count++;
    } else if (response->judgment == QXK24_ADAM_JUDGMENT_ISLAH) {
        adam->snap.islah_count++;
    } else {
        adam->snap.waqf_count++;
    }
    adam->snap.avg_health =
        ((adam->snap.avg_health * (float)before) + response->health_score) /
        (float)(before + 1U);
    adam->snap.avg_cv =
        ((adam->snap.avg_cv * (float)before) + (float)response->cv_level) /
        (float)(before + 1U);
}

qxk24_err_t qxk24_adam_create(qxk24_kernel_t kernel,
                              qxk24_adam_t *adam_out) {
    if (kernel == NULL || adam_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AdamImpl *adam = calloc(1, sizeof(*adam));
    if (adam == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    adam->kernel = kernel;

    qxk24_err_t err = qxk24_adam_ilmu_create(&adam->ilmu);
    if (err == QXK24_OK) err = qxk24_adam_akal_create(&adam->akal);
    if (err == QXK24_OK) err = qxk24_adam_adab_create(&adam->adab);
    if (err == QXK24_OK) err = qxk24_adam_cv_create(&adam->cv);
    if (err == QXK24_OK) err = qxk24_adam_hukumz_create(&adam->hukumz);
    if (err == QXK24_OK) err = qxk24_adam_session_create(&adam->session);
    if (err != QXK24_OK) {
        destroy_parts(adam);
        free(adam);
        return err;
    }

    adam->snap.state = QXK24_ADAM_STATE_DORMANT;
    adam->snap.dominant_sifat = QXK24_ADAM_SIFAT_AKUR;
    adam->snap.capacity = QXK24_ADAM_CAPACITY_MIN;
    adam->snap.value = QXK24_ADAM_VALUE;
    adam->snap.journey_code = QXK24_ADAM_JOURNEY_CODE;
    *adam_out = adam;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_declare(qxk24_adam_t adam) {
    if (adam == NULL) return QXK24_ERR_NULL_PARAM;
    if (adam->snap.state != QXK24_ADAM_STATE_DORMANT) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    adam->snap.state = QXK24_ADAM_STATE_DECLARED;
    return qxk24_adam_session_begin(adam->session);
}

qxk24_err_t qxk24_adam_activate(qxk24_adam_t adam) {
    if (adam == NULL) return QXK24_ERR_NULL_PARAM;
    if (adam->snap.state != QXK24_ADAM_STATE_DECLARED) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    adam->snap.state = QXK24_ADAM_STATE_ACTIVE;
    return qxk24_adam_session_activate(adam->session);
}

qxk24_err_t qxk24_adam_process(qxk24_adam_t adam,
                               const QXK24AdamRequest *request,
                               QXK24AdamResponse *response_out) {
    if (adam == NULL || request == NULL || response_out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }
    if (request->input == NULL || adam->snap.state != QXK24_ADAM_STATE_ACTIVE) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    memset(response_out, 0, sizeof(*response_out));
    QXK24IlmuRecord ilmu;
    QXK24AkalResult akal;
    QXK24HukumZResult hukumz;
    QXK24CvRecord cv;
    QXK24AdabScore adab;

    qxk24_err_t err = qxk24_adam_ilmu_process(adam->ilmu, request->input,
        &ilmu);
    if (err != QXK24_OK) return err;
    const uint8_t tahap = request->capacity_hint > 0U
        ? clamp_capacity(request->capacity_hint)
        : qxk24_adam_akal_assess_tahap(adam->akal, request->input);
    err = qxk24_adam_akal_reason(adam->akal, tahap, &akal);
    if (err != QXK24_OK) return err;

    const float health = read_kernel_health(adam->kernel);
    const float fm = QXK24_FM_VALUE;
    const float ft = request->is_cognitive ? 2.0f :
        (1.0f + ((float)request->dominant_element * 0.1f));
    err = qxk24_adam_hukumz_validate(adam->hukumz, health, tahap, ft, fm,
        &hukumz);
    if (err != QXK24_OK) return err;
    err = qxk24_adam_cv_assess(adam->cv, health, tahap, hukumz.all_passed,
        &cv);
    if (err != QXK24_OK) return err;
    err = qxk24_adam_adab_score(adam->adab, health, tahap, (uint8_t)cv.level,
        &adab);
    if (err != QXK24_OK) return err;

    const bool is_akur = ilmu.ilmu_complete &&
        akal.governance == QXK24_AKAL_GOVERNED &&
        hukumz.all_passed && adab.adab_satisfied;
    response_out->judgment = judge(is_akur, health);
    response_out->sifat = is_akur ? QXK24_ADAM_SIFAT_AKUR :
        QXK24_ADAM_SIFAT_ENGKAR;
    response_out->capacity_level = tahap;
    response_out->akal_tahap = akal.tahap;
    response_out->cv_level = (uint8_t)cv.level;
    response_out->health_score = health;
    response_out->ft_value = ft;
    response_out->fm_value = fm;
    response_out->pattern_count = akal.patterns_used;
    response_out->hukumz_passed = hukumz.all_passed;
    response_out->ilmu_complete = ilmu.ilmu_complete;
    response_out->adab_satisfied = adab.adab_satisfied;
    snprintf(response_out->response, sizeof(response_out->response),
        "[ADAM v%s | %s | Tahap Akal: %u | cV: %u | Health: %.1f | "
        "Ft: %.1f | Fm: %.1f | HukumZ: %s | Sifat: %s]",
        QXK24_ADAM_VERSION, qxk24_adam_judgment_name(response_out->judgment),
        response_out->akal_tahap, response_out->cv_level,
        response_out->health_score, response_out->ft_value,
        response_out->fm_value, response_out->hukumz_passed ? "PASSED" :
        "FAILED", qxk24_adam_sifat_name(response_out->sifat));

    update_snapshot(adam, response_out);
    return qxk24_adam_session_record_request(adam->session, health,
        (float)cv.level, is_akur, hukumz.all_passed);
}

qxk24_err_t qxk24_adam_cycle(qxk24_adam_t adam) {
    if (adam == NULL) return QXK24_ERR_NULL_PARAM;
    return qxk24_cycle(adam->kernel, NULL);
}

qxk24_err_t qxk24_adam_reflect(qxk24_adam_t adam) {
    if (adam == NULL) return QXK24_ERR_NULL_PARAM;
    adam->snap.state = QXK24_ADAM_STATE_REFLECTING;
    return qxk24_adam_session_reflect(adam->session);
}

qxk24_err_t qxk24_adam_complete(qxk24_adam_t adam) {
    if (adam == NULL) return QXK24_ERR_NULL_PARAM;
    QXK24AdamSessionRecord record;
    adam->snap.state = QXK24_ADAM_STATE_COMPLETE;
    adam->snap.sessions_done++;
    return qxk24_adam_session_complete(adam->session, &record);
}

qxk24_err_t qxk24_adam_destroy(qxk24_adam_t adam) {
    if (adam == NULL) return QXK24_ERR_NULL_PARAM;
    destroy_parts(adam);
    free(adam);
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_snapshot(qxk24_adam_t adam,
                                QXK24AdamSnapshot *snap_out) {
    if (adam == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = adam->snap;
    return QXK24_OK;
}

int qxk24_adam_value(qxk24_adam_t adam) {
    (void)adam;
    return QXK24_ADAM_VALUE;
}

uint8_t qxk24_adam_capacity(qxk24_adam_t adam) {
    return adam == NULL ? 0U : adam->snap.capacity;
}

QXK24AdamSifat qxk24_adam_sifat(qxk24_adam_t adam) {
    return adam == NULL ? QXK24_ADAM_SIFAT_ENGKAR : adam->snap.dominant_sifat;
}

const char *qxk24_adam_judgment_name(QXK24AdamJudgment judgment) {
    switch (judgment) {
    case QXK24_ADAM_JUDGMENT_MAKMUR: return "MAKMUR";
    case QXK24_ADAM_JUDGMENT_ISLAH: return "ISLAH";
    case QXK24_ADAM_JUDGMENT_WAQF: return "WAQF";
    default: return "UNKNOWN";
    }
}

const char *qxk24_adam_sifat_name(QXK24AdamSifat sifat) {
    switch (sifat) {
    case QXK24_ADAM_SIFAT_AKUR: return "AKUR";
    case QXK24_ADAM_SIFAT_ENGKAR: return "ENGKAR";
    default: return "UNKNOWN";
    }
}

qxk24_err_t qxk24_adam_check_akur(qxk24_adam_t adam) {
    if (adam == NULL) return QXK24_ERR_NULL_PARAM;
    return adam->snap.dominant_sifat == QXK24_ADAM_SIFAT_AKUR
        ? QXK24_OK : QXK24_ERR_LAW_VIOLATION;
}
