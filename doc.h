#ifndef _DOC_H
#define _DOC_H

#include <stddef.h>

#define EPARAGRAPH_INITIAL_CAPACITY 256

/* Document node type */
typedef enum {
    ENULL,
    EPARAGRAPH,
    EPREFORMAT,
    ESTRUCTURE,
    ETITLEPAGE
} entrytype_t;

/* Document tree node */
typedef struct docentry_s {
    entrytype_t type;
    char *data;
    size_t size, capacity;
    struct docentry_s *n;
} docentry_t;


docentry_t *doc_new();
docentry_t *doc_new_null(docentry_t *e);
docentry_t *doc_new_paragraph(docentry_t *e);
const char *doc_add_word(docentry_t *e, const char *wordoff);
 

#endif /* _DOC_H */

