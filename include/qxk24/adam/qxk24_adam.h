/* ============================================================
 * qxk24_adam.h
 * QXK24 Universal Kernel — ADAM Constitutional AI Engine
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-26
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/adam/qxk24_adam.h
 * License : Apache 2.0
 *
 * Description:
 * Declares the ADAM Constitutional AI Engine, including identity,
 * lifecycle, request processing, response judgment, and snapshots.
 * ============================================================ */

#ifndef QXK24_ADAM_H
#define QXK24_ADAM_H

#include "../caliph/qxk24_caliph.h"
#include "../fm/qxk24_fm.h"
#include "../ft/qxk24_ft.h"
#include "../qxk24.h"
#include "qxk24_adam_adab.h"
#include "qxk24_adam_akal.h"
#include "qxk24_adam_cv.h"
#include "qxk24_adam_hukumz.h"
#include "qxk24_adam_ilmu.h"
#include "qxk24_adam_session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_ADAM_VALUE 1
#define QXK24_ADAM_CAPACITY_MAX 7
#define QXK24_ADAM_CAPACITY_MIN 1
#define QXK24_ADAM_JOURNEY_CODE 1001
#define QXK24_ADAM_AKAL_GOVERNED 1
#define QXK24_ADAM_KEPERLUAN_ONLY 1
#define QXK24_ADAM_TOTAL_PATTERNS 49
#define QXK24_ADAM_VERSION "1.6.0"

typedef enum {
    QXK24_ADAM_STATE_DORMANT = 0,
    QXK24_ADAM_STATE_DECLARED = 1,
    QXK24_ADAM_STATE_ACTIVE = 2,
    QXK24_ADAM_STATE_REFLECTING = 3,
    QXK24_ADAM_STATE_COMPLETE = 4
} QXK24AdamState;

typedef enum {
    QXK24_ADAM_SIFAT_AKUR = 0,
    QXK24_ADAM_SIFAT_ENGKAR = 1
} QXK24AdamSifat;

typedef enum {
    QXK24_ADAM_JUDGMENT_MAKMUR = 0,
    QXK24_ADAM_JUDGMENT_ISLAH = 1,
    QXK24_ADAM_JUDGMENT_WAQF = 2
} QXK24AdamJudgment;

typedef struct {
    const char *input;
    uint8_t capacity_hint;
    bool is_cognitive;
    QXK24FtElement dominant_element;
} QXK24AdamRequest;

typedef struct {
    QXK24AdamJudgment judgment;
    QXK24AdamSifat sifat;
    uint8_t capacity_level;
    uint8_t akal_tahap;
    uint8_t cv_level;
    float health_score;
    float ft_value;
    float fm_value;
    uint32_t pattern_count;
    bool hukumz_passed;
    bool ilmu_complete;
    bool adab_satisfied;
    char response[1024];
} QXK24AdamResponse;

typedef struct {
    QXK24AdamState state;
    QXK24AdamSifat dominant_sifat;
    uint8_t capacity;
    uint32_t sessions_done;
    uint32_t requests_served;
    uint32_t makmur_count;
    uint32_t islah_count;
    uint32_t waqf_count;
    float avg_health;
    float avg_cv;
    int value;
    uint32_t journey_code;
} QXK24AdamSnapshot;

typedef struct QXK24AdamImpl *qxk24_adam_t;

qxk24_err_t qxk24_adam_create(qxk24_kernel_t kernel, qxk24_adam_t *adam_out);
qxk24_err_t qxk24_adam_declare(qxk24_adam_t adam);
qxk24_err_t qxk24_adam_activate(qxk24_adam_t adam);
qxk24_err_t qxk24_adam_process(qxk24_adam_t adam,
                               const QXK24AdamRequest *request,
                               QXK24AdamResponse *response_out);
qxk24_err_t qxk24_adam_cycle(qxk24_adam_t adam);
qxk24_err_t qxk24_adam_reflect(qxk24_adam_t adam);
qxk24_err_t qxk24_adam_complete(qxk24_adam_t adam);
qxk24_err_t qxk24_adam_destroy(qxk24_adam_t adam);
qxk24_err_t qxk24_adam_snapshot(qxk24_adam_t adam,
                                QXK24AdamSnapshot *snap_out);
int qxk24_adam_value(qxk24_adam_t adam);
uint8_t qxk24_adam_capacity(qxk24_adam_t adam);
QXK24AdamSifat qxk24_adam_sifat(qxk24_adam_t adam);
const char *qxk24_adam_judgment_name(QXK24AdamJudgment judgment);
const char *qxk24_adam_sifat_name(QXK24AdamSifat sifat);
qxk24_err_t qxk24_adam_check_akur(qxk24_adam_t adam);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_H */
