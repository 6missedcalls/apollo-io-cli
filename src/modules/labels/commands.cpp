#include "modules/labels/commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "modules/labels/api.h"
#include "modules/labels/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_label_table(const std::vector<Label>& labels) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "NAME", "MODALITY", "COUNT"});
        for (const auto& label : labels) {
            output_csv_row({
                label.id,
                label.name,
                label.modality,
                std::to_string(label.cached_count)
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",       4, 24, false},
        {"NAME",     4, 25, false},
        {"MODALITY", 4, 18, false},
        {"COUNT",    4,  8, true}
    });

    for (const auto& label : labels) {
        table.add_row({
            label.id,
            label.name,
            label.modality,
            std::to_string(label.cached_count)
        });
    }

    if (table.empty()) {
        print_warning("No labels found.");
        return;
    }

    table.render(std::cout);
}

void render_label_json(const std::vector<Label>& labels) {
    json arr = json::array();
    for (const auto& label : labels) {
        json j;
        j["id"] = label.id;
        j["name"] = label.name;
        j["modality"] = label.modality;
        j["cached_count"] = label.cached_count;
        j["created_at"] = label.created_at;
        j["updated_at"] = label.updated_at;
        arr.push_back(j);
    }
    output_json(arr);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void labels_commands::register_commands(CLI::App& app) {
    auto* labels = app.add_subcommand("labels", "Manage labels (lists)");
    labels->require_subcommand(1);

    // -----------------------------------------------------------------------
    // labels list
    // -----------------------------------------------------------------------
    {
        auto* cmd = labels->add_subcommand("list", "List labels");

        auto modality = std::make_shared<std::string>();
        cmd->add_option("--modality", *modality,
            "Filter by modality (contacts, accounts, emailer_campaigns)");

        cmd->callback([modality]() {
            try {
                auto labels = labels_api::list_labels(*modality);

                if (get_output_format() == OutputFormat::Json) {
                    render_label_json(labels);
                } else {
                    render_label_table(labels);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }
}
