#include "route_handler.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <filesystem>

RouteHandler::RouteHandler() {
    register_default_routes();
}

void RouteHandler::register_route(const std::string& method, const std::string& path, RouteCallback callback) {
    routes_.push_back({method, path, callback});
}

HTTPResponse RouteHandler::handle_request(const HTTPRequest& request) {
    std::string method_str;
    switch (request.get_method()) {
        case HTTPRequest::Method::GET: method_str = "GET"; break;
        case HTTPRequest::Method::POST: method_str = "POST"; break;
        case HTTPRequest::Method::PUT: method_str = "PUT"; break;
        case HTTPRequest::Method::DELETE: method_str = "DELETE"; break;
        case HTTPRequest::Method::HEAD: method_str = "HEAD"; break;
        case HTTPRequest::Method::OPTIONS: method_str = "OPTIONS"; break;
        default: method_str = "UNKNOWN"; break;
    }
    
    // Find matching route
    for (const auto& route : routes_) {
        if (route.method == method_str && path_matches(route.path, request.get_path())) {
            return route.callback(request);
        }
    }
    
    // No matching route found
    return HTTPResponse::not_found("Route not found: " + request.get_path());
}

void RouteHandler::register_default_routes() {
    // Root route
    register_route("GET", "/", [this](const HTTPRequest& req) { return handle_root(req); });
    
    // Health check route
    register_route("GET", "/health", [this](const HTTPRequest& req) { return handle_health(req); });
    
    // Echo route for testing
    register_route("GET", "/echo", [this](const HTTPRequest& req) { return handle_echo(req); });
    register_route("POST", "/echo", [this](const HTTPRequest& req) { return handle_echo(req); });
    
    // Static file serving
    register_route("GET", "/static", [this](const HTTPRequest& req) { return handle_static_file(req); });
}

bool RouteHandler::path_matches(const std::string& route_path, const std::string& request_path) const {
    if (route_path == request_path) {
        return true;
    }
    
    // Handle wildcard routes (simple implementation)
    if (route_path.back() == '*' && 
        request_path.substr(0, route_path.length() - 1) == route_path.substr(0, route_path.length() - 1)) {
        return true;
    }
    
    return false;
}

std::vector<std::string> RouteHandler::split_path(const std::string& path) const {
    std::vector<std::string> parts;
    std::istringstream stream(path);
    std::string part;
    
    while (std::getline(stream, part, '/')) {
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    return parts;
}

HTTPResponse RouteHandler::handle_root(const HTTPRequest& request) {
    std::string html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>C++ HTTP Server</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; }
        .endpoint { background: #f8f9fa; padding: 15px; margin: 10px 0; border-radius: 5px; border-left: 4px solid #007bff; }
        .method { font-weight: bold; color: #007bff; }
        .path { font-family: monospace; color: #28a745; }
        .description { color: #666; margin-top: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 C++ HTTP Server</h1>
        <p>Welcome to your custom HTTP server built with C++!</p>
        
        <h2>Available Endpoints:</h2>
        
        <div class="endpoint">
            <div><span class="method">GET</span> <span class="path">/</span></div>
            <div class="description">This page - server information and available endpoints</div>
        </div>
        
        <div class="endpoint">
            <div><span class="method">GET</span> <span class="path">/health</span></div>
            <div class="description">Health check endpoint</div>
        </div>
        
        <div class="endpoint">
            <div><span class="method">GET/POST</span> <span class="path">/echo</span></div>
            <div class="description">Echo endpoint - returns request data</div>
        </div>
        
        <div class="endpoint">
            <div><span class="method">GET</span> <span class="path">/static</span></div>
            <div class="description">Static file serving (if files exist)</div>
        </div>
        
        <h2>Features:</h2>
        <ul>
            <li>Multi-threaded request handling</li>
            <li>HTTP/1.1 compliant</li>
            <li>Support for GET, POST, PUT, DELETE methods</li>
            <li>Query parameter parsing</li>
            <li>Header parsing and response generation</li>
            <li>Clean, modular architecture</li>
        </ul>
        
        <p><em>Server is running on port 8080</em></p>
    </div>
</body>
</html>
    )";
    
    HTTPResponse response;
    response.set_html_response(html);
    return response;
}

HTTPResponse RouteHandler::handle_health(const HTTPRequest& request) {
    std::string json = R"({
    "status": "healthy",
    "server": "C++ HTTP Server",
    "timestamp": ")" + std::to_string(time(nullptr)) + R"(",
    "uptime": "running"
})";
    
    HTTPResponse response;
    response.set_json_response(json);
    return response;
}

HTTPResponse RouteHandler::handle_echo(const HTTPRequest& request) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"method\": \"";
    
    switch (request.get_method()) {
        case HTTPRequest::Method::GET: json << "GET"; break;
        case HTTPRequest::Method::POST: json << "POST"; break;
        case HTTPRequest::Method::PUT: json << "PUT"; break;
        case HTTPRequest::Method::DELETE: json << "DELETE"; break;
        case HTTPRequest::Method::HEAD: json << "HEAD"; break;
        case HTTPRequest::Method::OPTIONS: json << "OPTIONS"; break;
        default: json << "UNKNOWN"; break;
    }
    
    json << "\",\n";
    json << "  \"path\": \"" << request.get_path() << "\",\n";
    json << "  \"version\": \"" << request.get_version() << "\",\n";
    json << "  \"headers\": {\n";
    
    bool first_header = true;
    for (const auto& header : request.get_headers()) {
        if (!first_header) json << ",\n";
        json << "    \"" << header.first << "\": \"" << header.second << "\"";
        first_header = false;
    }
    json << "\n  },\n";
    
    json << "  \"query_params\": {\n";
    bool first_param = true;
    for (const auto& param : request.get_query_params()) {
        if (!first_param) json << ",\n";
        json << "    \"" << param.first << "\": \"" << param.second << "\"";
        first_param = false;
    }
    json << "\n  },\n";
    
    json << "  \"body\": \"" << request.get_body() << "\"\n";
    json << "}";
    
    HTTPResponse response;
    response.set_json_response(json.str());
    return response;
}

HTTPResponse RouteHandler::handle_static_file(const HTTPRequest& request) {
    // This is a simple implementation - in production you'd want more security
    std::string path = request.get_path();
    if (path == "/static") {
        path = "/static/index.html";
    }
    
    // Remove /static prefix
    if (path.substr(0, 7) == "/static") {
        path = path.substr(7);
    }
    
    // Security: prevent directory traversal
    if (path.find("..") != std::string::npos) {
        return HTTPResponse::bad_request("Invalid path");
    }
    
    // Try to serve from current directory
    std::string file_path = "." + path;
    
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        return HTTPResponse::not_found("File not found: " + path);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    HTTPResponse response;
    response.set_status_code(HTTPResponse::StatusCode::OK);
    response.set_body(content);
    
    // Set appropriate content type based on file extension
    if (path.length() >= 5 && path.substr(path.length() - 5) == ".html") {
        response.set_content_type("text/html; charset=utf-8");
    } else if (path.length() >= 4 && path.substr(path.length() - 4) == ".css") {
        response.set_content_type("text/css");
    } else if (path.length() >= 3 && path.substr(path.length() - 3) == ".js") {
        response.set_content_type("application/javascript");
    } else if (path.length() >= 5 && path.substr(path.length() - 5) == ".json") {
        response.set_content_type("application/json");
    } else if (path.length() >= 4 && path.substr(path.length() - 4) == ".png") {
        response.set_content_type("image/png");
    } else if ((path.length() >= 4 && path.substr(path.length() - 4) == ".jpg") || 
               (path.length() >= 5 && path.substr(path.length() - 5) == ".jpeg")) {
        response.set_content_type("image/jpeg");
    } else {
        response.set_content_type("application/octet-stream");
    }
    
    return response;
} 