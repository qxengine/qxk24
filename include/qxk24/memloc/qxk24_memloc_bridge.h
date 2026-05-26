/* ============================================================
 * qxk24_memloc_bridge.h
 * QXK24 Universal Kernel - QXMemloc K24at Bridge
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/memloc/qxk24_memloc_bridge.h
 * License : Apache 2.0
 *
 * Description:
 * Declares QXMemloc as the K24at memory locator under QXEngine.
 * ============================================================ */

#ifndef QXK24_MEMLOC_BRIDGE_H
#define QXK24_MEMLOC_BRIDGE_H

#include "qxk24/resource/qxk24_memory_qxengine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QXK24_MEMLOC_K24_LEVEL QXK24_LEVEL_ATOM
#define QXK24_MEMLOC_NAME "QXMemloc-K24at"
#define QXK24_MEMLOC_AMA_COLUMN 32U

typedef struct QXK24MemlocRecord_s {
    uint64_t address;
    uint64_t size_bytes;
    QXK24Direction direction;
    uint32_t ama_column;
    bool active;
} QXK24MemlocRecord;

typedef struct QXK24MemlocConfig_s {
    uint64_t total_bytes;
    uint32_t max_records;
    bool enable_ama_check;
} QXK24MemlocConfig;

typedef struct QXK24MemlocBridge_s *qxk24_memloc_t;

QXK24MemlocConfig qxk24_memloc_default_config(void);
qxk24_memloc_t qxk24_memloc_create(const QXK24MemlocConfig *config,
                                   qxk24_qxengine_t gov);
qxk24_err_t qxk24_memloc_locate(qxk24_memloc_t ml,
                                uint64_t bytes,
                                QXK24Direction dir,
                                QXK24MemlocRecord *out);
qxk24_err_t qxk24_memloc_release(qxk24_memloc_t ml, uint64_t address);
float qxk24_memloc_health_score(qxk24_memloc_t ml);
uint32_t qxk24_memloc_active_count(qxk24_memloc_t ml);
uint32_t qxk24_memloc_ama_column(void);
void qxk24_memloc_destroy(qxk24_memloc_t ml);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_MEMLOC_BRIDGE_H */
