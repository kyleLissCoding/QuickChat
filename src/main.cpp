// Main execution file for the chat application
// This program can run in two modes: as a server (to host chat rooms) or as a client (to join chat rooms)
// Usage: ./program [server|client]

#include <iostream>
#include "client/Client.h"
#include "server/Server.h"

/**
 * Main entry point for the chat application
 * Determines whether to run as a server or client based on command-line arguments
 * 
 * @param argc Number of command-line arguments (should be at least 2)
 * @param argv Array of command-line argument strings
 *             argv[0] = program name
 *             argv[1] = mode ("server" or "client")
 * @return 0 on success, 1 on error
 */
int main(int argc, char* argv[]) {
    // Check if the user provided the required command-line argument
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [server|client]" << std::endl;
        return 1;
    }

    // Extract the mode from command-line arguments
    std::string mode = argv[1];

    // SERVER MODE: Run as a chat server that accepts multiple client connections
    if (mode == "server") {
        try {
            // Create a server instance that listens on port 8080
            // This server will handle multiple concurrent client connections
            Server server(8080);
            
            // Start the server - this will block and run indefinitely
            // The server will accept client connections and facilitate chat between them
            server.start();
        } catch (const std::exception& e) {
            // Handle any errors that occur during server creation or operation
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } 
    // CLIENT MODE: Run as a chat client that connects to an existing server
    else if (mode == "client") {
        try {
            // Create a client instance for connecting to the chat server
            Client client;
            
            // Attempt to connect to the server running on localhost (127.0.0.1) port 8080
            // This establishes the TCP connection and starts the message receiving thread
            if(!client.connect("127.0.0.1", 8080)) {
                std::cerr << "Failed to connect to server" << std::endl;
                return 1;
            }

            // Main client loop: continuously read user input and send messages
            std::string message;
            while (true) {
                // Read a full line of input from the user (including spaces)
                // This blocks until the user presses Enter
                std::getline(std::cin, message);
                
                // Check if user wants to exit the chat
                if (message == "exit") {
                    break; // Exit the input loop
                }
                
                // Send the user's message to the server
                // The server will then broadcast it to all other connected clients
                client.send_message(message);
            }

            // User chose to exit - gracefully disconnect from the server
            // This stops the receiving thread and closes the connection
            client.disconnect();
        } catch (const std::exception& err) {
            // Handle any errors that occur during client operation
            std::cerr << "Error: " << err.what() << std::endl;
            return 1;
        }
    }
    // If neither "server" nor "client" was specified, the usage was shown above
    // and the program will exit with return code 0

    return 0; // Successful program termination
}