#include "http_response.h"
#include <sstream>

HTTPResponse::HTTPResponse()
    : status_code_(StatusCode::OK) {
    // Set default headers
    headers_["Server"] = "C++ HTTP Server";
    headers_["Connection"] = "close";
}

void HTTPResponse::set_content_type(const std::string& content_type) {
    headers_["Content-Type"] = content_type;
}

void HTTPResponse::add_header(const std::string& name, const std::string& value) {
    headers_[name] = value;
}

std::string HTTPResponse::to_string() const {
    std::ostringstream response;
    
    // Status line
    response << "HTTP/1.1 " << status_code_to_string(status_code_) << " "
             << get_status_message(status_code_) << "\r\n";
    
    // Headers
    for (const auto& header : headers_) {
        response << header.first << ": " << header.second << "\r\n";
    }
    
    // Empty line separating headers from body
    response << "\r\n";
    
    // Body
    if (!body_.empty()) {
        response << body_;
    }
    
    return response.str();
}

void HTTPResponse::set_json_response(const std::string& json_data) {
    body_ = json_data;
    set_content_type("application/json");
}

void HTTPResponse::set_html_response(const std::string& html_data) {
    body_ = html_data;
    set_content_type("text/html; charset=utf-8");
}

void HTTPResponse::set_text_response(const std::string& text_data) {
    body_ = text_data;
    set_content_type("text/plain; charset=utf-8");
}

HTTPResponse HTTPResponse::ok(const std::string& body) {
    HTTPResponse response;
    response.set_status_code(StatusCode::OK);
    if (!body.empty()) {
        response.set_body(body);
    }
    return response;
}

HTTPResponse HTTPResponse::not_found(const std::string& message) {
    HTTPResponse response;
    response.set_status_code(StatusCode::NOT_FOUND);
    response.set_text_response(message);
    return response;
}

HTTPResponse HTTPResponse::bad_request(const std::string& message) {
    HTTPResponse response;
    response.set_status_code(StatusCode::BAD_REQUEST);
    response.set_text_response(message);
    return response;
}

HTTPResponse HTTPResponse::internal_error(const std::string& message) {
    HTTPResponse response;
    response.set_status_code(StatusCode::INTERNAL_SERVER_ERROR);
    response.set_text_response(message);
    return response;
}

std::string HTTPResponse::status_code_to_string(StatusCode code) const {
    return std::to_string(static_cast<int>(code));
}

std::string HTTPResponse::get_status_message(StatusCode code) const {
    switch (code) {
        case StatusCode::OK: return "OK";
        case StatusCode::CREATED: return "Created";
        case StatusCode::NO_CONTENT: return "No Content";
        case StatusCode::BAD_REQUEST: return "Bad Request";
        case StatusCode::NOT_FOUND: return "Not Found";
        case StatusCode::METHOD_NOT_ALLOWED: return "Method Not Allowed";
        case StatusCode::INTERNAL_SERVER_ERROR: return "Internal Server Error";
        case StatusCode::NOT_IMPLEMENTED: return "Not Implemented";
        default: return "Unknown";
    }
} 