#include "modules/sequences/commands.h"

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
#include "core/paginator.h"
#include "modules/sequences/api.h"
#include "modules/sequences/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

std::string format_rate(double rate) {
    int pct = static_cast<int>(rate * 100.0 + 0.5);
    return std::to_string(pct) + "%";
}

void render_sequence_table(const std::vector<Sequence>& sequences) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "NAME", "ACTIVE", "STEPS", "DELIVERED", "OPENED", "REPLIED", "RATE"});
        for (const auto& seq : sequences) {
            output_csv_row({
                seq.id,
                seq.name,
                seq.active ? "yes" : "no",
                std::to_string(seq.num_steps),
                std::to_string(seq.unique_delivered),
                std::to_string(seq.unique_opened),
                std::to_string(seq.unique_replied),
                format_rate(seq.open_rate)
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",        4, 24, false},
        {"NAME",      4, 30, false},
        {"ACTIVE",    4,  6, false},
        {"STEPS",     4,  5, true},
        {"DELIVERED", 4,  9, true},
        {"OPENED",    4,  7, true},
        {"REPLIED",   4,  7, true},
        {"RATE",      4,  8, false}
    });

    for (const auto& seq : sequences) {
        std::string active_str = seq.active ? color::green("yes") : "no";

        table.add_row({
            seq.id,
            seq.name,
            active_str,
            std::to_string(seq.num_steps),
            std::to_string(seq.unique_delivered),
            std::to_string(seq.unique_opened),
            std::to_string(seq.unique_replied),
            format_rate(seq.open_rate)
        });
    }

    if (table.empty()) {
        print_warning("No sequences found.");
        return;
    }

    table.render(std::cout);
}

void render_sequence_json(const std::vector<Sequence>& sequences) {
    json arr = json::array();
    for (const auto& seq : sequences) {
        json j;
        j["id"] = seq.id;
        j["name"] = seq.name;
        j["active"] = seq.active;
        j["num_steps"] = seq.num_steps;
        j["user_id"] = seq.user_id.value_or("");
        j["created_at"] = seq.created_at;
        j["unique_delivered"] = seq.unique_delivered;
        j["unique_opened"] = seq.unique_opened;
        j["unique_replied"] = seq.unique_replied;
        j["unique_bounced"] = seq.unique_bounced;
        j["open_rate"] = seq.open_rate;
        j["reply_rate"] = seq.reply_rate;
        j["bounce_rate"] = seq.bounce_rate;
        arr.push_back(j);
    }
    output_json(arr);
}

void render_sequence_detail(const Sequence& seq) {
    if (get_output_format() == OutputFormat::Json) {
        json j;
        j["id"] = seq.id;
        j["name"] = seq.name;
        j["active"] = seq.active;
        j["num_steps"] = seq.num_steps;
        j["user_id"] = seq.user_id.value_or("");
        j["created_at"] = seq.created_at;
        j["unique_delivered"] = seq.unique_delivered;
        j["unique_opened"] = seq.unique_opened;
        j["unique_replied"] = seq.unique_replied;
        j["unique_bounced"] = seq.unique_bounced;
        j["open_rate"] = seq.open_rate;
        j["reply_rate"] = seq.reply_rate;
        j["bounce_rate"] = seq.bounce_rate;

        json steps_arr = json::array();
        for (const auto& step : seq.steps) {
            json sj;
            sj["id"] = step.id;
            sj["type"] = step.type;
            sj["position"] = step.position;
            sj["wait_time"] = step.wait_time;
            sj["wait_mode"] = step.wait_mode;
            sj["subject"] = step.subject.value_or("");
            steps_arr.push_back(sj);
        }
        j["steps"] = steps_arr;

        output_json(j);
        return;
    }

    DetailRenderer detail;
    detail.add_section(seq.name);

    detail.add_field("Active", seq.active ? color::green("yes") : "no");
    detail.add_field("Steps", std::to_string(seq.num_steps));

    if (seq.user_id.has_value() && !seq.user_id.value().empty()) {
        detail.add_field("Owner", seq.user_id.value());
    }

    detail.add_blank_line();
    detail.add_section("Statistics");
    detail.add_field("Delivered", std::to_string(seq.unique_delivered));
    detail.add_field("Opened", std::to_string(seq.unique_opened));
    detail.add_field("Replied", std::to_string(seq.unique_replied));
    detail.add_field("Bounced", std::to_string(seq.unique_bounced));
    detail.add_field("Open Rate", format_rate(seq.open_rate));
    detail.add_field("Reply Rate", format_rate(seq.reply_rate));
    detail.add_field("Bounce Rate", format_rate(seq.bounce_rate));

    // Render steps if available
    if (!seq.steps.empty()) {
        detail.add_blank_line();
        detail.add_section("Steps");
        for (const auto& step : seq.steps) {
            std::string step_desc = "Step " + std::to_string(step.position + 1)
                + " (" + step.type + ")";
            if (step.wait_time > 0) {
                step_desc += " - wait " + std::to_string(step.wait_time) + " " + step.wait_mode;
            }
            if (step.subject.has_value() && !step.subject.value().empty()) {
                step_desc += " | " + step.subject.value();
            }
            detail.add_field("", step_desc);
        }
    }

    detail.add_blank_line();
    detail.add_field("ID", seq.id);
    detail.add_field("Created", seq.created_at);

    detail.render(std::cout);
}

std::vector<std::string> split_comma(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream ss(input);
    std::string item;
    while (std::getline(ss, item, ',')) {
        // Trim whitespace
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

void sequences_commands::register_commands(CLI::App& app) {
    auto* sequences = app.add_subcommand("sequences", "Manage email sequences");
    sequences->require_subcommand(1);

    // -----------------------------------------------------------------------
    // sequences list
    // -----------------------------------------------------------------------
    {
        auto* cmd = sequences->add_subcommand("list", "List sequences");

        struct ListOpts {
            std::string query;
            int page = 1;
            int per_page = 25;
            bool all = false;
        };
        auto opts = std::make_shared<ListOpts>();

        cmd->add_option("--query,-q", opts->query, "Search by sequence name");
        cmd->add_option("--page", opts->page, "Page number")->default_val(1);
        cmd->add_option("--per-page", opts->per_page, "Results per page")->default_val(25);
        cmd->add_flag("--all,-a", opts->all, "Fetch all pages");

        cmd->callback([opts]() {
            try {
                json body = json::object();
                if (!opts->query.empty()) {
                    body["q_keywords"] = opts->query;
                }

                if (opts->all) {
                    PaginationOptions pag_opts;
                    pag_opts.per_page = opts->per_page;
                    pag_opts.page = 1;
                    pag_opts.fetch_all = true;

                    Paginator<Sequence> paginator(
                        [&](int page, int per_page) -> OffsetPage<Sequence> {
                            json page_body = body;
                            page_body["page"] = page;
                            page_body["per_page"] = per_page;
                            return sequences_api::search_sequences(page_body);
                        },
                        pag_opts
                    );

                    auto all_seqs = paginator.fetch_all();
                    if (get_output_format() == OutputFormat::Json) {
                        render_sequence_json(all_seqs);
                    } else {
                        render_sequence_table(all_seqs);
                    }
                } else {
                    body["page"] = opts->page;
                    body["per_page"] = opts->per_page;

                    auto result = sequences_api::search_sequences(body);
                    if (get_output_format() == OutputFormat::Json) {
                        render_sequence_json(result.items);
                    } else {
                        render_sequence_table(result.items);
                        if (result.page_info.page < result.page_info.total_pages) {
                            print_warning("Page " + std::to_string(result.page_info.page)
                                + " of " + std::to_string(result.page_info.total_pages)
                                + ". Use --all to fetch all pages.");
                        }
                    }
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // sequences show <id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = sequences->add_subcommand("show", "Show sequence details");
        auto id = std::make_shared<std::string>();
        cmd->add_option("id", *id, "Sequence ID")->required();

        cmd->callback([id]() {
            try {
                auto seq = sequences_api::get_sequence(*id);
                render_sequence_detail(seq);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // sequences add-contacts <sequence_id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = sequences->add_subcommand("add-contacts", "Add contacts to a sequence");

        struct AddOpts {
            std::string sequence_id;
            std::string contact_ids;
            std::string email_account_id;
            std::string user_id;
        };
        auto opts = std::make_shared<AddOpts>();

        cmd->add_option("sequence_id", opts->sequence_id, "Sequence ID")->required();
        cmd->add_option("--contact-ids", opts->contact_ids, "Comma-separated contact IDs")->required();
        cmd->add_option("--email-account-id", opts->email_account_id, "Email account ID to send from")->required();
        cmd->add_option("--user-id", opts->user_id, "User ID to assign as owner");

        cmd->callback([opts]() {
            try {
                auto ids = split_comma(opts->contact_ids);
                if (ids.empty()) {
                    print_error("No contact IDs provided.");
                    return;
                }

                std::optional<std::string> user_opt = opts->user_id.empty()
                    ? std::nullopt
                    : std::make_optional(opts->user_id);

                auto result = sequences_api::add_contacts(
                    opts->sequence_id,
                    ids,
                    opts->email_account_id,
                    user_opt
                );

                if (get_output_format() == OutputFormat::Json) {
                    output_json(result);
                } else {
                    int count = 0;
                    if (result.contains("contacts") && result["contacts"].is_array()) {
                        count = static_cast<int>(result["contacts"].size());
                    }
                    print_success("Added " + std::to_string(count) + " contact(s) to sequence.");
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }
}
