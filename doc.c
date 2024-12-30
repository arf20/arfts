#include "doc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char *entrytype_names[] = {
    "null",
    "paragraph",
    "preformat",
    "structure",
    "titlepage",
    "pagebreak",
    "tableofcontents"
};

const char *structuretype_names[] = {
    "part",
    "chapter",
    "section",
    "subsection",
    "subsubsection"
};

docentry_t*
doc_new() {
    docentry_t *newdoc = malloc(sizeof(docentry_t));
    newdoc->type = ENULL;
    newdoc->n = NULL;
    return newdoc;
}

docentry_t*
doc_insert_null(docentry_t *e) {
    if (e->type == ENULL)
        return e;

    docentry_t *newe = malloc(sizeof(docentry_t));
    newe->type = ENULL;
    newe->n = NULL;
    newe->height = 0;

    e->n = newe;
    return newe;
}

docentry_t*
doc_insert_paragraph(docentry_t *e) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        /* morph curr null entry into a paragraph */
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = EPARAGRAPH;

    newe->data = malloc(EPARAGRAPH_INITIAL_CAPACITY);
    newe->capacity = EPARAGRAPH_INITIAL_CAPACITY;
    newe->size = 0;
    newe->height = 0;

    return newe;
}

const char*
doc_add_word(docentry_t *e, state_t *st, const char *wordoff) {
    if (e->type != EPARAGRAPH)
        return wordoff;

    int wordlen = strpbrk(wordoff, " \n") - wordoff;

    if (wordoff[wordlen] == '\n')
        st->linenum++;
    
    if (e->size + wordlen + 2 > e->capacity) {
        e->data = realloc(e->data, e->capacity * 2);
        e->capacity *= 2;
    }

    strncpy(e->data + e->size, wordoff, wordlen);
    e->data[e->size + wordlen] = ' ';
    e->data[e->size + wordlen + 1] = '\0';
    e->size += wordlen + 1;

    return wordoff + wordlen;
}

docentry_t*
doc_insert_titlepage(docentry_t *e) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = ETITLEPAGE;

    return newe;
}

docentry_t *
doc_insert_structure(docentry_t *e, structuretype_t type, const char *heading) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = ESTRUCTURE;

    newe->data = malloc(sizeof(docentry_structure_t));
    newe->size = newe->capacity = sizeof(docentry_structure_t);
    newe->height = 2;

    docentry_structure_t *s = (docentry_structure_t*)newe->data;
    s->type = type;
    s->heading = heading;

    return newe;
}

docentry_t*
doc_insert_pagebreak(docentry_t *e) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = EPAGEBREAK;
    newe->height = 0;

    return newe;
}

docentry_t*
doc_insert_tableofcontents(docentry_t *e) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = ETABLEOFCONTENTS;
    newe->height = 0;

    return newe;
}

