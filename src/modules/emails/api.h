#pragma once

#include <nlohmann/json.hpp>

#include "core/types.h"
#include "model.h"

namespace emails_api {

// Search outreach emails with filtering and pagination
// POST /api/v1/emailer_messages/search
OffsetPage<OutreachEmail> search_emails(const nlohmann::json& body);

}  // namespace emails_api
