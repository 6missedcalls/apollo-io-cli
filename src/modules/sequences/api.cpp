#include "modules/sequences/api.h"

#include <map>
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
    auto response = rest_get("sequences/get", {{"sequence_id", id}});

    Sequence seq;

    // Parse the emailer_campaign object
    if (response.contains("emailer_campaign") && !response["emailer_campaign"].is_null()) {
        from_json(response["emailer_campaign"], seq);
    }

    // Parse steps from emailer_steps array
    if (response.contains("emailer_steps") && response["emailer_steps"].is_array()) {
        // Build a map of step_id -> template for subject/body lookup
        std::map<std::string, json> step_templates;
        if (response.contains("emailer_touches") && response["emailer_touches"].is_array() &&
            response.contains("emailer_templates") && response["emailer_templates"].is_array()) {

            // Map touch step_id -> template_id
            std::map<std::string, std::string> touch_to_template;
            for (const auto& touch : response["emailer_touches"]) {
                auto step_id = touch.value("emailer_step_id", "");
                auto template_id = touch.value("emailer_template_id", "");
                if (!step_id.empty() && !template_id.empty()) {
                    touch_to_template[step_id] = template_id;
                }
            }

            // Map template_id -> template object
            std::map<std::string, json> templates_by_id;
            for (const auto& tmpl : response["emailer_templates"]) {
                auto tmpl_id = tmpl.value("id", "");
                if (!tmpl_id.empty()) {
                    templates_by_id[tmpl_id] = tmpl;
                }
            }

            // Combine: step_id -> template
            for (const auto& [step_id, template_id] : touch_to_template) {
                auto it = templates_by_id.find(template_id);
                if (it != templates_by_id.end()) {
                    step_templates[step_id] = it->second;
                }
            }
        }

        for (const auto& step_json : response["emailer_steps"]) {
            SequenceStep step;
            from_json(step_json, step);

            // Attach template subject/body if available
            auto tmpl_it = step_templates.find(step.id);
            if (tmpl_it != step_templates.end()) {
                const auto& tmpl = tmpl_it->second;
                if (tmpl.contains("subject") && !tmpl["subject"].is_null()) {
                    step.subject = tmpl["subject"].get<std::string>();
                }
                if (tmpl.contains("body_text") && !tmpl["body_text"].is_null()) {
                    step.body_text = tmpl["body_text"].get<std::string>();
                }
            }

            seq.steps.push_back(step);
        }
    }

    return seq;
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
