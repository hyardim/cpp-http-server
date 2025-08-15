#pragma once

#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <memory>
#include <functional>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class HTTPRequest;
class HTTPResponse;
class RouteHandler;

class HTTPServer {
public:
    HTTPServer(int port = 8080, int max_connections = 100);
    ~HTTPServer();

    // Start the server
    bool start();
    
    // Stop the server
    void stop();
    
    // Check if server is running
    bool is_running() const { return running_; }
    
    // Get server port
    int get_port() const { return port_; }

private:
    void accept_connections();
    void handle_client(int client_socket);
    void worker_thread();
    
    int port_;
    int max_connections_;
    int server_socket_;
    std::atomic<bool> running_;
    std::vector<std::thread> worker_threads_;
    std::unique_ptr<RouteHandler> route_handler_;
    
    // Socket address structures
    struct sockaddr_in server_addr_;
}; 