#pragma once

#include <string>

#include <nlohmann/json.hpp>

struct Label {
    std::string id;
    std::string name;
    std::string modality;  // contacts, accounts, emailer_campaigns
    int cached_count = 0;
    std::string created_at;
    std::string updated_at;
};

inline void from_json(const nlohmann::json& j, Label& l) {
    auto safe_str = [&](const char* key, const std::string& def = "") -> std::string {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return def;
    };

    l.id = safe_str("id");
    // Fallback to _id if id is missing
    if (l.id.empty()) {
        l.id = safe_str("_id");
    }
    l.name = safe_str("name");
    l.modality = safe_str("modality");
    l.cached_count = j.value("cached_count", 0);
    l.created_at = safe_str("created_at");
    l.updated_at = safe_str("updated_at");
}
