#include "modules/stages/commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "modules/stages/api.h"
#include "modules/stages/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_stage_table(const std::vector<Stage>& stages) {
    auto stage_label = [](const Stage& s) -> const std::string& {
        return s.display_name.empty() ? s.name : s.display_name;
    };

    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "NAME", "ORDER", "CATEGORY"});
        for (const auto& stage : stages) {
            output_csv_row({
                stage.id,
                stage_label(stage),
                std::to_string(stage.display_order),
                stage.category.value_or("")
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",       12, 24, false},
        {"NAME",      8, 25, false},
        {"ORDER",     4,  6, true},
        {"CATEGORY",  6, 15, false}
    });

    for (const auto& stage : stages) {
        std::string category = stage.category.value_or("");
        std::string category_colored = category;

        if (category == "succeeded") {
            category_colored = color::green(category);
        } else if (category == "failed") {
            category_colored = color::red(category);
        } else if (category == "in_progress") {
            category_colored = color::yellow(category);
        } else if (!category.empty()) {
            category_colored = color::gray(category);
        }

        table.add_row({
            stage.id,
            stage_label(stage),
            std::to_string(stage.display_order),
            category_colored
        });
    }

    if (table.empty()) {
        print_warning("No stages found.");
        return;
    }

    table.render(std::cout);
}

void render_stage_json(const std::vector<Stage>& stages) {
    json arr = json::array();
    for (const auto& stage : stages) {
        json j;
        j["id"] = stage.id;
        j["team_id"] = stage.team_id;
        j["display_name"] = stage.display_name;
        j["name"] = stage.name;
        j["display_order"] = stage.display_order;
        j["category"] = stage.category.has_value() ? json(stage.category.value()) : json(nullptr);
        j["default_exclude_for_leadgen"] = stage.default_exclude_for_leadgen;

        if (stage.is_meeting_set.has_value()) {
            j["is_meeting_set"] = stage.is_meeting_set.value();
        }
        if (stage.is_won.has_value()) {
            j["is_won"] = stage.is_won.value();
        }
        if (stage.is_closed.has_value()) {
            j["is_closed"] = stage.is_closed.value();
        }
        if (stage.probability.has_value()) {
            j["probability"] = stage.probability.value();
        }

        arr.push_back(j);
    }
    output_json(arr);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void stages_commands::register_commands(CLI::App& app) {
    auto* stages = app.add_subcommand("stages", "View pipeline stages");
    stages->require_subcommand(1);

    // -------------------------------------------------------------------
    // stages contacts
    // -------------------------------------------------------------------
    {
        auto* cmd = stages->add_subcommand("contacts", "List contact stages");

        cmd->callback([]() {
            try {
                auto stage_list = stages_api::list_contact_stages();

                if (get_output_format() == OutputFormat::Json) {
                    render_stage_json(stage_list);
                } else {
                    render_stage_table(stage_list);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }

    // -------------------------------------------------------------------
    // stages accounts
    // -------------------------------------------------------------------
    {
        auto* cmd = stages->add_subcommand("accounts", "List account stages");

        cmd->callback([]() {
            try {
                auto stage_list = stages_api::list_account_stages();

                if (get_output_format() == OutputFormat::Json) {
                    render_stage_json(stage_list);
                } else {
                    render_stage_table(stage_list);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }

    // -------------------------------------------------------------------
    // stages deals
    // -------------------------------------------------------------------
    {
        auto* cmd = stages->add_subcommand("deals", "List deal/opportunity stages");

        cmd->callback([]() {
            try {
                auto stage_list = stages_api::list_deal_stages();

                if (get_output_format() == OutputFormat::Json) {
                    render_stage_json(stage_list);
                } else {
                    render_stage_table(stage_list);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }
}
