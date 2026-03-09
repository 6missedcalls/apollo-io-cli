#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Account {
    std::string id;
    std::string name;
    std::optional<std::string> domain;
    std::optional<std::string> phone;
    std::optional<std::string> phone_status;       // no_status, valid, invalid
    std::optional<std::string> sanitized_phone;
    std::optional<std::string> owner_id;
    std::optional<std::string> creator_id;
    std::optional<std::string> account_stage_id;
    std::optional<std::string> team_id;
    std::optional<std::string> organization_id;
    std::optional<std::string> source;
    std::optional<std::string> original_source;
    std::optional<std::string> linkedin_url;
    std::optional<std::string> hubspot_id;
    std::optional<std::string> salesforce_id;
    std::optional<std::string> crm_owner_id;
    std::optional<std::string> parent_account_id;
    std::optional<std::string> existence_level;     // full, partial
    std::vector<std::string> label_ids;
    json typed_custom_fields;
    std::string modality;                           // "account"
    std::string created_at;
    std::string updated_at;
};

inline void from_json(const json& j, Account& a) {
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

    a.id = safe_str("id");
    a.name = safe_str("name");
    a.created_at = safe_str("created_at");
    a.updated_at = safe_str("updated_at");
    a.modality = safe_str("modality", "account");

    a.domain = opt_str("domain");
    a.phone = opt_str("phone");
    a.phone_status = opt_str("phone_status");
    a.sanitized_phone = opt_str("sanitized_phone");
    a.owner_id = opt_str("owner_id");
    a.creator_id = opt_str("creator_id");
    a.account_stage_id = opt_str("account_stage_id");
    a.team_id = opt_str("team_id");
    a.organization_id = opt_str("organization_id");
    a.source = opt_str("source");
    a.original_source = opt_str("original_source");
    a.linkedin_url = opt_str("linkedin_url");
    a.hubspot_id = opt_str("hubspot_id");
    a.salesforce_id = opt_str("salesforce_id");
    a.crm_owner_id = opt_str("crm_owner_id");
    a.parent_account_id = opt_str("parent_account_id");
    a.existence_level = opt_str("existence_level");

    // Label IDs array
    if (j.contains("label_ids") && j["label_ids"].is_array()) {
        for (const auto& lid : j["label_ids"]) {
            if (lid.is_string()) {
                a.label_ids.push_back(lid.get<std::string>());
            }
        }
    }

    // Typed custom fields
    if (j.contains("typed_custom_fields") && !j["typed_custom_fields"].is_null()) {
        a.typed_custom_fields = j["typed_custom_fields"];
    } else {
        a.typed_custom_fields = json::object();
    }
}

// ---------------------------------------------------------------------------
// Input types
// ---------------------------------------------------------------------------

struct AccountCreateInput {
    std::string name;  // required
    std::optional<std::string> domain;
    std::optional<std::string> phone;
    std::optional<std::string> owner_id;
    std::optional<std::string> account_stage_id;
    std::optional<std::string> linkedin_url;
    std::vector<std::string> append_label_names;
    json typed_custom_fields;
};

inline json to_json_body(const AccountCreateInput& input) {
    json body = json::object();

    if (!input.name.empty()) {
        body["name"] = input.name;
    }
    if (input.domain.has_value()) {
        body["domain"] = input.domain.value();
    }
    if (input.phone.has_value()) {
        body["phone"] = input.phone.value();
    }
    if (input.owner_id.has_value()) {
        body["owner_id"] = input.owner_id.value();
    }
    if (input.account_stage_id.has_value()) {
        body["account_stage_id"] = input.account_stage_id.value();
    }
    if (input.linkedin_url.has_value()) {
        body["linkedin_url"] = input.linkedin_url.value();
    }
    if (!input.append_label_names.empty()) {
        body["append_label_names"] = input.append_label_names;
    }
    if (!input.typed_custom_fields.is_null() && !input.typed_custom_fields.empty()) {
        body["typed_custom_fields"] = input.typed_custom_fields;
    }

    return body;
}

using AccountUpdateInput = AccountCreateInput;

struct OwnershipUpdate {
    std::vector<std::string> account_ids;
    std::string owner_id;
};

inline json to_json_body(const OwnershipUpdate& input) {
    json body = json::object();
    body["account_ids"] = input.account_ids;
    body["owner_id"] = input.owner_id;
    return body;
}
