#ifndef _COMMAND_H
#define _COMMAND_H

#include "types.h"
#include "doc.h"

const char *interpret_command(const char *cmd, doc_format_t *fmt,
    docentry_format_t *efmt, state_t *st, docentry_t **doc);

#endif /* _COMMAND_H */

