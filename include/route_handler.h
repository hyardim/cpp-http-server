#pragma once

#include "http_request.h"
#include "http_response.h"
#include <functional>
#include <map>
#include <string>
#include <vector>

class RouteHandler {
public:
    using RouteCallback = std::function<HTTPResponse(const HTTPRequest&)>;
    
    RouteHandler();
    
    // Register routes
    void register_route(const std::string& method, const std::string& path, RouteCallback callback);
    
    // Handle incoming request
    HTTPResponse handle_request(const HTTPRequest& request);
    
    // Register default routes
    void register_default_routes();

private:
    struct Route {
        std::string method;
        std::string path;
        RouteCallback callback;
    };
    
    std::vector<Route> routes_;
    
    // Helper methods
    bool path_matches(const std::string& route_path, const std::string& request_path) const;
    std::vector<std::string> split_path(const std::string& path) const;
    
    // Default route handlers
    HTTPResponse handle_root(const HTTPRequest& request);
    HTTPResponse handle_health(const HTTPRequest& request);
    HTTPResponse handle_echo(const HTTPRequest& request);
    HTTPResponse handle_static_file(const HTTPRequest& request);
}; 