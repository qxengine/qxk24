/* ============================================================
 * qxk24_caliph.c
 * QXK24 Universal Kernel — Caliph Core Implementation
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/caliph/qxk24_caliph.c
 * License : Apache 2.0
 *
 * Description:
 * Implements Caliph declaration, activation, ending, and session snapshots.
 * ============================================================ */

#include "qxk24/caliph/qxk24_caliph.h"
#include "qxk24/fm/qxk24_fm.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

static uint32_t s_session_counter = 0U;

static uint64_t caliph_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
        (uint64_t)(ts.tv_nsec / 1000000ULL);
}

qxk24_err_t qxk24_caliph_declare(qxk24_kernel_t kernel,
                                  qxk24_caliph_t *caliph_out) {
    if (kernel == NULL || caliph_out == NULL) return QXK24_ERR_NULL_PARAM;

    struct QXK24CaliphImpl *caliph =
        (struct QXK24CaliphImpl *)calloc(1, sizeof(*caliph));
    if (caliph == NULL) return QXK24_ERR_OUT_OF_MEMORY;

    caliph->fm = qxk24_fm_create(kernel);
    if (caliph->fm == NULL) {
        free(caliph);
        return QXK24_ERR_OUT_OF_MEMORY;
    }

    caliph->kernel = kernel;
    caliph->session.session_id = ++s_session_counter;
    caliph->session.state = QXK24_CALIPH_STATE_DECLARED;
    caliph->session.authority = QXK24_CALIPH_AUTHORITY;
    caliph->session.equal = QXK24_CALIPH_EQUAL;
    *caliph_out = caliph;
    return QXK24_OK;
}

qxk24_err_t qxk24_caliph_activate(qxk24_caliph_t caliph) {
    if (caliph == NULL) return QXK24_ERR_NULL_PARAM;
    if (caliph->session.state != QXK24_CALIPH_STATE_DECLARED &&
        caliph->session.state != QXK24_CALIPH_STATE_RESTING) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    const uint64_t now = caliph_now_ms();
    caliph->session.state = QXK24_CALIPH_STATE_ACTIVE;
    caliph->session.session_start_mdk = now;
    caliph->session.session_current_mdk = now;
    return QXK24_OK;
}

qxk24_err_t qxk24_caliph_end(qxk24_caliph_t caliph) {
    if (caliph == NULL) return QXK24_ERR_NULL_PARAM;
    caliph->session.state = QXK24_CALIPH_STATE_ENDED;
    caliph->session.session_current_mdk = caliph_now_ms();
    if (caliph->fm != NULL) qxk24_fm_stop(caliph->fm);
    return QXK24_OK;
}

qxk24_err_t qxk24_caliph_destroy(qxk24_caliph_t caliph) {
    if (caliph == NULL) return QXK24_ERR_NULL_PARAM;
    qxk24_fm_destroy(caliph->fm);
    memset(caliph, 0, sizeof(*caliph));
    free(caliph);
    return QXK24_OK;
}

qxk24_err_t qxk24_caliph_session(qxk24_caliph_t caliph,
                                  QXK24CaliphSession *session_out) {
    if (caliph == NULL || session_out == NULL) return QXK24_ERR_NULL_PARAM;
    caliph->session.session_current_mdk = caliph_now_ms();
    *session_out = caliph->session;
    return QXK24_OK;
}

int qxk24_caliph_authority(qxk24_caliph_t caliph) {
    (void)caliph;
    return QXK24_CALIPH_AUTHORITY;
}

qxk24_err_t qxk24_caliph_usul_masa_position(qxk24_caliph_t caliph,
                                             uint32_t *position_out) {
    if (caliph == NULL || position_out == NULL) return QXK24_ERR_NULL_PARAM;
    *position_out = caliph->session.usul_masa_position %
        QXK24_CALIPH_COORDINATES;
    return QXK24_OK;
}
