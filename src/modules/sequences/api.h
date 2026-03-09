#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "../../core/types.h"
#include "model.h"

namespace sequences_api {

// Search sequences with filtering and pagination
// POST /api/v1/emailer_campaigns/search
OffsetPage<Sequence> search_sequences(const nlohmann::json& body);

// Get full sequence details including steps
// GET /api/v1/sequences/get?sequence_id=<id>
Sequence get_sequence(const std::string& id);

// Add contacts to a sequence
// POST /api/v1/emailer_campaigns/{sequence_id}/add_contact_ids
nlohmann::json add_contacts(
    const std::string& sequence_id,
    const std::vector<std::string>& contact_ids,
    const std::string& email_account_id,
    const std::optional<std::string>& user_id = std::nullopt);

}  // namespace sequences_api
