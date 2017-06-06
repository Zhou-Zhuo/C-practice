#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include "bitmap.h"
#include "common.h"

struct grammar_tree_node {
    struct grammar_tree_node *parent;
    struct grammar_tree_node *child[2];
    int type;
#define CHAR    1
#define CAT     2
#define OR      3
#define STAR    4
#define END     5
    int value;

    /* information for creating DFA */
    int pos;
    int nullable;
    struct bitmap *firstpos;
    struct bitmap *lastpos;
    struct bitmap *followpos;
    int subpos_max;
};

#endif
