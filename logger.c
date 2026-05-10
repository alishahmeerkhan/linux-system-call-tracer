#include "logger.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void syscall_logger(pid_t pid, const char *syscall_name, long return_value, double elapsed_time) {
    int file_exists = access("syscall_log.log", F_OK) == 0;
    
    FILE *log_file = fopen("syscall_log.log", "a");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file\n");
        return;
    }

    if (!file_exists) {
        fprintf(log_file, "PID    | SYSCALL_NAME   | RETURN_VALUE | ELAPSED_TIME (s)\n");
        fprintf(log_file, "-------|----------------|--------------|-------------\n");
    }

    fprintf(log_file, "%-6d | %-16s | %-12ld | %.6f\n", pid, syscall_name, return_value, elapsed_time);
    fclose(log_file);
}