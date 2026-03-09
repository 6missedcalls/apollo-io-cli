#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

struct ApolloUser {
    std::string id;
    std::string first_name;
    std::string last_name;
    std::string name;
    std::string email;
    std::optional<std::string> team_id;
    std::optional<std::string> role;    // admin, manager, member
    std::string created_at;
};

inline void from_json(const nlohmann::json& j, ApolloUser& u) {
    auto safe_str = [&](const char* key, const std::string& def = "") -> std::string {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return def;
    };

    auto opt_str = [&](const char* key) -> std::optional<std::string> {
        if (j.contains(key) && !j[key].is_null()) {
            return j[key].get<std::string>();
        }
        return std::nullopt;
    };

    u.id = safe_str("id");
    u.first_name = safe_str("first_name");
    u.last_name = safe_str("last_name");
    u.name = safe_str("name");
    u.email = safe_str("email");
    u.team_id = opt_str("team_id");
    u.role = opt_str("role");
    u.created_at = safe_str("created_at");
}
