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

using json = nlohmann::json;

int main(int argc, char** argv) {
    // Bare `capollo` with no arguments — show splash screen
    if (argc == 1) {
        std::cout << R"(
   ██████╗ █████╗ ██████╗  ██████╗ ██╗     ██╗      ██████╗
  ██╔════╝██╔══██╗██╔══██╗██╔═══██╗██║     ██║     ██╔═══██╗
  ██║     ███████║██████╔╝██║   ██║██║     ██║     ██║   ██║
  ██║     ██╔══██║██╔═══╝ ██║   ██║██║     ██║     ██║   ██║
  ╚██████╗██║  ██║██║     ╚██████╔╝███████╗███████╗╚██████╔╝
   ╚═════╝╚═╝  ╚═╝╚═╝      ╚═════╝ ╚══════╝╚══════╝ ╚═════╝

  Apollo.io CLI for humans and AI agents

  Run 'capollo --help' for available commands
  Run 'capollo <command> --help' for command details
)" << std::endl;
        return 0;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    CLI::App app{"capollo \xe2\x80\x94 Apollo.io CLI for humans and AI agents"};
    app.add_flag_function("--version", [](int) {
        std::cout << R"(
   ██████╗ █████╗ ██████╗  ██████╗ ██╗     ██╗      ██████╗
  ██╔════╝██╔══██╗██╔══██╗██╔═══██╗██║     ██║     ██╔═══██╗
  ██║     ███████║██████╔╝██║   ██║██║     ██║     ██║   ██║
  ██║     ██╔══██║██╔═══╝ ██║   ██║██║     ██║     ██║   ██║
  ╚██████╗██║  ██║██║     ╚██████╔╝███████╗███████╗╚██████╔╝
   ╚═════╝╚═╝  ╚═╝╚═╝      ╚═════╝ ╚══════╝╚══════╝ ╚═════╝
)" << "  v" << CAPOLLO_VERSION << "\n" << std::endl;
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

    // capollo me — show current user info
    auto* me_cmd = app.add_subcommand("me", "Show your profile and account info");
    me_cmd->callback([&]() {
        // TODO: implement via users API
        std::cout << "Not yet implemented" << std::endl;
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
        // When a subcommand group is invoked without a subcommand (e.g. `capollo contacts`),
        // show help for that group instead of erroring.
        for (auto* sub : app.get_subcommands()) {
            if (sub->parsed()) {
                std::cout << sub->help() << std::endl;
                curl_global_cleanup();
                return 0;
            }
        }
        // Fallback: show top-level help
        std::cout << app.help() << std::endl;
        curl_global_cleanup();
        return 0;
    } catch (const CLI::ParseError& e) {
        int ret = app.exit(e);
        curl_global_cleanup();
        return ret;
    } catch (const ApolloError& e) {
        print_error(format_error(e));
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
