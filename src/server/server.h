// Structure for the server side of the chat

#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>


class Server {
    private:
        int server_socket;
        std::atomic<bool> running;
        std::vector<int> client_sockets;
        std::mutex clients_mutex;

        void handle_client(int client_socket);
        void broadcast_message(const std::string& message, int sender_socket);

    public:
        Server(int port = 8080);
        ~Server();

        void start();
        void stop();

};