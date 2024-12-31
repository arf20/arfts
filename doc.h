#ifndef _DOC_H
#define _DOC_H

#include <stddef.h>

#include "types.h"

#define EPARAGRAPH_INITIAL_CAPACITY 256

/* document node config */
typedef enum {
    AJUSTIFY,
    ALEFT,
    ACENTER,
    ARIGHT
} align_t;

typedef struct {
    align_t align;
    int indentparagraph;
    int columns;
} docentry_config_t;


/* Document node type */
typedef enum {
    ENULL,
    EPARAGRAPH,
    EFIGURE, /* preformat */
    ESTRUCTURE,
    ETITLEPAGE,
    EPAGEBREAK,
    ETABLEOFCONTENTS,
} entrytype_t;

extern const char *entrytype_names[];

/* Document tree node */
typedef struct docentry_s {
    entrytype_t type;
    docentry_config_t ecfg;
    int page, line, width, height;
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


typedef struct {
    const char *caption;
    const char *predata;
} docentry_figure_t;

docentry_t *doc_new();
docentry_t *doc_insert_null(docentry_t *e);
docentry_t *doc_insert_paragraph(docentry_t *e, docentry_config_t ecfg);
const char *doc_add_word(docentry_t *e, state_t *st, const char *wordoff);
docentry_t *doc_insert_titlepage(docentry_t *e);
docentry_t *doc_insert_structure(docentry_t *e, structuretype_t type,
    const char *heading);
docentry_t *doc_insert_pagebreak(docentry_t *e);
docentry_t *doc_insert_tableofcontents(docentry_t *e);
docentry_t *doc_insert_figure(docentry_t *e, docentry_config_t ecfg,
    const char *caption);
const char *doc_read_figure(docentry_t *e, state_t *st, const char *figoff);
 

#endif /* _DOC_H */

