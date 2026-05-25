/* ============================================================
 * qxk24_direction.c
 * QXK24 Universal Kernel — Six Constitutional Directions
 * ============================================================ */
/* Owner : Masa Bayu (Founder, ALAMTOLOGI & QIUBBX)
 * Created : 2026-05-25
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/direction/qxk24_direction.c
 * License : Apache 2.0
 *
 * Description:
 * Implements the six constitutional direction descriptors and the Law Z.3
 * direction-pair registry used by kernel resource balancing.
 */

#include "qxk24/qxk24.h"

static const QXK24DirectionDesc s_directions[QXK24_DIRECTION_COUNT] = {
    {
        QXK24_DIR_ATAS,
        "As", "Atas", "Top",
        "Governance — Constitutional scheduler, CORE memory, routing table"
    },
    {
        QXK24_DIR_BAWAH,
        "Bh", "Bawah", "Bottom",
        "Hardware — Physical pages, CPU registers, raw storage blocks"
    },
    {
        QXK24_DIR_DEPAN,
        "Dn", "Depan", "Front",
        "Execution — Active threads, AI and UI memory, active connections"
    },
    {
        QXK24_DIR_BELAKANG,
        "Bg", "Belakang", "Back",
        "Storage — Idle threads, IMG and SEC memory, passive buffers"
    },
    {
        QXK24_DIR_KANAN,
        "Kn", "Kanan", "Right",
        "Output — Write operations, outbound packets, energy discharge"
    },
    {
        QXK24_DIR_KIRI,
        "Kr", "Kiri", "Left",
        "Input — Read operations, inbound packets, energy charge"
    }
};

static const QXK24Direction s_pairs[QXK24_DIRECTION_COUNT] = {
    QXK24_DIR_BAWAH,
    QXK24_DIR_ATAS,
    QXK24_DIR_BELAKANG,
    QXK24_DIR_DEPAN,
    QXK24_DIR_KIRI,
    QXK24_DIR_KANAN
};

int32_t qxk24_direction_describe(QXK24Direction dir,
                                 QXK24DirectionDesc *out) {
    if (out == NULL) return QXK24_ERR_INVALID_ARGUMENT;
    if ((uint32_t)dir >= QXK24_DIRECTION_COUNT) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    *out = s_directions[(uint32_t)dir];
    return QXK24_OK;
}

QXK24Direction qxk24_direction_pair(QXK24Direction dir) {
    if ((uint32_t)dir >= QXK24_DIRECTION_COUNT) {
        return QXK24_DIR_ATAS;
    }
    return s_pairs[(uint32_t)dir];
}
