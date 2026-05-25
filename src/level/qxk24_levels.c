/* ============================================================
 * qxk24_levels.c
 * QXK24 Universal Kernel — Seven-Level K24 Hierarchy
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/level/qxk24_levels.c
 * License : Apache 2.0
 *
 * Description:
 * Implements the seven K24 level descriptors, scale values, total units,
 * and default budget calculations used by the kernel hierarchy.
 */

#include "qxk24/qxk24.h"

static const QXK24LevelDesc s_levels[QXK24_LEVEL_COUNT] = {
    {
        QXK24_LEVEL_ZARAH, "K24za", "Zarah", "Particle",
        QXK24_SCALE_ZARAH, QXK24_TOTAL_ZARAH,
        "Byte / Single Instruction"
    },
    {
        QXK24_LEVEL_JISIM, "K24ji", "Jisim", "Mass",
        QXK24_SCALE_JISIM, QXK24_TOTAL_JISIM,
        "Cache Line / CPU Register"
    },
    {
        QXK24_LEVEL_ATOM, "K24at", "Atom", "Atom",
        QXK24_SCALE_ATOM, QXK24_TOTAL_ATOM,
        "Thread / Memory Leaf"
    },
    {
        QXK24_LEVEL_SISTEM, "K24si", "Sistem", "System",
        QXK24_SCALE_SISTEM, QXK24_TOTAL_SISTEM,
        "Process / Segment Budget"
    },
    {
        QXK24_LEVEL_OBJEK, "K24ob", "Objek", "Object",
        QXK24_SCALE_OBJEK, QXK24_TOTAL_OBJEK,
        "Application / QXEngine Instance"
    },
    {
        QXK24_LEVEL_AKTIF, "K24ak", "Aktif", "Active",
        QXK24_SCALE_AKTIF, QXK24_TOTAL_AKTIF,
        "Platform Runtime / iOS / Android"
    },
    {
        QXK24_LEVEL_MODUL, "K24md", "Modul", "Module",
        QXK24_SCALE_MODUL, QXK24_TOTAL_MODUL,
        "Universal Kernel / QXK24"
    }
};

int32_t qxk24_level_describe(QXK24Level level, QXK24LevelDesc *out) {
    if (out == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)level >= QXK24_LEVEL_COUNT) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    *out = s_levels[(uint32_t)level];
    return QXK24_OK;
}

uint64_t qxk24_level_scale(QXK24Level level) {
    if ((uint32_t)level >= QXK24_LEVEL_COUNT) return 0ULL;
    return s_levels[(uint32_t)level].scale;
}

uint64_t qxk24_level_total(QXK24Level level) {
    if ((uint32_t)level >= QXK24_LEVEL_COUNT) return 0ULL;
    return s_levels[(uint32_t)level].total;
}

uint64_t qxk24_level_budget(QXK24Level level) {
    if ((uint32_t)level >= QXK24_LEVEL_COUNT) return 0ULL;
    return s_levels[(uint32_t)level].total * QXK24_BASE_UNIT_BYTES;
}
