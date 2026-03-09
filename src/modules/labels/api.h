#pragma once

#include <string>
#include <vector>

#include "model.h"

namespace labels_api {

// Get all labels, optionally filtered by modality
// GET /api/v1/labels
std::vector<Label> list_labels(const std::string& modality = "");

}  // namespace labels_api
