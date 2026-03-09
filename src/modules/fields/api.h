#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "model.h"

namespace fields_api {

// List all fields (system + custom)
// GET /api/v1/fields
std::vector<CustomField> list_fields();

// Create a new custom field
// POST /api/v1/fields
CustomField create_field(const nlohmann::json& body);

}  // namespace fields_api
