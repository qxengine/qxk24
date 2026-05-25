/* ============================================================
 * qxk24_governors.c
 * QXK24 Universal Kernel — Six Resource Governors
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/resource/qxk24_governors.c
 * License : Apache 2.0
 *
 * Description:
 * Provides thin factories for the six constitutional resource governors:
 * memory, CPU, network, storage, I/O, and energy.
 * ============================================================ */

#include "qxk24/resource/qxk24_resource_base.h"
#include <string.h>

static void base_config(QXK24GovernorConfig *cfg,
                        QXK24ResourceType type,
                        QXK24Direction direction,
                        QXK24Level level,
                        uint64_t budget,
                        const char *label) {
    memset(cfg, 0, sizeof(*cfg));
    cfg->type = type;
    cfg->direction = direction;
    cfg->level = level;
    cfg->budget_soft = budget;
    cfg->declared_x = QXK24_DEFAULT_DECLARED_X;
    cfg->tolerance_pct = QXK24_DEFAULT_TOLERANCE_PCT;
    strncpy(cfg->label, label, sizeof(cfg->label) - 1U);
}

int32_t qxk24_memory_governor_create(QXK24Level level,
                                     uint64_t budget_soft,
                                     QXK24ResourceGovernor **out) {
    QXK24GovernorConfig cfg;
    base_config(&cfg, QXK24_RESOURCE_MEMORY, QXK24_DIR_ATAS,
                level, budget_soft, "Memory-As");
    return qxk24_governor_base_create(&cfg, out);
}

int32_t qxk24_cpu_governor_create(QXK24Level level,
                                  uint64_t budget_ms,
                                  QXK24ResourceGovernor **out) {
    QXK24GovernorConfig cfg;
    base_config(&cfg, QXK24_RESOURCE_CPU, QXK24_DIR_DEPAN,
                level, budget_ms, "CPU-Dn");
    return qxk24_governor_base_create(&cfg, out);
}

int32_t qxk24_network_governor_create(QXK24Level level,
                                      uint64_t egress_budget,
                                      uint64_t ingress_budget,
                                      QXK24ResourceGovernor **out_kn,
                                      QXK24ResourceGovernor **out_kr) {
    if (out_kn == NULL || out_kr == NULL) return QXK24_ERR_INVALID_ARGUMENT;

    QXK24GovernorConfig cfg_kn;
    QXK24GovernorConfig cfg_kr;
    base_config(&cfg_kn, QXK24_RESOURCE_NETWORK, QXK24_DIR_KANAN,
                level, egress_budget, "Network-Kn-Egress");
    base_config(&cfg_kr, QXK24_RESOURCE_NETWORK, QXK24_DIR_KIRI,
                level, ingress_budget, "Network-Kr-Ingress");

    int32_t rc = qxk24_governor_base_create(&cfg_kn, out_kn);
    if (rc != QXK24_OK) return rc;

    rc = qxk24_governor_base_create(&cfg_kr, out_kr);
    if (rc != QXK24_OK) {
        qxk24_governor_base_destroy(*out_kn);
        *out_kn = NULL;
        return rc;
    }
    return QXK24_OK;
}

int32_t qxk24_storage_governor_create(QXK24Level level,
                                      uint64_t budget_bytes,
                                      QXK24ResourceGovernor **out) {
    QXK24GovernorConfig cfg;
    base_config(&cfg, QXK24_RESOURCE_STORAGE, QXK24_DIR_BELAKANG,
                level, budget_bytes, "Storage-Bg");
    return qxk24_governor_base_create(&cfg, out);
}

int32_t qxk24_io_governor_create(QXK24Level level,
                                 uint64_t budget_ops,
                                 QXK24ResourceGovernor **out) {
    QXK24GovernorConfig cfg;
    base_config(&cfg, QXK24_RESOURCE_IO, QXK24_DIR_BAWAH,
                level, budget_ops, "IO-Bh");
    return qxk24_governor_base_create(&cfg, out);
}

int32_t qxk24_energy_governor_create(QXK24Level level,
                                     uint64_t budget_mj,
                                     QXK24ResourceGovernor **out) {
    QXK24GovernorConfig cfg;
    base_config(&cfg, QXK24_RESOURCE_ENERGY, QXK24_DIR_KIRI,
                level, budget_mj, "Energy-Kr");
    return qxk24_governor_base_create(&cfg, out);
}
