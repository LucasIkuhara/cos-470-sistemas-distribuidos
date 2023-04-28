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


struct Consumer {
    int socket_fd{};
    struct sockaddr_in address{};
    int port = PORT;

    Consumer() {
        printf("Creating socket.\n");
        create_socket();
        printf("Socket created.\n");
        printf("Socket binding.\n");

        bind_address();
        printf("Start to listen.\n");

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

        while (true) {
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
    }
};

int main(int argc, char *argv[]) {

    Consumer consumer;
    consumer.consume();

    return 0;
}
