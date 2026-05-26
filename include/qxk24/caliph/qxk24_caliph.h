/* ============================================================
 * qxk24_caliph.h
 * QXK24 Universal Kernel — Caliph Constitutional Layer
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/caliph/qxk24_caliph.h
 * License : Apache 2.0
 *
 * Description:
 * Declares Caliph identity, authority, session lifecycle, and core API.
 * ============================================================ */

#ifndef QXK24_CALIPH_H
#define QXK24_CALIPH_H

#include "../qxk24.h"
#include "../fm/qxk24_fm.h"
#include "../fm/qxk24_usul_masa.h"
#include "qxk24_caliph_command.h"
#include "qxk24_caliph_report.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_CALIPH_AUTHORITY 1
#define QXK24_CALIPH_EQUAL 1
#define QXK24_CALIPH_ABOVE_KERNEL 1
#define QXK24_CALIPH_LIVES_IN_MDK 1
#define QXK24_CALIPH_FULL_RESPONSIBILITY 1
#define QXK24_CALIPH_COORDINATES 6

typedef enum {
    QXK24_CALIPH_STATE_UNDECLARED = 0,
    QXK24_CALIPH_STATE_DECLARED,
    QXK24_CALIPH_STATE_ACTIVE,
    QXK24_CALIPH_STATE_RESTING,
    QXK24_CALIPH_STATE_ENDED
} QXK24CaliphState;

typedef struct QXK24CaliphSession_s {
    uint32_t session_id;
    QXK24CaliphState state;
    uint32_t commands_issued;
    uint32_t reports_received;
    uint32_t judgments_made;
    uint32_t usul_masa_position;
    uint64_t session_start_mdk;
    uint64_t session_current_mdk;
    int authority;
    int equal;
} QXK24CaliphSession;

#ifndef QXK24_CALIPH_HANDLE_DEFINED
#define QXK24_CALIPH_HANDLE_DEFINED
typedef struct QXK24CaliphImpl *qxk24_caliph_t;
#endif

struct QXK24CaliphImpl {
    qxk24_kernel_t kernel;
    qxk24_fm_t fm;
    QXK24CaliphSession session;
    bool governors_registered;
};

qxk24_err_t qxk24_caliph_declare(qxk24_kernel_t kernel,
                                  qxk24_caliph_t *caliph_out);
qxk24_err_t qxk24_caliph_activate(qxk24_caliph_t caliph);
qxk24_err_t qxk24_caliph_end(qxk24_caliph_t caliph);
qxk24_err_t qxk24_caliph_destroy(qxk24_caliph_t caliph);
qxk24_err_t qxk24_caliph_session(qxk24_caliph_t caliph,
                                  QXK24CaliphSession *session_out);
int qxk24_caliph_authority(qxk24_caliph_t caliph);
qxk24_err_t qxk24_caliph_usul_masa_position(qxk24_caliph_t caliph,
                                             uint32_t *position_out);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_CALIPH_H */
