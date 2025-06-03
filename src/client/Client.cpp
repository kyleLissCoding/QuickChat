// Chat Client implementation

#include "Client.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

/**
 * Constructor: Creates a TCP socket for communication with the server
 * Initializes the client in a non-running state
 */
Client::Client() : running(false) {
    // Create a TCP socket using IPv4 (AF_INET) and stream protocol (SOCK_STREAM)
    // This socket will be used to establish connection with the server
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        throw std::runtime_error("Failed to create socket");
    }
}

/**
 * Destructor: Ensures proper cleanup when client object is destroyed
 */
Client::~Client() {
    disconnect(); // Gracefully disconnect from server and stop threads
    close(client_socket); // Close the socket file descriptor to free system resources
}

/**
 * Establishes connection to the chat server and starts message receiving
 * This function performs the TCP handshake and creates a background thread for incoming messages
 */
bool Client::connect(const std::string& server_address, int port) {
    // Configure the server address structure for connection
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;        // Use IPv4 protocol
    server_addr.sin_port = htons(port);      // Convert port to network byte order (big-endian)
    
    // Convert string IP address to binary format suitable for networking
    // inet_pton() converts human-readable IP (like "127.0.0.1") to binary format
    if (inet_pton(AF_INET, server_address.c_str(), &server_addr.sin_addr) <= 0) {
        return false; // Invalid IP address format
    }

    // Attempt to establish TCP connection with the server
    // ::connect() performs the three-way handshake (SYN, SYN-ACK, ACK)
    if (::connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return false; // Connection failed (server might be down or unreachable)
    }

    // Connection successful - start the client's operations
    running = true;
    
    // Create a separate thread to handle incoming messages from the server
    // This allows simultaneous sending (main thread) and receiving (background thread)
    receive_thread = std::thread(&Client::receive_messages, this);
    return true;
}

/**
 * Gracefully disconnects from the server and stops all client operations
 * Ensures proper cleanup of threads and resources
 */
void Client::disconnect() {
    running = false; // Signal the receiving thread to stop
    
    // Wait for the receiving thread to finish its current operation and terminate
    // joinable() checks if the thread is still active and can be joined
    if (receive_thread.joinable()) {
        receive_thread.join(); // Block until the thread completes
    }
}

/**
 * Sends a text message to the server
 * The server will broadcast this message to all other connected clients
 */
bool Client::send_message(const std::string& message) {
    // Send the message data over the TCP connection
    // send() transmits the message bytes to the server
    // Returns number of bytes sent, or -1 on error
    return send(client_socket, message.c_str(), message.length(), 0) > 0;
}

/**
 * Background thread function that continuously receives messages from the server
 * This runs in a separate thread to allow non-blocking message reception
 * while the main thread handles user input
 */
void Client::receive_messages() {
    char buffer[1024]; // Buffer to store incoming message data (up to 1024 bytes per message)
    
    // Keep receiving messages while the client is connected and running
    while (running) {
        // Clear the buffer to ensure no leftover data from previous messages
        memset(buffer, 0, sizeof(buffer));
        
        // Receive data from the server socket
        // recv() blocks until data arrives or connection is closed
        int bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);

        // Check if connection was closed or an error occurred
        if (bytes_read <= 0) {
            break; // Exit the receiving loop
        }

        // Display the received message to the user's console
        // Convert buffer to string using exact number of bytes received
        std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;
    }
}