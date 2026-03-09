#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

// ---------------------------------------------------------------------------
// PhoneNumber
// ---------------------------------------------------------------------------

struct PhoneNumber {
    std::string raw_number;
    std::string type;               // work_direct, mobile, work_hq, home, other
    std::string sanitized_number;
};

inline void from_json(const nlohmann::json& j, PhoneNumber& p) {
    p.raw_number = j.value("raw_number", "");
    p.type = j.value("type", "");
    p.sanitized_number = j.value("sanitized_number", "");
}

inline nlohmann::json to_json_body(const PhoneNumber& p) {
    nlohmann::json j = nlohmann::json::object();
    if (!p.raw_number.empty()) j["raw_number"] = p.raw_number;
    if (!p.type.empty()) j["type"] = p.type;
    if (!p.sanitized_number.empty()) j["sanitized_number"] = p.sanitized_number;
    return j;
}

// ---------------------------------------------------------------------------
// Contact
// ---------------------------------------------------------------------------

struct Contact {
    std::string id;
    std::string first_name;
    std::string last_name;
    std::string name;
    std::string email;
    std::string email_status;       // verified, guessed, unavailable, bounced, pending_manual_fulfillment, unknown

    std::optional<std::string> organization_id;
    std::optional<std::string> organization_name;
    std::optional<std::string> title;
    std::optional<std::string> owner_id;
    std::optional<std::string> linkedin_url;

    std::vector<PhoneNumber> phone_numbers;

    std::optional<std::string> present_raw_address;
    std::optional<std::string> city;
    std::optional<std::string> state;
    std::optional<std::string> country;
    std::optional<std::string> postal_code;

    std::optional<std::string> contact_stage_id;
    std::vector<std::string> label_ids;
    std::optional<std::string> account_id;
    std::optional<std::string> source;
    std::optional<std::string> original_source;

    std::string created_at;
    std::string updated_at;

    nlohmann::json typed_custom_fields;

    std::optional<std::string> existence_level;  // full, invisible

    // Enrichment fields (from person wrapper in GET/PUT responses)
    std::optional<std::string> photo_url;
    std::optional<std::string> twitter_url;
    std::optional<std::string> facebook_url;
    std::optional<std::string> github_url;
    std::optional<std::string> headline;
    std::optional<std::string> seniority;
    std::vector<std::string> departments;
};

inline void from_json(const nlohmann::json& j, Contact& c) {
    // Null-safe accessors: j.value() doesn't handle null, only missing keys
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

    c.id = safe_str("id");
    c.first_name = safe_str("first_name");
    c.last_name = safe_str("last_name");
    c.name = safe_str("name");
    c.email = safe_str("email");
    c.email_status = safe_str("email_status");

    c.organization_id = opt_str("organization_id");
    c.organization_name = opt_str("organization_name");
    c.title = opt_str("title");
    c.owner_id = opt_str("owner_id");
    c.linkedin_url = opt_str("linkedin_url");

    // Phone numbers array
    c.phone_numbers.clear();
    if (j.contains("phone_numbers") && j["phone_numbers"].is_array()) {
        for (const auto& pn : j["phone_numbers"]) {
            PhoneNumber phone;
            from_json(pn, phone);
            c.phone_numbers.push_back(phone);
        }
    }

    c.present_raw_address = opt_str("present_raw_address");
    c.city = opt_str("city");
    c.state = opt_str("state");
    c.country = opt_str("country");
    c.postal_code = opt_str("postal_code");

    c.contact_stage_id = opt_str("contact_stage_id");

    // Label IDs array
    c.label_ids.clear();
    if (j.contains("label_ids") && j["label_ids"].is_array()) {
        for (const auto& lid : j["label_ids"]) {
            if (lid.is_string()) {
                c.label_ids.push_back(lid.get<std::string>());
            }
        }
    }

    c.account_id = opt_str("account_id");
    c.source = opt_str("source");
    c.original_source = opt_str("original_source");

    c.created_at = safe_str("created_at");
    c.updated_at = safe_str("updated_at");

    // Typed custom fields as raw JSON
    if (j.contains("typed_custom_fields") && !j["typed_custom_fields"].is_null()) {
        c.typed_custom_fields = j["typed_custom_fields"];
    } else {
        c.typed_custom_fields = nlohmann::json::object();
    }

    c.existence_level = opt_str("existence_level");

    // Enrichment fields (may not be present on all response types)
    c.photo_url = opt_str("photo_url");
    c.twitter_url = opt_str("twitter_url");
    c.facebook_url = opt_str("facebook_url");
    c.github_url = opt_str("github_url");
    c.headline = opt_str("headline");
    c.seniority = opt_str("seniority");

    // Departments array
    c.departments.clear();
    if (j.contains("departments") && j["departments"].is_array()) {
        for (const auto& dep : j["departments"]) {
            if (dep.is_string()) {
                c.departments.push_back(dep.get<std::string>());
            }
        }
    }
}

// ---------------------------------------------------------------------------
// ContactCreateInput / ContactUpdateInput
// ---------------------------------------------------------------------------

struct ContactCreateInput {
    std::optional<std::string> first_name;
    std::optional<std::string> last_name;
    std::optional<std::string> email;
    std::optional<std::string> organization_name;
    std::optional<std::string> title;
    std::optional<std::string> account_id;
    std::optional<std::string> owner_id;
    std::optional<std::string> linkedin_url;
    std::optional<std::string> present_raw_address;
    std::optional<std::string> contact_stage_id;
    std::vector<std::string> label_names;
    std::vector<PhoneNumber> phone_numbers;
    nlohmann::json typed_custom_fields;
};

inline nlohmann::json to_json_body(const ContactCreateInput& input) {
    nlohmann::json body = nlohmann::json::object();

    if (input.first_name.has_value()) body["first_name"] = input.first_name.value();
    if (input.last_name.has_value()) body["last_name"] = input.last_name.value();
    if (input.email.has_value()) body["email"] = input.email.value();
    if (input.organization_name.has_value()) body["organization_name"] = input.organization_name.value();
    if (input.title.has_value()) body["title"] = input.title.value();
    if (input.account_id.has_value()) body["account_id"] = input.account_id.value();
    if (input.owner_id.has_value()) body["owner_id"] = input.owner_id.value();
    if (input.linkedin_url.has_value()) body["linkedin_url"] = input.linkedin_url.value();
    if (input.present_raw_address.has_value()) body["present_raw_address"] = input.present_raw_address.value();
    if (input.contact_stage_id.has_value()) body["contact_stage_id"] = input.contact_stage_id.value();

    if (!input.label_names.empty()) {
        body["label_names"] = input.label_names;
    }

    if (!input.phone_numbers.empty()) {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& pn : input.phone_numbers) {
            arr.push_back(to_json_body(pn));
        }
        body["phone_numbers"] = arr;
    }

    if (!input.typed_custom_fields.is_null() && !input.typed_custom_fields.empty()) {
        body["typed_custom_fields"] = input.typed_custom_fields;
    }

    return body;
}

// ContactUpdateInput is the same shape as create (all fields optional)
using ContactUpdateInput = ContactCreateInput;
