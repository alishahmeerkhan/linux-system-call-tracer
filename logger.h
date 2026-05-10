#ifndef LOGGER_H
#define LOGGER_H

#include <sys/types.h>

void syscall_logger(pid_t pid, const char *syscall_name, long return_value, double elapsed_time);

#endif