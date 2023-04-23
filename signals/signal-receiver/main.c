#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Map signals 
void treatSigUsr1(int sig) {
    printf("SIGUSR1 received.\n");
};

void treatSigUsr2(int sig) {
    printf("SIGUSR2 received.\n");
};

void treatSigInt(int sig) {
    printf("SIGINT received.\nAborting..\n");
    exit(0);
};


int main(int argc, char *argv[])
{
    // Read and print the receiver's PID
    int pid = getpid();
    printf("Signal receiver PID: %d\n", pid);

    // Check if an argument is present for busy vs blocking waiting
    if (argc < 2) {
        printf("Missing argument for busy or blocking waiting. Aborting..\n");
        exit(1);
    }

    // Decide between blocking or busy waiting
    int blockMode;
    if (strcmp(argv[1], "block\n") == 0 || strcmp(argv[1], "block") == 0) { 
        printf("Blocking-wait will be used.\n");
        blockMode = 1;
    }
    else {
        printf("Busy-wait will be used.\n");
        blockMode = 0;
    }

    // Register signal handlers
    if (signal(SIGUSR1, treatSigUsr1) == SIG_ERR)
       printf("Unable to register a handler for signal SIGUSR1");
        
    if (signal(SIGUSR2, treatSigUsr2) == SIG_ERR)
       printf("Unable to register a handler for signal SIGUSR2");
        
    if (signal(SIGINT, treatSigInt) == SIG_ERR)
       printf("Unable to register a handler for signal SIGTERM");

    // Wait for signals
    while (1) {
        if (blockMode) {
            sleep(1);
        }
    }

    return 0;
}