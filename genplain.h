#ifndef _GENPLAIN_H
#define _GENPLAIN_H

#include "types.h"
#include "doc.h"

#include <stdio.h>

void generate_plain(const docconfig_t *cfg, docentry_t *doc, FILE *o);

#endif /* _GENPLAIN_H */

