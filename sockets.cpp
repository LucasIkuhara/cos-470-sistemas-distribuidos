#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cmath>
#include <sys/wait.h>
#include <cstring>


const int PORT = 10800;
volatile bool program_terminated = false;

struct Producer {
    int socket_fd{};
    struct sockaddr_in consumer_address{};
    int port = PORT;

    Producer() {
        signal(SIGINT, [](int n){ program_terminated = true; });
        create_socket();
        connect_to_consumer();
        srand(time(nullptr));
    }

    void create_socket() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(0);
        }
    }

    void connect_to_consumer() {
        memset(&consumer_address, '0', sizeof(consumer_address));
        consumer_address.sin_family = AF_INET;
        consumer_address.sin_port = htons(port);

        if (inet_pton(AF_INET, "127.0.0.1", &consumer_address.sin_addr) <= 0) {
            std::cerr << "Invalid address/ Address not supported" << std::endl;
            exit(0);
        }

        if (connect(socket_fd, (struct sockaddr*)&consumer_address, sizeof(consumer_address)) < 0) {
            std::cerr << "Connection Failed" << std::endl;
            close(socket_fd);
            exit(0);
        }
    }

    void send_number(int number) {
        auto message = std::to_string(number);
        send(socket_fd, message.c_str(), message.length(), 0);
    }

    std::string receive_response() {
        char buffer[16] = {0};
        recv(socket_fd, buffer, sizeof(buffer), 0);
        return std::string(buffer);
    }

    void produce() {
        int last_number = 0;

        while (!program_terminated) {
            auto random_num = last_number + (rand() % 100 + 1);
            last_number = random_num;

            send_number(random_num);

            auto response = receive_response();

            std::cout << "Received: " << response << std::endl;

            sleep(1);
        }

        std::cout << "Producer: Received SIGINT. Sending 0 and exiting" << std::endl;
        send_number(0);
        // sleep(1);    

        close(socket_fd);
        exit(0);
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
            exit(0);
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
            close(socket_fd);
            exit(0);
        }
    }

    void start_listen() {
        if (listen(socket_fd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            close(socket_fd);
            exit(0);
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
            exit(0);
        }

        while (true) {
            char buffer[16] = {0};
            recv(producer_socket, buffer, sizeof(buffer), 0);
            int number = std::stoi(buffer);

            if(number == 0) {
                std::cout << "Consumer: Received 0. Exiting" << std::endl;
                close(socket_fd);
                close(producer_socket);
                exit(0);
            }

            std::cout << "Consumed: " << number << std::endl;

            std::string message = is_prime(number) ? "Prime" : "Not Prime";
            send(producer_socket, message.c_str(), message.length(), 0);
        }
    }
};

int main() {

    int consumer_pid = fork();
    if (consumer_pid == 0) {
        Consumer consumer;
        consumer.consume();
    }

    else {
        Producer producer;
        producer.produce();
    }

    int status;
    wait(&status);

    return 0;
}
