#include "modules/emails/commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "core/paginator.h"
#include "modules/emails/api.h"
#include "modules/emails/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_email_table(const std::vector<OutreachEmail>& emails) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "SUBJECT", "STATUS", "CONTACT", "SENT"});
        for (const auto& email : emails) {
            output_csv_row({
                email.id,
                email.subject.value_or(""),
                email.status.value_or(""),
                email.contact_id.value_or(""),
                email.sent_at.value_or("")
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",      4, 24, false},
        {"SUBJECT", 4, 30, false},
        {"STATUS",  4, 10, false},
        {"CONTACT", 4, 12, false},
        {"SENT",    4, 20, false}
    });

    for (const auto& email : emails) {
        std::string status = email.status.value_or("");
        std::string status_str;
        if (status == "replied") {
            status_str = color::green(status);
        } else if (status == "opened" || status == "clicked") {
            status_str = color::cyan(status);
        } else if (status == "sent" || status == "delivered") {
            status_str = color::blue(status);
        } else if (status == "bounced" || status == "failed" || status == "spam_blocked") {
            status_str = color::red(status);
        } else {
            status_str = status;
        }

        table.add_row({
            email.id,
            email.subject.value_or(""),
            status_str,
            email.contact_id.value_or(""),
            email.sent_at.value_or("")
        });
    }

    if (table.empty()) {
        print_warning("No emails found.");
        return;
    }

    table.render(std::cout);
}

void render_email_json(const std::vector<OutreachEmail>& emails) {
    json arr = json::array();
    for (const auto& email : emails) {
        json j;
        j["id"] = email.id;
        j["subject"] = email.subject.value_or("");
        j["status"] = email.status.value_or("");
        j["contact_id"] = email.contact_id.value_or("");
        j["emailer_campaign_id"] = email.emailer_campaign_id.value_or("");
        j["sent_at"] = email.sent_at.value_or("");
        j["opened_at"] = email.opened_at.value_or("");
        j["replied_at"] = email.replied_at.value_or("");
        j["clicked_at"] = email.clicked_at.value_or("");
        arr.push_back(j);
    }
    output_json(arr);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void emails_commands::register_commands(CLI::App& app) {
    auto* emails = app.add_subcommand("emails", "Manage outreach emails");
    emails->require_subcommand(1);

    // -----------------------------------------------------------------------
    // emails search
    // -----------------------------------------------------------------------
    {
        auto* cmd = emails->add_subcommand("search", "Search outreach emails");

        struct SearchOpts {
            std::string sequence_id;
            std::string status;
            int page = 1;
            int per_page = 100;
        };
        auto opts = std::make_shared<SearchOpts>();

        cmd->add_option("--sequence-id", opts->sequence_id, "Filter by sequence ID");
        cmd->add_option("--status", opts->status,
            "Filter by status (sent, opened, clicked, replied, bounced, etc.)");
        cmd->add_option("--page", opts->page, "Page number")->default_val(1);
        cmd->add_option("--per-page", opts->per_page, "Results per page (max 100)")->default_val(100);

        cmd->callback([opts]() {
            try {
                json body = json::object();
                body["page"] = opts->page;
                body["per_page"] = opts->per_page;

                if (!opts->sequence_id.empty()) {
                    body["emailer_campaign_id"] = opts->sequence_id;
                }
                if (!opts->status.empty()) {
                    body["status"] = opts->status;
                }

                auto result = emails_api::search_emails(body);

                if (get_output_format() == OutputFormat::Json) {
                    render_email_json(result.items);
                } else {
                    render_email_table(result.items);
                    if (result.page_info.page < result.page_info.total_pages) {
                        print_warning("Page " + std::to_string(result.page_info.page)
                            + " of " + std::to_string(result.page_info.total_pages)
                            + ". Use --page to navigate.");
                    }
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }
}
