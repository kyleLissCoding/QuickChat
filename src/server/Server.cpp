// Server implementation

#include "Server.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

/**
 * Constructor: Creates and configures the server socket
 * Sets up the socket for TCP communication and binds it to the specified port
 */
Server::Server(int port) : running(false) {
    // Create a TCP socket using IPv4 (AF_INET) and stream protocol (SOCK_STREAM)
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        throw std::runtime_error("Failed to create server socket");
    }

    // Configure the server address structure
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;        // Use IPv4
    server_address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
    server_address.sin_port = htons(port);       // Convert port to network byte order

    // Bind the socket to the specified address and port
    // This reserves the port for this server application
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        throw std::runtime_error("Failed to bind server socket");
    }
}

/**
 * Destructor: Ensures proper cleanup when server object is destroyed
 */
Server::~Server() {
    stop(); // Stop the server and close all client connections
    // Close the server socket file descriptor to free up system resources
    close(server_socket);
}

/**
 * Main server loop: Starts accepting client connections
 * Creates a new thread for each client to handle concurrent communication
 */
void Server::start() {
    running = true; // Set the server to running state
    
    // Listen for incoming connections, allowing up to 5 pending connections in the queue
    // This means up to 5 clients can be waiting to connect while the server is busy
    listen(server_socket, 5);

    std::cout << "Server started, waiting for connections..." << std::endl;

    // Main server loop: continuously accept new client connections
    while (running) {
        // Accept a new client connection (this call blocks until a client connects)
        // Returns a new socket file descriptor specifically for this client
        int client_socket = accept(server_socket, nullptr, nullptr);
        
        if (client_socket < 0) {
            // Only report error if server is still supposed to be running
            if (running) {
                std::cerr << "Failed to accept client connection" << std::endl;
            }
            continue; // Skip to next iteration and try to accept another connection
        }

        // Thread-safe addition of new client to the client list
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            client_sockets.push_back(client_socket);
        }

        // Create a new thread to handle this specific client
        // detach() allows the thread to run independently without needing to be joined
        std::thread client_thread(&Server::handle_client, this, client_socket);
        client_thread.detach();
    }
}

/**
 * Stops the server and cleans up all resources
 * Closes all client connections and resets the client list
 */
void Server::stop() {
    running = false; // Signal all threads to stop
    
    // Thread-safe cleanup of all client connections
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        // Close all client sockets to disconnect clients gracefully
        for (int socket : client_sockets) {
            close(socket);
        }
        // Clear the client list
        client_sockets.clear();
    }
}

/**
 * Handles communication with a single client in a dedicated thread
 * Continuously receives messages from the client and broadcasts them to others
 */
void Server::handle_client(int client_socket) {
    char buffer[1024]; // Buffer to store incoming messages (up to 1024 bytes)
    
    // Keep handling messages while server is running
    while (running) {
        // Clear the buffer to ensure no leftover data from previous messages
        memset(buffer, 0, sizeof(buffer));
        
        // Receive data from the client socket
        // recv() blocks until data is available or connection is closed
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        
        // Check if client disconnected or error occurred
        if (bytes_received <= 0) {
            break; // Exit the loop to clean up this client
        }
        
        // Broadcast the received message to all other connected clients
        // Convert buffer to string using exact number of bytes received
        broadcast_message(std::string(buffer, bytes_received), client_socket);
    }

    // Client disconnected or error occurred - clean up
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        // Find and remove this client's socket from the active client list
        auto it = std::find(client_sockets.begin(), client_sockets.end(), client_socket);
        if (it != client_sockets.end()) {
            client_sockets.erase(it);
        }
    }

    // Close the client socket to free up resources
    close(client_socket);
}

/**
 * Broadcasts a message to all connected clients except the sender
 * Ensures thread-safe access to the client list while sending messages
 */
void Server::broadcast_message(const std::string& message, int sender_socket) {
    // Lock the client list to prevent modification during iteration
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    // Send message to each connected client except the sender
    for (int socket : client_sockets) {
        if (socket != sender_socket) {
            // Send the message to this client
            // send() transmits the message data over the TCP connection
            send(socket, message.c_str(), message.length(), 0);
        }
    }
}