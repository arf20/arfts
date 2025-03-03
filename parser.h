#ifndef _PARSER_H
#define _PARSER_H

#include "doc.h"
#include "types.h"

#include <stdio.h>

void parse_file(const char *src, doc_format_t *fmt, docentry_t *doc);

#endif /* _PARSER_H */

