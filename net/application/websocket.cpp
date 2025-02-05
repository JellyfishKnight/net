#include "websocket.hpp"
#include "connection.hpp"
#include "enum_parser.hpp"
#include "http_client.hpp"
#include "http_parser.hpp"
#include "http_server.hpp"
#include "websocket_utils.hpp"
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace net {

WebSocketClient::WebSocketClient(std::shared_ptr<TcpClient> client): HttpClient(client) {
    m_parser = std::make_shared<WebSocketParser>();
}

std::optional<std::string> WebSocketClient::upgrade(const HttpRequest& upgrade_req) {
    auto res = HttpClient::get(upgrade_req.url(), upgrade_req.headers());
    if (res.status_code() != HttpResponseCode::SWITCHING_PROTOCOLS) {
        return "Failed to upgrade to websocket";
    } else {
        m_websocket_status = WebSocketStatus::CONNECTED;
        return std::nullopt;
    }
}

std::optional<std::string> WebSocketClient::close() {
    return HttpClient::close();
}

std::optional<std::string> WebSocketClient::read_ws(WebSocketFrame& data) {
    std::vector<uint8_t> buffer(1024);
    auto err = m_client->read(buffer);
    if (err.has_value()) {
        return err;
    }
    auto frame = m_parser->read_frame(buffer);
    if (frame.has_value()) {
        data = std::move(frame.value());
        return std::nullopt;
    } else {
        return "Failed to parse frame";
    }
}

std::optional<std::string> WebSocketClient::write_ws(const WebSocketFrame& data) {
    return m_client->write(m_parser->write_frame(data));
}

WebSocketClient::~WebSocketClient() {
    if (m_client) {
        if (m_client->status() == ConnectionStatus::CONNECTED) {
            m_client->close();
        }
        m_client.reset();
    }
}

HttpResponse WebSocketClient::get(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::get(path, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_get(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_get(path, headers, version);
}

HttpResponse WebSocketClient::post(
    const std::string& path,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::post(path, body, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_post(
    const std::string& path,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_post(path, body, headers, version);
}

HttpResponse WebSocketClient::put(
    const std::string& path,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::put(path, body, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_put(
    const std::string& path,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_put(path, body, headers, version);
}

HttpResponse WebSocketClient::del(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::del(path, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_del(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_del(path, headers, version);
}

HttpResponse WebSocketClient::patch(
    const std::string& path,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::patch(path, body, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_patch(
    const std::string& path,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_patch(path, body, headers, version);
}

HttpResponse WebSocketClient::head(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::head(path, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_head(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_head(path, headers, version);
}

HttpResponse WebSocketClient::options(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::options(path, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_options(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_options(path, headers, version);
}

HttpResponse WebSocketClient::connect(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& header,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::connect(path, header, version);
}

std::future<HttpResponse> WebSocketClient::async_connect(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_connect(path, headers, version);
}

HttpResponse WebSocketClient::trace(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::trace(path, headers, version);
}

std::future<HttpResponse> WebSocketClient::async_trace(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& headers,
    const std::string& version
) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::async_trace(path, headers, version);
}

std::optional<std::string> WebSocketClient::write_http(const HttpRequest& req) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::write_http(req);
}

std::optional<std::string> WebSocketClient::read_http(HttpResponse& res) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    return HttpClient::read_http(res);
}

std::shared_ptr<WebSocketClient> WebSocketClient::get_shared() {
    return std::static_pointer_cast<WebSocketClient>(HttpClient::get_shared());
}

/*************************WebSocket Server************************ */
/*************************WebSocket Server************************ */
/*************************WebSocket Server************************ */
/*************************WebSocket Server************************ */
/*************************WebSocket Server************************ */
/*************************WebSocket Server************************ */
/*************************WebSocket Server************************ */

WebSocketServer::WebSocketServer(std::shared_ptr<TcpServer> server): HttpServer(server) {
    set_handler();
}

void WebSocketServer::get(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::get(path, handler);
}

void WebSocketServer::post(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::post(path, handler);
}

void WebSocketServer::put(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::put(path, handler);
}

void WebSocketServer::del(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::del(path, handler);
}

void WebSocketServer::head(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::head(path, handler);
}

void WebSocketServer::trace(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::trace(path, handler);
}

void WebSocketServer::connect(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::connect(path, handler);
}

void WebSocketServer::options(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::options(path, handler);
}

void WebSocketServer::patch(const std::string path, std::function<HttpResponse(const HttpRequest&)> handler) {
    assert(m_websocket_status != WebSocketStatus::CONNECTED && "Protocol has upgraded to websocket!");
    HttpServer::patch(path, handler);
}

std::optional<std::string> WebSocketServer::accept_ws_connection(const HttpRequest& req, std::vector<uint8_t>& res) {
    if (req.headers().find("sec-websocket-key") == req.headers().end()) {
        return "Invalid websocket request";
    }
    auto key = req.headers().at("sec-websocket-key");
    auto accept_key = net::generate_websocket_accept_key(key);
    HttpResponse response;
    response.set_version(HTTP_VERSION_1_1)
        .set_status_code(HttpResponseCode::SWITCHING_PROTOCOLS)
        .set_reason(std::string(utils::dump_enum(HttpResponseCode::SWITCHING_PROTOCOLS)))
        .set_header("Upgrade", "websocket")
        .set_header("Connection", "Upgrade")
        .set_header("Sec-WebSocket-Accept", accept_key)
        .set_header("Sec-WebSocket-Version", "13");
    HttpParser parser;
    res = parser.write_res(response);
    return std::nullopt;
}

void WebSocketServer::set_handler() {
    auto http_handler = [this](Connection::ConstSharedPtr conn) {
        while (m_server->status() == ConnectionStatus::CONNECTED) {
            std::vector<uint8_t> req(1024);
            std::vector<uint8_t> res;
            auto err = m_server->read(req, conn);
            if (err.has_value()) {
                std::cerr << "Failed to read from socket: " << err.value() << std::endl;
                break;
            }

            HttpRequest request;
            HttpResponse response;
            if (!m_parsers.contains({ conn->m_client_ip, conn->m_client_service })) {
                m_parsers[{ conn->m_client_ip, conn->m_client_service }] = std::make_shared<HttpParser>();
            }
            auto& parser = m_parsers.at({ conn->m_client_ip, conn->m_client_service });
            // auto not_finished = parser->read_req(request, req);
            // if (not_finished.has_value()) {
            //     res.clear();
            //     continue;
            // }
            auto method = request.method();
            auto path = request.url();
            std::unordered_map<std::string, std::function<HttpResponse(const HttpRequest&)>>::iterator handler;
            // check if the request is a websocket upgrade request
            if (request.headers().find("upgrade") != request.headers().end() && m_allowed_paths.contains(path)) {
                // check if request is correct
                if (request.headers().at("upgrade") == "websocket" && request.headers().at("connection") == "Upgrade") {
                    if (!m_ws_parsers.contains({ conn->m_client_ip, conn->m_client_service })) {
                        m_ws_parsers[{ conn->m_client_ip, conn->m_client_service }] =
                            std::make_shared<WebSocketParser>();
                    }
                    m_ws_connections_flag.insert({ conn->m_client_ip, conn->m_client_service });
                    auto err = accept_ws_connection(request, res);
                    if (err.has_value()) {
                        response.set_version(HTTP_VERSION_1_1)
                            .set_status_code(HttpResponseCode::BAD_REQUEST)
                            .set_reason(std::string(utils::dump_enum(HttpResponseCode::BAD_REQUEST)))
                            .set_header("Content-Length", "0");
                        res = parser->write_res(response);
                    }
                } else {
                    response.set_version(HTTP_VERSION_1_1)
                        .set_status_code(HttpResponseCode::BAD_REQUEST)
                        .set_reason(std::string(utils::dump_enum(HttpResponseCode::BAD_REQUEST)))
                        .set_header("Content-Length", "0");
                    res = parser->write_res(response);
                }
                err = m_server->write(res, conn);
                if (err.has_value()) {
                    std::cerr << "Failed to write to socket: " << err.value() << std::endl;
                }
                break;
            }

            // handle normal http request
            try {
                handler = m_handlers.at(method).find(path);
            } catch (const std::out_of_range& e) {
                if (m_error_handlers.find(HttpResponseCode::BAD_REQUEST) != m_error_handlers.end()) {
                    response = m_error_handlers.at(HttpResponseCode::BAD_REQUEST)(request);
                } else {
                    response.set_version(HTTP_VERSION_1_1)
                        .set_status_code(HttpResponseCode::BAD_REQUEST)
                        .set_reason(std::string(utils::dump_enum(HttpResponseCode::BAD_REQUEST)))
                        .set_header("Content-Length", "0");
                }
            }

            if (handler == m_handlers.at(method).end()) {
                if (m_error_handlers.find(HttpResponseCode::NOT_FOUND) != m_error_handlers.end()) {
                    response = m_error_handlers.at(HttpResponseCode::NOT_FOUND)(request);
                } else {
                    response.set_version(HTTP_VERSION_1_1)
                        .set_status_code(HttpResponseCode::NOT_FOUND)
                        .set_reason(std::string(utils::dump_enum(HttpResponseCode::NOT_FOUND)))
                        .set_header("Content-Length", "0");
                }
            } else {
                try {
                    response = handler->second(request);
                } catch (const HttpResponseCode& e) {
                    if (m_error_handlers.find(e) != m_error_handlers.end()) {
                        response = m_error_handlers.at(e)(request);
                    } else {
                        response.set_version(HTTP_VERSION_1_1)
                            .set_status_code(e)
                            .set_reason(std::string(utils::dump_enum(e)))
                            .set_header("Content-Length", "0");
                    }
                }
            }
            res = parser->write_res(response);
            err = m_server->write(res, conn);
            if (err.has_value()) {
                std::cerr << "Failed to write to socket: " << err.value() << std::endl;
                break;
            }
        }
    };

    auto handler = [this, http_handler](Connection::ConstSharedPtr conn) {
        while (m_server->status() == net::ConnectionStatus::CONNECTED) {
            // parse request
            if (m_ws_connections_flag.contains({ conn->m_client_ip, conn->m_client_service })) {
                m_ws_handler(conn);
            } else {
                http_handler(conn);
            }
        }
    };
    m_server->on_accept(std::move(handler));
}

WebSocketServer::~WebSocketServer() {}

WebSocketStatus WebSocketServer::ws_status() const {
    return m_websocket_status;
}

void WebSocketServer::add_websocket_handler(std::function<void(Connection::ConstSharedPtr conn)> handler) {
    m_ws_handler = std::move(handler);
}

void WebSocketServer::allowed_path(const std::string& path) {
    m_allowed_paths.insert(path);
}

std::shared_ptr<WebSocketServer> WebSocketServer::get_shared() {
    return std::static_pointer_cast<WebSocketServer>(HttpServer::get_shared());
}

std::optional<std::string>
WebSocketServer::write_websocket_frame(const WebSocketFrame& frame, Connection::ConstSharedPtr conn) {
    std::vector<uint8_t> data;
    if (conn == nullptr) {
        for (auto& [key, connection]: m_server->get_connections()) {
            if (m_ws_connections_flag.contains({ connection->m_client_ip, connection->m_client_service })) {
                auto parser = m_ws_parsers.at({ connection->m_client_ip, connection->m_client_service });
                data = parser->write_frame(frame);

                auto err = m_server->write(data, connection);
                if (err.has_value()) {
                    return err;
                }
            }
        }
        return std::nullopt;
    }
    assert(
        m_ws_connections_flag.contains({ conn->m_client_ip, conn->m_client_service })
        && "Connection is not a websocket connection"
    );
    auto parser = m_ws_parsers.at({ conn->m_client_ip, conn->m_client_service });
    data = parser->write_frame(frame);
    return m_server->write(data, conn);
}

std::optional<std::string>
WebSocketServer::read_websocket_frame(WebSocketFrame& frame, Connection::ConstSharedPtr conn) {
    if (conn == nullptr) {
        for (auto& [key, connection]: m_server->get_connections()) {
            if (m_ws_connections_flag.contains({ connection->m_client_ip, connection->m_client_service })) {
                auto parser = m_ws_parsers.at({ connection->m_client_ip, connection->m_client_service });
                std::vector<uint8_t> data;
                auto err = m_server->read(data, connection);
                if (err.has_value()) {
                    return err;
                }
                auto result = parser->read_frame(data);
                if (!result.has_value()) {
                    return "Failed to parse frame";
                }
                frame = std::move(result.value());
                return std::nullopt;
            }
        }
    }
    assert(
        m_ws_connections_flag.contains({ conn->m_client_ip, conn->m_client_service })
        && "Connection is not a websocket connection"
    );
    auto parser = m_ws_parsers.at({ conn->m_client_ip, conn->m_client_service });
    std::vector<uint8_t> data(1024);
    auto err = m_server->read(data, conn);
    if (err.has_value()) {
        return err;
    }
    auto result = parser->read_frame(data);
    if (!result.has_value()) {
        return "Not finished yet";
    }
    frame = std::move(result.value());
    return std::nullopt;
}

} // namespace net