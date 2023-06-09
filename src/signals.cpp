#include <iostream>
#include <unistd.h>
#include <csignal>
#include <sys/wait.h>
#include <string.h>

struct SignalSender {
    const pid_t target_pid;

    void send_signal(const int signal_num) const {
        auto result = kill(target_pid, signal_num);
        if (result == -1) {
            std::cerr << "Error: Process with PID " << target_pid << " does not exist\n";
        }
    }
};

struct SignalReceiver {
    const bool use_blocking_wait;

    void run() {
        signal(SIGINT, handle_sigint);
        signal(SIGTERM, handle_sigterm);
        signal(SIGUSR1, handle_sigusr1);

        std::cout << "SignalReceiver is waiting for signals...\n";

        if (use_blocking_wait) {
            blocking_wait();
        } else {
            busy_wait();
        }
    }

    static void blocking_wait() {
        while (true) {
            pause();
        }
    }

    static void busy_wait() {
        while (true) {
            std::cout << "Busy waiting...\n";
            sleep(1);
        }
    }

    static void handle_sigint(const int signal_num) {
        std::cout << "Received SIGINT signal\n";
    }

    static void handle_sigusr1(const int signal_num) {
        std::cout << "Received SIGUSR1 signal\n";
    }

    static void handle_sigterm(const int signal_num) {
        std::cout << "Received SIGTERM signal\n";
        exit(EXIT_SUCCESS);
    }
};


int main(int argc, char *argv[]) {

    int blockMode;
    if (strcmp(argv[1], "block\n") == 0 || strcmp(argv[1], "block") == 0) { 
        printf("Blocking-wait will be used.\n");
        blockMode = 1;
    }
    else {
        printf("Busy-wait will be used.\n");
        blockMode = 0;
    }

    auto receiver_pid = fork();
    if (receiver_pid == 0) {
        auto receiver = SignalReceiver{blockMode};
        receiver.run();
        exit(EXIT_SUCCESS);
    }

    auto sender_pid = fork();
    if (sender_pid == 0) {
        auto sender = SignalSender{receiver_pid};

        for (const auto signal : {SIGINT, SIGUSR1, SIGTERM}) {
            sender.send_signal(signal);
            sleep(1);
        }

        exit(EXIT_SUCCESS);
    }

    // Wait for both processes to exit
    int status;
    wait(&status);

    return 0;
}