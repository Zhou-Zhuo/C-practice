#ifndef READ_FILE_H
#define READ_FILE_H

#include "common.h"

ssize_t get_file_len(int fd);
int read_file(int fd, void **ppbuf, size_t *plen, size_t limit);

#endif
