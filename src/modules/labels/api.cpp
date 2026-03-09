#include "modules/labels/api.h"

#include <string>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace labels_api {

std::vector<Label> list_labels(const std::string& modality) {
    json response;

    if (!modality.empty()) {
        // Use the filtered endpoint
        response = rest_get("labels", {{"modality", modality}});
    } else {
        response = rest_get("labels");
    }

    std::vector<Label> labels;

    // Response is a bare JSON array (not wrapped in an object)
    if (response.is_array()) {
        for (const auto& item : response) {
            Label label;
            from_json(item, label);

            // Apply client-side modality filter if needed
            if (!modality.empty() && label.modality != modality) {
                continue;
            }

            labels.push_back(label);
        }
    }

    return labels;
}

}  // namespace labels_api
