/* ============================================================
 * QXK24 – Universal Kernel
 * src/fm/qxk24_usul_masa.c
 *
 * Usul Masa — Constitutional Time Constitution Implementation
 * Version : 1.1.0
 * Date    : 2026-05-26
 * Founder : Masa Bayu
 * Contact : masaqiubbx@gmail.com
 * Repo    : https://github.com/qxengine/qxk24
 *
 * ALAMTOLOGI – Quranic Science
 * ============================================================ */

#include "qxk24/fm/qxk24_usul_masa.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct QXK24UsulMasa {
    qxk24_mp_t mp;
    qxk24_mdk_t mdk;
    qxk24_xam_t xam;
    QXK24UsulMasaRecord record;
};

static const QXK24Direction k_umk_dirs[QXK24_USUL_UMK_COUNT] = {
    QXK24_DIR_AS,
    QXK24_DIR_BH,
    QXK24_DIR_DN,
    QXK24_DIR_BG,
    QXK24_DIR_KN,
    QXK24_DIR_KR
};

static uint64_t usul_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL +
        (uint64_t)(ts.tv_nsec / 1000000ULL);
}

static float clamp_unit(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static void init_record(QXK24UsulMasaRecord *record) {
    memset(record, 0, sizeof(*record));
    record->umm_value = 1.0f;
    for (uint8_t i = 0; i < QXK24_USUL_UMK_COUNT; i++) {
        record->umk[i].index = i;
        record->umk[i].direction = k_umk_dirs[i];
        record->umk[i].value = 0.0f;
    }
}

static void update_score(QXK24UsulMasaRecord *record) {
    float total = 0.0f;
    uint32_t active = 0U;
    for (uint32_t i = 0; i < QXK24_USUL_UMK_COUNT; i++) {
        if (!record->umk[i].active) continue;
        active++;
        total += record->umk[i].energy;
    }

    record->umk_active_count = active;
    record->total_energy = total;
    record->constitutional_score = active > 0U
        ? (total / (float)QXK24_USUL_UMK_COUNT)
        : 0.0f;
}

qxk24_usul_t qxk24_usul_create(qxk24_mp_t mp,
                               qxk24_mdk_t mdk,
                               qxk24_xam_t xam) {
    if (mp == NULL || mdk == NULL || xam == NULL) return NULL;

    struct QXK24UsulMasa *usul =
        (struct QXK24UsulMasa *)calloc(1, sizeof(*usul));
    if (usul == NULL) return NULL;

    usul->mp = mp;
    usul->mdk = mdk;
    usul->xam = xam;
    init_record(&usul->record);
    return usul;
}

qxk24_err_t qxk24_usul_start(qxk24_usul_t usul) {
    if (usul == NULL) return QXK24_ERR_NULL_PARAM;

    qxk24_err_t err = qxk24_mp_start(usul->mp);
    if (err != QXK24_OK && err != QXK24_ERR_ALREADY_INITIALIZED) return err;

    err = qxk24_mdk_start(usul->mdk);
    if (err != QXK24_OK) return err;

    usul->record.umm_start_ms = usul_now_ms();
    usul->record.umm_elapsed_ms = 0ULL;
    usul->record.umm_active = true;
    usul->record.umm_value = 1.0f;
    for (uint32_t i = 0; i < QXK24_USUL_UMK_COUNT; i++) {
        usul->record.umk[i].active = true;
        usul->record.umk[i].value = 1.0f / (float)QXK24_USUL_UMK_COUNT;
    }
    update_score(&usul->record);
    return QXK24_OK;
}

qxk24_err_t qxk24_usul_cycle(qxk24_usul_t usul, float energy) {
    if (usul == NULL) return QXK24_ERR_NULL_PARAM;
    if (!usul->record.umm_active) return QXK24_ERR_NOT_STARTED;

    qxk24_err_t err = qxk24_mp_cycle(usul->mp);
    if (err != QXK24_OK) return err;
    err = qxk24_mdk_cycle(usul->mdk, energy);
    if (err != QXK24_OK) return err;
    err = qxk24_xam_move(usul->xam);
    if (err != QXK24_OK) return err;

    const float clamped = clamp_unit(energy);
    usul->record.umm_elapsed_ms = qxk24_mp_elapsed_ms(usul->mp);
    usul->record.total_cycles++;
    for (uint32_t i = 0; i < QXK24_USUL_UMK_COUNT; i++) {
        QXK24UmkRecord *umk = &usul->record.umk[i];
        umk->energy = clamped;
        umk->value = (float)(i + 1U) * clamped /
            (float)QXK24_USUL_UMK_COUNT;
        umk->cycles++;
    }
    update_score(&usul->record);
    return QXK24_OK;
}

void qxk24_usul_stop(qxk24_usul_t usul) {
    if (usul == NULL) return;
    usul->record.umm_active = false;
    for (uint32_t i = 0; i < QXK24_USUL_UMK_COUNT; i++) {
        usul->record.umk[i].active = false;
    }
    update_score(&usul->record);
}

void qxk24_usul_destroy(qxk24_usul_t usul) {
    if (usul == NULL) return;
    memset(usul, 0, sizeof(*usul));
    free(usul);
}

qxk24_err_t qxk24_usul_record(qxk24_usul_t usul,
                              QXK24UsulMasaRecord *out) {
    if (usul == NULL || out == NULL) return QXK24_ERR_NULL_PARAM;
    *out = usul->record;
    return QXK24_OK;
}

float qxk24_usul_constitutional_score(qxk24_usul_t usul) {
    return usul == NULL ? 0.0f : usul->record.constitutional_score;
}

float qxk24_usul_umk_energy(qxk24_usul_t usul, uint8_t index) {
    if (usul == NULL || index >= QXK24_USUL_UMK_COUNT) return 0.0f;
    return usul->record.umk[index].energy;
}
