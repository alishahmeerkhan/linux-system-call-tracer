#include "colors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>


int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("%s-- Error --%s\n", COLOR_BOLD_RED, COLOR_RESET);
		printf("%sCommand Line Format: 		./main -p pid		[Track External Process]%s\n", COLOR_GREY, COLOR_RESET);
		printf("%sCommand Line Format:		./main <command>	[Track Child Process]%s\n", COLOR_GREY, COLOR_RESET);
		return 1;
	}

	pid_t target_pid;

	if (strncmp(argv[1], "-p", 2) == 0) {  /* Track External Process  */
		pid_t pid;

		if (argc < 3) {
			printf("%s-- Error --%s\n", COLOR_BOLD_RED, COLOR_RESET);
			printf("%sMissing PID for external process tracking.%s\n", COLOR_GREY, COLOR_RESET);
			return 1;
		}
		target_pid = atoi(argv[2]);
		if (target_pid < 0) {
			printf("%s-- Error --%s\n", COLOR_BOLD_RED, COLOR_RESET);
			printf("%sInvalid PID for external process tracking.%s\n", COLOR_GREY, COLOR_RESET);
			return 1;
		}
		
		printf("[INFO] External Process Tracking Started for PID: %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET);

		// ptrace logic here
	}
	else {
		target_pid = fork();

		if (target_pid < 0) {
			printf("%s[ERROR]: Failed to fork process.%s\n", COLOR_BOLD_RED, COLOR_RESET);
			return 1;
		}
		else if (target_pid == 0) {
			printf("[INFO] Child Process Started with PID %s%d%s\n", COLOR_CYAN, getpid(), COLOR_RESET);
			
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);

			execlp(argv[1], argv[1], NULL);
			printf("%s[ERROR]: Failed to execute command %s%s\n", COLOR_BOLD_RED, argv[1], COLOR_RESET);
			printf("%s[HELP] Please check if the command is valid.%s\n", COLOR_GREY, COLOR_RESET);
			exit(1);
		}
		else {
			int status; // child returned status
			while (1) {
				waitpid(target_pid, &status, 0);

				if (WIFEXITED(status)) {
					printf("\n\n");
					printf("[INFO] Child Process with PID %s%d%s exited with status %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_YELLOW, WEXITSTATUS(status), COLOR_RESET);
					break;
				}
				else if (WIFSIGNALED(status)) {
					printf("[INFO] Child Process with PID %s%d%s terminated by signal %s%d%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_RED, WTERMSIG(status), COLOR_RESET);
					break;
				}
				else if (WIFSTOPPED(status)) {
					ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
					continue;
				}
			}
		}
	}
}
