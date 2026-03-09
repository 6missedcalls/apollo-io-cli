#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct CustomField {
    std::string id;
    std::string name;
    std::string label;
    std::string field_type;  // text, number, date, datetime, dropdown, etc.
    std::optional<std::string> modality;
    bool system_field = false;
    std::vector<std::string> picklist_values;
};

inline void from_json(const json& j, CustomField& f) {
    auto safe_str = [&](const char* key, const std::string& def = "") -> std::string {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return def;
    };

    auto safe_bool = [&](const char* key, bool def = false) -> bool {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<bool>();
        return def;
    };

    auto opt_str = [&](const char* key) -> std::optional<std::string> {
        if (j.contains(key) && !j[key].is_null()) {
            return j[key].get<std::string>();
        }
        return std::nullopt;
    };

    f.id = safe_str("id");
    f.name = safe_str("name");
    f.label = safe_str("label");

    // field_type can come from "type" or "field_type"
    f.field_type = safe_str("field_type");
    if (f.field_type.empty()) {
        f.field_type = safe_str("type");
    }

    f.modality = opt_str("modality");
    f.system_field = safe_bool("system_field");

    // Picklist values can be array of objects or array of strings
    f.picklist_values.clear();
    if (j.contains("picklist_values") && j["picklist_values"].is_array()) {
        for (const auto& pv : j["picklist_values"]) {
            if (pv.is_string()) {
                f.picklist_values.push_back(pv.get<std::string>());
            } else if (pv.is_object() && pv.contains("value") && !pv["value"].is_null()) {
                f.picklist_values.push_back(pv["value"].get<std::string>());
            }
        }
    }
}
