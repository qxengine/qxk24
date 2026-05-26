/* ============================================================
 * qxk24_caliph_command.h
 * QXK24 Universal Kernel — Caliph Constitutional Commands
 * ============================================================
 * Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
 * Created : 2026-05-24
 * Repository : https://github.com/qxengine/qxk24
 * Path : include/qxk24/caliph/qxk24_caliph_command.h
 * License : Apache 2.0
 *
 * Description:
 * Declares constitutional commands issued by the Caliph to the QXK24 kernel.
 * ============================================================ */

#ifndef QXK24_CALIPH_COMMAND_H
#define QXK24_CALIPH_COMMAND_H

#include "../qxk24.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef QXK24_CALIPH_HANDLE_DEFINED
#define QXK24_CALIPH_HANDLE_DEFINED
typedef struct QXK24CaliphImpl *qxk24_caliph_t;
#endif

typedef enum {
    QXK24_CMD_START = 0x01,
    QXK24_CMD_CYCLE = 0x02,
    QXK24_CMD_STOP = 0x03,
    QXK24_CMD_REGISTER_GOVERNORS = 0x04,
    QXK24_CMD_HEALTH_CHECK = 0x05,
    QXK24_CMD_GENERATE_REPORT = 0x06,
    QXK24_CMD_RESET_VIOLATIONS = 0x07,
    QXK24_CMD_FM_CYCLE = 0x08
} QXK24CaliphCommand;

typedef enum {
    QXK24_CMD_RESULT_OBEYED = 0,
    QXK24_CMD_RESULT_DEFERRED = 1,
    QXK24_CMD_RESULT_FAULT = 2
} QXK24CommandResult;

qxk24_err_t qxk24_caliph_command(qxk24_caliph_t caliph,
                                  QXK24CaliphCommand command,
                                  QXK24CommandResult *result_out);
qxk24_err_t qxk24_caliph_command_startup(qxk24_caliph_t caliph,
                                          QXK24CommandResult *result_out);

#ifdef __cplusplus
}
#endif

#endif /* QXK24_CALIPH_COMMAND_H */
