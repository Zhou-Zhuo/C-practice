#include "bitmap.h"
#include "symbol_table.h"

int symbol_table_init(struct symbol_table *tbl)
{
    tbl->internal_bm = new_bitmap(256);
    return 0;
}

void symbol_add(struct symbol_table *tbl, char c)
{
    char c_plus1 = c + 1;
    if (!is_bit_set(tbl->internal_bm, c_plus1)) {
        ++ tbl->symbol_nr;
        if (c > tbl->symbol_max)
            tbl->symbol_max = c;
        bitmap_set_bit(tbl->internal_bm, c + 1);
    }
}

int is_symbol_valid(struct symbol_table *tbl, char c)
{
    return is_bit_set(tbl->internal_bm, c + 1);
}

int get_symbol_nr(struct symbol_table *tbl)
{
    return tbl->symbol_nr;
}

int get_symbol_max(struct symbol_table *tbl)
{
    return tbl->symbol_max;
}
