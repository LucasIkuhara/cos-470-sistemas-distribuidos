#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


// Map signals 
void treatSigUsr1(int sig) {
    printf("SIGUSR1 received.\n");
};

void treatSigUsr2(int sig) {
    printf("SIGUSR2 received.\n");
};

void treatSigInt(int sig) {
    printf("SIGINT received.\nAborting..");
    exit(0);
};


int main(void)
{

    // Register signal handlers
    if (signal(SIGUSR1, treatSigUsr1) == SIG_ERR)
       printf("Unable to register a handler for signal SIGUSR1");
        
    if (signal(SIGUSR2, treatSigUsr2) == SIG_ERR)
       printf("Unable to register a handler for signal SIGUSR2");
        
    if (signal(SIGINT, treatSigInt) == SIG_ERR)
       printf("Unable to register a handler for signal SIGTERM");

    while (1) {
        sleep(1);
    }

    return 0;
}