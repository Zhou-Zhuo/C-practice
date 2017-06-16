#ifndef BITMAP_H
#define BITMAP_H

#include "common.h"

struct bitmap {
    size_t size_in_bit;
    size_t size_in_word;
    uint32_t bits[];
};

#define sizeof_bm(bm) (sizeof(bm) + 4 * (bm).size_in_word)

#define DIV_ROUND_UP(a, b) (((a) + (b) - 1) / (b))

#define is_bit_set(bm, bit) \
    ((bm)->bits[DIV_ROUND_UP((bit), 32) - 1] & (1 << (((bit) - 1) % 32)))


int find_next_bit(struct bitmap *bm, int start);

#define for_each_set_bit(b, bm) \
    for ((b) = find_next_bit((bm), 1); \
            (b) != -1; \
            (b) = find_next_bit((bm), (b) + 1))

struct bitmap* bitmap_reset(struct bitmap *bm);
struct bitmap* new_bitmap(size_t size_in_bit);
struct bitmap* new_bitmap_cpy(struct bitmap *src);
struct bitmap* bitmap_set_bit(struct bitmap *bm, int bit);
struct bitmap* bitmap_set_bm(struct bitmap *bm, struct bitmap *bmm);
struct bitmap* bitmap_or(struct bitmap *bm, struct bitmap *bm1, struct bitmap *bm2);
int is_bitmap_empty(struct bitmap *bm);
int bitmap_eq(struct bitmap *bm1, struct bitmap *bm2);

#endif
