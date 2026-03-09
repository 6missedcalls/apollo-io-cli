#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "model.h"

namespace enrichment_api {

// People enrichment/match
// POST /api/v1/people/match (params as query string)
PersonMatch people_match(const nlohmann::json& body);

// Organization enrichment
// GET /api/v1/organizations/enrich?domain=<domain>
OrgEnrichment org_enrich(const std::string& domain);

}  // namespace enrichment_api
