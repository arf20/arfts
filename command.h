#ifndef _COMMAND_H
#define _COMMAND_H

#include "types.h"
#include "doc.h"

const char *interpret_command(const char *cmd, docconfig_t *cfg,
    docentry_config_t *ecfg, state_t *st, docentry_t **doc);

#endif /* _COMMAND_H */

