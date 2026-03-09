#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

struct OutreachEmail {
    std::string id;
    std::optional<std::string> subject;
    std::optional<std::string> status;  // sent, opened, clicked, replied, bounced, etc.
    std::optional<std::string> contact_id;
    std::optional<std::string> emailer_campaign_id;
    std::optional<std::string> sent_at;
    std::optional<std::string> opened_at;
    std::optional<std::string> replied_at;
    std::optional<std::string> clicked_at;
};

inline void from_json(const nlohmann::json& j, OutreachEmail& e) {
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

    e.id = safe_str("id");
    e.subject = opt_str("subject");
    e.status = opt_str("status");
    e.contact_id = opt_str("contact_id");
    e.emailer_campaign_id = opt_str("emailer_campaign_id");
    e.sent_at = opt_str("sent_at");
    e.opened_at = opt_str("opened_at");
    e.replied_at = opt_str("replied_at");
    e.clicked_at = opt_str("clicked_at");
}
