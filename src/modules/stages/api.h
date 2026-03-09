#pragma once

#include <vector>

#include "modules/stages/model.h"

namespace stages_api {

// GET /api/v1/contact_stages
std::vector<Stage> list_contact_stages();

// GET /api/v1/account_stages
std::vector<Stage> list_account_stages();

// GET /api/v1/opportunity_stages
std::vector<Stage> list_deal_stages();

}  // namespace stages_api
