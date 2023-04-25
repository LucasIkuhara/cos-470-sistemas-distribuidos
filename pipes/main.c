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

/*
Consumer source-code.
Responsible for reading numbers.
*/
void consumer(int* descriptor) {

    printf("Started consumer.\n");
    int lastNum = 1;

    // Create reading loop
    while(lastNum != 0) {

        // Read pipe output and store at lastNum
        read(descriptor[0], &lastNum, sizeof(int));
        printf("Consumer: %d\n", lastNum);
    }

}

/*
Producer source-code.
Responsible for generating numbers.
*/
void producer(int* descriptor, int n) {

    printf("Started producer.\n");

    for (int i = 0; i < n; i++) {

        // Send value N by writing to the pipe
        write (descriptor[1], &n, sizeof(int));
    }
}



int main(int argc, char *argv[]) {

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
        producer(pipeDesc, 100);
    }

    // Consumer code
    else {
        consumer(pipeDesc);
    }

    return 0;
}