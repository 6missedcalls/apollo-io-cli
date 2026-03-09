#include "modules/tasks/api.h"

#include <string>
#include <utility>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace tasks_api {

OffsetPage<Task> search_tasks(const json& search_body) {
    // Tasks search uses POST method with query parameters.
    // rest_post doesn't accept query params, so encode them into the path.
    std::string path = "tasks/search";
    std::string query;

    auto append_param = [&](const std::string& key, const std::string& value) {
        query += query.empty() ? "?" : "&";
        query += key + "=" + value;
    };

    if (search_body.contains("sort_by_field") && !search_body["sort_by_field"].is_null()) {
        append_param("sort_by_field", search_body["sort_by_field"].get<std::string>());
    }
    if (search_body.contains("page")) {
        append_param("page", std::to_string(search_body["page"].get<int>()));
    }
    if (search_body.contains("per_page")) {
        append_param("per_page", std::to_string(search_body["per_page"].get<int>()));
    }
    if (search_body.contains("open_factor_names") && search_body["open_factor_names"].is_array()) {
        for (const auto& name : search_body["open_factor_names"]) {
            append_param("open_factor_names[]", name.get<std::string>());
        }
    }

    auto response = rest_post(path + query, json::object());

    OffsetPage<Task> result;

    // Unwrap tasks array
    if (response.contains("tasks") && response["tasks"].is_array()) {
        for (const auto& item : response["tasks"]) {
            Task task;
            from_json(item, task);
            result.items.push_back(std::move(task));
        }
    }

    // Unwrap pagination
    if (response.contains("pagination") && !response["pagination"].is_null()) {
        from_json(response["pagination"], result.page_info);
    }

    return result;
}

Task create_task(const TaskCreateInput& input) {
    auto body = to_json_body(input);
    auto response = rest_post("tasks", body);

    if (!response.contains("task") || response["task"].is_null()) {
        throw ApolloError(ErrorKind::Api, "Failed to create task: unexpected response format");
    }

    Task task;
    from_json(response["task"], task);
    return task;
}

json bulk_create(const json& tasks_array) {
    auto response = rest_post("tasks/bulk_create", tasks_array);
    return response;
}

}  // namespace tasks_api
