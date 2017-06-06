#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "bitmap.h"

struct symbol_table {
    struct bitmap *internal_bm;
    int symbol_nr;
};

#define for_each_symbol_plus_one_in_table(s, t) \
    for_each_set_bit((s), (t)->internal_bm)

int symbol_table_init(struct symbol_table *tbl);
void symbol_add(struct symbol_table *tbl, char c);
int get_symbol_nr(struct symbol_table *tbl);

#endif
