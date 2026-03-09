#pragma once

#include <string>
#include <utility>
#include <vector>

struct HttpResponse {
    long status_code = 0;
    std::string body;
    std::string error_message;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
    HttpClient(HttpClient&& other) noexcept;
    HttpClient& operator=(HttpClient&& other) noexcept;

    HttpResponse get(
        const std::string& url,
        const std::vector<std::pair<std::string, std::string>>& headers);

    HttpResponse post(
        const std::string& url,
        const std::string& body,
        const std::vector<std::pair<std::string, std::string>>& headers);

    HttpResponse put(
        const std::string& url,
        const std::string& body,
        const std::vector<std::pair<std::string, std::string>>& headers);

    HttpResponse patch(
        const std::string& url,
        const std::string& body,
        const std::vector<std::pair<std::string, std::string>>& headers);

    HttpResponse del(
        const std::string& url,
        const std::vector<std::pair<std::string, std::string>>& headers);

    void set_timeout(long timeout_seconds) noexcept;

private:
    HttpResponse execute(
        const std::string& url,
        const std::string& method,
        const std::string& body,
        const std::vector<std::pair<std::string, std::string>>& headers);

    void* curl_handle_ = nullptr;
    long timeout_seconds_ = 30;
};
