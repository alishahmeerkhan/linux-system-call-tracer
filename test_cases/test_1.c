#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("This is a test program 1 for the process tracker.\n");
    
    while (1) {
        printf("Running... Press Ctrl+C to exit.\n");
        sleep(10);
        execlp("ls", "ls", NULL);
        perror("execlp failed");
        exit(1);
    }
    return 0;
}