#ifndef _DOC_H
#define _DOC_H

#include <stddef.h>

//#include "types.h"

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
    ELIST
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


typedef enum {
    LITEMIZE,
    LENUMERATE
} list_type_t;

typedef struct {
    list_type_t type;
    const char *caption;
    size_t count;
    const char **items;
} docentry_list_t;


docentry_t *doc_new();
docentry_t *doc_insert_null(docentry_t *e);
docentry_t *doc_insert_paragraph(docentry_t *e, docentry_config_t *ecfg);
docentry_t *doc_insert_titlepage(docentry_t *e);
docentry_t *doc_insert_structure(docentry_t *e, structuretype_t type,
    const char *heading);
docentry_t *doc_insert_pagebreak(docentry_t *e);
docentry_t *doc_insert_tableofcontents(docentry_t *e);
docentry_t *doc_insert_figure(docentry_t *e, const docentry_config_t *ecfg,
    const char *caption);
docentry_t *doc_insert_list(docentry_t *e, const docentry_config_t *ecfg,
    list_type_t type, const char *caption);
void doc_list_insert(docentry_t *e, const char *text);

#endif /* _DOC_H */

