#include "util.h"

char *
strip(char *s) {
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

