#include "bitmap.h"
#include <string.h>

int main()
{
    struct bitmap *bm = new_bitmap(256);
    uint32_t bits[] = {0x12345678, 0xdeadbeef, 0xffffffff, 0xffffffff,
    0x55aa55aa, 0x55aa55aa, 0x00000000, 0x98765432};

    memcpy(bm->bits, bits, 32);

    int b;
    uint8_t bytes[32] = {0};
    for_each_set_bit(b, bm) {
        bytes[(b + 7) / 8 - 1] |= (1 << ((b - 1) % 8));
    }
    int i;
    for (i = 0; i < 32; i ++)
        printf("%02x ", bytes[i]);
    printf("\n");

    return 0;
}
