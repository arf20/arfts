#include "util.h"

#include <stddef.h>
#include <string.h>

const char *
strip(const char *s) {
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

const char *
tokenize(const char *s, char **tok) {
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

size_t
count_utf8_code_points_n(const char *s, size_t n) {
    size_t count = 0;
    for (size_t i = 0; i < n && *s; i++) {
        count += (*s++ & 0xC0) != 0x80;
    }
    return count;
}

size_t
count_utf8_code_points(const char *s) {
    size_t count = 0;
    while (*s) {
        count += (*s++ & 0xC0) != 0x80;
    }
    return count;
}

