#include "common.h"
#include "read_file.h"
#include "token_iter.h"

#define READ_FILE_LIMIT (1024 * 100)
#define read_file(f, b, l) read_file(f, b, l, READ_FILE_LIMIT)

int token_split(char *c)
{
    return isspace(*c);
}

int main(int argc, char **argv)
{
    int fd;
    void *pbuf;
    size_t len;
    int ret;

    if (argc != 2) {
        fprintf(stderr, "usage fault\n");
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "failed on open(): %s\n", strerror(errno));
        return 2;
    }

    ret = read_file(fd, &pbuf, &len);
    if (ret < 0) {
        fprintf(stderr, "failed on read_file()\n");
        return 3;
    }

    char *data = (char *) realloc(pbuf, len + 1);
    if (!data) {
        fprintf(stderr, "failed on realloc()\n");
        return 4;
    }
    data[len] = '\0';

    fprintf(stderr, "file size: %ld\n", len);

    char *l, *nl;
    for_each_line(l, nl, data)
        printf("%s\n\n", l);

    char *t, *nt;
    for_each_token(t, nt, data, token_split)
        printf("<%s>\n", t);

    free(data);

    close(fd);

    return 0;
}
