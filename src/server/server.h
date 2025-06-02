// Structure for the server side of the chat application
// This server handles multiple client connections concurrently using threads

#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Server class that manages multiple client connections for a chat application
 * Uses TCP sockets for reliable communication and multithreading for concurrent client handling
 */
class Server {
    private:
        // Socket file descriptor for the server to listen for incoming connections
        int server_socket;
        
        // Thread-safe boolean flag to control server running state
        // Atomic ensures thread-safe access without explicit locking
        std::atomic<bool> running;
        
        // Vector storing file descriptors of all connected client sockets
        // Each element represents an active client connection
        std::vector<int> client_sockets;
        
        // Mutex to protect concurrent access to the client_sockets vector
        // Prevents race conditions when multiple threads modify the client list
        std::mutex clients_mutex;

        /**
         * Handles communication with a single client in a dedicated thread
         * Continuously listens for messages from the client and broadcasts them
         * @param client_socket File descriptor of the client socket to handle
         */
        void handle_client(int client_socket);
        
        /**
         * Broadcasts a message to all connected clients except the sender
         * Thread-safe function that sends the same message to multiple clients
         * @param message The text message to broadcast
         * @param sender_socket Socket of the client who sent the message (excluded from broadcast)
         */
        void broadcast_message(const std::string& message, int sender_socket);

    public:
        /**
         * Constructor that initializes the server socket and binds it to a port
         * Sets up the server socket for listening but doesn't start accepting connections yet
         * @param port Port number to bind the server socket (default: 8080)
         */
        Server(int port = 8080);
        
        /**
         * Destructor that ensures proper cleanup of server resources
         * Stops the server and closes the server socket
         */
        ~Server();

        /**
         * Starts the server and begins accepting client connections
         * Runs the main server loop that accepts new clients and creates threads for each
         * This function blocks until stop() is called
         */
        void start();
        
        /**
         * Stops the server and closes all client connections
         * Sets running flag to false and cleans up all active client sockets
         */
        void stop();
};