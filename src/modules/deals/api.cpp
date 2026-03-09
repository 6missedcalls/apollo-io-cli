#include "modules/deals/api.h"

#include <string>
#include <utility>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace deals_api {

OffsetPage<Deal> search_deals(
    const std::string& sort_by_field,
    int page,
    int per_page
) {
    std::vector<std::pair<std::string, std::string>> params;

    if (!sort_by_field.empty()) {
        params.emplace_back("sort_by_field", sort_by_field);
    }
    params.emplace_back("page", std::to_string(page));
    params.emplace_back("per_page", std::to_string(per_page));

    auto response = rest_get("opportunities/search", params);

    OffsetPage<Deal> result;

    // Unwrap opportunities array
    if (response.contains("opportunities") && response["opportunities"].is_array()) {
        for (const auto& item : response["opportunities"]) {
            Deal deal;
            from_json(item, deal);
            result.items.push_back(std::move(deal));
        }
    }

    // Unwrap pagination
    if (response.contains("pagination") && !response["pagination"].is_null()) {
        from_json(response["pagination"], result.page_info);
    }

    return result;
}

Deal get_deal(const std::string& id) {
    auto response = rest_get("opportunities/" + id);

    if (!response.contains("opportunity") || response["opportunity"].is_null()) {
        throw ApolloError(ErrorKind::NotFound, "Deal not found: " + id);
    }

    Deal deal;
    from_json(response["opportunity"], deal);
    return deal;
}

Deal create_deal(const DealCreateInput& input) {
    auto body = to_json_body(input);
    auto response = rest_post("opportunities", body);

    if (!response.contains("opportunity") || response["opportunity"].is_null()) {
        throw ApolloError(ErrorKind::Api, "Failed to create deal: unexpected response format");
    }

    Deal deal;
    from_json(response["opportunity"], deal);
    return deal;
}

Deal update_deal(const std::string& id, const DealUpdateInput& input) {
    auto body = to_json_body(input);
    auto response = rest_patch("opportunities/" + id, body);

    if (!response.contains("opportunity") || response["opportunity"].is_null()) {
        throw ApolloError(ErrorKind::Api, "Failed to update deal: unexpected response format");
    }

    Deal deal;
    from_json(response["opportunity"], deal);
    return deal;
}

}  // namespace deals_api
