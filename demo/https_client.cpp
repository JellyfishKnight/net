#include "http_client.hpp"
#include "ssl.hpp"
#include "tcp.hpp"
#include <filesystem>
#include <memory>

std::string getExecutablePath() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        result[count] = '\0';
        return std::string(result);
    }
    return "";
}

int main() {
    auto ctx = net::SSLContext::create();
    auto exe_path = getExecutablePath();
    auto execDir = std::filesystem::path(exe_path).parent_path().string();

    ctx->set_certificates(execDir + "/template/keys/certificate.crt", execDir + "/template/keys/private.key");

    net::SSLClient::SharedPtr ssl_client = std::make_shared<net::SSLClient>(ctx, "127.0.0.1", "8080");

    net::HttpClient client(ssl_client);

    client.connect_server();

    while (true) {
        std::string input;
        std::cin >> input;
        if (input == "exit") {
            client.close();
            return 0;
        }
        if (input == "s") {
            input.clear();
        }
        auto res = client.get("/" + input);
        std::cout << "Http version: " << res.version() << std::endl;
        std::cout << "Status Code: " << static_cast<int>(res.status_code()) << std::endl;
        std::cout << "Reason: " << res.reason() << std::endl;
        std::cout << "Headers: " << std::endl;
        for (auto& [key, value]: res.headers()) {
            std::cout << key << ": " << value << std::endl;
        }
        std::cout << "Body: " << res.body() << std::endl;
    }

    return 0;
}