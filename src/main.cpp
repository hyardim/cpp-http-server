#include "http_server.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

std::atomic<bool> running(true);

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully..." << std::endl;
    running = false;
}

int main(int argc, char* argv[]) {
    int port = 8080;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--port" || arg == "-p") {
            if (i + 1 < argc) {
                port = std::atoi(argv[++i]);
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -p, --port PORT    Port to listen on (default: 8080)\n"
                      << "  -h, --help         Show this help message\n"
                      << std::endl;
            return 0;
        }
    }
    
    // Set up signal handling
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    std::cout << "🚀 Starting C++ HTTP Server on port " << port << std::endl;
    std::cout << "Press Ctrl+C to stop the server\n" << std::endl;
    
    // Create and start server
    HTTPServer server(port);
    
    if (!server.start()) {
        std::cerr << "Failed to start server!" << std::endl;
        return 1;
    }
    
    // Wait for shutdown signal
    while (running && server.is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Stop server
    server.stop();
    
    std::cout << "Server stopped successfully." << std::endl;
    return 0;
} 