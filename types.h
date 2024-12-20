#ifndef _TYPES_H
#define _TYPES_H

/* Typesetting config for document */
typedef struct {
    int pagewidth, pageheight, tabstop, indentparagraphs, titlepage;
    char *headerl, *headerc, *headerr, *footerl, *footerc, *footerr;
    char *title, *author, *date;
} docconfig_t;

/* Reader state */
typedef struct {

} state_t;


/* Document node type */
typedef enum {
    PARAGRAPH
} entrytype_t;

/* Document tree node */
typedef struct docentry_s {
    entrytype_t type;
    char *data;
    size_t size;
    struct docnode_s n;
} docentry_t;

#endif /* _TYPES_H */

