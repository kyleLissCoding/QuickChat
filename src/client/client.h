// Client for individual users to connect to the chat server
// This client handles both sending messages to the server and receiving messages from other users

#pragma once
#include <string>
#include <thread>
#include <atomic>       // Provides atomic data types and operations for thread-safe concurrent programming
#include <sys/socket.h> // Provides socket API functions for network communication (socket(), bind(), listen(), etc.)
#include <netinet/in.h> // Defines Internet protocol/address structures like sockaddr_in for IPv4 networking

/**
 * Client class that connects to a chat server and handles two-way communication
 * Uses a separate thread for receiving messages to allow simultaneous send/receive operations
 */
class Client {
    private:
        // Socket file descriptor for connection to the server
        // This is the communication channel between client and server
        int client_socket;
        
        // Thread-safe boolean flag to control the client's running state
        // Atomic ensures safe access from multiple threads without explicit locking
        std::atomic<bool> running;
        
        // Dedicated thread for receiving messages from the server
        // This allows the client to receive messages while the main thread handles user input
        std::thread receive_thread;

        /**
         * Continuously receives messages from the server in a separate thread
         * This function runs in a loop, listening for incoming messages from the server
         * and displaying them to the user's console
         */
        void receive_messages();

    public:
        /**
         * Constructor that creates a TCP socket for server communication
         * Initializes the client socket but doesn't connect to any server yet
         */
        Client();
        
        /**
         * Destructor that ensures proper cleanup of client resources
         * Disconnects from server and closes the socket
         */
        ~Client();

        /**
         * Establishes connection to a chat server
         * Creates a TCP connection to the specified server and starts the message receiving thread
         * @param serverAddress IP address of the server (e.g., "127.0.0.1" for localhost)
         * @param port Port number the server is listening on (e.g., 8080)
         * @return true if connection successful, false otherwise
         */
        bool connect(const std::string& serverAddress, int port);
        
        /**
         * Disconnects from the server and stops the receiving thread
         * Gracefully closes the connection and cleans up resources
         */
        void disconnect();
        
        /**
         * Sends a text message to the server
         * The server will then broadcast this message to all other connected clients
         * @param message The text message to send to the server
         * @return true if message was sent successfully, false otherwise
         */
        bool send_message(const std::string& message);
};