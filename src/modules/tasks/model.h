#pragma once

#include <optional>
#include <string>

#include <nlohmann/json.hpp>

struct Task {
    std::string id;
    std::optional<std::string> user_id;
    std::optional<std::string> contact_id;
    std::string type;       // call, outreach_manual_email, linkedin_step_message, etc.
    std::string priority;   // high, medium, low
    std::string status;     // scheduled, completed, skipped
    std::optional<std::string> due_at;
    std::optional<std::string> title;
    std::optional<std::string> note;
    std::string created_at;
    std::string updated_at;
};

inline void from_json(const nlohmann::json& j, Task& t) {
    auto safe_str = [&](const char* key, const std::string& def = "") -> std::string {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return def;
    };
    auto opt_str = [&](const char* key) -> std::optional<std::string> {
        if (j.contains(key) && !j[key].is_null()) return j[key].get<std::string>();
        return std::nullopt;
    };

    t.id = safe_str("id");
    t.user_id = opt_str("user_id");
    t.contact_id = opt_str("contact_id");
    t.type = safe_str("type");
    t.priority = safe_str("priority", "medium");
    t.status = safe_str("status");
    t.due_at = opt_str("due_at");
    t.title = opt_str("title");
    t.note = opt_str("note");
    t.created_at = safe_str("created_at");
    t.updated_at = safe_str("updated_at");
}

struct TaskCreateInput {
    std::string user_id;     // required
    std::string contact_id;  // required
    std::string type;        // required
    std::string status;      // required
    std::string due_at;      // required (ISO 8601)
    std::optional<std::string> priority;  // defaults to "medium"
    std::optional<std::string> title;
    std::optional<std::string> note;
};

inline nlohmann::json to_json_body(const TaskCreateInput& input) {
    nlohmann::json body = nlohmann::json::object();

    body["user_id"] = input.user_id;
    body["contact_id"] = input.contact_id;
    body["type"] = input.type;
    body["status"] = input.status;
    body["due_at"] = input.due_at;

    if (input.priority.has_value()) {
        body["priority"] = input.priority.value();
    }
    if (input.title.has_value()) {
        body["title"] = input.title.value();
    }
    if (input.note.has_value()) {
        body["note"] = input.note.value();
    }

    return body;
}
