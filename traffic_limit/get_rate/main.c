#include "common.h"
#include "alarm.h"
#include "token_iter.h"
#include <sys/timerfd.h>

int token_split(char *c)
{
    return isspace(*c);
}

static int parse_bytes(const char *buf, int len,
        const char *intf, uint64_t *rx, uint64_t *tx)
{
    char *data;

    data = malloc(len + 1);
    if (!data)
        error("failed on malloc()\n");
    memcpy(data, buf, len);
    data[len] = '\0';

    char *l, *nl;
    char tmp[128];
    int il = strlen(intf);
    int found = 0;
    int j = 1;
    for_each_line(l, nl, data) {
        if (j++ < 3)
            continue;
        char *t, *nt;
        int i = 1;
        for_each_token(t, nt, l, token_split) {
            if (i == 1 && !(strncmp(t, intf, il) == 0 && t[il] == ':'))
                continue;
            found = 1;
            if (i == 2)
                *rx = atoll(t);
            if (i == 10) {
                *tx = atoll(t);
                break;
            }
            ++i;
        }
    }

    free(data);
    if (!found)
        return -ENODEV;

    return 0;
}

#define PERIOD 1000000 /* us */

int main(int argc, char **argv)
{
    int fd;
    int afd;
    int ret;
    char buf[4096];
    size_t len;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <interface>\n", argv[0]);
        exit(1);
    }

    fd = open("/proc/net/dev", O_RDONLY);
    if (fd < 0)
        error("open() failed: %s\n", strerror(errno));

    afd = create_alarm(PERIOD);
    if (afd < 0)
        error("create_alarm() failed\n");

    unsigned long long wakeups;
    uint64_t rx, tx, last_rx = -1, last_tx = -1;
    double rx_rate, tx_rate;
    int f1 = 0, f2 = 0;
    while (1) {
        ret = lseek(fd, 0, SEEK_SET);
        if (ret < 0)
            error("failed on lseek(): %s\n", strerror(errno));
        len = read(fd, buf, 4096);
        if (len < 0)
            error("read() failed: %s\n", strerror(errno));

        ret = parse_bytes(buf, len, argv[1], &rx, &tx);
        if (ret < 0)
            error("failed to parse bytes: %s\n",
                    ret == -ENODEV ? strerror(ENODEV) : "");

        if (f1) {
            rx_rate = (double) (rx - last_rx) / ((double) PERIOD / 1000000.0);
            tx_rate = (double) (tx - last_tx) / ((double) PERIOD / 1000000.0);
            const char *move = "\033[2A";
            static int f = 0;
            if (f2)
                write(STDOUT_FILENO, move, strlen(move));
            else
                f2 = 1;
            printf("RX: %20.2f B/s\nTX: %20.2f B/s\n", rx_rate, tx_rate);
        } else {
            f1 = 1;
        }

        last_rx = rx;
        last_tx = tx;

        ret = read(afd, &wakeups, sizeof(wakeups));
        if (ret < 0)
            error("read(alarm_fd) failed: %s\n", strerror(errno));
    }

    return 0;
}
