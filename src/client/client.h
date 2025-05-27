// Client for individual users

#pragma once
#include <string>
#include <thread>
#include <atomic>  // Provides atomic data types and operations for thread-safe concurrent programming
#include <sys/socket.h>     // Provides socket API functions for network communication (socket(), bind(), listen(), etc.)
#include <netinet/in.h>     // Defines Internet protocol/address structures like sockaddr_in for IPv4 networking

class Client {
    private:
        int client_socket;
        std::atomic<bool> running;
        std::thread receive_thread;

        void receive_messages();

    public:
        Client();
        ~Client();

        bool connect(const std::string& serverAddress, int port);
        void disconnect();
        bool send_message(const std::string& message);

};