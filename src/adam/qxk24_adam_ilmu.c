/**
 * @file qxk24_adam_ilmu.c
 * @brief ADAM Ilmu Engine implementation
 * ALAMTOLOGI – Quranic Science | Founder: Masa Bayu
 */

#include "qxk24/adam/qxk24_adam_ilmu.h"
#include <stdlib.h>
#include <string.h>

struct QXK24AdamIlmuImpl {
    QXK24IlmuSnapshot snap;
};

static float avg_next(float avg, uint32_t count, float value) {
    if (count == 0U) return value;
    return ((avg * (float)count) + value) / (float)(count + 1U);
}

qxk24_err_t qxk24_adam_ilmu_create(qxk24_adam_ilmu_t *ilmu_out) {
    if (ilmu_out == NULL) return QXK24_ERR_NULL_PARAM;
    struct QXK24AdamIlmuImpl *ilmu = calloc(1, sizeof(*ilmu));
    if (ilmu == NULL) return QXK24_ERR_OUT_OF_MEMORY;
    ilmu->snap.current_stage = QXK24_ILMU_STAGE_BACA;
    *ilmu_out = ilmu;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_ilmu_process(qxk24_adam_ilmu_t ilmu,
                                    const char *input,
                                    QXK24IlmuRecord *record_out) {
    if (ilmu == NULL || input == NULL || record_out == NULL) {
        return QXK24_ERR_NULL_PARAM;
    }

    const size_t len = strlen(input);
    const float confidence = len == 0U ? 0.5f : (len > 64U ? 1.0f : 0.75f);
    memset(record_out, 0, sizeof(*record_out));
    record_out->type = len == 0U ? QXK24_ILMU_PV : QXK24_ILMU_AV;
    record_out->stage = QXK24_ILMU_STAGE_APLIKASI;
    record_out->baca_done = true;
    record_out->bukti_done = true;
    record_out->faham_done = true;
    record_out->aplikasi_done = true;
    record_out->ilmu_complete = true;
    record_out->confidence = confidence;
    record_out->applications = 1U;

    const uint32_t before = ilmu->snap.ilmu_count;
    if (record_out->type == QXK24_ILMU_PV) ilmu->snap.pv_count++;
    else ilmu->snap.av_count++;
    ilmu->snap.ilmu_count++;
    ilmu->snap.current_stage = QXK24_ILMU_STAGE_APLIKASI;
    ilmu->snap.avg_confidence =
        avg_next(ilmu->snap.avg_confidence, before, confidence);
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_ilmu_advance(qxk24_adam_ilmu_t ilmu,
                                    QXK24IlmuRecord *record) {
    if (ilmu == NULL || record == NULL) return QXK24_ERR_NULL_PARAM;
    if (record->stage < QXK24_ILMU_STAGE_APLIKASI) {
        record->stage = (QXK24IlmuStage)((int)record->stage + 1);
    }
    record->baca_done = record->stage >= QXK24_ILMU_STAGE_BACA;
    record->bukti_done = record->stage >= QXK24_ILMU_STAGE_BUKTI;
    record->faham_done = record->stage >= QXK24_ILMU_STAGE_FAHAM;
    record->aplikasi_done = record->stage >= QXK24_ILMU_STAGE_APLIKASI;
    record->ilmu_complete = record->aplikasi_done;
    ilmu->snap.current_stage = record->stage;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_ilmu_check_complete(const QXK24IlmuRecord *record) {
    if (record == NULL) return QXK24_ERR_NULL_PARAM;
    return record->ilmu_complete ? QXK24_OK : QXK24_ERR_LAW_VIOLATION;
}

qxk24_err_t qxk24_adam_ilmu_snapshot(qxk24_adam_ilmu_t ilmu,
                                     QXK24IlmuSnapshot *snap_out) {
    if (ilmu == NULL || snap_out == NULL) return QXK24_ERR_NULL_PARAM;
    *snap_out = ilmu->snap;
    return QXK24_OK;
}

qxk24_err_t qxk24_adam_ilmu_destroy(qxk24_adam_ilmu_t ilmu) {
    if (ilmu == NULL) return QXK24_ERR_NULL_PARAM;
    free(ilmu);
    return QXK24_OK;
}

const char *qxk24_adam_ilmu_stage_name(QXK24IlmuStage stage) {
    switch (stage) {
    case QXK24_ILMU_STAGE_BACA: return "BACA";
    case QXK24_ILMU_STAGE_BUKTI: return "BUKTI";
    case QXK24_ILMU_STAGE_FAHAM: return "FAHAM";
    case QXK24_ILMU_STAGE_APLIKASI: return "APLIKASI";
    default: return "UNKNOWN";
    }
}

const char *qxk24_adam_ilmu_type_name(QXK24IlmuType type) {
    switch (type) {
    case QXK24_ILMU_PV: return "pV";
    case QXK24_ILMU_AV: return "aV";
    default: return "UNKNOWN";
    }
}
