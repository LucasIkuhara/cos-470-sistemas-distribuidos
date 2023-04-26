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
Checks wether a number is a prime or not. If it is, return 1, else 0.
*/
int isPrimeNumber(int n) { 

    for (int i = 2; i <= n / 2; ++i) {

        if (n % i == 0) {
            return 0;
        }
  }

  return 1;
}

/*
Consumer source-code.
Responsible for reading numbers.
*/
void consumer(int* descriptor) {

    printf("Started consumer.\n");
    int lastNum = 1;

    // Create reading loop
    while(1) {

        // Read pipe output and store at lastNum
        read(descriptor[0], &lastNum, sizeof(int));

        // Stop upon receiving zero
        if (lastNum == 0) break;

        // Checks if lastNum is a prime
        if(isPrimeNumber(lastNum)) {
            printf("Consumer: Received %d, which is a prime.\n", lastNum);
        }

        else {
            printf("Consumer: Received %d, which is not a prime.\n", lastNum);
        }
    }

}

/*
Producer source-code.
Responsible for generating numbers.
*/
void producer(int* descriptor, int n) {

    printf("Started producer.\n");
    int value = 1;

    // Use time as Seed for RNG
    srand(time(NULL));

    for (int i = 0; i < n; i++) {

        // Random value generation from 1 to 100
        int delta = (rand() % 100) + 1;
        value = value + delta;

        // Send value N by writing to the pipe
        write (descriptor[1], &value, sizeof(int));
    }

    // Send stop command to consumer
    int stop = 0;
    write (descriptor[1], &stop, sizeof(int));
}



int main(int argc, char *argv[]) {

    // Check if an argument is present for busy vs blocking waiting
    if (argc < 2) {
        printf("Missing argument N. A number of iterations should be given as an argument. Eg. './pipes.out 100'\nAborting.\n");
        exit(1);
    }

    // Parse argv to int
    char *p;
    int n = (int) strtol( argv[1],&p, 10);

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
        producer(pipeDesc, n);
    }

    // Consumer code
    else {
        consumer(pipeDesc);
    }

    return 0;
}