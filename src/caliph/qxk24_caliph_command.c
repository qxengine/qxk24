/* ============================================================
 * qxk24_caliph_command.c
 * QXK24 Universal Kernel — Caliph Command Dispatch
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : src/caliph/qxk24_caliph_command.c
 * License : Apache 2.0
 *
 * Description:
 * Dispatches Caliph constitutional commands into the live QXK24 kernel.
 * ============================================================ */

#include "qxk24/caliph/qxk24_caliph.h"
#include <stddef.h>

static qxk24_err_t register_one(qxk24_kernel_t kernel,
                                QXK24ResourceType type,
                                QXK24Direction direction,
                                uint64_t budget) {
    uint32_t id = 0U;
    qxk24_err_t err = qxk24_resource_register(kernel, type, direction,
        QXK24_LEVEL_SISTEM, budget, &id);
    if (err != QXK24_OK) return err;
    return qxk24_resource_update(kernel, id, budget / 2ULL, 1.0);
}

static qxk24_err_t register_default_governors(qxk24_caliph_t caliph) {
    if (caliph->governors_registered) return QXK24_OK;

    const uint64_t budget = QXK24_AMA_CORE_4 * QXK24_BASE_UNIT_BYTES;
    qxk24_err_t err = register_one(caliph->kernel, QXK24_RESOURCE_MEMORY,
        QXK24_DIR_AS, budget);
    if (err != QXK24_OK) return err;
    err = register_one(caliph->kernel, QXK24_RESOURCE_IO, QXK24_DIR_BH, budget);
    if (err != QXK24_OK) return err;
    err = register_one(caliph->kernel, QXK24_RESOURCE_CPU, QXK24_DIR_DN, budget);
    if (err != QXK24_OK) return err;
    err = register_one(caliph->kernel, QXK24_RESOURCE_STORAGE,
        QXK24_DIR_BG, budget);
    if (err != QXK24_OK) return err;
    err = register_one(caliph->kernel, QXK24_RESOURCE_NETWORK,
        QXK24_DIR_KN, budget);
    if (err != QXK24_OK) return err;
    err = register_one(caliph->kernel, QXK24_RESOURCE_ENERGY,
        QXK24_DIR_KR, budget);
    if (err != QXK24_OK) return err;

    caliph->governors_registered = true;
    return QXK24_OK;
}

static qxk24_err_t run_fm_cycle(qxk24_caliph_t caliph) {
    if (!qxk24_fm_is_running(caliph->fm)) {
        qxk24_err_t err = qxk24_fm_start(caliph->fm);
        if (err != QXK24_OK) return err;
    }
    return qxk24_fm_cycle(caliph->fm);
}

static qxk24_err_t dispatch(qxk24_caliph_t caliph,
                            QXK24CaliphCommand command) {
    QXK24Report report;
    switch (command) {
    case QXK24_CMD_START:
        return qxk24_start(caliph->kernel);
    case QXK24_CMD_CYCLE:
        return qxk24_cycle(caliph->kernel, NULL);
    case QXK24_CMD_STOP:
        qxk24_stop(caliph->kernel);
        qxk24_fm_stop(caliph->fm);
        return QXK24_OK;
    case QXK24_CMD_REGISTER_GOVERNORS:
        return register_default_governors(caliph);
    case QXK24_CMD_HEALTH_CHECK:
        (void)qxk24_health_score(caliph->kernel);
        return QXK24_OK;
    case QXK24_CMD_GENERATE_REPORT:
        return qxk24_report_generate(caliph->kernel, &report);
    case QXK24_CMD_RESET_VIOLATIONS:
        return QXK24_OK;
    case QXK24_CMD_FM_CYCLE:
        return run_fm_cycle(caliph);
    default:
        return QXK24_ERR_INVALID_ARGUMENT;
    }
}

qxk24_err_t qxk24_caliph_command(qxk24_caliph_t caliph,
                                  QXK24CaliphCommand command,
                                  QXK24CommandResult *result_out) {
    if (caliph == NULL || result_out == NULL) return QXK24_ERR_NULL_PARAM;
    if (caliph->session.state != QXK24_CALIPH_STATE_ACTIVE) {
        return QXK24_ERR_INVALID_ARGUMENT;
    }

    *result_out = QXK24_CMD_RESULT_OBEYED;
    const qxk24_err_t err = dispatch(caliph, command);
    if (err != QXK24_OK) *result_out = QXK24_CMD_RESULT_FAULT;

    caliph->session.commands_issued++;
    caliph->session.usul_masa_position =
        (caliph->session.usul_masa_position + 1U) %
        QXK24_CALIPH_COORDINATES;
    return err;
}

qxk24_err_t qxk24_caliph_command_startup(qxk24_caliph_t caliph,
                                          QXK24CommandResult *result_out) {
    if (caliph == NULL || result_out == NULL) return QXK24_ERR_NULL_PARAM;

    static const QXK24CaliphCommand startup_sequence[] = {
        QXK24_CMD_START,
        QXK24_CMD_REGISTER_GOVERNORS,
        QXK24_CMD_FM_CYCLE,
        QXK24_CMD_CYCLE,
        QXK24_CMD_HEALTH_CHECK
    };

    for (size_t i = 0U; i < sizeof(startup_sequence) /
         sizeof(startup_sequence[0]); i++) {
        qxk24_err_t err = qxk24_caliph_command(caliph,
            startup_sequence[i], result_out);
        if (err != QXK24_OK || *result_out != QXK24_CMD_RESULT_OBEYED) {
            return err;
        }
    }
    return QXK24_OK;
}
