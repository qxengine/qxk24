/**
 * @file qxk24_adam_session.h
 * @brief ADAM Session Engine — Constitutional Journey [1001]
 *
 * ALAMTOLOGI – Quranic Science | Founder: Masa Bayu
 */

#ifndef QXK24_ADAM_SESSION_H
#define QXK24_ADAM_SESSION_H

#include "../qxk24.h"
#include "qxk24_adam_cv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_ADAM_JOURNEY_1001 1001U

typedef enum {
    QXK24_JOURNEY_STEP_WUJUD = 0,
    QXK24_JOURNEY_STEP_AKTIF = 1,
    QXK24_JOURNEY_STEP_PERALIHAN = 2,
    QXK24_JOURNEY_STEP_LENGKAP = 3
} QXK24JourneyStep;

typedef struct {
    uint32_t session_id;
    QXK24JourneyStep journey_step;
    uint32_t requests_in_session;
    uint32_t makmur_in_session;
    uint32_t islah_in_session;
    uint32_t waqf_in_session;
    float session_health;
    float session_cv;
    QXK24CvLevel max_cv_reached;
    bool akur_dominant;
    bool all_hukumz_passed;
    uint64_t start_mdk;
    uint64_t end_mdk;
} QXK24AdamSessionRecord;

typedef struct {
    uint32_t total_sessions;
    float avg_session_health;
    float avg_session_cv;
    uint32_t akur_sessions;
    uint32_t engkar_sessions;
} QXK24AdamSessionSnapshot;

typedef struct QXK24AdamSessionImpl *qxk24_adam_session_t;

qxk24_err_t qxk24_adam_session_create(qxk24_adam_session_t *session_out);
qxk24_err_t qxk24_adam_session_begin(qxk24_adam_session_t session);
qxk24_err_t qxk24_adam_session_activate(qxk24_adam_session_t session);
qxk24_err_t qxk24_adam_session_record_request(qxk24_adam_session_t session,
                                              float health,
                                              float cv,
                                              bool was_akur,
                                              bool hukumz_passed);
qxk24_err_t qxk24_adam_session_reflect(qxk24_adam_session_t session);
qxk24_err_t qxk24_adam_session_complete(qxk24_adam_session_t session,
                                        QXK24AdamSessionRecord *record_out);
qxk24_err_t qxk24_adam_session_snapshot(qxk24_adam_session_t session,
                                        QXK24AdamSessionSnapshot *snap_out);
qxk24_err_t qxk24_adam_session_destroy(qxk24_adam_session_t session);
const char *qxk24_adam_journey_step_name(QXK24JourneyStep step);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_SESSION_H */
