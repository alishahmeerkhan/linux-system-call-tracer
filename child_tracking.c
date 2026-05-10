#define _POSIX_C_SOURCE 199309L

#include "child_tracking.h"
#include "colors.h"
#include "syscall_names.h"
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <time.h>

void track_child_process(const char* command, struct timespec *start_time) {
    pid_t target_pid = fork();

	if (target_pid < 0) {
		printf("%s[ERROR]: Failed to fork process.%s\n", COLOR_BOLD_RED, COLOR_RESET);
		exit(EXIT_FAILURE);
	}
	else if (target_pid == 0) {
		printf("[INFO] Child Process Started with PID %s%d%s\n", COLOR_CYAN, getpid(), COLOR_RESET);
		
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);

		execlp(command, command, NULL);
		printf("%s[ERROR]: Failed to execute command %s%s\n", COLOR_BOLD_RED, command, COLOR_RESET);
		printf("%s[HELP] Please check if the command is valid.%s\n", COLOR_GREY, COLOR_RESET);
		exit(1);
	}
	else {
		int status; // child returned status
		while (1) {
			waitpid(target_pid, &status, 0);

			if (WIFEXITED(status)) {
				struct timespec end_time;
            	clock_gettime(CLOCK_MONOTONIC, &end_time);
				double elapsed_time = (end_time.tv_sec - start_time->tv_sec) + (end_time.tv_nsec - start_time->tv_nsec) / 1e9;
				printf("[INFO] Total Tracking Time: %s%.2f seconds%s\n", COLOR_BOLD_GREEN, elapsed_time, COLOR_RESET);
				printf("\n\n");
				printf("[INFO] Child Process with PID %s%d%s exited with status %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_YELLOW, WEXITSTATUS(status), COLOR_RESET);
				break;
			}
			else if (WIFSIGNALED(status)) {
				printf("[INFO] Child Process with PID %s%d%s terminated by signal %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_RED, WTERMSIG(status), COLOR_RESET);
				break;
			}
			else if (WIFSTOPPED(status)) {
				static int isEntry = 1;
				unsigned long syscall_id  = 0;
				char syscall_name[30];

				if (isEntry) {
					struct user_regs_struct regs;
					ptrace(PTRACE_GETREGS, target_pid, NULL, &regs);
					syscall_id = regs.orig_rax;

					printf("[INFO] Process %s%d%s called System Call: %s%lu%s", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_BOLD_GREEN, syscall_id, COLOR_RESET);
					
					strcpy(syscall_name, get_syscall_name(syscall_id));
					
					
					isEntry = 0;
					ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
				}
				else {
					struct user_regs_struct regs;
					ptrace(PTRACE_GETREGS, target_pid, NULL, &regs);
					long int return_value = regs.orig_rax;

					struct timespec end_time;
            		clock_gettime(CLOCK_MONOTONIC, &end_time);
					double elapsed_time = (end_time.tv_sec - start_time->tv_sec) + (end_time.tv_nsec - start_time->tv_nsec) / 1e9;

					printf("\t%s[%f]%s\n", COLOR_BOLD_GREEN, elapsed_time, COLOR_RESET);

					syscall_logger(target_pid, syscall_name, return_value, elapsed_time);

					isEntry = 1;
				}

				ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
				continue;
			}
		}
	}
}