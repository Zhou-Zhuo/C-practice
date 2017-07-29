#include "common.h"
#include <sys/timerfd.h>

int create_alarm(unsigned int period /* us */)
{
    int tfd;
    struct itimerspec itval;
    int ret;

    tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (tfd < 0) {
        loge("failed on timerfd_create(): %s\n", strerror(errno));
        return -1;
    }

    int sec = period / 1000000;
    int nsec = (period % 1000000) * 1000;
    itval.it_interval.tv_sec = sec;
    itval.it_interval.tv_nsec = nsec;
    itval.it_value.tv_sec = sec;
    itval.it_value.tv_nsec = nsec;
    ret = timerfd_settime(tfd, 0, &itval, NULL);
    if (ret < 0) {
        loge("failed on timerfd_settime(): %s\n", strerror(errno));
        return ret;
    }

    return tfd;
}
