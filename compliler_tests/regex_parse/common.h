#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define error(x...) \
    do { \
        fprintf(stderr, x); \
        exit(1); \
    } while(0)

#endif
