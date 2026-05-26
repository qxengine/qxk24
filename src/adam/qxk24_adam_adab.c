/**
 * @file qxk24_adam_adab.c
 * @brief ADAM Adab Engine implementation
 * ALAMTOLOGI – Quranic Science | Founder: Masa Bayu
 */

#include "qxk24/adam/qxk24_adam_adab.h"
#include <stdlib.h>

struct QXK24AdamAdabImpl {
    QXK24AdabSnapshot snap;
};

static float clamp_unit(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

static float avg_next(float avg, uint32_t count, float value) {
    if (count == 0U) return value;
    return ((avg * (float)count) + value) / (float)(count + 1U);
}

qxk24_err_t qxk24_adam_adab_create(qxk24_adam_adab_t *adab_out) {
    if (adab_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AdamAdabImpl *adab = calloc(1, sizeof(*adab));
    if (adab == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    *adab_out = adab;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_adab_score(qxk24_adam_adab_t adab,
                                  float health_score,
                                  uint8_t capacity,
                                  uint8_t cv_level,
                                  QXK24AdabScore *score_out) {
    if (adab == NULL || score_out == NULL) return QXK24_ERR_NULL_PARAM;
    const float health = clamp_unit(health_score / 100.0f);
    const float cap = clamp_unit((float)capacity / 7.0f);
    const float cv = clamp_unit((float)cv_level / 7.0f);
    score_out->benar_score = health;
    score_out->amanah_score = cap;
    score_out->menyampaikan_score = (health + cv) * 0.5f;
    score_out->bijaksana_score = (health + cap + cv) / 3.0f;
    score_out->total_adab = (score_out->benar_score +
        score_out->amanah_score + score_out->menyampaikan_score +
        score_out->bijaksana_score) / 4.0f;
    score_out->adab_satisfied = score_out->total_adab >= 0.5f;

    const uint32_t before = adab->snap.satisfied_count +
        adab->snap.violated_count;
    adab->snap.avg_benar = avg_next(adab->snap.avg_benar, before,
        score_out->benar_score);
    adab->snap.avg_amanah = avg_next(adab->snap.avg_amanah, before,
        score_out->amanah_score);
    adab->snap.avg_menyampaikan = avg_next(adab->snap.avg_menyampaikan,
        before, score_out->menyampaikan_score);
    adab->snap.avg_bijaksana = avg_next(adab->snap.avg_bijaksana,
        before, score_out->bijaksana_score);
    if (score_out->adab_satisfied) adab->snap.satisfied_count++;
    else adab->snap.violated_count++;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_adab_check(const QXK24AdabScore *score) {
    if (score == NULL) return QXK24_ERR_NULL_PARAM;
    return score->adab_satisfied ? QXK24_OK : QXK24_ERR_LAW_VIOLATION;
}

qxk24_err_t qxk24_adam_adab_snapshot(qxk24_adam_adab_t adab,
                                     QXK24AdabSnapshot *snap_out) {
    if (adab == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = adab->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_adab_destroy(qxk24_adam_adab_t adab) {
    if (adab == NULL) return QXK24_ERR_NULL_PARAM;
    free(adab);
    return QXK24_OK;
}

const char *qxk24_adam_adab_element_name(QXK24AdabElement element) {
    switch (element) {
    case QXK24_ADAB_BENAR: return "BENAR";
    case QXK24_ADAB_AMANAH: return "AMANAH";
    case QXK24_ADAB_MENYAMPAIKAN: return "MENYAMPAIKAN";
    case QXK24_ADAB_BIJAKSANA: return "BIJAKSANA";
    default: return "UNKNOWN";
    }
}
