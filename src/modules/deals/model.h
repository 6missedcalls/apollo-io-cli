#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

struct Deal {
    std::string id;
    std::string name;
    double amount = 0;
    bool is_closed = false;
    bool is_won = false;
    std::optional<std::string> closed_date;
    std::optional<std::string> account_id;
    std::optional<std::string> owner_id;
    std::optional<std::string> stage_name;
    std::optional<std::string> opportunity_stage_id;
    std::optional<std::string> source;
    std::optional<std::string> description;
    std::optional<std::string> team_id;
    nlohmann::json typed_custom_fields;
    std::string created_at;
    std::string updated_at;
};

inline void from_json(const nlohmann::json& j, Deal& d) {
    auto safe_str = [&](const char* key, const std::string& def = "") -> std::string {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return def;
    };
    auto safe_double = [&](const char* key, double def = 0.0) -> double {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<double>();
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

    d.id = safe_str("id");
    d.name = safe_str("name");
    d.amount = safe_double("amount");
    d.is_closed = safe_bool("is_closed");
    d.is_won = safe_bool("is_won");
    d.closed_date = opt_str("closed_date");
    d.account_id = opt_str("account_id");
    d.owner_id = opt_str("owner_id");
    d.stage_name = opt_str("stage_name");
    d.opportunity_stage_id = opt_str("opportunity_stage_id");
    d.source = opt_str("source");
    d.description = opt_str("description");
    d.team_id = opt_str("team_id");
    d.created_at = safe_str("created_at");
    d.updated_at = safe_str("updated_at");

    if (j.contains("typed_custom_fields") && !j["typed_custom_fields"].is_null()) {
        d.typed_custom_fields = j["typed_custom_fields"];
    } else {
        d.typed_custom_fields = nlohmann::json::object();
    }
}

struct DealCreateInput {
    std::string name;  // required
    std::optional<std::string> owner_id;
    std::optional<std::string> account_id;
    std::optional<double> amount;
    std::optional<std::string> opportunity_stage_id;
    std::optional<std::string> closed_date;
    nlohmann::json typed_custom_fields;
};

inline nlohmann::json to_json_body(const DealCreateInput& input) {
    nlohmann::json body = nlohmann::json::object();

    if (!input.name.empty()) {
        body["name"] = input.name;
    }
    if (input.owner_id.has_value()) {
        body["owner_id"] = input.owner_id.value();
    }
    if (input.account_id.has_value()) {
        body["account_id"] = input.account_id.value();
    }
    if (input.amount.has_value()) {
        body["amount"] = std::to_string(static_cast<long long>(input.amount.value()));
    }
    if (input.opportunity_stage_id.has_value()) {
        body["opportunity_stage_id"] = input.opportunity_stage_id.value();
    }
    if (input.closed_date.has_value()) {
        body["closed_date"] = input.closed_date.value();
    }
    if (!input.typed_custom_fields.is_null() && !input.typed_custom_fields.empty()) {
        body["typed_custom_fields"] = input.typed_custom_fields;
    }

    return body;
}

using DealUpdateInput = DealCreateInput;
