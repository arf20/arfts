#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "command.h"

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

    char *src = malloc(size);
    fread(src, 1, size, f);

    fclose(f);

    const char *cursor = src;
    state_t st;
    while (*cursor) {
        cursor = strip(cursor);

        if (*cursor == '.') {
            /* command */
            cursor = interpret_command(cursor, cfg, &st, doc);
        } else if (*cursor == '\n')
            cursor++;
            /* new paragraph */
        else {
            /* text */
            
        }
    }

    free(src);
}

docentry_t *
doc_new() {
    docentry_t *newdoc = malloc(sizeof(docentry_t));
}


int
main(int argc, char **argv) {
    if (argc != 2) {
        usage(*argv);
        exit(1);
    }

    docconfig_t cfg;
    docentry_t *doc;
    parse_file(argv[1], &cfg);

    return 0;
}

