#pragma once

#include <string>

#include "core/types.h"
#include "modules/deals/model.h"

namespace deals_api {

// GET /api/v1/opportunities/search
OffsetPage<Deal> search_deals(
    const std::string& sort_by_field = "",
    int page = 1,
    int per_page = 25);

// GET /api/v1/opportunities/{id}
Deal get_deal(const std::string& id);

// POST /api/v1/opportunities
Deal create_deal(const DealCreateInput& input);

// PATCH /api/v1/opportunities/{id}  (NOT PUT!)
Deal update_deal(const std::string& id, const DealUpdateInput& input);

}  // namespace deals_api
