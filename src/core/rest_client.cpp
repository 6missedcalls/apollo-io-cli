#include "core/rest_client.h"

#include <string>
#include <utility>
#include <vector>

#include <curl/curl.h>

#include "core/auth.h"
#include "core/error.h"
#include "core/http_client.h"
#include "core/output.h"

using json = nlohmann::json;

static const std::string BASE_URL = "https://api.apollo.io/api/v1/";

std::string build_url(
    const std::string& path,
    const std::vector<std::pair<std::string, std::string>>& query_params
) {
    std::string url = BASE_URL + path;

    if (query_params.empty()) {
        return url;
    }

    // Use a temporary CURL handle for URL-encoding, with RAII cleanup
    struct CurlGuard {
        CURL* handle = curl_easy_init();
        ~CurlGuard() { if (handle) curl_easy_cleanup(handle); }
    };
    CurlGuard guard;

    if (guard.handle == nullptr) {
        // Fall back to unencoded values if curl init fails
        url += '?';
        bool first = true;
        for (const auto& [key, value] : query_params) {
            if (!first) {
                url += '&';
            }
            url += key + "=" + value;
            first = false;
        }
        return url;
    }

    url += '?';
    bool first = true;
    for (const auto& [key, value] : query_params) {
        if (!first) {
            url += '&';
        }
        char* encoded = curl_easy_escape(guard.handle, value.c_str(), static_cast<int>(value.size()));
        if (encoded != nullptr) {
            url += key + "=" + std::string(encoded);
            curl_free(encoded);
        } else {
            url += key + "=" + value;
        }
        first = false;
    }

    return url;
}

static json parse_json_response(const std::string& response_body) {
    try {
        return json::parse(response_body);
    } catch (const json::parse_error&) {
        throw ApolloError(
            ErrorKind::Internal,
            "Failed to parse API response. The server returned invalid JSON."
        );
    }
}

json rest_get(
    const std::string& path,
    const std::vector<std::pair<std::string, std::string>>& query_params
) {
    return with_retry([&]() {
        auto url = build_url(path, query_params);
        print_verbose(">> GET " + url);
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.get(url, headers);
        print_verbose("<< " + std::to_string(response.status_code) + " (" + std::to_string(response.body.size()) + " bytes)");

        check_http_status(response.status_code, response.body);
        auto parsed = parse_json_response(response.body);
        check_rest_errors(parsed);

        return parsed;
    });
}

json rest_post(
    const std::string& path,
    const json& body
) {
    return with_retry([&]() {
        auto url = build_url(path);
        print_verbose(">> POST " + url);
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.post(url, body.dump(), headers);
        print_verbose("<< " + std::to_string(response.status_code) + " (" + std::to_string(response.body.size()) + " bytes)");

        check_http_status(response.status_code, response.body);
        auto parsed = parse_json_response(response.body);
        check_rest_errors(parsed);

        return parsed;
    });
}

json rest_put(
    const std::string& path,
    const json& body
) {
    return with_retry([&]() {
        auto url = build_url(path);
        print_verbose(">> PUT " + url);
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.put(url, body.dump(), headers);
        print_verbose("<< " + std::to_string(response.status_code) + " (" + std::to_string(response.body.size()) + " bytes)");

        check_http_status(response.status_code, response.body);
        auto parsed = parse_json_response(response.body);
        check_rest_errors(parsed);

        return parsed;
    });
}

json rest_patch(
    const std::string& path,
    const json& body
) {
    return with_retry([&]() {
        auto url = build_url(path);
        print_verbose(">> PATCH " + url);
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.patch(url, body.dump(), headers);
        print_verbose("<< " + std::to_string(response.status_code) + " (" + std::to_string(response.body.size()) + " bytes)");

        check_http_status(response.status_code, response.body);
        auto parsed = parse_json_response(response.body);
        check_rest_errors(parsed);

        return parsed;
    });
}

json rest_delete(const std::string& path) {
    return with_retry([&]() {
        auto url = build_url(path);
        print_verbose(">> DELETE " + url);
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.del(url, headers);
        print_verbose("<< " + std::to_string(response.status_code) + " (" + std::to_string(response.body.size()) + " bytes)");

        check_http_status(response.status_code, response.body);
        auto parsed = parse_json_response(response.body);
        check_rest_errors(parsed);

        return parsed;
    });
}
