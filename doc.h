#ifndef _DOC_H
#define _DOC_H

#include <stddef.h>

#include "types.h"

#define EPARAGRAPH_INITIAL_CAPACITY 256

/* Document node type */
typedef enum {
    ENULL,
    EPARAGRAPH,
    EPREFORMAT,
    ESTRUCTURE,
    ETITLEPAGE,
    EPAGEBREAK,
    ETABLEOFCONTENTS
} entrytype_t;

extern const char *entrytype_names[];

/* Document tree node */
typedef struct docentry_s {
    entrytype_t type;
    int page, line, height;
    char *data;
    size_t size, capacity;
    struct docentry_s *n;
} docentry_t;


typedef enum {
    SPART,
    SCHAPTER,
    SSECTION,
    SSUBSECTION,
    SSUBSUBSECTION
} structuretype_t;

extern const char *structuretype_names[];

typedef struct {
    structuretype_t type;
    const char *heading;
} docentry_structure_t;


docentry_t *doc_new();
docentry_t *doc_insert_null(docentry_t *e);
docentry_t *doc_insert_paragraph(docentry_t *e);
const char *doc_add_word(docentry_t *e, state_t *st, const char *wordoff);
docentry_t *doc_insert_titlepage(docentry_t *e);
docentry_t *doc_insert_structure(docentry_t *e, structuretype_t type,
    const char *heading);
docentry_t *doc_insert_pagebreak(docentry_t *e);
docentry_t *doc_insert_tableofcontents(docentry_t *e);
 

#endif /* _DOC_H */

