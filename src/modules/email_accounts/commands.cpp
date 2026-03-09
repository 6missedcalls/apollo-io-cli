#include "modules/email_accounts/commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "modules/email_accounts/api.h"
#include "modules/email_accounts/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_email_account_table(const std::vector<EmailAccount>& accounts) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "EMAIL", "PROVIDER", "STATUS", "ACTIVE", "LIMIT"});
        for (const auto& acct : accounts) {
            output_csv_row({
                acct.id,
                acct.email,
                acct.provider.value_or(""),
                acct.warmup_status.value_or(""),
                acct.active ? "yes" : "no",
                std::to_string(acct.max_outbound_per_hour)
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",       4, 24, false},
        {"EMAIL",    4, 30, false},
        {"PROVIDER", 4, 10, false},
        {"STATUS",   4, 12, false},
        {"ACTIVE",   4,  6, false},
        {"LIMIT",    4,  8, true}
    });

    for (const auto& acct : accounts) {
        std::string active_str = acct.active ? color::green("yes") : "no";

        std::string status = acct.warmup_status.value_or("");
        std::string status_str;
        if (status == "active") {
            status_str = color::green(status);
        } else if (status == "completed") {
            status_str = color::cyan(status);
        } else if (status == "paused" || status == "stopped") {
            status_str = color::yellow(status);
        } else {
            status_str = status;
        }

        table.add_row({
            acct.id,
            acct.email,
            acct.provider.value_or(""),
            status_str,
            active_str,
            std::to_string(acct.max_outbound_per_hour)
        });
    }

    if (table.empty()) {
        print_warning("No email accounts found.");
        return;
    }

    table.render(std::cout);
}

void render_email_account_json(const std::vector<EmailAccount>& accounts) {
    json arr = json::array();
    for (const auto& acct : accounts) {
        json j;
        j["id"] = acct.id;
        j["email"] = acct.email;
        j["provider"] = acct.provider.value_or("");
        j["warmup_status"] = acct.warmup_status.value_or("");
        j["active"] = acct.active;
        j["max_outbound_per_hour"] = acct.max_outbound_per_hour;
        j["true_warmup_daily_limit"] = acct.true_warmup_daily_limit;
        arr.push_back(j);
    }
    output_json(arr);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void email_accounts_commands::register_commands(CLI::App& app) {
    auto* email_accounts = app.add_subcommand("email-accounts", "Manage email accounts");
    email_accounts->require_subcommand(1);

    // -----------------------------------------------------------------------
    // email-accounts list
    // -----------------------------------------------------------------------
    {
        auto* cmd = email_accounts->add_subcommand("list", "List email accounts");

        cmd->callback([]() {
            try {
                auto accounts = email_accounts_api::list_email_accounts();

                if (get_output_format() == OutputFormat::Json) {
                    render_email_account_json(accounts);
                } else {
                    render_email_account_table(accounts);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }
}
