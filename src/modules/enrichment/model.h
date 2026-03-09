#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

// ---------------------------------------------------------------------------
// PersonMatch
// ---------------------------------------------------------------------------

struct PersonMatch {
    std::string id;
    std::string first_name;
    std::string last_name;
    std::string name;
    std::optional<std::string> email;
    std::optional<std::string> title;
    std::optional<std::string> linkedin_url;
    std::optional<std::string> headline;
    std::optional<std::string> seniority;
    std::optional<std::string> city;
    std::optional<std::string> state;
    std::optional<std::string> country;
    std::optional<std::string> organization_name;
    nlohmann::json raw;  // Store full response for --json output
};

inline void from_json(const nlohmann::json& j, PersonMatch& p) {
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

    p.id = safe_str("id");
    p.first_name = safe_str("first_name");
    p.last_name = safe_str("last_name");
    p.name = safe_str("name");
    p.email = opt_str("email");
    p.title = opt_str("title");
    p.linkedin_url = opt_str("linkedin_url");
    p.headline = opt_str("headline");
    p.seniority = opt_str("seniority");
    p.city = opt_str("city");
    p.state = opt_str("state");
    p.country = opt_str("country");

    // Organization name from nested organization object
    if (j.contains("organization") && !j["organization"].is_null()) {
        const auto& org = j["organization"];
        if (org.contains("name") && !org["name"].is_null()) {
            p.organization_name = org["name"].get<std::string>();
        }
    }
    if (!p.organization_name.has_value()) {
        p.organization_name = opt_str("organization_name");
    }

    p.raw = j;
}

// ---------------------------------------------------------------------------
// OrgEnrichment
// ---------------------------------------------------------------------------

struct OrgEnrichment {
    std::string id;
    std::string name;
    std::optional<std::string> domain;
    std::optional<std::string> industry;
    std::optional<int> employee_count;
    std::optional<int> founded_year;
    std::optional<std::string> linkedin_url;
    std::optional<std::string> twitter_url;
    std::optional<std::string> phone;
    std::optional<std::string> city;
    std::optional<std::string> state;
    std::optional<std::string> country;
    std::optional<std::string> description;
    std::optional<std::string> short_description;
    std::optional<int64_t> annual_revenue;
    std::optional<std::string> annual_revenue_printed;
    nlohmann::json funding_data;
    nlohmann::json raw;
};

inline void from_json(const nlohmann::json& j, OrgEnrichment& o) {
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

    auto opt_int = [&](const char* key) -> std::optional<int> {
        if (j.contains(key) && !j[key].is_null() && j[key].is_number_integer()) {
            return j[key].get<int>();
        }
        return std::nullopt;
    };

    o.id = safe_str("id");
    o.name = safe_str("name");
    o.domain = opt_str("primary_domain");
    if (!o.domain.has_value()) {
        o.domain = opt_str("domain");
    }
    o.industry = opt_str("industry");
    o.employee_count = opt_int("estimated_num_employees");
    o.founded_year = opt_int("founded_year");
    o.linkedin_url = opt_str("linkedin_url");
    o.twitter_url = opt_str("twitter_url");

    // Phone from primary_phone object or top-level
    if (j.contains("primary_phone") && !j["primary_phone"].is_null() &&
        j["primary_phone"].contains("number") && !j["primary_phone"]["number"].is_null()) {
        o.phone = j["primary_phone"]["number"].get<std::string>();
    } else {
        o.phone = opt_str("phone");
    }

    o.city = opt_str("city");
    o.state = opt_str("state");
    o.country = opt_str("country");
    o.description = opt_str("description");
    o.short_description = opt_str("short_description");
    if (!o.description.has_value()) {
        o.description = opt_str("seo_description");
    }

    if (j.contains("annual_revenue") && !j["annual_revenue"].is_null() && j["annual_revenue"].is_number()) {
        o.annual_revenue = j["annual_revenue"].get<int64_t>();
    }
    o.annual_revenue_printed = opt_str("annual_revenue_printed");

    if (j.contains("funding_events") && j["funding_events"].is_array()) {
        o.funding_data = j["funding_events"];
    } else {
        o.funding_data = nlohmann::json::array();
    }

    o.raw = j;
}
