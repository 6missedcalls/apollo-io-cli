#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "core/types.h"
#include "modules/tasks/model.h"

namespace tasks_api {

// POST /api/v1/tasks/search (query params for sort/page, POST method)
OffsetPage<Task> search_tasks(const nlohmann::json& search_body);

// POST /api/v1/tasks
Task create_task(const TaskCreateInput& input);

// POST /api/v1/tasks/bulk_create
nlohmann::json bulk_create(const nlohmann::json& tasks_array);

// NOTE: No update_task -- Apollo has NO Update Task endpoint

}  // namespace tasks_api
