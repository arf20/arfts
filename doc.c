#include "doc.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const char *entrytype_names[] = {
    "null",
    "paragraph",
    "figure",
    "structure",
    "titlepage",
    "pagebreak",
    "tableofcontents",
    "list"
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
    memset(newdoc, 0, sizeof(docentry_t));
    newdoc->type = ENULL;
    newdoc->n = NULL;
    return newdoc;
}

docentry_t*
doc_insert_null(docentry_t *e) {
    if (e->type == ENULL)
        return e;

    docentry_t *newe = malloc(sizeof(docentry_t));
    memset(newe, 0, sizeof(docentry_t));
    newe->type = ENULL;
    newe->n = NULL;
    newe->height = 0;

    e->n = newe;
    return newe;
}

docentry_t*
doc_insert_paragraph(docentry_t *e, docentry_config_t *ecfg) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        /* morph curr null entry into a paragraph */
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        memset(newe, 0, sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = EPARAGRAPH;
    newe->ecfg = *ecfg;

    newe->data = malloc(EPARAGRAPH_INITIAL_CAPACITY);
    newe->capacity = EPARAGRAPH_INITIAL_CAPACITY;
    newe->size = 0;
    newe->height = 0;

    return newe;
}

docentry_t*
doc_insert_titlepage(docentry_t *e) {
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        memset(newe, 0, sizeof(docentry_t));
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
        memset(newe, 0, sizeof(docentry_t));
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
        memset(newe, 0, sizeof(docentry_t));
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
        memset(newe, 0, sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = ETABLEOFCONTENTS;
    newe->height = 0;

    return newe;
}

docentry_t*
doc_insert_figure(docentry_t *e, const docentry_config_t *ecfg,
const char *caption)
{
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        memset(newe, 0, sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = EFIGURE;
    newe->ecfg = *ecfg;
    newe->height = 0;
    newe->data = malloc(newe->size = sizeof(docentry_figure_t));
    newe->size = newe->capacity = sizeof(docentry_figure_t);
    
    ((docentry_figure_t*)e->data)->caption = caption;

    return newe;
}

docentry_t*
doc_insert_list(docentry_t *e, const docentry_config_t *ecfg, list_type_t type,
    const char *caption)
{
    docentry_t *newe = NULL;
    if (e->type == ENULL) {
        newe = e;
    } else {
        newe = malloc(sizeof(docentry_t));
        memset(newe, 0, sizeof(docentry_t));
        newe->n = NULL;
        e->n = newe;
    }
    newe->type = ELIST;
    newe->ecfg = *ecfg;
    newe->height = 0;
    newe->data = malloc(newe->size = sizeof(docentry_list_t));
    newe->size = newe->capacity = sizeof(docentry_list_t);
    
    docentry_list_t* el = (docentry_list_t*)e->data;
    el->type = type;
    el->caption = caption;
    el->count = 0;
    el->items = NULL;

    return newe;
}

void
doc_list_insert(docentry_t *e, const char *text) {
    if (e->type != ELIST)
        return;
    docentry_list_t* el = (docentry_list_t*)e->data;
    el->count++;
    el->items = realloc(el->items, sizeof(char*) * el->count);
    el->items[el->count - 1] = text;
}

