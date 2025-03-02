#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "doc.h"
#include "genplain.h"
#include "parser.h"

#define STDIN_READ_SIZE    4096

void
usage(char *argv0) {
    printf("usage: %s <input> <output>\n\n", argv0);
    exit(1);
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
docconfig_print(const doc_format_t *cfg) {
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
    const char *inputstr = NULL, *outputstr = NULL;
    FILE *input = NULL, *output = NULL;
    char *inputbuff = NULL;
    int debug = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc)
                usage(*argv);
            outputstr = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-d") == 0)
            debug = 1;
        else if (!inputstr)
            inputstr = argv[i];
        else
            usage(*argv);
    }

    if (!inputstr || strcmp(inputstr, "-") == 0) {
        input = stdin;
        size_t alloc = STDIN_READ_SIZE, size = 0;
        int read = 0;
        inputbuff = malloc(STDIN_READ_SIZE);

        while (1) {
            read = fread(inputbuff + size, 1, STDIN_READ_SIZE, input);
            size += read;
            if (read != STDIN_READ_SIZE)
                break;
            alloc += STDIN_READ_SIZE;
            inputbuff = realloc(inputbuff, alloc);
        }

        inputbuff[size] = '\0';

    } else {
        input = fopen(inputstr, "r");

        fseek(input, 0, SEEK_END); 
        size_t size = ftell(input);
        fseek(input, 0, SEEK_SET);

        inputbuff = malloc(size + 1);
        fread(inputbuff, 1, size, input);
        inputbuff[size] = '\0';

        fclose(input);
    }


    if (outputstr) {
        if (strcmp(outputstr, "-") == 0)
            output = stdout;
        else
            output = fopen(outputstr, "w");
    } else
        output = stdout;


    if (!input) {
        fprintf(stderr, "Error opening input: %s\n", strerror(errno));
        exit(1);
    }


    doc_format_t cfg = { 80, 137, 4, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL};
    docentry_t *doc = doc_new();
    parse_file(inputbuff, &cfg, doc);

    free(inputbuff);

    generate_plain(&cfg, doc, output);

    /* debug */
    if (debug) {
        doc_print(doc); 
        docconfig_print(&cfg);
    }

    return 0;
}

