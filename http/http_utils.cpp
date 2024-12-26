#include "http_utils.hpp"

#include <iostream>
#include <istream>
#include <string>
#include <sstream>

namespace http {

const std::unordered_map<std::string, HttpMethod> method_map = {
    {"GET", HttpMethod::GET},
    {"POST", HttpMethod::POST},
    {"PUT", HttpMethod::PUT},
    {"DELETE", HttpMethod::DELETE},
    {"HEAD", HttpMethod::HEAD},
    {"OPTIONS", HttpMethod::OPTIONS},
    {"CONNECT", HttpMethod::CONNECT},
    {"TRACE", HttpMethod::TRACE},
    {"PATCH", HttpMethod::PATCH},
};

std::string create_request(const HttpRequest& request) {
    std::string req;
    req += request.url + " " + request.version + "\r\n";
    for (const auto& [key, value] : request.headers) {
        req += key + ": " + value + "\r\n";
    }
    req += "\r\n";
    return req;
}

bool parse_request_line(std::istream& stream, HttpRequest& request) {
    std::string line;
    if (stream.eof()) {
        return false;
    }
    std::string method, url, version;
    stream >> method >> url >> version;
    if (method_map.find(method) == method_map.end()) {
        return false;
    }
    request.method = method_map.at(method);
    request.url = url;
    request.version = version;
    return true;
}

bool parse_request_header(std::istream& stream, HttpRequest& request) {
    std::string line;
    // if the stream is empty, return false
    if (stream.eof()) {
        return false;
    }
    // read the first line
    while (std::getline(stream, line)) {
        // if the line is empty, return true
        if (line.empty()) {
            return true;
        }
        // find the position of the colon
        std::size_t pos = line.find(":");
        if (pos == std::string::npos) {
            return false;
        }
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        request.headers[key] = value;
    }
    return true;
}

HttpRequest parse_request(const std::string& request) {
    HttpRequest req;
    std::istringstream request_stream(request);
    if (!parse_request_line(request_stream, req)) {
        throw std::runtime_error("Failed to parse request line");
    }
    if (!parse_request_header(request_stream, req)) {
        throw std::runtime_error("Failed to parse request header");
    }
    return req;
}

std::string create_response(const HttpResponse& response) {
    std::string res;
    res += response.version + " " + std::to_string(static_cast<int>(response.code)) + "\r\n";
    for (const auto& [key, value] : response.headers) {
        res += key + ": " + value + "\r\n";
    }
    res += "\r\n";
    res += response.body;
    return res;
}

HttpResponse parse_response(const std::string& response) {
    
}

} // namespace http