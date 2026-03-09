#pragma once

#include <string>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

// Build full Apollo API URL from path and optional query parameters.
// Base: https://api.apollo.io/api/v1/
// Query parameter values are URL-encoded.
std::string build_url(
    const std::string& path,
    const std::vector<std::pair<std::string, std::string>>& query_params = {});

// Execute REST calls — each handles auth, error checking, JSON parsing.
// All functions automatically retry on 429 rate limit errors.

nlohmann::json rest_get(
    const std::string& path,
    const std::vector<std::pair<std::string, std::string>>& query_params = {});

nlohmann::json rest_post(
    const std::string& path,
    const nlohmann::json& body = nlohmann::json::object());

nlohmann::json rest_put(
    const std::string& path,
    const nlohmann::json& body = nlohmann::json::object());

nlohmann::json rest_patch(
    const std::string& path,
    const nlohmann::json& body = nlohmann::json::object());

nlohmann::json rest_delete(const std::string& path);
