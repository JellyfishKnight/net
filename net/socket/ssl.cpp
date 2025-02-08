#include "ssl.hpp"
#include "remote_target.hpp"
#include "socket_base.hpp"
#include "tcp.hpp"
#include <algorithm>
#include <memory>
#include <optional>
#include <utility>

namespace net {

enum class SSLMethod : uint8_t {

};

SSLContext::SSLContext() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    m_ctx = std::shared_ptr<SSL_CTX>(SSL_CTX_new(TLS_method()), [](SSL_CTX* ctx) { SSL_CTX_free(ctx); });
    if (m_ctx == nullptr) {
        throw std::runtime_error("Failed to create SSL context");
    }
}

SSLContext::~SSLContext() {
    ERR_free_strings();
    EVP_cleanup();
}

void SSLContext::set_certificates(const std::string& cert_file, const std::string& key_file) {
    if (SSL_CTX_use_certificate_file(m_ctx.get(), cert_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
        throw std::runtime_error("Failed to load certificate file");
    }
    if (SSL_CTX_use_PrivateKey_file(m_ctx.get(), key_file.c_str(), SSL_FILETYPE_PEM) <= 0) {
        throw std::runtime_error("Failed to load key file");
    }
    if (!SSL_CTX_check_private_key(m_ctx.get())) {
        throw std::runtime_error("Private key does not match the certificate public key");
    }
}

std::shared_ptr<SSL_CTX> SSLContext::get() {
    return m_ctx;
}

SSLClient::SSLClient(std::shared_ptr<SSLContext> ctx, const std::string& ip, const std::string& service):
    TcpClient(ip, service),
    m_ctx(std::move(ctx)) {
    m_ssl = std::shared_ptr<SSL>(SSL_new(m_ctx->get().get()), [](SSL* ssl) { SSL_free(ssl); });
    if (m_ssl == nullptr) {
        throw std::runtime_error("Failed to create SSL object");
    }
    SSL_set_fd(m_ssl.get(), m_fd);
}

std::optional<std::string> SSLClient::connect() {
    if (TcpClient::connect().has_value()) {
        return "Failed to connect to server";
    }
    if (SSL_connect(m_ssl.get()) <= 0) {
        return "Failed to establish SSL connection";
    }
    return std::nullopt;
}

std::optional<std::string> SSLClient::write(const std::vector<uint8_t>& data) {
    if (SSL_write(m_ssl.get(), data.data(), data.size()) <= 0) {
        return "Failed to write data";
    }
    return std::nullopt;
}

std::optional<std::string> SSLClient::read(std::vector<uint8_t>& data) {
    int num_bytes = SSL_read(m_ssl.get(), data.data(), data.size());
    if (num_bytes <= 0) {
        return "Failed to read data";
    }
    data.resize(num_bytes);
    return std::nullopt;
}

std::optional<std::string> SSLClient::close() {
    if (SSL_shutdown(m_ssl.get()) == 0) {
        return "Failed to shutdown SSL connection";
    }
    return TcpClient::close();
}

std::shared_ptr<SSLClient> SSLClient::get_shared() {
    return std::static_pointer_cast<SSLClient>(TcpClient::get_shared());
}

SSLServer::SSLServer(std::shared_ptr<SSLContext> ctx, const std::string& ip, const std::string& service):
    TcpServer(ip, service),
    m_ctx(std::move(ctx)) {}

std::optional<std::string> SSLServer::listen() {
    auto opt = TcpServer::listen();
    if (opt.has_value()) {
        return opt.value();
    }
    return std::nullopt;
}

std::optional<std::string> SSLServer::read(std::vector<uint8_t>& data, const RemoteTarget& conn) {
    auto& ssl = m_ssls.at(conn.m_client_fd);

    int num_bytes = SSL_read(ssl.get(), data.data(), data.size());
    if (num_bytes < 0) {
        if (m_logger_set) {
            NET_LOG_ERROR(m_logger, "Failed to read from socket: {}", get_error_msg());
        }
        return get_error_msg();
    }
    if (num_bytes == 0) {
        if (m_logger_set) {
            NET_LOG_ERROR(m_logger, "Connection reset by peer while reading");
        }
        return "Connection reset by peer while reading";
    }
    data.resize(num_bytes);
    return std::nullopt;
}

std::optional<std::string> SSLServer::write(const std::vector<uint8_t>& data, const RemoteTarget& conn) {
    auto& ssl = m_ssls.at(conn.m_client_fd);

    auto num_bytes = SSL_write(ssl.get(), data.data(), data.size());
    if (num_bytes < 0) {
        if (m_logger_set) {
            NET_LOG_ERROR(m_logger, "Failed to write to socket: {}", get_error_msg());
        }
        return get_error_msg();
    }
    if (num_bytes == 0) {
        if (m_logger_set) {
            NET_LOG_ERROR(m_logger, "Connection reset by peer while writing");
        }
        return "Connection reset by peer while writing";
    }
    return std::nullopt;
}

std::optional<std::string> SSLServer::close() {
    std::for_each(m_remotes.begin(), m_remotes.end(), [this](auto& conn) {
        SSL_shutdown(m_ssls.at(conn.second.m_client_fd).get());
    });
    return TcpServer::close();
}

void SSLServer::handle_connection(const RemoteTarget& conn) {
    auto& ssl = m_ssls.at(conn.m_client_fd);
    if (SSL_accept(ssl.get()) <= 0) {
        if (m_logger_set) {
            NET_LOG_ERROR(m_logger, "Failed to establish SSL connection");
        }
        std::cerr << std::format("Failed to establish SSL connection\n");
        return;
    }
    TcpServer::handle_connection(conn);
}

RemoteTarget SSLServer::create_remote(int remote_fd) {
    m_ssls[remote_fd] = std::shared_ptr<SSL>(SSL_new(m_ctx->get().get()), [](SSL* ssl) { SSL_free(ssl); });
    SSL_set_fd(m_ssls[remote_fd].get(), remote_fd);
    return TcpServer::create_remote(remote_fd);
}

std::shared_ptr<SSLServer> SSLServer::get_shared() {
    return std::static_pointer_cast<SSLServer>(TcpServer::get_shared());
}

} // namespace net