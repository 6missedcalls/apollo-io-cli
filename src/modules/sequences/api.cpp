#include "modules/sequences/api.h"

#include <string>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace sequences_api {

OffsetPage<Sequence> search_sequences(const json& body) {
    auto response = rest_post("emailer_campaigns/search", body);

    OffsetPage<Sequence> result;

    // Parse emailer_campaigns array
    if (response.contains("emailer_campaigns") && response["emailer_campaigns"].is_array()) {
        for (const auto& item : response["emailer_campaigns"]) {
            Sequence seq;
            from_json(item, seq);
            result.items.push_back(seq);
        }
    }

    // Parse pagination metadata
    if (response.contains("pagination") && !response["pagination"].is_null()) {
        from_json(response["pagination"], result.page_info);
    }

    return result;
}

Sequence get_sequence(const std::string& id) {
    // The GET /api/v1/sequences/get endpoint is non-functional (returns 404).
    // Workaround: use the search endpoint to find the sequence by ID.
    json body = json::object();
    body["per_page"] = 100;

    int page = 1;
    while (true) {
        body["page"] = page;
        auto response = rest_post("emailer_campaigns/search", body);

        if (response.contains("emailer_campaigns") && response["emailer_campaigns"].is_array()) {
            for (const auto& item : response["emailer_campaigns"]) {
                if (item.contains("id") && item["id"].is_string() && item["id"].get<std::string>() == id) {
                    Sequence seq;
                    from_json(item, seq);
                    // Note: search endpoint does not return step/touch/template details
                    return seq;
                }
            }

            // Check if there are more pages
            if (response.contains("pagination") && !response["pagination"].is_null()) {
                auto total_pages = response["pagination"].value("total_pages", 1);
                if (page >= total_pages) break;
                page++;
            } else {
                break;
            }
        } else {
            break;
        }
    }

    throw ApolloError(ErrorKind::NotFound, "Sequence not found: " + id);
}

json add_contacts(
    const std::string& sequence_id,
    const std::vector<std::string>& contact_ids,
    const std::string& email_account_id,
    const std::optional<std::string>& user_id
) {
    json body = json::object();
    body["contact_ids"] = contact_ids;
    body["emailer_campaign_id"] = sequence_id;
    body["send_email_from_email_account_id"] = email_account_id;

    if (user_id.has_value()) {
        body["user_id"] = user_id.value();
    }

    return rest_post("emailer_campaigns/" + sequence_id + "/add_contact_ids", body);
}

}  // namespace sequences_api
