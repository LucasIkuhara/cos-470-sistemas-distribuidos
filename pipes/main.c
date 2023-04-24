#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>  
#include <errno.h> 
#include <stdlib.h>


// Creates a new pipe and save descriptors to the input array
void createPipe(int descriptor[]) {

    // Creates a new pipe
    if(pipe(descriptor) < 0) {

        // Error handling
        printf("Failed to create a pipe. Aborting.\n");
        exit(1);
    }
}

// Closes an open pipe given its descriptors
void closePipe(int* descriptor) {

    close (descriptor[0]);
    close (descriptor[1]);
}



int main(void) {

    // Creates a new pipe using this int[] as a descriptor
    int pipeDesc[2];
    createPipe(pipeDesc);

    // Fork to create a new process
    int pid = fork ();
    if (pid < 0) {

        // Error handling for fork command
        printf("Failed to perform fork. Aborting.\n");
        exit(1); // Finaliza o programa com código 1
    }

    // Producer code
    if (pid > 0) {
        printf("A");
    }

    // Consumer code
    else {
        printf("B");

    }

    return 0;
}