# 🚀 C++ HTTP Server

A modern, multi-threaded HTTP server built with C++17, featuring clean architecture, comprehensive HTTP/1.1 support, and extensible routing.

## ✨ Features

- **Multi-threaded**: Handles multiple concurrent connections efficiently
- **HTTP/1.1 Compliant**: Full support for HTTP/1.1 protocol
- **Extensible Routing**: Easy to add new endpoints and handlers
- **Query Parameter Parsing**: Automatic parsing of URL query parameters
- **Header Management**: Comprehensive HTTP header handling
- **Static File Serving**: Built-in support for serving static files
- **Signal Handling**: Graceful shutdown with Ctrl+C
- **Cross-platform**: Works on Linux, macOS, and Windows (with appropriate modifications)

## 🏗️ Architecture

The server is built with a modular, object-oriented design:

- **HTTPServer**: Main server class managing connections and threading
- **HTTPRequest**: Parses and represents incoming HTTP requests
- **HTTPResponse**: Generates and formats HTTP responses
- **RouteHandler**: Manages routing and request handling

## 📋 Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.16 or higher
- POSIX-compliant system (Linux, macOS)

## 🔨 Building

### Prerequisites

Make sure you have the following installed:
- C++ compiler (GCC, Clang, or MSVC)
- CMake
- Make (or Ninja)

### Build Steps

1. **Clone or navigate to the project directory:**
   ```bash
   cd "HTTP Server"
   ```

2. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the project:**
   ```bash
   make
   ```
   
   Or with Ninja:
   ```bash
   ninja
   ```

5. **Install (optional):**
   ```bash
   sudo make install
   ```

## 🚀 Usage

### Basic Usage

Start the server on the default port (8080):
```bash
./http_server
```

### Custom Port

Start the server on a specific port:
```bash
./http_server --port 3000
```

### Help

Show available options:
```bash
./http_server --help
```

## 🌐 Available Endpoints

### GET /
- **Description**: Server information and available endpoints
- **Response**: HTML page with server details

### GET /health
- **Description**: Health check endpoint
- **Response**: JSON with server status

### GET/POST /echo
- **Description**: Echo endpoint for testing
- **Response**: JSON with request details (method, headers, body, etc.)

### GET /static
- **Description**: Static file serving
- **Response**: Serves files from the current directory

## 🔧 Customization

### Adding New Routes

To add custom endpoints, modify the `RouteHandler` class:

```cpp
// In route_handler.cpp
void RouteHandler::register_default_routes() {
    // ... existing routes ...
    
    // Add your custom route
    register_route("GET", "/api/users", [this](const HTTPRequest& req) {
        return handle_users(req);
    });
}

HTTPResponse RouteHandler::handle_users(const HTTPRequest& request) {
    // Your custom logic here
    std::string json = R"({"users": ["user1", "user2"]})";
    
    HTTPResponse response;
    response.set_json_response(json);
    return response;
}
```

### Custom Response Types

The `HTTPResponse` class supports various content types:

```cpp
HTTPResponse response;

// JSON response
response.set_json_response(R"({"key": "value"})");

// HTML response
response.set_html_response("<h1>Hello World</h1>");

// Text response
response.set_text_response("Plain text content");

// Custom headers
response.add_header("X-Custom-Header", "custom-value");
```

## 🧪 Testing

### Using curl

Test the server endpoints:

```bash
# Test root endpoint
curl http://localhost:8080/

# Test health endpoint
curl http://localhost:8080/health

# Test echo endpoint
curl -X POST -d "Hello Server" http://localhost:8080/echo

# Test with query parameters
curl "http://localhost:8080/echo?name=test&value=123"
```

### Using a web browser

Open your browser and navigate to:
- `http://localhost:8080/` - Main page
- `http://localhost:8080/health` - Health check
- `http://localhost:8080/echo` - Echo endpoint

## 🔒 Security Considerations

This is a development server with basic security features:

- **Directory Traversal Protection**: Basic protection against `../` attacks
- **Request Timeout**: 5-second timeout for client connections
- **Connection Limits**: Configurable maximum connections

**Note**: For production use, implement additional security measures:
- HTTPS/TLS support
- Input validation and sanitization
- Rate limiting
- Authentication and authorization
- Logging and monitoring

## 🐛 Troubleshooting

### Common Issues

1. **Port already in use:**
   ```bash
   # Check what's using the port
   lsof -i :8080
   
   # Kill the process or use a different port
   ./http_server --port 8081
   ```

2. **Permission denied:**
   ```bash
   # Use a port above 1024 for non-root users
   ./http_server --port 8080
   ```

3. **Build errors:**
   - Ensure you have C++17 support
   - Check CMake version (3.16+)
   - Verify all dependencies are installed

### Debug Mode

To build with debug information:

```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

## 📚 API Reference

### HTTPServer Class

```cpp
class HTTPServer {
public:
    HTTPServer(int port = 8080, int max_connections = 100);
    bool start();
    void stop();
    bool is_running() const;
    int get_port() const;
};
```

### HTTPRequest Class

```cpp
class HTTPRequest {
public:
    enum class Method { GET, POST, PUT, DELETE, HEAD, OPTIONS, UNKNOWN };
    
    Method get_method() const;
    const std::string& get_path() const;
    const std::string& get_version() const;
    const std::map<std::string, std::string>& get_headers() const;
    const std::string& get_body() const;
    const std::map<std::string, std::string>& get_query_params() const;
    
    std::string get_header(const std::string& name) const;
    bool has_header(const std::string& name) const;
    std::string get_query_param(const std::string& name) const;
};
```

### HTTPResponse Class

```cpp
class HTTPResponse {
public:
    enum class StatusCode { OK, CREATED, NO_CONTENT, BAD_REQUEST, NOT_FOUND, 
                           METHOD_NOT_ALLOWED, INTERNAL_SERVER_ERROR, NOT_IMPLEMENTED };
    
    void set_status_code(StatusCode code);
    void set_body(const std::string& body);
    void set_content_type(const std::string& content_type);
    void add_header(const std::string& name, const std::string& value);
    
    std::string to_string() const;
    
    // Factory methods
    static HTTPResponse ok(const std::string& body = "");
    static HTTPResponse not_found(const std::string& message = "Not Found");
    static HTTPResponse bad_request(const std::string& message = "Bad Request");
    static HTTPResponse internal_error(const std::string& message = "Internal Server Error");
};
```

## 🤝 Contributing

Feel free to contribute to this project by:
- Reporting bugs
- Suggesting new features
- Submitting pull requests
- Improving documentation

## 📄 License

This project is open source and available under the MIT License.

## 🙏 Acknowledgments

- Built with modern C++17 features
- Inspired by Node.js Express.js routing patterns
- Uses POSIX socket APIs for cross-platform compatibility

---

**Happy coding! 🎉** 