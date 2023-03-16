#include "dc_sync.h"


#include <bits/types/clockid_t.h>
#include <time.h>

long int period_nanosec(int frequency)
{
    return NANOSEC_PER_SEC / frequency;
}

uint64_t timespecToNanoSec(struct timespec ts)
{
    return (uint64_t)(ts.tv_sec * NANOSEC_PER_SEC + ts.tv_nsec);
}

struct timespec addTimespec(struct timespec time1, struct timespec time2)
{
    struct timespec res;

    if ((time1.tv_nsec + time2.tv_nsec) >= NANOSEC_PER_SEC) {
        res.tv_sec = time1.tv_sec + time2.tv_sec + 1;
        res.tv_nsec = time1.tv_nsec + time2.tv_nsec - NANOSEC_PER_SEC;
    } else {
        res.tv_sec = time1.tv_sec + time2.tv_sec;
        res.tv_nsec = time1.tv_nsec + time2.tv_nsec;
    }

    return res;
}

int sleep_task(clockid_t clock_to_use, int flag, const struct timespec* required_duration, struct timespec* remaining_duration)
{
    clock_nanosleep(clock_to_use, flag, required_duration, remaining_duration);
}