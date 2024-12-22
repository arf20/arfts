#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "command.h"
#include "doc.h"

void
usage(char *argv0) {
    printf("usage: %s <file>\n\n", argv0);
}


void
parse_file(const char *fname, docconfig_t *cfg, docentry_t *doc) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(1);
    }

    fseek(f, 0, SEEK_END); 
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *src = malloc(size + 1);
    fread(src, 1, size, f);
    src[size] = '\0';

    fclose(f);

    const char *cursor = src;
    state_t st = { 0 };
    st.linenum = 1;
    docentry_t *cur_entry = doc; /* current document entry */
    while (*cursor) {
        cursor = strip(cursor);

        if (*cursor == '.') {
            /* command */
            cursor = interpret_command(cursor, cfg, &st, &cur_entry);
        } else if (*cursor == '\n') {
            cursor++;
            st.linenum++;

            if (st.prev_nl) {
                /* break paragraph */
                cur_entry = doc_insert_null(cur_entry);
            }

            st.prev_nl = 1;
        }
        else {
            /* body word */
            if (cur_entry->type != EPARAGRAPH)
                cur_entry = doc_insert_paragraph(cur_entry);

            cursor = doc_add_word(cur_entry, &st, cursor);

            st.prev_nl = 0;
        }
    }

    free(src);
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


    /* debug */
    int c = 0;
    for (docentry_t *e = doc; e != NULL; e = e->n, c++) {
        printf(" doc[%d]: %s", c, entrytype_names[e->type]);
        switch (e->type) {
            case ENULL: break;
            case EPARAGRAPH: printf(" \"%.*s\"", (int)e->size, e->data); break;
            case EPREFORMAT: break;
            case ESTRUCTURE: {
                docentry_structure_t *es = (docentry_structure_t*)e->data;
                printf(": %s \"%s\"", structuretype_names[es->type],
                    es->heading);
            } break;
            case ETITLEPAGE: break;
            case EPAGEBREAK: break;
            case ETABLEOFCONTENTS: break;
        }
        putchar('\n');
    }

    return 0;
}

