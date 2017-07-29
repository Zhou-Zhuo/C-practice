#include "common.h"
#include "rw_all.h"

ssize_t get_file_len(int fd)
{
    ssize_t save, l;

    save = lseek(fd, 0, SEEK_CUR);
    if (save < 0) {
        loge("failed on lseek(SEEK_CUR): %s\n", strerror(errno));
        return save;
    }

    l = lseek(fd, 0, SEEK_END);
    if (l < 0) {
        loge("failed on lseek(SEEK_END): %s\n", strerror(errno));
        return l;
    }

    save = lseek(fd, save, SEEK_SET);
    if (save < 0) {
        loge("failed on lseek(save): %s\n", strerror(errno));
        return save;
    }

    return l;
}

/* free *ppbuf outside if succeeded */
int read_file(int fd, void **ppbuf, size_t *plen, size_t limit)
{
    ssize_t l;
    void *pbuf = NULL;

    l = get_file_len(fd);
    if (l < 0) {
        loge("failed to get file length\n");
        return -1;
    } else if (l > limit) {
        loge("file larger than limit(%ld)\n", limit);
        return -2;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        loge("failed to seek to file set: %s\n", strerror(errno));
        return -1;
    }


    pbuf = malloc(l);
    if (!pbuf) {
        loge("failed on malloc()\n");
        return -1;
    }

    if (read_all(fd, pbuf, l) != l) {
        loge("failed on read_all(): %s\n", strerror(errno));
        goto err;
    }

    *ppbuf = pbuf;
    *plen = l;

    return 0;

err:
    if (pbuf)
        free(pbuf);
    return -1;
}
