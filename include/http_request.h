#pragma once

#include <string>
#include <map>
#include <vector>

class HTTPRequest {
public:
    enum class Method {
        GET,
        POST,
        PUT,
        DELETE,
        HEAD,
        OPTIONS,
        UNKNOWN
    };

    HTTPRequest();
    
    // Parse raw HTTP request data
    bool parse(const std::string& raw_request);
    
    // Getters
    Method get_method() const { return method_; }
    const std::string& get_path() const { return path_; }
    const std::string& get_version() const { return version_; }
    const std::map<std::string, std::string>& get_headers() const { return headers_; }
    const std::string& get_body() const { return body_; }
    const std::map<std::string, std::string>& get_query_params() const { return query_params_; }
    
    // Utility methods
    std::string get_header(const std::string& name) const;
    bool has_header(const std::string& name) const;
    std::string get_query_param(const std::string& name) const;

private:
    void parse_request_line(const std::string& line);
    void parse_headers(const std::vector<std::string>& header_lines);
    void parse_query_string(const std::string& query_string);
    Method parse_method(const std::string& method_str);
    
    Method method_;
    std::string path_;
    std::string version_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::map<std::string, std::string> query_params_;
}; 