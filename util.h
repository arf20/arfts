#ifndef _UTIL_H
#define _UTIL_H

#include <stddef.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

const char *strip(const char *);
const char *tokenize(const char *s, char **tok);
size_t count_utf8_code_points(const char *s);
size_t count_utf8_code_points_n(const char *s, size_t n);
int num_places(int n);
 
#endif /* _UTIL_H */

