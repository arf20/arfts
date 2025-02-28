#ifndef _PARSER_H
#define _PARSER_H

#include "doc.h"
#include "types.h"

void parse_file(const char *fname, doc_format_t *cfg, docentry_t *doc);

#endif /* _PARSER_H */

