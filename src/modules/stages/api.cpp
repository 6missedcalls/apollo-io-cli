#include "modules/stages/api.h"

#include <string>
#include <utility>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace {

std::vector<Stage> parse_stages(const json& response, const std::string& key) {
    std::vector<Stage> stages;

    if (response.contains(key) && response[key].is_array()) {
        for (const auto& item : response[key]) {
            Stage stage;
            from_json(item, stage);
            stages.push_back(std::move(stage));
        }
    }

    return stages;
}

}  // namespace

namespace stages_api {

std::vector<Stage> list_contact_stages() {
    auto response = rest_get("contact_stages");
    return parse_stages(response, "contact_stages");
}

std::vector<Stage> list_account_stages() {
    auto response = rest_get("account_stages");
    return parse_stages(response, "account_stages");
}

std::vector<Stage> list_deal_stages() {
    auto response = rest_get("opportunity_stages");
    return parse_stages(response, "opportunity_stages");
}

}  // namespace stages_api
