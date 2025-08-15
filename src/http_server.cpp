#include "http_server.h"
#include "http_request.h"
#include "http_response.h"
#include "route_handler.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

HTTPServer::HTTPServer(int port, int max_connections)
    : port_(port), max_connections_(max_connections), server_socket_(-1), running_(false) {
    route_handler_ = std::make_unique<RouteHandler>();
}

HTTPServer::~HTTPServer() {
    stop();
}

bool HTTPServer::start() {
    // Create socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
        return false;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_socket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }
    
    // Bind socket
    memset(&server_addr_, 0, sizeof(server_addr_));
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = INADDR_ANY;
    server_addr_.sin_port = htons(port_);
    
    if (bind(server_socket_, (struct sockaddr*)&server_addr_, sizeof(server_addr_)) < 0) {
        std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }
    
    // Listen for connections
    if (listen(server_socket_, max_connections_) < 0) {
        std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
        close(server_socket_);
        return false;
    }
    
    // Set socket to non-blocking mode
    int flags = fcntl(server_socket_, F_GETFL, 0);
    fcntl(server_socket_, F_SETFL, flags | O_NONBLOCK);
    
    running_ = true;
    std::cout << "HTTP Server started on port " << port_ << std::endl;
    
    // Start worker threads
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4;
    
    for (int i = 0; i < num_threads; ++i) {
        worker_threads_.emplace_back(&HTTPServer::worker_thread, this);
    }
    
    // Start accepting connections in main thread
    accept_connections();
    
    return true;
}

void HTTPServer::stop() {
    if (!running_) return;
    
    running_ = false;
    
    // Close server socket
    if (server_socket_ >= 0) {
        close(server_socket_);
        server_socket_ = -1;
    }
    
    // Wait for worker threads to finish
    for (auto& thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
    
    std::cout << "HTTP Server stopped" << std::endl;
}

void HTTPServer::accept_connections() {
    while (running_) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_socket < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No pending connections, sleep briefly
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            std::cerr << "Error accepting connection: " << strerror(errno) << std::endl;
            continue;
        }
        
        // Handle client in a new thread
        std::thread client_thread(&HTTPServer::handle_client, this, client_socket);
        client_thread.detach();
    }
}

void HTTPServer::handle_client(int client_socket) {
    char buffer[4096];
    std::string request_data;
    
    // Set client socket timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    
    // Read request data
    ssize_t bytes_read;
    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        request_data += buffer;
        
        // Check if we have received the complete request
        if (request_data.find("\r\n\r\n") != std::string::npos) {
            break;
        }
    }
    
    if (bytes_read < 0) {
        std::cerr << "Error reading from client: " << strerror(errno) << std::endl;
        close(client_socket);
        return;
    }
    
    // Parse and handle request
    HTTPRequest request;
    HTTPResponse response;
    
    if (request.parse(request_data)) {
        response = route_handler_->handle_request(request);
    } else {
        response = HTTPResponse::bad_request("Invalid HTTP request");
    }
    
    // Send response
    std::string response_str = response.to_string();
    send(client_socket, response_str.c_str(), response_str.length(), 0);
    
    close(client_socket);
}

void HTTPServer::worker_thread() {
    while (running_) {
        // Worker threads can handle additional tasks if needed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
} 