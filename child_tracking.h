#ifndef CHILD_TRACKING_H
#define CHILD_TRACKING_H

#include <time.h>

void track_child_process(const char *command, struct timespec *start_time);

#endif