#ifndef _UTIL_H
#define _UTIL_H

#include <stddef.h>

const char *strip(const char *);
const char *tokenize(const char *s, char **tok);
size_t count_utf8_code_points(const char *s);


#endif /* _UTIL_H */

