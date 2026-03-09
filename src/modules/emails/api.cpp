#include "modules/emails/api.h"

#include <string>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace emails_api {

OffsetPage<OutreachEmail> search_emails(const json& body) {
    auto response = rest_post("emailer_messages/search", body);

    OffsetPage<OutreachEmail> result;

    // Parse emailer_messages array
    if (response.contains("emailer_messages") && response["emailer_messages"].is_array()) {
        for (const auto& item : response["emailer_messages"]) {
            OutreachEmail email;
            from_json(item, email);
            result.items.push_back(email);
        }
    }

    // Parse pagination metadata
    if (response.contains("pagination") && !response["pagination"].is_null()) {
        from_json(response["pagination"], result.page_info);
    }

    return result;
}

}  // namespace emails_api
