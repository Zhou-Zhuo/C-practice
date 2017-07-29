#ifndef RW_ALL_H
#define RW_ALL_H

int write_all(int fd, const void *buf, size_t size);
int read_all(int fd, void *buf, size_t size);

#endif
