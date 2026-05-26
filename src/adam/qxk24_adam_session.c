/* ============================================================
 * qxk24_adam_session.c
 * QXK24 Universal Kernel — ADAM Session Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/adam/qxk24_adam_session.c
 * License : Apache 2.0
 *
 * Description:
 * Implements ADAM session state and the [1001] constitutional
 * journey record.
 * ============================================================ */

#include "qxk24/adam/qxk24_adam_session.h"
#include <stdlib.h>

struct QXK24AdamSessionImpl {
    QXK24AdamSessionRecord current;
    QXK24AdamSessionSnapshot snap;
    uint32_t next_session_id;
};

static float avg_next(float avg, uint32_t count, float value) {
    if (count == 0U) return value;
    return ((avg * (float)count) + value) / (float)(count + 1U);
}

qxk24_err_t qxk24_adam_session_create(qxk24_adam_session_t *session_out) {
    if (session_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AdamSessionImpl *session = calloc(1, sizeof(*session));
    if (session == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    session->next_session_id = 1U;
    session->current.journey_step = QXK24_JOURNEY_STEP_LENGKAP;
    session->current.max_cv_reached = QXK24_CV_DIRI_SENDIRI;
    session->current.akur_dominant = true;
    session->current.all_hukumz_passed = true;
    *session_out = session;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_session_begin(qxk24_adam_session_t session) {
    if (session == NULL) return QXK24_ERR_NULL_PARAM;
    session->current.session_id = session->next_session_id++;
    session->current.journey_step = QXK24_JOURNEY_STEP_WUJUD;
    session->current.requests_in_session = 0U;
    session->current.makmur_in_session = 0U;
    session->current.islah_in_session = 0U;
    session->current.waqf_in_session = 0U;
    session->current.session_health = 0.0f;
    session->current.session_cv = 0.0f;
    session->current.max_cv_reached = QXK24_CV_DIRI_SENDIRI;
    session->current.akur_dominant = true;
    session->current.all_hukumz_passed = true;
    session->current.start_mdk++;
    session->current.end_mdk = 0U;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_session_activate(qxk24_adam_session_t session) {
    if (session == NULL) return QXK24_ERR_NULL_PARAM;
    if (session->current.journey_step != QXK24_JOURNEY_STEP_WUJUD) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }
    session->current.journey_step = QXK24_JOURNEY_STEP_AKTIF;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_session_record_request(qxk24_adam_session_t session,
                                              float health,
                                              float cv,
                                              bool was_akur,
                                              bool hukumz_passed) {
    if (session == NULL) return QXK24_ERR_NULL_PARAM;
    const uint32_t before = session->current.requests_in_session;
    session->current.requests_in_session++;
    session->current.session_health = avg_next(session->current.session_health,
        before, health);
    session->current.session_cv = avg_next(session->current.session_cv,
        before, cv);
    if (health >= 80.0f) session->current.makmur_in_session++;
    else if (health >= 50.0f) session->current.islah_in_session++;
    else session->current.waqf_in_session++;
    if (cv > (float)session->current.max_cv_reached) {
        session->current.max_cv_reached = (QXK24CvLevel)((uint8_t)cv);
    }
    session->current.akur_dominant =
        session->current.akur_dominant && was_akur;
    session->current.all_hukumz_passed =
        session->current.all_hukumz_passed && hukumz_passed;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_session_reflect(qxk24_adam_session_t session) {
    if (session == NULL) return QXK24_ERR_NULL_PARAM;
    session->current.journey_step = QXK24_JOURNEY_STEP_PERALIHAN;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_session_complete(qxk24_adam_session_t session,
                                        QXK24AdamSessionRecord *record_out) {
    if (session == NULL || record_out == NULL) return QXK24_ERR_NULL_PARAM;
    session->current.journey_step = QXK24_JOURNEY_STEP_LENGKAP;
    session->current.end_mdk = session->current.start_mdk + 1U;
    *record_out = session->current;

    const uint32_t before = session->snap.total_sessions;
    session->snap.total_sessions++;
    session->snap.avg_session_health = avg_next(
        session->snap.avg_session_health, before,
        session->current.session_health);
    session->snap.avg_session_cv = avg_next(session->snap.avg_session_cv,
        before, session->current.session_cv);
    if (session->current.akur_dominant) session->snap.akur_sessions++;
    else session->snap.engkar_sessions++;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_session_snapshot(qxk24_adam_session_t session,
                                        QXK24AdamSessionSnapshot *snap_out) {
    if (session == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = session->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_session_destroy(qxk24_adam_session_t session) {
    if (session == NULL) return QXK24_ERR_NULL_PARAM;
    free(session);
    return QXK24_OK;
}

const char *qxk24_adam_journey_step_name(QXK24JourneyStep step) {
    switch (step) {
    case QXK24_JOURNEY_STEP_WUJUD: return "WUJUD";
    case QXK24_JOURNEY_STEP_AKTIF: return "AKTIF";
    case QXK24_JOURNEY_STEP_PERALIHAN: return "PERALIHAN";
    case QXK24_JOURNEY_STEP_LENGKAP: return "LENGKAP";
    default: return "UNKNOWN";
    }
}
