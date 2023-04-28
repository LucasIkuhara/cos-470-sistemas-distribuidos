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


struct Producer {
    int socket_fd{};
    struct sockaddr_in consumer_address{};
    int port = PORT;

    Producer() {
        create_socket();

        while(!connect_to_consumer()) {
            // printf("Retrying connection\n");
        }
        srand(time(nullptr));
    }

    void create_socket() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            std::cerr << "Error creating socket" << std::endl;
            exit(0);
        }
    }

    bool connect_to_consumer() {
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
            return false;
        }

        return true;
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

    void produce(int numbers_to_generate) {
        int last_number = 0;
        int produced_numbers = 0;

        while (produced_numbers <= numbers_to_generate) {
            auto random_num = last_number + (rand() % 100 + 1);
            last_number = random_num;

            send_number(random_num);

            auto response = receive_response();

            std::cout << "Received: " << response << std::endl;

            produced_numbers++;
            sleep(1);
        }

        send_number(0);

        close(socket_fd);
        exit(0);
    }
};

int main(int argc, char *argv[]) {

    // Read N
    int n = std::stoi(argv[1]);

    Producer producer;
    producer.produce(n);

    return 0;
}