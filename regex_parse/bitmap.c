#include "bitmap.h"
#include <string.h>
#include <stdlib.h>

struct bitmap* bitmap_reset(struct bitmap *bm)
{
    memset(bm->bits, 0, 4 * bm->size_in_word);
    return bm;
}

struct bitmap* new_bitmap(size_t size_in_bit)
{
    struct bitmap *bm;
    int size_in_word = DIV_ROUND_UP(size_in_bit, 32);

    bm = malloc(sizeof(*bm) + 4 * size_in_word);
    if (!bm)
        error("bitmap malloc failed\n");

    bm->size_in_bit = size_in_bit;
    bm->size_in_word = size_in_word;

    memset(bm->bits, 0, 4 * bm->size_in_word);

    return bm;
}

struct bitmap* new_bitmap_cpy(struct bitmap *src)
{
    struct bitmap *bm;

    bm = malloc(sizeof_bm(*src));
    if (!bm)
        error("bitmap malloc failed\n");

    memcpy(bm, src, sizeof_bm(*src));

    return bm;
}

struct bitmap* bitmap_set_bit(struct bitmap *bm, int bit)
{
    if (bit > bm->size_in_bit)
        error("bitmap set out of range. %d > %d\n", bit, (int) bm->size_in_bit);

    assert(bit > 0);
    bm->bits[DIV_ROUND_UP(bit, 32) - 1] |= 1 << ((bit - 1) % 32);

    return bm;
}

struct bitmap* bitmap_or(struct bitmap *bm, struct bitmap *bm1, struct bitmap *bm2)
{
    int i;

    for (i = 0; i < bm->size_in_word &&
            i < bm1->size_in_word && i < bm2->size_in_word; i ++)
        bm->bits[i] = bm1->bits[i] | bm2->bits[i];

    if (bm->size_in_bit % 32)
        bm->bits[DIV_ROUND_UP(bm->size_in_bit, 32) - 1] &=
            (((uint32_t) ~0) >> (32 - (bm->size_in_bit % 32)));

    return bm;
}

struct bitmap* bitmap_set_bm(struct bitmap *bm, struct bitmap *bmm)
{
    return bitmap_or(bm, bm, bmm);
}

int find_next_bit(struct bitmap *bm, int start)
{
    int bit = start;

    for (; bit <= bm->size_in_bit; bit ++)
        if (is_bit_set(bm, bit))
            return bit;

    return -1;
}

int is_bitmap_empty(struct bitmap *bm)
{
    int i;

    for (i = 0; i < bm->size_in_word; i ++)
        if (bm->bits[i])
            return 0;

    return 1;
}

int bitmap_eq(struct bitmap *bm1, struct bitmap *bm2)
{
    return !memcmp(bm1, bm2, sizeof_bm(*bm1));
}
