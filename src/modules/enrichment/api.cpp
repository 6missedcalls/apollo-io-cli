#include "modules/enrichment/api.h"

#include <string>
#include <utility>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace enrichment_api {

PersonMatch people_match(const json& body) {
    // Apollo people/match takes params as query string on a POST request
    std::vector<std::pair<std::string, std::string>> query_params;

    auto add_param = [&](const char* key) {
        if (body.contains(key) && !body[key].is_null() && body[key].is_string()) {
            std::string val = body[key].get<std::string>();
            if (!val.empty()) {
                query_params.emplace_back(key, val);
            }
        }
    };

    add_param("first_name");
    add_param("last_name");
    add_param("name");
    add_param("email");
    add_param("organization_name");
    add_param("domain");
    add_param("linkedin_url");
    add_param("id");

    if (query_params.empty()) {
        throw ApolloError(ErrorKind::Validation, "At least one identifying parameter is required for people match");
    }

    // Apollo people/match is POST with query string params (unusual but per spec)
    std::string path = "people/match?";
    for (size_t i = 0; i < query_params.size(); ++i) {
        if (i > 0) path += "&";
        path += query_params[i].first + "=" + query_params[i].second;
    }

    auto response = rest_post(path, json::object());

    if (!response.contains("person") || response["person"].is_null()) {
        throw ApolloError(ErrorKind::NotFound, "No matching person found");
    }

    PersonMatch match;
    from_json(response["person"], match);
    return match;
}

OrgEnrichment org_enrich(const std::string& domain) {
    if (domain.empty()) {
        throw ApolloError(ErrorKind::Validation, "Domain is required for organization enrichment");
    }

    auto response = rest_get("organizations/enrich", {{"domain", domain}});

    if (!response.contains("organization") || response["organization"].is_null()) {
        throw ApolloError(ErrorKind::NotFound, "No matching organization found for domain: " + domain);
    }

    OrgEnrichment org;
    from_json(response["organization"], org);
    return org;
}

}  // namespace enrichment_api
