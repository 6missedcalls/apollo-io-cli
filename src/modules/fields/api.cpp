#include "modules/fields/api.h"

#include <string>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace fields_api {

std::vector<CustomField> list_fields() {
    auto response = rest_get("fields");

    std::vector<CustomField> fields;

    if (response.contains("fields") && response["fields"].is_array()) {
        for (const auto& item : response["fields"]) {
            CustomField field;
            from_json(item, field);
            fields.push_back(field);
        }
    }

    return fields;
}

CustomField create_field(const json& body) {
    auto response = rest_post("fields", body);

    CustomField field;
    from_json(response, field);
    return field;
}

}  // namespace fields_api
