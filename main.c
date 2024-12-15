#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"

void
usage(char *argv0) {
    printf("usage: %s <file>\n\n");
}

int
main(int argc, char **argv) {
    if (argc != 2) {
        usage(*argv);
        exit(1);
    }

    char *fname = argv[1];
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

    char *cursor = src;
    while (*cursor) {
        cursor = strip(cursor);

        if (*cursor == '.') {
            /* command */

        }
    }


    free(src);
    return 0;
}

