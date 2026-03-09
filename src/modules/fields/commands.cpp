#include "modules/fields/commands.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "modules/fields/api.h"
#include "modules/fields/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_field_table(const std::vector<CustomField>& fields) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "LABEL", "TYPE", "MODALITY", "SYSTEM"});
        for (const auto& field : fields) {
            output_csv_row({
                field.id,
                field.label,
                field.field_type,
                field.modality.value_or(""),
                field.system_field ? "yes" : "no"
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",       4, 24, false},
        {"LABEL",    4, 25, false},
        {"TYPE",     4, 12, false},
        {"MODALITY", 4, 12, false},
        {"SYSTEM",   4,  6, false}
    });

    for (const auto& field : fields) {
        std::string system_str = field.system_field ? color::gray("yes") : "no";

        table.add_row({
            field.id,
            field.label,
            field.field_type,
            field.modality.value_or(""),
            system_str
        });
    }

    if (table.empty()) {
        print_warning("No fields found.");
        return;
    }

    table.render(std::cout);
}

void render_field_json(const std::vector<CustomField>& fields) {
    json arr = json::array();
    for (const auto& field : fields) {
        json j;
        j["id"] = field.id;
        j["name"] = field.name;
        j["label"] = field.label;
        j["field_type"] = field.field_type;
        j["modality"] = field.modality.value_or("");
        j["system_field"] = field.system_field;
        if (!field.picklist_values.empty()) {
            j["picklist_values"] = field.picklist_values;
        }
        arr.push_back(j);
    }
    output_json(arr);
}

void render_field_detail(const CustomField& field) {
    if (get_output_format() == OutputFormat::Json) {
        json j;
        j["id"] = field.id;
        j["name"] = field.name;
        j["label"] = field.label;
        j["field_type"] = field.field_type;
        j["modality"] = field.modality.value_or("");
        j["system_field"] = field.system_field;
        if (!field.picklist_values.empty()) {
            j["picklist_values"] = field.picklist_values;
        }
        output_json(j);
        return;
    }

    DetailRenderer detail;
    detail.add_section(field.label);

    detail.add_field("Name", field.name);
    detail.add_field("Type", field.field_type);
    detail.add_field("Modality", field.modality.value_or(""));
    detail.add_field("System", field.system_field ? "yes" : "no");

    if (!field.picklist_values.empty()) {
        detail.add_blank_line();
        detail.add_section("Picklist Values");
        for (const auto& val : field.picklist_values) {
            detail.add_field("", val);
        }
    }

    detail.add_blank_line();
    detail.add_field("ID", field.id);

    detail.render(std::cout);
}

std::vector<std::string> split_comma(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream ss(input);
    std::string item;
    while (std::getline(ss, item, ',')) {
        size_t start = item.find_first_not_of(' ');
        size_t end = item.find_last_not_of(' ');
        if (start != std::string::npos) {
            result.push_back(item.substr(start, end - start + 1));
        }
    }
    return result;
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void fields_commands::register_commands(CLI::App& app) {
    auto* fields = app.add_subcommand("fields", "Manage custom fields");
    fields->require_subcommand(1);

    // -----------------------------------------------------------------------
    // fields list
    // -----------------------------------------------------------------------
    {
        auto* cmd = fields->add_subcommand("list", "List all fields");

        auto modality = std::make_shared<std::string>();
        cmd->add_option("--modality", *modality,
            "Filter by modality (contact, account, opportunity)");

        cmd->callback([modality]() {
            try {
                auto fields = fields_api::list_fields();

                // Client-side modality filter
                if (!modality->empty()) {
                    std::vector<CustomField> filtered;
                    for (const auto& field : fields) {
                        if (field.modality.has_value() && field.modality.value() == *modality) {
                            filtered.push_back(field);
                        }
                    }
                    fields = filtered;
                }

                if (get_output_format() == OutputFormat::Json) {
                    render_field_json(fields);
                } else {
                    render_field_table(fields);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }

    // -----------------------------------------------------------------------
    // fields create
    // -----------------------------------------------------------------------
    {
        auto* cmd = fields->add_subcommand("create", "Create a custom field");

        struct CreateOpts {
            std::string label;
            std::string type;
            std::string modality;
            std::string picklist_values;
        };
        auto opts = std::make_shared<CreateOpts>();

        cmd->add_option("--label", opts->label, "Display label for the field")->required();
        cmd->add_option("--type", opts->type,
            "Field type (text, number, date, datetime, dropdown, multi_select, boolean, etc.)")->required();
        cmd->add_option("--modality", opts->modality,
            "Entity type (contact, account, opportunity)")->required();
        cmd->add_option("--picklist-values", opts->picklist_values,
            "Comma-separated picklist values (for dropdown/multi_select)");

        cmd->callback([opts]() {
            try {
                json body = json::object();
                body["label"] = opts->label;
                body["field_type"] = opts->type;
                body["modality"] = opts->modality;

                if (!opts->picklist_values.empty()) {
                    body["picklist_values"] = split_comma(opts->picklist_values);
                }

                auto field = fields_api::create_field(body);
                print_success("Created field \"" + field.label + "\"");
                render_field_detail(field);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }
}
