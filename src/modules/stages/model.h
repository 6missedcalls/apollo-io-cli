#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Stage {
    std::string id;
    std::string team_id;
    std::string display_name;
    std::string name;
    int display_order = 0;
    std::optional<std::string> category;  // in_progress, failed, succeeded, null
    std::optional<bool> is_meeting_set;
    bool default_exclude_for_leadgen = false;
    // Deal-specific fields
    std::optional<bool> is_won;
    std::optional<bool> is_closed;
    std::optional<double> probability;
};

inline void from_json(const json& j, Stage& s) {
    auto safe_str = [&](const char* key, const std::string& def = "") -> std::string {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return def;
    };
    auto safe_int = [&](const char* key, int def = 0) -> int {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<int>();
        return def;
    };
    auto safe_bool = [&](const char* key, bool def = false) -> bool {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<bool>();
        return def;
    };
    auto opt_str = [&](const char* key) -> std::optional<std::string> {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return std::nullopt;
    };

    s.id = safe_str("id");
    s.team_id = safe_str("team_id");
    s.display_name = safe_str("display_name");
    s.name = safe_str("name");
    s.display_order = safe_int("display_order");
    s.category = opt_str("category");
    s.default_exclude_for_leadgen = safe_bool("default_exclude_for_leadgen");

    if (j.contains("is_meeting_set") && !j["is_meeting_set"].is_null()) {
        s.is_meeting_set = j["is_meeting_set"].get<bool>();
    }
    if (j.contains("is_won") && !j["is_won"].is_null()) {
        s.is_won = j["is_won"].get<bool>();
    }
    if (j.contains("is_closed") && !j["is_closed"].is_null()) {
        s.is_closed = j["is_closed"].get<bool>();
    }
    if (j.contains("probability") && !j["probability"].is_null()) {
        s.probability = j["probability"].get<double>();
    }
}
