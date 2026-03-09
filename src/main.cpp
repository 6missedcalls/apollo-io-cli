#include <iostream>
#include <string>

#include <CLI/CLI.hpp>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "core/version.h"
#include "modules/contacts/commands.h"
#include "modules/accounts/commands.h"
#include "modules/deals/commands.h"
#include "modules/tasks/commands.h"
#include "modules/sequences/commands.h"
#include "modules/enrichment/commands.h"
#include "modules/stages/commands.h"
#include "modules/labels/commands.h"
#include "modules/users/commands.h"
#include "modules/fields/commands.h"
#include "modules/email_accounts/commands.h"
#include "modules/emails/commands.h"
#include "modules/config/commands.h"
#include "modules/cache/commands.h"
#include "modules/users/api.h"
#include "modules/users/model.h"

using json = nlohmann::json;

static const char* APOLLO_BANNER = R"(
   █████╗ ██████╗  ██████╗ ██╗     ██╗      ██████╗
  ██╔══██╗██╔══██╗██╔═══██╗██║     ██║     ██╔═══██╗
  ███████║██████╔╝██║   ██║██║     ██║     ██║   ██║
  ██╔══██║██╔═══╝ ██║   ██║██║     ██║     ██║   ██║
  ██║  ██║██║     ╚██████╔╝███████╗███████╗╚██████╔╝
  ╚═╝  ╚═╝╚═╝      ╚═════╝ ╚══════╝╚══════╝ ╚═════╝
)";

int main(int argc, char** argv) {
    // Bare `apollo` with no arguments — show splash screen
    if (argc == 1) {
        std::cout << APOLLO_BANNER
                  << "  Apollo.io CLI for humans and AI agents\n"
                  << "  built by 6missedcalls\n"
                  << "  v" << APOLLO_VERSION << "\n\n"
                  << "  Run 'apollo --help' for available commands\n"
                  << "  Run 'apollo <command> --help' for command details\n"
                  << "\n";
        return 0;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    CLI::App app{"Apollo CLI for humans and AI agents"};
    app.add_flag_function("--version", [](int) {
        std::cout << APOLLO_BANNER
                  << "  v" << APOLLO_VERSION << "\n"
                  << "  built by 6missedcalls\n";
        throw CLI::Success();
    }, "Show version");
    app.require_subcommand();
    app.fallthrough();
    app.get_formatter()->column_width(32);

    // Global flags
    bool json_output = false;
    bool csv_output = false;
    bool no_color = false;
    bool verbose = false;

    app.add_flag("--json", json_output, "Output raw JSON");
    app.add_flag("--csv", csv_output, "Output CSV");
    app.add_flag("--no-color", no_color, "Disable colors");
    app.add_flag("--verbose", verbose, "Show debug info");

    // Apply global flags before any subcommand callback runs
    app.parse_complete_callback([&]() {
        if (json_output) set_output_format(OutputFormat::Json);
        if (csv_output) set_output_format(OutputFormat::Csv);
        if (no_color) color::set_enabled(false);
        if (verbose) set_verbose(true);
    });

    // Register module commands — CRM Core
    contacts_commands::register_commands(app);
    accounts_commands::register_commands(app);
    deals_commands::register_commands(app);
    stages_commands::register_commands(app);

    // Register module commands — Engagement
    tasks_commands::register_commands(app);
    sequences_commands::register_commands(app);
    emails_commands::register_commands(app);
    email_accounts_commands::register_commands(app);

    // Register module commands — Data
    enrichment_commands::register_commands(app);
    labels_commands::register_commands(app);
    users_commands::register_commands(app);
    fields_commands::register_commands(app);

    // Register module commands — System
    config_commands::register_commands(app);
    cache_commands::register_commands(app);

    // Apollo CLI: me — show current user info
    auto* me_cmd = app.add_subcommand("me", "Show your profile and account info");
    me_cmd->callback([&]() {
        try {
            auto users = users_api::list_users();
            // Find current user (first user is typically the API key owner)
            if (users.empty()) {
                print_warning("No user data available");
                return;
            }
            const auto& me = users[0];
            if (get_output_format() == OutputFormat::Json) {
                nlohmann::json j;
                j["id"] = me.id;
                j["name"] = me.name;
                j["email"] = me.email;
                j["role"] = me.role.value_or("");
                j["team_id"] = me.team_id.value_or("");
                output_json(j);
            } else {
                DetailRenderer detail;
                detail.add_field("ID", me.id);
                detail.add_field("Name", me.name);
                detail.add_field("Email", me.email);
                if (me.role.has_value() && !me.role->empty()) detail.add_field("Role", me.role.value());
                if (me.team_id.has_value()) detail.add_field("Team ID", me.team_id.value());
                detail.render(std::cout);
            }
        } catch (const ApolloError& e) {
            print_error(format_error(e));
            throw;
        }
    });

    // --- Group subcommands (Cobra-style) ---
    app.get_subcommand("contacts")->group("CRM Core");
    app.get_subcommand("accounts")->group("CRM Core");
    app.get_subcommand("deals")->group("CRM Core");
    app.get_subcommand("stages")->group("CRM Core");

    app.get_subcommand("tasks")->group("Engagement");
    app.get_subcommand("sequences")->group("Engagement");
    app.get_subcommand("emails")->group("Engagement");
    app.get_subcommand("email-accounts")->group("Engagement");

    app.get_subcommand("enrichment")->group("Data");
    app.get_subcommand("labels")->group("Data");
    app.get_subcommand("users")->group("Data");
    app.get_subcommand("fields")->group("Data");

    app.get_subcommand("config")->group("System");
    app.get_subcommand("cache")->group("System");
    me_cmd->group("System");

    try {
        app.parse(argc, argv);
    } catch (const CLI::RequiredError& e) {
        // When a subcommand group is invoked without a subcommand (e.g. `apollo contacts`),
        // show help for that group instead of erroring.
        for (auto* sub : app.get_subcommands()) {
            if (sub->parsed()) {
                std::cout << sub->help() << "\n";
                curl_global_cleanup();
                return 0;
            }
        }
        // Fallback: show top-level help
        std::cout << app.help() << "\n";
        curl_global_cleanup();
        return 0;
    } catch (const CLI::ParseError& e) {
        int ret = app.exit(e);
        curl_global_cleanup();
        return ret;
    } catch (const ApolloError& e) {
        // Error already printed by command callback
        curl_global_cleanup();
        return 1;
    } catch (const std::exception& e) {
        print_error(std::string("Unexpected error: ") + e.what());
        curl_global_cleanup();
        return 1;
    }

    curl_global_cleanup();
    return 0;
}
