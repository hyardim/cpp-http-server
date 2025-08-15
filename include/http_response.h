#pragma once

#include <string>
#include <map>

class HTTPResponse {
public:
    enum class StatusCode {
        OK = 200,
        CREATED = 201,
        NO_CONTENT = 204,
        BAD_REQUEST = 400,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501
    };

    HTTPResponse();
    
    // Setters
    void set_status_code(StatusCode code) { status_code_ = code; }
    void set_body(const std::string& body) { body_ = body; }
    void set_content_type(const std::string& content_type);
    void add_header(const std::string& name, const std::string& value);
    
    // Generate HTTP response string
    std::string to_string() const;
    
    // Utility methods
    void set_json_response(const std::string& json_data);
    void set_html_response(const std::string& html_data);
    void set_text_response(const std::string& text_data);
    
    // Static factory methods for common responses
    static HTTPResponse ok(const std::string& body = "");
    static HTTPResponse not_found(const std::string& message = "Not Found");
    static HTTPResponse bad_request(const std::string& message = "Bad Request");
    static HTTPResponse internal_error(const std::string& message = "Internal Server Error");

private:
    std::string status_code_to_string(StatusCode code) const;
    std::string get_status_message(StatusCode code) const;
    
    StatusCode status_code_;
    std::string body_;
    std::map<std::string, std::string> headers_;
}; 