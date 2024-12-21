#ifndef _TYPES_H
#define _TYPES_H

#include <stddef.h>

/* Typesetting config for document */
typedef struct {
    int pagewidth, pageheight, tabstop, indentparagraphs, titlepage;
    char *headerl, *headerc, *headerr, *footerl, *footerc, *footerr;
    char *title, *author, *date;
} docconfig_t;

/* Reader state */
typedef struct {
    int prev_nl, in_pre;
} state_t;


#endif /* _TYPES_H */

