#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "doc.h"
#include "genplain.h"
#include "parser.h"

void
usage(char *argv0) {
    printf("usage: %s <file>\n\n", argv0);
}

void
doc_print(const docentry_t *doc) {
    int i = 0;
    for (const docentry_t *e = doc; e != NULL; e = e->n, i++) {
        fprintf(stderr, " doc[I%d,P%d,L%d][%dx%d]: %s",
            i, e->page, e->line, e->width, e->height, entrytype_names[e->type]);
        switch (e->type) {
            case ENULL: break;
            case EPARAGRAPH: {
                fprintf(stderr, " \"%.*s\"", (int)e->size, e->data);
            } break;
            case EFIGURE: {
                docentry_figure_t *ef = (docentry_figure_t*)e->data;
                fprintf(stderr, " \"%s\" {\n%s\n}", ef->caption, ef->predata);
            } break;
            case ESTRUCTURE: {
                docentry_structure_t *es = (docentry_structure_t*)e->data;
                fprintf(stderr, ": %s \"%s\"", structuretype_names[es->type],
                    es->heading);
            } break;
            case ELIST: {
                docentry_list_t* el = (docentry_list_t*)e->data;
                fputc('\n', stderr);
                for (size_t j = 0; j < el->count; j++)
                    fprintf(stderr, "-> %s\n", el->items[j].content);
            } break;
            case ETABLE: {
                docentry_table_t* et = (docentry_table_t*)e->data;
                fputc('\n', stderr);
                for (int r = 0; r < et->nrows; r++) {
                    for (int c = 0; c < et->ncols; c++) {
                        fprintf(stderr, "%s, ", et->cells[(r * et->ncols) + c]);
                    }
                    fputc('\n', stderr);
                }

             } break;
            case ETITLEPAGE: break;
            case EPAGEBREAK: break;
            case ETABLEOFCONTENTS: break;
        }
        fputc('\n', stderr);
    }
}

void
docconfig_print(const docconfig_t *cfg) {
    fprintf(stderr,
        "doc config:\n page dimensions: %dx%d\n tabstop: %s\n"
        " margins: %d %d %d %d\n"
        " header: (left) \"%s\" (center) \"%s\" (right) \"%s\"\n"
        " footer: (left) \"%s\" (center) \"%s\" (right) \"%s\"\n"
        " title: %s\n author: %s\n date: %s\n",
        cfg->pagewidth, cfg->pageheight, cfg->tabstop ? "true" : "false",
        cfg->margint, cfg->marginl, cfg->marginb, cfg->marginr,
        cfg->headerl, cfg->headerc, cfg->headerr,
        cfg->footerl, cfg->footerc, cfg->footerr,
        cfg->title, cfg->author, cfg->date
    );
}

int
main(int argc, char **argv) {
    if (argc != 2) {
        usage(*argv);
        exit(1);
    }

    docconfig_t cfg = { 0 };
    docentry_t *doc = doc_new();
    parse_file(argv[1], &cfg, doc);

    generate_plain(&cfg, doc, stdout);

    /* debug */
    doc_print(doc); 
    docconfig_print(&cfg);

    return 0;
}

