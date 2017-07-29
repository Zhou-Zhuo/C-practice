#ifndef TOKEN_ITER_H
#define TOKEN_ITER_H
#include <ctype.h>

typedef int (*token_splitter_t)(char *c);

static inline char *cut_one_token(char **s, token_splitter_t split)
{
    char *t;

    if (!*s || !**s)
        return NULL;

    while (**s && split(*s))
        ++ *s;
    if (!**s)
        return NULL;

    t = *s;
    while (*++t && !split(t));
    if (*t)
        *t = '\0';
    else
        return NULL;

    return ++t;
}

#define for_each_token(t, nt, b, sp) \
    for ((t) = (b), (nt) = cut_one_token(&(t), sp); \
            (nt);  \
            (t) = (nt), (nt) = cut_one_token(&(t), sp))

static inline int line_split(char *c)
{
    return *c == '\n';
}

#define for_each_line(l, nl, b) for_each_token(l, nl, b, line_split)

#endif
