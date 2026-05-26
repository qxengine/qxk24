/* ============================================================
 * qxk24.h
 * QXK24 Universal Kernel — Master Public Header
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/qxk24.h
 * License : Apache 2.0
 *
 * Description:
 * Defines the complete public C contract for the QXK24 Universal Kernel:
 * constants, levels, directions, resources, flows, reports, lifecycle,
 * query APIs, and inline constitutional math helpers.
 *
 * ARCHITECTURE: K24
 *   K = 1    — The invariant constant. Never changes.
 *   2        — Leraian 2. Pairs. Passive and active.
 *   4        — Leraian 4. Four directions.
 *
 * UNIVERSAL FORMULA:
 *   X = m / t
 * ============================================================ */

#ifndef QXK24_H
#define QXK24_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_VERSION_MAJOR   1
#define QXK24_VERSION_MINOR   0
#define QXK24_VERSION_PATCH   0
#define QXK24_VERSION_TAG     "beta.1"
#define QXK24_VERSION_STRING  "1.0.0-beta.1"

#define QXK24_K                   1U
#define QXK24_LERAIAN_2           2U
#define QXK24_LERAIAN_4           4U
#define QXK24_LEVEL_COUNT         7U
#define QXK24_DIRECTION_COUNT     6U
#define QXK24_SCALE_BASE          6U
#define QXK24_AMA_BASE            2U
#define QXK24_AMA_FACTOR          4U
#define QXK24_AMA_MAX_COLUMNS     18U
#define QXK24_LAW_COUNT           8U
#define QXK24_CYCLE_INTERVAL_MS   5000U

#define QXK24_SCALE_ZARAH         1ULL
#define QXK24_SCALE_JISIM         6ULL
#define QXK24_SCALE_ATOM          36ULL
#define QXK24_SCALE_SISTEM        216ULL
#define QXK24_SCALE_OBJEK         1296ULL
#define QXK24_SCALE_AKTIF         7776ULL
#define QXK24_SCALE_MODUL         46656ULL

#define QXK24_TOTAL_ZARAH         1ULL
#define QXK24_TOTAL_JISIM         7ULL
#define QXK24_TOTAL_ATOM          43ULL
#define QXK24_TOTAL_SISTEM        259ULL
#define QXK24_TOTAL_OBJEK         1555ULL
#define QXK24_TOTAL_AKTIF         9331ULL
#define QXK24_TOTAL_MODUL         55987ULL

#define QXK24_AMA_CORE_1          8ULL
#define QXK24_AMA_CORE_2          32ULL
#define QXK24_AMA_CORE_3          128ULL
#define QXK24_AMA_CORE_4          512ULL
#define QXK24_AMA_CORE_5          2048ULL
#define QXK24_AMA_CORE_6          8192ULL
#define QXK24_AMA_CORE_7          32768ULL

#define QXK24_AMA_TOTAL_1         22ULL
#define QXK24_AMA_TOTAL_2         86ULL
#define QXK24_AMA_TOTAL_3         342ULL
#define QXK24_AMA_TOTAL_4         1366ULL
#define QXK24_AMA_TOTAL_5         5462ULL
#define QXK24_AMA_TOTAL_6         21846ULL
#define QXK24_AMA_TOTAL_7         87382ULL

#define QXK24_BASE_UNIT_BYTES     (1ULL * 1024ULL * 1024ULL)
#define QXK24_HARD_RATIO_NUM      3U
#define QXK24_HARD_RATIO_DEN      2U

#define QXK24_SCORE_SOVEREIGN     95.0
#define QXK24_SCORE_PROFESSIONAL  80.0
#define QXK24_SCORE_STANDARD      60.0

#define QXK24_WEIGHT_Z1           0.20
#define QXK24_WEIGHT_Z2           0.20
#define QXK24_WEIGHT_Z3           0.15
#define QXK24_WEIGHT_Z4           0.15
#define QXK24_WEIGHT_X1           0.12
#define QXK24_WEIGHT_X2           0.10
#define QXK24_WEIGHT_X3           0.05
#define QXK24_WEIGHT_X4           0.03

#define QXK24_DEFAULT_DECLARED_X       1.0
#define QXK24_DEFAULT_TOLERANCE_PCT    15.0
#define QXK24_MAX_RESOURCES            256U
#define QXK24_MAX_ACTIVE_FLOWS         512U
#define QXK24_MAX_LABEL_LEN            64U
#define QXK24_MAX_LOG_LEN              512U

#define QXK24_OK                       0
#define QXK24_ERR_INVALID_ARGUMENT    -1
#define QXK24_ERR_OUT_OF_MEMORY       -2
#define QXK24_ERR_NOT_INITIALIZED     -3
#define QXK24_ERR_ALREADY_INITIALIZED -4
#define QXK24_ERR_BUDGET_EXCEEDED     -5
#define QXK24_ERR_LEVEL_MISMATCH      -6
#define QXK24_ERR_DIRECTION_FAULT     -7
#define QXK24_ERR_FLOW_VIOLATION      -8
#define QXK24_ERR_LAW_VIOLATION       -9
#define QXK24_ERR_NOT_FOUND           -10
#define QXK24_ERR_CAPACITY_FULL       -11
#define QXK24_ERR_NULL_PARAM          QXK24_ERR_INVALID_ARGUMENT
#define QXK24_ERR_CAPACITY            QXK24_ERR_CAPACITY_FULL
#define QXK24_ERR_NOT_STARTED         QXK24_ERR_NOT_INITIALIZED
#define QXK24_ERR_FAULT               QXK24_ERR_LAW_VIOLATION

typedef enum QXK24Level_e {
    QXK24_LEVEL_ZARAH = 0,
    QXK24_LEVEL_JISIM = 1,
    QXK24_LEVEL_ATOM = 2,
    QXK24_LEVEL_SISTEM = 3,
    QXK24_LEVEL_OBJEK = 4,
    QXK24_LEVEL_AKTIF = 5,
    QXK24_LEVEL_MODUL = 6
} QXK24Level;

typedef enum QXK24Direction_e {
    QXK24_DIR_ATAS = 0,
    QXK24_DIR_BAWAH = 1,
    QXK24_DIR_DEPAN = 2,
    QXK24_DIR_BELAKANG = 3,
    QXK24_DIR_KANAN = 4,
    QXK24_DIR_KIRI = 5
} QXK24Direction;

#define QXK24_DIR_AS QXK24_DIR_ATAS
#define QXK24_DIR_BH QXK24_DIR_BAWAH
#define QXK24_DIR_DN QXK24_DIR_DEPAN
#define QXK24_DIR_BG QXK24_DIR_BELAKANG
#define QXK24_DIR_KN QXK24_DIR_KANAN
#define QXK24_DIR_KR QXK24_DIR_KIRI

typedef enum QXK24FlowType_e {
    QXK24_FLOW_OAT_SEHALA = 0,
    QXK24_FLOW_OAT_PUSINGAN = 1,
    QXK24_FLOW_OASS_SEHALA = 2,
    QXK24_FLOW_OASS_PUSINGAN = 3
} QXK24FlowType;

typedef enum QXK24FlowDir_e {
    QXK24_FLOW_IKJ = 0,
    QXK24_FLOW_LWJ = 1
} QXK24FlowDir;

typedef enum QXK24CertTier_e {
    QXK24_CERT_NONE = 0,
    QXK24_CERT_STANDARD = 1,
    QXK24_CERT_PROFESSIONAL = 2,
    QXK24_CERT_SOVEREIGN = 3
} QXK24CertTier;

#define QXK24_CERT_UNCERTIFIED QXK24_CERT_NONE

typedef enum QXK24ResourceType_e {
    QXK24_RESOURCE_MEMORY = 0,
    QXK24_RESOURCE_CPU = 1,
    QXK24_RESOURCE_NETWORK = 2,
    QXK24_RESOURCE_STORAGE = 3,
    QXK24_RESOURCE_IO = 4,
    QXK24_RESOURCE_ENERGY = 5
} QXK24ResourceType;

typedef struct QXK24AMAPattern_s {
    uint32_t level;
    uint32_t column_count;
    uint64_t columns[QXK24_AMA_MAX_COLUMNS];
    uint64_t core_value;
    uint64_t total_boxes;
    uint64_t soft_budget_bytes;
    uint64_t hard_budget_bytes;
} QXK24AMAPattern;

typedef struct QXK24LevelDesc_s {
    QXK24Level level;
    const char *code;
    const char *name;
    const char *name_en;
    uint64_t scale;
    uint64_t total;
    const char *digital;
} QXK24LevelDesc;

typedef struct QXK24DirectionDesc_s {
    QXK24Direction direction;
    const char *code;
    const char *name;
    const char *name_en;
    const char *role;
} QXK24DirectionDesc;

typedef struct QXK24Resource_s {
    uint32_t resource_id;
    QXK24ResourceType type;
    QXK24Direction direction;
    QXK24Level level;
    uint64_t budget_soft;
    uint64_t budget_hard;
    uint64_t used_bytes;
    double x_measured;
    double x_declared;
    double tolerance_pct;
    uint64_t flow_count;
    uint64_t violation_count;
    uint64_t timestamp_registered_ms;
    uint64_t timestamp_last_update_ms;
    bool active;
} QXK24Resource;

typedef struct QXK24Flow_s {
    uint64_t flow_id;
    QXK24FlowType type;
    QXK24FlowDir direction;
    uint32_t source_resource_id;
    uint32_t dest_resource_id;
    uint64_t bytes;
    uint64_t timestamp_start_ms;
    uint64_t timestamp_end_ms;
    bool completed;
    bool active;
} QXK24Flow;

typedef struct QXK24LawScore_s {
    double z1_pattern;
    double z2_limit;
    double z3_pairs;
    double z4_equilibrium;
    double x1_knowledge;
    double x2_ethics;
    double x3_creativity;
    double x4_economy;
} QXK24LawScore;

typedef struct QXK24Report_s {
    uint64_t cycle_index;
    uint64_t timestamp_ms;
    QXK24LawScore law_scores;
    double health_score;
    QXK24CertTier cert_tier;
    uint32_t active_resources;
    uint32_t active_flows;
    uint64_t total_bytes_governed;
    double global_utilisation;
    char log[QXK24_MAX_LOG_LEN];
} QXK24Report;

typedef struct QXK24Config_s {
    uint64_t physical_ram_bytes;
    uint32_t luman_peringkat;
    double declared_x;
    double tolerance_pct;
    char kernel_label[QXK24_MAX_LABEL_LEN];
} QXK24Config;

typedef struct QXK24Kernel_s *QXK24KernelHandle;
typedef int32_t qxk24_err_t;
typedef QXK24KernelHandle qxk24_kernel_t;

static inline QXK24Config qxk24_default_config(void) {
    QXK24Config cfg = {
        8ULL * 1024ULL * 1024ULL * 1024ULL,
        4U,
        QXK24_DEFAULT_DECLARED_X,
        QXK24_DEFAULT_TOLERANCE_PCT,
        "qxk24-default"
    };
    return cfg;
}

int32_t qxk24_create(const QXK24Config *config, QXK24KernelHandle *out_handle);
int32_t qxk24_start(QXK24KernelHandle handle);
int32_t qxk24_cycle(QXK24KernelHandle handle, QXK24Report *out_report);
void qxk24_stop(QXK24KernelHandle handle);
void qxk24_destroy(QXK24KernelHandle handle);

int32_t qxk24_ama_compute(uint32_t level, QXK24AMAPattern *out_pattern);
uint64_t qxk24_ama_column_value(uint32_t level, uint32_t column_index);
uint64_t qxk24_ama_total_boxes(uint32_t level);
uint64_t qxk24_ama_core_value(uint32_t level);

int32_t qxk24_level_describe(QXK24Level level, QXK24LevelDesc *out_desc);
uint64_t qxk24_level_scale(QXK24Level level);
uint64_t qxk24_level_total(QXK24Level level);
uint64_t qxk24_level_budget(QXK24Level level);

int32_t qxk24_direction_describe(QXK24Direction dir,
                                 QXK24DirectionDesc *out_desc);
QXK24Direction qxk24_direction_pair(QXK24Direction dir);

int32_t qxk24_resource_register(QXK24KernelHandle handle,
                                QXK24ResourceType type,
                                QXK24Direction direction,
                                QXK24Level level,
                                uint64_t budget_soft,
                                uint32_t *out_resource_id);
int32_t qxk24_resource_update(QXK24KernelHandle handle,
                              uint32_t resource_id,
                              uint64_t used_bytes,
                              double x_measured);
int32_t qxk24_resource_release(QXK24KernelHandle handle,
                               uint32_t resource_id);

int32_t qxk24_flow_begin(QXK24KernelHandle handle,
                         QXK24FlowType type,
                         QXK24FlowDir dir,
                         uint32_t src_id,
                         uint32_t dst_id,
                         uint64_t bytes,
                         uint64_t *out_flow_id);
int32_t qxk24_flow_complete(QXK24KernelHandle handle, uint64_t flow_id);
int32_t qxk24_flow_cancel(QXK24KernelHandle handle, uint64_t flow_id);

void qxk24_notify_pressure(QXK24KernelHandle handle, int32_t tier);
void qxk24_notify_lifecycle(QXK24KernelHandle handle, int32_t foreground);
void qxk24_notify_screen(QXK24KernelHandle handle,
                         int32_t width_px,
                         int32_t height_px,
                         float scale);

double qxk24_health_score(QXK24KernelHandle handle);
QXK24CertTier qxk24_cert_tier(QXK24KernelHandle handle);
uint64_t qxk24_cycle_index(QXK24KernelHandle handle);
uint32_t qxk24_active_resources(QXK24KernelHandle handle);
uint32_t qxk24_active_flows(QXK24KernelHandle handle);
int32_t qxk24_get_law_scores(QXK24KernelHandle handle,
                             QXK24LawScore *out_scores);

static inline uint64_t qxk24_pow6(uint32_t n) {
    uint64_t r = 1ULL;
    for (uint32_t i = 0; i < n; i++) r *= 6ULL;
    return r;
}

static inline uint64_t qxk24_total_units(uint32_t n) {
    if (n == 0) return 1ULL;
    return (qxk24_pow6(n) - 1ULL) / 5ULL;
}

static inline uint64_t qxk24_pow4(uint32_t n) {
    uint64_t r = 1ULL;
    for (uint32_t i = 0; i < n; i++) r *= 4ULL;
    return r;
}

static inline uint64_t qxk24_ama_col_inline(uint32_t n) {
    if (n == 0) return 1ULL;
    return 2ULL * qxk24_pow4(n);
}

static inline uint64_t qxk24_hard_budget(uint64_t soft) {
    return (soft * QXK24_HARD_RATIO_NUM) / QXK24_HARD_RATIO_DEN;
}

#ifdef __cplusplus
}
#endif

#endif /* QXK24_H */
