#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct EmailAccount {
    std::string id;
    std::string email;
    std::optional<std::string> provider;  // gmail, microsoft, smtp, sendgrid
    std::optional<std::string> warmup_status;  // never_started, active, paused, completed, stopped
    bool active = false;
    int max_outbound_per_hour = 0;
    int true_warmup_daily_limit = 0;
};

inline void from_json(const json& j, EmailAccount& ea) {
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

    ea.id = safe_str("id");
    ea.email = safe_str("email");
    ea.provider = opt_str("type");
    ea.warmup_status = opt_str("mailwarming_status");
    ea.active = safe_bool("active");
    ea.max_outbound_per_hour = j.value("max_outbound_emails_per_hour", 0);
    ea.true_warmup_daily_limit = j.value("true_warmup_daily_limit", 0);
}
