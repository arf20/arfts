#include "util.h"

#include <stddef.h>
#include <string.h>

const char *
strip(const char *s) {
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

const char *tokenize(const char *s, char **tok) {
    s = strip(s);
    const char *end = NULL;

    if (*s == '"') {
        end = strchr(s + 1, '"');
        if (!end || end > strchr(s + 1, '\n')) {
            *tok = NULL;
            return s;
        }
        *tok = strndup(s + 1, end - (s + 1));
        return end + 1;
    } else {
        end = strpbrk(s, " \n");
        if (!end) {
            *tok = NULL;
            return s;
        }
        if (end - s)
            *tok = strndup(s, end - s);
        else
            *tok = NULL;
        return end;
    }
}

