#ifndef EXTERNAL_TRACKING_H
#define EXTERNAL_TRACKING_H

#include <sys/types.h>
#include <time.h>

void track_external_process(pid_t pid, struct timespec *start_time);

#endif