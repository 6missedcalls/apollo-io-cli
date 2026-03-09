#include "modules/users/commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "modules/users/api.h"
#include "modules/users/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_user_table(const std::vector<ApolloUser>& users) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "NAME", "EMAIL", "ROLE"});
        for (const auto& user : users) {
            output_csv_row({
                user.id,
                user.name,
                user.email,
                user.role.value_or("")
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",    4, 24, false},
        {"NAME",  4, 25, false},
        {"EMAIL", 4, 30, false},
        {"ROLE",  4, 10, false}
    });

    for (const auto& user : users) {
        table.add_row({
            user.id,
            user.name,
            user.email,
            user.role.value_or("")
        });
    }

    if (table.empty()) {
        print_warning("No users found.");
        return;
    }

    table.render(std::cout);
}

void render_user_json(const std::vector<ApolloUser>& users) {
    json arr = json::array();
    for (const auto& user : users) {
        json j;
        j["id"] = user.id;
        j["first_name"] = user.first_name;
        j["last_name"] = user.last_name;
        j["name"] = user.name;
        j["email"] = user.email;
        j["team_id"] = user.team_id.value_or("");
        j["role"] = user.role.value_or("");
        j["created_at"] = user.created_at;
        arr.push_back(j);
    }
    output_json(arr);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void users_commands::register_commands(CLI::App& app) {
    auto* users = app.add_subcommand("users", "Manage workspace users");
    users->require_subcommand(1);

    // -----------------------------------------------------------------------
    // users list
    // -----------------------------------------------------------------------
    {
        auto* cmd = users->add_subcommand("list", "List workspace users");

        cmd->callback([]() {
            try {
                auto users = users_api::list_users();

                if (get_output_format() == OutputFormat::Json) {
                    render_user_json(users);
                } else {
                    render_user_table(users);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }
}
