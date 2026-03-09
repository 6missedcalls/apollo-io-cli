#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// SequenceStep
// ---------------------------------------------------------------------------

struct SequenceStep {
    std::string id;
    std::string type;       // auto_email, manual_email, call, action_item, linkedin_step_message, etc.
    int position = 0;
    int wait_time = 0;
    std::string wait_mode;  // minute, hour, day
    std::optional<std::string> subject;
    std::optional<std::string> body_text;
};

inline void from_json(const json& j, SequenceStep& s) {
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

    s.id = safe_str("id");
    s.type = safe_str("type");
    s.position = j.value("position", 0);
    s.wait_time = j.value("wait_time", 0);
    s.wait_mode = safe_str("wait_mode");
    s.subject = opt_str("subject");
    s.body_text = opt_str("body_text");
}

// ---------------------------------------------------------------------------
// Sequence
// ---------------------------------------------------------------------------

struct Sequence {
    std::string id;
    std::string name;
    bool active = false;
    int num_steps = 0;
    std::optional<std::string> user_id;
    std::string created_at;

    // Statistics
    int unique_delivered = 0;
    int unique_opened = 0;
    int unique_replied = 0;
    int unique_bounced = 0;
    double open_rate = 0;
    double reply_rate = 0;
    double bounce_rate = 0;

    // Detail-only
    std::vector<SequenceStep> steps;
};

inline void from_json(const json& j, Sequence& s) {
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

    s.id = safe_str("id");
    s.name = safe_str("name");
    s.active = safe_bool("active");
    s.num_steps = j.value("num_steps", 0);
    s.user_id = opt_str("user_id");
    s.created_at = safe_str("created_at");

    s.unique_delivered = j.value("unique_delivered", 0);
    s.unique_opened = j.value("unique_opened", 0);
    s.unique_replied = j.value("unique_replied", 0);
    s.unique_bounced = j.value("unique_bounced", 0);
    s.open_rate = j.value("open_rate", 0.0);
    s.reply_rate = j.value("reply_rate", 0.0);
    s.bounce_rate = j.value("bounce_rate", 0.0);
}
