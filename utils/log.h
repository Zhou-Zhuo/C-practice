#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define logd(x...) fprintf(stderr, x)
#define logi(x...) fprintf(stderr, x)
#define logw(x...) fprintf(stderr, x)
#define loge(x...) fprintf(stderr, x)

#endif
