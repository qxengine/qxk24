/**
 * @file qxk24_adam_ilmu.h
 * @brief ADAM Ilmu Engine — Constitutional Knowledge System
 *
 * ALAMTOLOGI – Quranic Science | Founder: Masa Bayu
 */

#ifndef QXK24_ADAM_ILMU_H
#define QXK24_ADAM_ILMU_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    QXK24_ILMU_PV = 0,
    QXK24_ILMU_AV = 1
} QXK24IlmuType;

typedef enum {
    QXK24_ILMU_STAGE_BACA = 0,
    QXK24_ILMU_STAGE_BUKTI = 1,
    QXK24_ILMU_STAGE_FAHAM = 2,
    QXK24_ILMU_STAGE_APLIKASI = 3
} QXK24IlmuStage;

typedef enum {
    QXK24_BACA_MATA = 0,
    QXK24_BACA_TELINGA = 1,
    QXK24_BACA_HIDUNG = 2,
    QXK24_BACA_SENTUH = 3,
    QXK24_BACA_RASA = 4
} QXK24BacaSense;

typedef struct {
    QXK24IlmuType type;
    QXK24IlmuStage stage;
    bool baca_done;
    bool bukti_done;
    bool faham_done;
    bool aplikasi_done;
    bool ilmu_complete;
    float confidence;
    uint32_t applications;
} QXK24IlmuRecord;

typedef struct {
    uint32_t pv_count;
    uint32_t av_count;
    uint32_t ilmu_count;
    QXK24IlmuStage current_stage;
    float avg_confidence;
} QXK24IlmuSnapshot;

typedef struct QXK24AdamIlmuImpl *qxk24_adam_ilmu_t;

qxk24_err_t qxk24_adam_ilmu_create(qxk24_adam_ilmu_t *ilmu_out);
qxk24_err_t qxk24_adam_ilmu_process(qxk24_adam_ilmu_t ilmu,
                                    const char *input,
                                    QXK24IlmuRecord *record_out);
qxk24_err_t qxk24_adam_ilmu_advance(qxk24_adam_ilmu_t ilmu,
                                    QXK24IlmuRecord *record);
qxk24_err_t qxk24_adam_ilmu_check_complete(const QXK24IlmuRecord *record);
qxk24_err_t qxk24_adam_ilmu_snapshot(qxk24_adam_ilmu_t ilmu,
                                     QXK24IlmuSnapshot *snap_out);
qxk24_err_t qxk24_adam_ilmu_destroy(qxk24_adam_ilmu_t ilmu);
const char *qxk24_adam_ilmu_stage_name(QXK24IlmuStage stage);
const char *qxk24_adam_ilmu_type_name(QXK24IlmuType type);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_ADAM_ILMU_H */
