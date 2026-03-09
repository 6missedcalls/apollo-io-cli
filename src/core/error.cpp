#include "error.h"

#include <regex>
#include <sstream>
#include <string>

ApolloError::ApolloError(ErrorKind kind, const std::string& message)
    : std::runtime_error(message), kind_(kind), retry_after_(std::nullopt) {}

ApolloError::ApolloError(ErrorKind kind, const std::string& message, int retry_after)
    : std::runtime_error(message), kind_(kind), retry_after_(retry_after) {}

ErrorKind ApolloError::kind() const noexcept {
    return kind_;
}

std::optional<int> ApolloError::retry_after() const noexcept {
    return retry_after_;
}

// Parse rate limit wait time from Apollo's 429 message body.
// Example: "The maximum number of api calls allowed for api/v1/... is 600 times per hour..."
// Returns seconds to wait, or std::nullopt if unparseable.
static std::optional<int> parse_rate_limit_wait(const std::string& message) {
    // Look for "X times per hour" or "X times per minute"
    std::regex hour_re(R"((\d+)\s+times?\s+per\s+hour)", std::regex_constants::icase);
    std::regex minute_re(R"((\d+)\s+times?\s+per\s+minute)", std::regex_constants::icase);

    std::smatch match;
    if (std::regex_search(message, match, hour_re)) {
        // Rate is per hour — wait 60 seconds as a reasonable backoff
        return 60;
    }
    if (std::regex_search(message, match, minute_re)) {
        // Rate is per minute — wait 10 seconds as a reasonable backoff
        return 10;
    }

    return std::nullopt;
}

void check_http_status(long status_code, const std::string& response_body) {
    if (status_code >= 200 && status_code < 300) {
        return;
    }

    if (status_code == 401) {
        std::string message = "Authentication failed — check your APOLLO_API_KEY";

        try {
            auto body = json::parse(response_body);
            if (body.contains("error") && body["error"].is_string()) {
                message = body["error"].get<std::string>();
            }
        } catch (const json::parse_error&) {
            // Response body is not JSON — use default message
        }

        throw ApolloError(ErrorKind::Auth, message);
    }

    if (status_code == 403) {
        std::string message = "Master API key required for this endpoint";

        try {
            auto body = json::parse(response_body);
            if (body.contains("error") && body["error"].is_string()) {
                message = body["error"].get<std::string>();
            }
        } catch (const json::parse_error&) {
            // Response body is not JSON — use default message
        }

        throw ApolloError(ErrorKind::Forbidden, message);
    }

    if (status_code == 404) {
        throw ApolloError(ErrorKind::NotFound, "Resource not found");
    }

    if (status_code == 429) {
        std::string message = "Rate limited by Apollo API";
        std::optional<int> wait_seconds;

        try {
            auto body = json::parse(response_body);
            if (body.contains("message") && body["message"].is_string()) {
                message = body["message"].get<std::string>();
                wait_seconds = parse_rate_limit_wait(message);
            }
        } catch (const json::parse_error&) {
            // Response body is not JSON — no wait time available
        }

        if (wait_seconds.has_value()) {
            throw ApolloError(ErrorKind::RateLimit, message, wait_seconds.value());
        }
        throw ApolloError(ErrorKind::RateLimit, message);
    }

    if (status_code >= 500) {
        std::string message =
            "Apollo API internal error (HTTP " + std::to_string(status_code) + ")";
        throw ApolloError(ErrorKind::Internal, message);
    }

    std::string message =
        "Unexpected HTTP status " + std::to_string(status_code);
    throw ApolloError(ErrorKind::Network, message);
}

void check_rest_errors(const json& response) {
    if (!response.contains("error")) {
        return;
    }

    if (response["error"].is_string()) {
        std::string message = response["error"].get<std::string>();

        // Check for specific error codes
        if (response.contains("error_code") && response["error_code"].is_string()) {
            std::string code = response["error_code"].get<std::string>();
            if (code == "API_INACCESSIBLE") {
                throw ApolloError(ErrorKind::Auth, message);
            }
        }

        throw ApolloError(ErrorKind::Api, message);
    }

    if (response["error"].is_object()) {
        const auto& err = response["error"];
        if (err.contains("message") && err["message"].is_string()) {
            throw ApolloError(ErrorKind::Api, err["message"].get<std::string>());
        }
    }

    throw ApolloError(ErrorKind::Api, "Unknown API error");
}

std::string format_error(const ApolloError& e) {
    switch (e.kind()) {
        case ErrorKind::Auth:
            return std::string("Authentication error: ") + e.what();
        case ErrorKind::Forbidden:
            return std::string("Forbidden: ") + e.what();
        case ErrorKind::RateLimit: {
            std::string msg = "Rate limited";
            if (e.retry_after().has_value()) {
                msg += ", retry after " + std::to_string(e.retry_after().value()) + "s";
            }
            return msg;
        }
        case ErrorKind::NotFound:
            return std::string("Not found: ") + e.what();
        case ErrorKind::Api:
            return std::string("API error: ") + e.what();
        case ErrorKind::Validation:
            return std::string("Validation error: ") + e.what();
        case ErrorKind::Network:
            return std::string("Network error: ") + e.what();
        case ErrorKind::Internal:
            return std::string("Internal error: ") + e.what();
    }
    return std::string("Error: ") + e.what();
}
