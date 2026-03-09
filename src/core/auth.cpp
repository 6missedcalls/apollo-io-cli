#include "core/auth.h"

#include "core/config.h"
#include "core/error.h"

#include <cstdlib>

std::string get_api_key() {
    const char* env_key = std::getenv("APOLLO_API_KEY");
    if (env_key != nullptr && env_key[0] != '\0') {
        std::string key(env_key);
        if (!validate_api_key(key)) {
            throw ApolloError(
                ErrorKind::Auth,
                "Invalid API key in APOLLO_API_KEY environment variable. "
                "The key must not be empty."
            );
        }
        return key;
    }

    Config config = load_config();
    if (!config.api_key.empty()) {
        if (!validate_api_key(config.api_key)) {
            throw ApolloError(
                ErrorKind::Auth,
                "Invalid API key in config file. "
                "Run 'capollo config init' to reconfigure."
            );
        }
        return config.api_key;
    }

    throw ApolloError(
        ErrorKind::Auth,
        "No API key found. Set APOLLO_API_KEY environment variable or run 'capollo config init'"
    );
}

bool validate_api_key(const std::string& key) {
    return !key.empty();
}

std::vector<std::pair<std::string, std::string>> get_auth_headers() {
    return {{"x-api-key", get_api_key()}};
}
