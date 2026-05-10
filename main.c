#include "colors.h"
// #include "logger.h"

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
	time_t start_time = time(NULL);

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



		if (ptrace(PTRACE_ATTACH, target_pid, NULL, NULL) < 0) {
			printf("[ERROR] Failed to attach to process with PID %s%d%s\n", COLOR_BOLD_RED, target_pid, COLOR_RESET);
			return 1;
		}

		waitpid(target_pid, NULL, 0);

		ptrace(PTRACE_SETOPTIONS, target_pid, NULL, PTRACE_O_TRACESYSGOOD);	

		ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);

		while (1) {
			int status;
			waitpid(target_pid, &status, 0);

			if (WIFEXITED(status)) {
				time_t end_time = time(NULL);
				double elapsed_time = difftime(end_time, start_time);
				printf("[INFO] Total Tracking Time: %s%.2f seconds%s\n", COLOR_BOLD_GREEN, elapsed_time, COLOR_RESET);
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

				if (isEntry) {
					struct user_regs_struct regs;
					ptrace(PTRACE_GETREGS, target_pid, NULL, &regs);
					unsigned long syscall_id = regs.orig_rax;

					time_t end_time = time(NULL);;
					double elapsed_time = difftime(end_time, start_time);
					printf("%f", elapsed_time);
					printf("[INFO] Process %s%d%s called System Call: %s%lu%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_BOLD_GREEN, syscall_id, COLOR_RESET);
					// log_syscall_entry(target_pid, syscall_id);

					FILE *log_file = fopen("syscall_log.log", "a");
    				fprintf(log_file, "[PID %d] Entered syscall: %lu\n", target_pid, syscall_id);
    				fclose(log_file);
					
					isEntry = 0;
					ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
				}
				else {
					isEntry = 1;
				}

				ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
				continue;
			}
		}
	}
	else {  // child tracking
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
					static int isEntry = 1;

					if (isEntry) {
						struct user_regs_struct regs;
						ptrace(PTRACE_GETREGS, target_pid, NULL, &regs);
						unsigned long syscall_id = regs.orig_rax;

						time_t end_time = time(NULL);
						double elapsed_time = difftime(end_time, start_time);
						printf("%f", elapsed_time);
						printf("[INFO] Process %s%d%s called System Call: %s%lu%s\n", COLOR_CYAN, target_pid, COLOR_RESET, COLOR_BOLD_GREEN, syscall_id, COLOR_RESET);
						// log_syscall_entry(target_pid, syscall_id);

						FILE *log_file = fopen("syscall_log.log", "a");
    					fprintf(log_file, "[PID %d] Entered syscall: %lu\n", target_pid, syscall_id);
    					fclose(log_file);
					
						isEntry = 0;
						ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
					}
					else {
						isEntry = 1;
					}

					ptrace(PTRACE_SYSCALL, target_pid, NULL, NULL);
					continue;
				}
			}
		}
	}
}
