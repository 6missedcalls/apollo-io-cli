#include "core/http_client.h"

#include <stdexcept>
#include <string>

namespace {

struct WriteContext {
    std::string* buffer;
};

auto write_callback(
    char* ptr,
    size_t size,
    size_t nmemb,
    void* userdata
) -> size_t {
    auto* context = static_cast<WriteContext*>(userdata);
    const size_t total_bytes = size * nmemb;
    context->buffer->append(ptr, total_bytes);
    return total_bytes;
}

} // namespace

HttpClient::HttpClient()
    : curl_handle_(curl_easy_init()) {
    if (curl_handle_ == nullptr) {
        throw std::runtime_error("Failed to initialize libcurl handle");
    }
}

HttpClient::~HttpClient() {
    if (curl_handle_ != nullptr) {
        curl_easy_cleanup(curl_handle_);
    }
}

HttpClient::HttpClient(HttpClient&& other) noexcept
    : curl_handle_(other.curl_handle_),
      timeout_seconds_(other.timeout_seconds_) {
    other.curl_handle_ = nullptr;
    other.timeout_seconds_ = 30;
}

HttpClient& HttpClient::operator=(HttpClient&& other) noexcept {
    if (this == &other) {
        return *this;
    }
    if (curl_handle_ != nullptr) {
        curl_easy_cleanup(curl_handle_);
    }
    curl_handle_ = other.curl_handle_;
    timeout_seconds_ = other.timeout_seconds_;
    other.curl_handle_ = nullptr;
    other.timeout_seconds_ = 30;
    return *this;
}

HttpResponse HttpClient::get(
    const std::string& url,
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    return execute(url, "GET", "", headers);
}

HttpResponse HttpClient::post(
    const std::string& url,
    const std::string& body,
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    return execute(url, "POST", body, headers);
}

HttpResponse HttpClient::put(
    const std::string& url,
    const std::string& body,
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    return execute(url, "PUT", body, headers);
}

HttpResponse HttpClient::patch(
    const std::string& url,
    const std::string& body,
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    return execute(url, "PATCH", body, headers);
}

HttpResponse HttpClient::del(
    const std::string& url,
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    return execute(url, "DELETE", "", headers);
}

void HttpClient::set_timeout(long timeout_seconds) noexcept {
    timeout_seconds_ = timeout_seconds;
}

HttpResponse HttpClient::execute(
    const std::string& url,
    const std::string& method,
    const std::string& body,
    const std::vector<std::pair<std::string, std::string>>& headers
) {
    if (curl_handle_ == nullptr) {
        return HttpResponse{0, "", "HttpClient has been moved from"};
    }

    // Reset all options from any previous request
    curl_easy_reset(curl_handle_);

    HttpResponse response;
    std::string response_body;

    WriteContext write_ctx{&response_body};

    // Build the headers slist
    struct curl_slist* header_list = nullptr;
    for (const auto& [name, value] : headers) {
        const std::string formatted = name + ": " + value;
        header_list = curl_slist_append(header_list, formatted.c_str());
    }

    curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle_, CURLOPT_CUSTOMREQUEST, method.c_str());

    if (method == "POST" || method == "PUT" || method == "PATCH") {
        curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl_handle_, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
    }

    curl_easy_setopt(curl_handle_, CURLOPT_HTTPHEADER, header_list);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &write_ctx);
    curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, timeout_seconds_);
    // Disable redirects — API endpoints should not redirect,
    // and following redirects could leak the API key header
    curl_easy_setopt(curl_handle_, CURLOPT_FOLLOWLOCATION, 0L);
    // Explicitly enforce TLS certificate verification
    curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl_handle_, CURLOPT_SSL_VERIFYHOST, 2L);
    // Enable TCP keepalive
    curl_easy_setopt(curl_handle_, CURLOPT_TCP_KEEPALIVE, 1L);

    const CURLcode result = curl_easy_perform(curl_handle_);

    // Clean up the headers slist regardless of outcome
    if (header_list != nullptr) {
        curl_slist_free_all(header_list);
    }

    if (result != CURLE_OK) {
        response.status_code = 0;
        response.body = "";
        response.error_message = curl_easy_strerror(result);
        return response;
    }

    long http_code = 0;
    curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, &http_code);

    response.status_code = http_code;
    response.body = std::move(response_body);
    response.error_message = "";

    return response;
}
