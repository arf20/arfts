#ifndef _TYPES_H
#define _TYPES_H

#include <stddef.h>

#include "doc.h"

/* Typesetting config for document */
typedef struct {
    int pagewidth, pageheight, tabstop, margint, marginl,
        marginb, marginr;
    char *headerl, *headerc, *headerr, *footerl, *footerc, *footerr;
    char *title, *author, *date;
} doc_format_t;

/* Reader state */
typedef struct {
    int linenum, prev_nl, in_fig, in_item, in_table;
} state_t;


#endif /* _TYPES_H */

