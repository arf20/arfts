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
    "list",
    "table",
    "bibliography"
};

const char *structuretype_names[] = {
    "part",
    "chapter",
    "section",
    "subsection",
    "subsubsection"
};

const char *listtype_names[] = {
    "itemize",
    "enumerate"
};


docentry_t*
doc_insert_entry(docentry_t *e, const docentry_format_t *efmt,
    entrytype_t type)
{
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
    newe->type = type;
    newe->efmt = *efmt;

    return newe;
}

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
doc_insert_paragraph(docentry_t *e, docentry_format_t *efmt) {
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
    newe->efmt = *efmt;

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
doc_insert_figure(docentry_t *e, const docentry_format_t *efmt,
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
    newe->efmt = *efmt;
    newe->height = 0;
    newe->data = malloc(newe->size = sizeof(docentry_figure_t));
    newe->size = newe->capacity = sizeof(docentry_figure_t);
    
    docentry_figure_t* ef = (docentry_figure_t*)newe->data;
    ef->caption = caption;
    ef->predata = NULL;

    return newe;
}

docentry_t*
doc_insert_list(docentry_t *e, const docentry_format_t *efmt, list_type_t type,
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
    newe->efmt = *efmt;
    newe->height = 0;
    newe->data = malloc(newe->size = sizeof(docentry_list_t));
    newe->size = newe->capacity = sizeof(docentry_list_t);
    
    docentry_list_t* el = (docentry_list_t*)newe->data;
    el->type = type;
    el->caption = caption;
    el->count = 0;
    el->itemspacing = 0;
    el->items = NULL;

    return newe;
}

void
doc_list_insert(docentry_t *e) {
    if (e->type != ELIST)
        return;
    docentry_list_t* el = (docentry_list_t*)e->data;
    el->count++;
    el->items = realloc(el->items, sizeof(docentry_list_item_t) * el->count);
    docentry_list_item_t *li = &el->items[el->count - 1];
    li->size = 0;
    li->capacity = EPARAGRAPH_INITIAL_CAPACITY;
    li->content = malloc(EPARAGRAPH_INITIAL_CAPACITY);
    li->content[0] = '\0';
}

docentry_t*
doc_insert_table(docentry_t *e, const docentry_format_t *efmt,
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
    newe->type = ETABLE;
    newe->efmt = *efmt;
    newe->height = 0;
    newe->data = malloc(newe->size = sizeof(docentry_table_t));
    newe->size = newe->capacity = sizeof(docentry_table_t);
    
    docentry_table_t* et = (docentry_table_t*)newe->data;
    et->caption = caption;
    et->ncols = et->nrows = 0;
    et->cells = NULL;

    return newe;
}

docentry_t *
doc_insert_bibliography(docentry_t *e, const docentry_format_t *efmt) {
    docentry_t *newe = doc_insert_entry(e, efmt, EBIBLIOGRAPHY);

    newe->height = 0;
    newe->data = malloc(sizeof(docentry_bibliography_t));
    newe->size = newe->capacity = sizeof(docentry_bibliography_t);
   
    docentry_bibliography_t* eb = (docentry_bibliography_t*)newe->data;
    eb->count = 0;
    eb->refs = malloc(0);

    return newe;
}

void
doc_bibliography_insert(docentry_t *e, const char *refname,
    const char *citation)
{
    if (e->type != EBIBLIOGRAPHY)
        return;
    docentry_bibliography_t* eb = (docentry_bibliography_t*)e->data;
    eb->count++;
    eb->refs = realloc(eb->refs, sizeof(docentry_list_item_t) * eb->count);
    docentry_bibliography_ref_t *br = &eb->refs[eb->count - 1];
    br->refname = refname;
    br->citation = citation;
}

