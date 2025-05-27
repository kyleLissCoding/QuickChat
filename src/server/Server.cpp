// Server implementation

#include "Server.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

Server::Server(int port) : running(false) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        throw std::runtime_error("Failed to create server socket");
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        throw std::runtime_error("Failed to bind server socket");
    }

}

Server::~Server() {
    stop();
    // Close the server socket file descriptor to free up system resources
    close(server_socket);
}

void Server::start() {
    running = true;
    // Listen for incoming connections, allowing up to 5 pending connections in the queue
    listen(server_socket, 5);

    std::cout << "Server started, waiting for connections..." << std::endl;

    while (running) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            if (running) {
                std::cerr << "Failed to accept client connection" << std::endl;
            }
            continue;
        }

        std::lock_guard<std::mutex> lock(clients_mutex);
        client_sockets.push_back(client_socket);

        std::thread client_thread(&Server::handle_client, this, client_socket);
        client_thread.detach();
    }
}

void Server::stop() {

}

void Server::handle_client(int client_socket) {

}

void Server::broadcast_message(const std::string& message, int sender_socket) {

}