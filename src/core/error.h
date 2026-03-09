#pragma once

#include <chrono>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>
#include <thread>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class ErrorKind {
    Network,
    Auth,
    Forbidden,
    NotFound,
    RateLimit,
    Api,
    Validation,
    Internal
};

class ApolloError : public std::runtime_error {
public:
    ApolloError(ErrorKind kind, const std::string& message);
    ApolloError(ErrorKind kind, const std::string& message, int retry_after);

    [[nodiscard]] ErrorKind kind() const noexcept;
    [[nodiscard]] std::optional<int> retry_after() const noexcept;

private:
    ErrorKind kind_;
    std::optional<int> retry_after_;
};

// Check HTTP status code and throw appropriate ApolloError
// Apollo error formats:
// 401: {"error": "api/v1/... is not accessible with this api_key", "error_code": "API_INACCESSIBLE"}
// 403: {"error": "Master API key is required to access this endpoint."}
// 429: {"message": "The maximum number of api calls allowed for api/v1/... is 600 times per hour..."}
void check_http_status(long status_code, const std::string& response_body);

// Check parsed JSON response for error fields
void check_rest_errors(const json& response);

// Retry wrapper for rate-limited requests
// F must be callable returning some type T
// Retries up to max_retries times with exponential backoff on RateLimit errors
// For 429: parses wait time from message body (regex for "X times per hour")
// Default backoff: 1s, 2s, 4s, 8s...
template <typename F>
auto with_retry(F&& fn, int max_retries = 3) -> decltype(fn()) {
    for (int attempt = 0; attempt <= max_retries; ++attempt) {
        try {
            return fn();
        } catch (const ApolloError& e) {
            if (e.kind() != ErrorKind::RateLimit || attempt == max_retries) {
                throw;
            }
            int wait_seconds = e.retry_after().value_or(1 << attempt);
            std::this_thread::sleep_for(std::chrono::seconds(wait_seconds));
        }
    }
    throw ApolloError(ErrorKind::Internal, "Retry loop exited unexpectedly");
}

// Format a user-friendly error message
std::string format_error(const ApolloError& e);
