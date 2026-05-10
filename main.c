#define _POSIX_C_SOURCE 199309L

#include "colors.h"
#include "child_tracking.h"
#include "external_tracking.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>


int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("%s-- Error --%s\n", COLOR_BOLD_RED, COLOR_RESET);
		printf("%sCommand Line Format: 		sudo ./main -p pid		[Track External Process]%s\n", COLOR_GREY, COLOR_RESET);
		printf("%sCommand Line Format:		sudo ./main <command>	[Track Child Process]%s\n", COLOR_GREY, COLOR_RESET);
		return 1;
	}

	pid_t target_pid;
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);

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
		
		track_external_process(target_pid, &start_time);
	}
	else {  // child tracking
		track_child_process(argv[1], &start_time);
	}
}
