#include "common.h"
#include "log.h"

int write_all(int fd, const void *buf, size_t size)
{
    int left = size;
    int total = 0;
    int once = 0;

    while (left > 0) {
        once = write(fd, (char*)buf + total, left);
        if (once < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                usleep(1000 * 5);
                continue;
            } else {
                loge("%s: %s\n", __func__, strerror(errno));
                return once;
            }
        }

        left -= once;
        total += once;
    }

    return total;
}

int read_all(int fd, void *buf, size_t size)
{
    int left = size;
    int total = 0;
    int once = 0;

    while (left > 0) {
        once = read(fd, (char*)buf + total, left);
        if (once < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN) {
                usleep(1000 * 5);
                continue;
            } else {
                loge("%s: %s\n", __func__, strerror(errno));
                return once;
            }
        }
        if (once == 0)
            break;

        left -= once;
        total += once;
    }

    return total;
}
