#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("This is a test program 3 for the process tracker.\n");
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        execlp("pwd", "pwd", NULL);
        perror("execlp failed");
        exit(1);
    } 
    else {
        // Parent process
        wait(NULL); // Wait for the child process to finish
    }
    return 0;
} 