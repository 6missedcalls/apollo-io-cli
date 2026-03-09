#include "modules/users/api.h"

#include <string>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace users_api {

std::vector<ApolloUser> list_users() {
    std::vector<ApolloUser> all_users;
    int page = 1;

    while (true) {
        auto response = rest_get("users/search", {
            {"page", std::to_string(page)},
            {"per_page", "100"}
        });

        if (response.contains("users") && response["users"].is_array()) {
            for (const auto& item : response["users"]) {
                ApolloUser user;
                from_json(item, user);
                all_users.push_back(user);
            }
        }

        // Check pagination
        if (response.contains("pagination") && !response["pagination"].is_null()) {
            int total_pages = response["pagination"].value("total_pages", 1);
            if (page >= total_pages) {
                break;
            }
        } else {
            break;
        }

        ++page;
    }

    return all_users;
}

}  // namespace users_api
