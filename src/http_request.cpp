#include "http_request.h"
#include <sstream>
#include <algorithm>
#include <cctype>

HTTPRequest::HTTPRequest()
    : method_(Method::UNKNOWN), path_(""), version_(""), body_("") {
}

bool HTTPRequest::parse(const std::string& raw_request) {
    if (raw_request.empty()) {
        return false;
    }
    
    // Split request into lines
    std::istringstream stream(raw_request);
    std::string line;
    
    // Parse request line
    if (!std::getline(stream, line)) {
        return false;
    }
    parse_request_line(line);
    
    // Parse headers
    std::vector<std::string> header_lines;
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        if (line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            header_lines.push_back(line);
        }
    }
    parse_headers(header_lines);
    
    // Parse body (if any)
    std::string body_part;
    while (std::getline(stream, line)) {
        body_part += line + "\n";
    }
    if (!body_part.empty()) {
        body_ = body_part.substr(0, body_part.length() - 1); // Remove trailing newline
    }
    
    return true;
}

void HTTPRequest::parse_request_line(const std::string& line) {
    std::istringstream stream(line);
    std::string method_str, path_str, version_str;
    
    if (stream >> method_str >> path_str >> version_str) {
        method_ = parse_method(method_str);
        path_ = path_str;
        version_ = version_str;
        
        // Parse query string if present
        size_t query_pos = path_.find('?');
        if (query_pos != std::string::npos) {
            std::string query_string = path_.substr(query_pos + 1);
            path_ = path_.substr(0, query_pos);
            parse_query_string(query_string);
        }
    }
}

void HTTPRequest::parse_headers(const std::vector<std::string>& header_lines) {
    for (const auto& line : header_lines) {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string name = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            
            // Trim whitespace
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            headers_[name] = value;
        }
    }
}

void HTTPRequest::parse_query_string(const std::string& query_string) {
    std::istringstream stream(query_string);
    std::string param;
    
    while (std::getline(stream, param, '&')) {
        size_t equal_pos = param.find('=');
        if (equal_pos != std::string::npos) {
            std::string name = param.substr(0, equal_pos);
            std::string value = param.substr(equal_pos + 1);
            query_params_[name] = value;
        }
    }
}

HTTPRequest::Method HTTPRequest::parse_method(const std::string& method_str) {
    std::string upper_method = method_str;
    std::transform(upper_method.begin(), upper_method.end(), upper_method.begin(), ::toupper);
    
    if (upper_method == "GET") return Method::GET;
    if (upper_method == "POST") return Method::POST;
    if (upper_method == "PUT") return Method::PUT;
    if (upper_method == "DELETE") return Method::DELETE;
    if (upper_method == "HEAD") return Method::HEAD;
    if (upper_method == "OPTIONS") return Method::OPTIONS;
    
    return Method::UNKNOWN;
}

std::string HTTPRequest::get_header(const std::string& name) const {
    auto it = headers_.find(name);
    return (it != headers_.end()) ? it->second : "";
}

bool HTTPRequest::has_header(const std::string& name) const {
    return headers_.find(name) != headers_.end();
}

std::string HTTPRequest::get_query_param(const std::string& name) const {
    auto it = query_params_.find(name);
    return (it != query_params_.end()) ? it->second : "";
} 