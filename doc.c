#include "doc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
        docentry_t *newe = malloc(sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = EPARAGRAPH;

    newe->data = malloc(EPARAGRAPH_INITIAL_CAPACITY);
    newe->capacity = EPARAGRAPH_INITIAL_CAPACITY;
    newe->size = 0;

    return newe;
}

docentry_t*
doc_insert_titlepage(docentry_t *e) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        docentry_t *newe = malloc(sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = ETITLEPAGE;

    return newe;
}

const char*
doc_add_word(docentry_t *e, state_t *st, const char *wordoff) {
    if (e->type != EPARAGRAPH)
        return wordoff;

    int wordlen = strpbrk(wordoff, " \n") - wordoff;

    if (wordoff[wordlen] == '\n')
        st->linenum++;
    
    if (e->size + wordlen + 1 > e->capacity) {
        e->data = realloc(e->data, e->capacity * 2);
        e->capacity *= 2;
    }

    strncpy(e->data + e->size, wordoff, wordlen);
    e->data[e->size + wordlen] = ' ';
    e->size += wordlen + 1;

    printf("word: %.*s\n", wordlen, wordoff);

    return wordoff + wordlen;
}

