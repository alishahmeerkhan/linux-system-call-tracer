#define _POSIX_C_SOURCE 199309L

#include "external_tracking.h"
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

void track_external_process(pid_t target_pid, struct timespec *start_time) {
    printf("[INFO] External Process Tracking Started for PID: %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET);



	if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) < 0) {
		printf("[ERROR] Failed to attach to process with PID %s%d%s\n", COLOR_BOLD_RED, target_pid, COLOR_RESET);
		exit(EXIT_FAILURE);
	}

	waitpid(target_pid, NULL, 0);

	ptrace(PTRACE_SETOPTIONS, target_pid, NULL, PTRACE_O_TRACESYSGOOD);	

	ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);

	while (1) {
		int status;
		waitpid(target_pid, &status, 0);

		if (WIFEXITED(status)) {
			struct timespec end_time;
            clock_gettime(CLOCK_MONOTONIC, &end_time);
			double elapsed_time = (end_time.tv_sec - start_time->tv_sec) + (end_time.tv_nsec - start_time->tv_nsec) / 1e9;
			printf("\n[INFO] Total Tracking Time: %s%.2f seconds%s\n", COLOR_BOLD_GREEN, elapsed_time, COLOR_RESET);
			printf("\n\n");
			printf("[INFO] External Process with PID %s%d%s exited with status %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_YELLOW, WEXITSTATUS(status), COLOR_RESET);
			break;
		}
		else if (WIFSIGNALED(status)) {
			printf("[INFO] External Process with PID %s%d%s terminated by signal %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_RED, WTERMSIG(status), COLOR_RESET);
			break;
		}
		else if (WIFSTOPPED(status)) {
			static int isEntry = 1;
			unsigned long syscall_id = 0;
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

				syscall_logger(target_pid, syscall_name, 0, elapsed_time);

				isEntry = 1;
			}

			ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
			continue;
		}
	}
}