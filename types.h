#ifndef _TYPES_H
#define _TYPES_H

typedef struct {
    int pagewidth, pageheight, tabstop, indentparagraphs, titlepage;
    char *headerl, *headerc, *headerr, *footerl, *footerc, *footerr;
    char *title, *author, *date;
} docconfig_t;

typedef struct {

} state_t;

#endif /* _TYPES_H */

