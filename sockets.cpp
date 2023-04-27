#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cmath>

#define PORT 10800

struct Producer {
    int socket_fd{};
    struct sockaddr_in consumer_address{};
    int port = PORT;

    Producer() {
        create_socket();
        connect_to_consumer();
        srand(time(nullptr));
    }

    void create_socket() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
    }

    void connect_to_consumer() {
        memset(&consumer_address, '0', sizeof(consumer_address));
        consumer_address.sin_family = AF_INET;
        consumer_address.sin_port = htons(port);

        if (inet_pton(AF_INET, "127.0.0.1", &consumer_address.sin_addr) <= 0) {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            exit(1);
        }

        if (connect(socket_fd, (struct sockaddr*)&consumer_address, sizeof(consumer_address)) < 0) {
            std::cerr << "Connection Failed" << std::endl;
            exit(1);
        }
    }

    void produce() {
        while (true) {
            int random_num = rand() % 1000 + 1;
            std::string message = std::to_string(random_num);

            send(socket_fd, message.c_str(), message.length(), 0);

            char buffer[16] = {0};
            recv(socket_fd, buffer, sizeof(buffer), 0);

            std::cout << "Received: " << buffer << std::endl;

            sleep(1);
        }
    }
};

struct Consumer {
    int socket_fd{};
    struct sockaddr_in address{};
    int port = PORT;

    Consumer() {
        create_socket();
        bind_address();
        start_listen();
    }

    void create_socket() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd == 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(1);
        }
    }

    void bind_address() {
        int addr_len = sizeof(address);

        memset(&address, '0', addr_len);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(socket_fd, (struct sockaddr *)&address, addr_len) < 0) {
            std::cerr << "Bind failed" << std::endl;
            exit(1);
        }
    }

    void start_listen() {
        if (listen(socket_fd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            exit(1);
        }
    }

    static bool is_prime(int n) {
        if (n <= 1) {
            return false;
        }

        for (int i = 2; i <= sqrt(n); i++) {
            if (n % i == 0) {
                return false;
            }
        }

        return true;
    }

    void consume() {
        int producer_socket;
        int addr_len = sizeof(address);

        if ((producer_socket = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            exit(1);
        }

        while (true) {
            char buffer[16] = {0};
            recv(producer_socket, buffer, sizeof(buffer), 0);
            int number = std::stoi(buffer);

            std::cout << "Consumed: " << number << std::endl;

            std::string message = is_prime(number) ? "Prime" : "Not Prime";
            send(producer_socket, message.c_str(), message.length(), 0);
        }
    }
};

int main() {
    int producer_pid = fork();
    if (producer_pid == 0) {
        Producer producer;
        producer.produce();
    }

    int consumer_pid = fork();
    if (consumer_pid == 0) {
        Consumer consumer;
        consumer.consume();
    }

    int status;
    waitpid(producer_pid, &status, 0);
    waitpid(consumer_pid, &status, 0);

    return 0;
}