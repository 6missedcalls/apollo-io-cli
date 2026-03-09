#include "api.h"

#include <string>
#include <utility>
#include <vector>

#include "../../core/error.h"
#include "../../core/rest_client.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

// Extract a Contact from a "person" wrapper that Apollo returns for GET/PUT.
// The person object contains enrichment fields at the top level and a nested
// "contact" object with the core contact fields.  We merge both into one
// Contact struct so callers get the richest possible view.
Contact parse_person_wrapper(const json& person) {
    Contact contact;

    // Start from the nested "contact" object if it exists
    if (person.contains("contact") && !person["contact"].is_null()) {
        from_json(person["contact"], contact);
    } else {
        // Fallback: the person object itself contains the contact fields
        from_json(person, contact);
    }

    // Overlay enrichment fields from the person level
    auto opt_str = [&](const json& j, const char* key) -> std::optional<std::string> {
        if (j.contains(key) && !j[key].is_null()) {
            return j[key].get<std::string>();
        }
        return std::nullopt;
    };

    auto enrich = [&](const json& p) {
        if (!contact.photo_url.has_value()) contact.photo_url = opt_str(p, "photo_url");
        if (!contact.twitter_url.has_value()) contact.twitter_url = opt_str(p, "twitter_url");
        if (!contact.facebook_url.has_value()) contact.facebook_url = opt_str(p, "facebook_url");
        if (!contact.github_url.has_value()) contact.github_url = opt_str(p, "github_url");
        if (!contact.headline.has_value()) contact.headline = opt_str(p, "headline");
        if (!contact.seniority.has_value()) contact.seniority = opt_str(p, "seniority");

        // Overlay location fields from person if not already set from contact
        if (!contact.city.has_value()) contact.city = opt_str(p, "city");
        if (!contact.state.has_value()) contact.state = opt_str(p, "state");
        if (!contact.country.has_value()) contact.country = opt_str(p, "country");

        // Departments
        if (contact.departments.empty() && p.contains("departments") && p["departments"].is_array()) {
            for (const auto& dep : p["departments"]) {
                if (dep.is_string()) {
                    contact.departments.push_back(dep.get<std::string>());
                }
            }
        }
    };

    enrich(person);

    return contact;
}

}  // namespace

// ---------------------------------------------------------------------------
// API implementations
// ---------------------------------------------------------------------------

namespace contacts_api {

OffsetPage<Contact> search_contacts(const json& search_body) {
    auto response = rest_post("contacts/search", search_body);

    OffsetPage<Contact> result;

    // Parse contacts array
    if (response.contains("contacts") && response["contacts"].is_array()) {
        for (const auto& item : response["contacts"]) {
            Contact contact;
            from_json(item, contact);
            result.items.push_back(std::move(contact));
        }
    }

    // Parse pagination
    if (response.contains("pagination") && !response["pagination"].is_null()) {
        from_json(response["pagination"], result.page_info);
    }

    return result;
}

Contact get_contact(const std::string& id) {
    auto response = rest_get("contacts/" + id);

    // Response wraps in "person" object
    if (response.contains("person") && !response["person"].is_null()) {
        return parse_person_wrapper(response["person"]);
    }

    // Fallback: try direct contact parsing
    if (response.contains("contact") && !response["contact"].is_null()) {
        Contact contact;
        from_json(response["contact"], contact);
        return contact;
    }

    throw ApolloError(ErrorKind::Api, "Unexpected response format for GET contact");
}

Contact create_contact(const ContactCreateInput& input) {
    json body = to_json_body(input);
    auto response = rest_post("contacts", body);

    // Response wraps in "contact"
    if (response.contains("contact") && !response["contact"].is_null()) {
        Contact contact;
        from_json(response["contact"], contact);
        return contact;
    }

    throw ApolloError(ErrorKind::Api, "Unexpected response format for create contact");
}

Contact update_contact(const std::string& id, const ContactUpdateInput& input) {
    json body = to_json_body(input);
    auto response = rest_put("contacts/" + id, body);

    // Update may wrap in "person" (with nested "contact") or directly in "contact"
    if (response.contains("person") && !response["person"].is_null()) {
        return parse_person_wrapper(response["person"]);
    }

    if (response.contains("contact") && !response["contact"].is_null()) {
        Contact contact;
        from_json(response["contact"], contact);
        return contact;
    }

    throw ApolloError(ErrorKind::Api, "Unexpected response format for update contact");
}

void delete_contact(const std::string& id) {
    rest_delete("contacts/" + id);
}

std::vector<Contact> bulk_create(const std::vector<ContactCreateInput>& inputs) {
    json body = json::object();
    json contacts_arr = json::array();

    for (const auto& input : inputs) {
        contacts_arr.push_back(to_json_body(input));
    }

    body["contacts"] = contacts_arr;

    auto response = rest_post("contacts/bulk_create", body);

    std::vector<Contact> result;
    if (response.contains("contacts") && response["contacts"].is_array()) {
        for (const auto& item : response["contacts"]) {
            Contact contact;
            from_json(item, contact);
            result.push_back(std::move(contact));
        }
    }

    return result;
}

json bulk_update(const json& contacts_array) {
    json body = json::object();
    body["contacts"] = contacts_array;

    return rest_post("contacts/bulk_update", body);
}

}  // namespace contacts_api
