#include "core/rest_client.h"

#include <string>
#include <utility>
#include <vector>

#include <curl/curl.h>

#include "core/auth.h"
#include "core/error.h"
#include "core/http_client.h"

static const std::string BASE_URL = "https://api.apollo.io/api/v1/";

std::string build_url(
    const std::string& path,
    const std::vector<std::pair<std::string, std::string>>& query_params
) {
    std::string url = BASE_URL + path;

    if (query_params.empty()) {
        return url;
    }

    // Use a temporary CURL handle for URL-encoding
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
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
        char* encoded = curl_easy_escape(curl, value.c_str(), static_cast<int>(value.size()));
        if (encoded != nullptr) {
            url += key + "=" + std::string(encoded);
            curl_free(encoded);
        } else {
            url += key + "=" + value;
        }
        first = false;
    }

    curl_easy_cleanup(curl);
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
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.get(url, headers);

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
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.post(url, body.dump(), headers);

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
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.put(url, body.dump(), headers);

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
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.patch(url, body.dump(), headers);

        check_http_status(response.status_code, response.body);
        auto parsed = parse_json_response(response.body);
        check_rest_errors(parsed);

        return parsed;
    });
}

json rest_delete(const std::string& path) {
    return with_retry([&]() {
        auto url = build_url(path);
        auto headers = get_auth_headers();
        headers.emplace_back("Content-Type", "application/json");

        HttpClient client;
        auto response = client.del(url, headers);

        check_http_status(response.status_code, response.body);
        auto parsed = parse_json_response(response.body);
        check_rest_errors(parsed);

        return parsed;
    });
}
