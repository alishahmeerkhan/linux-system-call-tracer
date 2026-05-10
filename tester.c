#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("This is a test program for the process tracker.\n");
    
    while (1) {
        printf("Running... Press Ctrl+C to exit.\n");
        sleep(10);
        execlp("ls", "ls", NULL);  // This will replace the current process with 'ls' command
        // If execlp fails, we will print an error message and exit
        perror("execlp failed");
        exit(1);
    }
    return 0;
}