#pragma once

#include <string>
#include <utility>
#include <vector>

// Returns the API key. Priority:
// 1. APOLLO_API_KEY env var
// 2. api_key from config file
// 3. Throws ApolloError(Auth) with setup instructions
std::string get_api_key();

// Validate the API key by checking it is non-empty.
// Apollo keys have no standard prefix, so only a presence check is performed.
bool validate_api_key(const std::string& key);

// Returns auth headers for Apollo API requests.
// Apollo uses x-api-key header (not Bearer token).
std::vector<std::pair<std::string, std::string>> get_auth_headers();
