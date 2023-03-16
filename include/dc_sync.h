#ifndef DC_SYNC_H
#define DC_SYNC_H

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <sys/time.h>


#define NANOSEC_PER_SEC (1000000000L)

long int period_nanosec(int frequency);

uint64_t timespecToNanoSec(struct timespec ts);

struct timespec addTimespec(struct timespect time1, struct timespec time2);

int sleep_task(clockid_t clock_to_use, int flag, const timespec* required_duration, timespec* remaining_duration);

#endif // DC_SYNC_H
