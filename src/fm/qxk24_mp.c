/* ============================================================
 * QXK24 – Universal Kernel
 * src/fm/qxk24_mp.c
 *
 * Masa Pengawal (MP) — Master Time Implementation
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 * ============================================================ */

#include "qxk24/fm/qxk24_mp.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct QXK24MP {
    QXK24MPType type;
    uint64_t start_ms;
    uint64_t elapsed_ms;
    float ratio;
    bool active;
    uint32_t mdk_count;
    char name[32];
};

static uint64_t mp_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
        (uint64_t)(ts.tv_nsec / 1000000ULL);
}

static qxk24_mp_t mp_create(QXK24MPType type, const char *name) {
    struct QXK24MP *mp = (struct QXK24MP *)calloc(1, sizeof(*mp));
    if (mp == NULL) return NULL;

    mp->type = type;
    mp->ratio = 1.0f;
    mp->mdk_count = QXK24_FM_MDK_COUNT;
    strncpy(mp->name, name, sizeof(mp->name) - 1U);
    return mp;
}

qxk24_mp_t qxk24_mp_create_mpi(void) {
    return mp_create(QXK24_MP_INDUK, "MPi");
}

qxk24_mp_t qxk24_mp_create_mpp(qxk24_mp_t parent, const char *name) {
    if (parent == NULL) return NULL;
    const char *label = (name == NULL || name[0] == '\0') ? "MPP" : name;
    return mp_create(QXK24_MP_PROSES, label);
}

qxk24_err_t qxk24_mp_start(qxk24_mp_t mp) {
    if (mp == NULL) return QXK24_ERR_NULL_PARAM;
    if (mp->active) return QXK24_OK;

    mp->start_ms = mp_now_ms();
    mp->elapsed_ms = 0ULL;
    mp->ratio = 1.0f;
    mp->active = true;
    return QXK24_OK;
}

qxk24_err_t qxk24_mp_cycle(qxk24_mp_t mp) {
    if (mp == NULL) return QXK24_ERR_NULL_PARAM;
    if (!mp->active) return QXK24_ERR_NOT_STARTED;

    uint64_t elapsed = mp_now_ms() - mp->start_ms;
    if (elapsed <= mp->elapsed_ms) elapsed = mp->elapsed_ms + 1ULL;
    mp->elapsed_ms = elapsed;
    mp->ratio = 1.0f;
    return QXK24_OK;
}

void qxk24_mp_stop(qxk24_mp_t mp) {
    if (mp == NULL) return;
    mp->active = false;
}

void qxk24_mp_destroy(qxk24_mp_t mp) {
    if (mp == NULL) return;
    memset(mp, 0, sizeof(*mp));
    free(mp);
}

qxk24_err_t qxk24_mp_record(qxk24_mp_t mp, QXK24MPRecord *out) {
    if (mp == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;

    memset(out, 0, sizeof(*out));
    out->type = mp->type;
    out->start_ms = mp->start_ms;
    out->elapsed_ms = mp->elapsed_ms;
    out->ratio = mp->ratio;
    out->active = mp->active;
    out->mdk_count = mp->mdk_count;
    strncpy(out->name, mp->name, sizeof(out->name) - 1U);
    return QXK24_OK;
}

float qxk24_mp_ratio(qxk24_mp_t mp) {
    return mp == NULL ? 0.0f : mp->ratio;
}

uint64_t qxk24_mp_elapsed_ms(qxk24_mp_t mp) {
    return mp == NULL ? 0ULL : mp->elapsed_ms;
}

QXK24MPType qxk24_mp_type(qxk24_mp_t mp) {
    return mp == NULL ? QXK24_MP_INDUK : mp->type;
}

bool qxk24_mp_is_active(qxk24_mp_t mp) {
    return mp != NULL && mp->active;
}
