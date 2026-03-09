#include "modules/deals/commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "core/resolver.h"
#include "core/paginator.h"
#include "modules/deals/api.h"
#include "modules/deals/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

std::string format_amount(double amount) {
    if (amount == 0) return "0";
    auto val = static_cast<long long>(amount);
    std::string s = std::to_string(val);

    // Insert commas for readability
    int insert_pos = static_cast<int>(s.length()) - 3;
    while (insert_pos > 0) {
        s.insert(static_cast<size_t>(insert_pos), ",");
        insert_pos -= 3;
    }

    return s;
}

std::string deal_status_label(bool is_won, bool is_closed) {
    if (is_won && is_closed) return "Won";
    if (!is_won && is_closed) return "Lost";
    return "Open";
}

void render_deal_table(const std::vector<Deal>& deals) {
    const auto& r = resolve::get_resolver();

    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "NAME", "AMOUNT", "STAGE", "STATUS", "OWNER", "CLOSE_DATE"});
        for (const auto& deal : deals) {
            output_csv_row({
                deal.id,
                deal.name,
                format_amount(deal.amount),
                deal.stage_name.value_or(""),
                deal_status_label(deal.is_won, deal.is_closed),
                r.user_name(deal.owner_id.value_or("")),
                deal.closed_date.value_or("")
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",         12, 26, false},
        {"NAME",       10, 25, false},
        {"AMOUNT",      6, 10, true},
        {"STAGE",       6, 15, false},
        {"STATUS",      6, 10, false},
        {"OWNER",       6, 20, false},
        {"CLOSE_DATE",  6, 12, false}
    });

    for (const auto& deal : deals) {
        std::string status_text = deal_status_label(deal.is_won, deal.is_closed);
        std::string status_colored = color::deal_status(deal.is_won, deal.is_closed, status_text);

        table.add_row({
            deal.id,
            deal.name,
            format_amount(deal.amount),
            deal.stage_name.value_or(""),
            status_colored,
            r.user_name(deal.owner_id.value_or("")),
            deal.closed_date.value_or("")
        });
    }

    if (table.empty()) {
        print_warning("No deals found.");
        return;
    }

    table.render(std::cout);
}

void render_deal_json(const std::vector<Deal>& deals) {
    json arr = json::array();
    for (const auto& deal : deals) {
        json j;
        j["id"] = deal.id;
        j["name"] = deal.name;
        j["amount"] = deal.amount;
        j["is_closed"] = deal.is_closed;
        j["is_won"] = deal.is_won;
        j["stage_name"] = deal.stage_name.value_or("");
        j["opportunity_stage_id"] = deal.opportunity_stage_id.value_or("");
        j["owner_id"] = deal.owner_id.value_or("");
        j["account_id"] = deal.account_id.value_or("");
        j["closed_date"] = deal.closed_date.value_or("");
        j["source"] = deal.source.value_or("");
        j["created_at"] = deal.created_at;
        j["updated_at"] = deal.updated_at;
        arr.push_back(j);
    }
    output_json(arr);
}

void render_deal_detail(const Deal& deal) {
    if (get_output_format() == OutputFormat::Json) {
        json j;
        j["id"] = deal.id;
        j["name"] = deal.name;
        j["amount"] = deal.amount;
        j["is_closed"] = deal.is_closed;
        j["is_won"] = deal.is_won;
        j["stage_name"] = deal.stage_name.value_or("");
        j["opportunity_stage_id"] = deal.opportunity_stage_id.value_or("");
        j["owner_id"] = deal.owner_id.value_or("");
        j["account_id"] = deal.account_id.value_or("");
        j["closed_date"] = deal.closed_date.value_or("");
        j["source"] = deal.source.value_or("");
        j["description"] = deal.description.value_or("");
        j["team_id"] = deal.team_id.value_or("");
        j["typed_custom_fields"] = deal.typed_custom_fields;
        j["created_at"] = deal.created_at;
        j["updated_at"] = deal.updated_at;
        output_json(j);
        return;
    }

    DetailRenderer detail;
    detail.add_section(deal.name);

    detail.add_field("ID", deal.id);
    detail.add_field("Amount", format_amount(deal.amount));

    std::string status_text = deal_status_label(deal.is_won, deal.is_closed);
    detail.add_field("Status", color::deal_status(deal.is_won, deal.is_closed, status_text));

    if (deal.stage_name.has_value()) {
        detail.add_field("Stage", deal.stage_name.value());
    }
    if (deal.owner_id.has_value()) {
        const auto& r = resolve::get_resolver();
        detail.add_field("Owner", r.user_name(deal.owner_id.value()));
    }
    if (deal.account_id.has_value()) {
        detail.add_field("Account ID", deal.account_id.value());
    }
    if (deal.closed_date.has_value()) {
        detail.add_field("Close Date", deal.closed_date.value());
    }
    if (deal.source.has_value()) {
        detail.add_field("Source", deal.source.value());
    }

    detail.add_field("Created", deal.created_at);
    detail.add_field("Updated", deal.updated_at);

    if (deal.description.has_value() && !deal.description.value().empty()) {
        detail.add_blank_line();
        detail.add_section("Description");
        detail.add_markdown(deal.description.value());
    }

    detail.render(std::cout);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void deals_commands::register_commands(CLI::App& app) {
    auto* deals = app.add_subcommand("deals", "Manage deals/opportunities");
    deals->require_subcommand(1);

    // -------------------------------------------------------------------
    // deals list
    // -------------------------------------------------------------------
    {
        auto* cmd = deals->add_subcommand("list", "List deals");

        struct ListOpts {
            std::string sort;
            int page = 1;
            int per_page = 25;
            bool all = false;
            int limit = 0;
        };
        auto opts = std::make_shared<ListOpts>();

        cmd->add_option("--sort", opts->sort, "Sort by field (amount, is_closed, is_won)");
        cmd->add_option("--page", opts->page, "Page number")->default_val(1);
        cmd->add_option("--per-page", opts->per_page, "Results per page")->default_val(25);
        cmd->add_flag("--all,-a", opts->all, "Fetch all pages");
        cmd->add_option("--limit,-n", opts->limit, "Maximum number of deals to return");

        cmd->callback([opts]() {
            try {
                PaginationOptions pag_opts;
                pag_opts.page = opts->page;
                pag_opts.per_page = opts->per_page;
                pag_opts.fetch_all = opts->all;
                pag_opts.limit = opts->limit;

                Paginator<Deal> paginator(
                    [&](int page, int per_page) -> OffsetPage<Deal> {
                        return deals_api::search_deals(opts->sort, page, per_page);
                    },
                    pag_opts
                );

                auto deals = paginator.fetch_all();

                if (get_output_format() == OutputFormat::Json) {
                    render_deal_json(deals);
                } else {
                    render_deal_table(deals);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }

    // -------------------------------------------------------------------
    // deals show <id>
    // -------------------------------------------------------------------
    {
        auto* cmd = deals->add_subcommand("show", "Show deal details");
        auto id = std::make_shared<std::string>();
        cmd->add_option("id", *id, "Deal ID")->required();

        cmd->callback([id]() {
            try {
                auto deal = deals_api::get_deal(*id);
                render_deal_detail(deal);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }

    // -------------------------------------------------------------------
    // deals create
    // -------------------------------------------------------------------
    {
        auto* cmd = deals->add_subcommand("create", "Create a new deal");

        struct CreateOpts {
            std::string name;
            std::string owner_id;
            std::string account_id;
            double amount = -1;
            std::string stage_id;
            std::string close_date;
        };
        auto opts = std::make_shared<CreateOpts>();

        cmd->add_option("--name", opts->name, "Deal name")->required();
        cmd->add_option("--owner-id", opts->owner_id, "Owner user ID");
        cmd->add_option("--account-id", opts->account_id, "Account ID");
        cmd->add_option("--amount", opts->amount, "Deal amount");
        cmd->add_option("--stage-id", opts->stage_id, "Opportunity stage ID");
        cmd->add_option("--close-date", opts->close_date, "Close date (YYYY-MM-DD)");

        cmd->callback([opts]() {
            try {
                DealCreateInput input;
                input.name = opts->name;

                if (!opts->owner_id.empty()) {
                    input.owner_id = opts->owner_id;
                }
                if (!opts->account_id.empty()) {
                    input.account_id = opts->account_id;
                }
                if (opts->amount >= 0) {
                    input.amount = opts->amount;
                }
                if (!opts->stage_id.empty()) {
                    input.opportunity_stage_id = opts->stage_id;
                }
                if (!opts->close_date.empty()) {
                    input.closed_date = opts->close_date;
                }

                auto deal = deals_api::create_deal(input);
                if (get_output_format() != OutputFormat::Json) {
                    print_success("Deal created: " + deal.id);
                }
                render_deal_detail(deal);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }

    // -------------------------------------------------------------------
    // deals update <id>
    // -------------------------------------------------------------------
    {
        auto* cmd = deals->add_subcommand("update", "Update a deal");

        struct UpdateOpts {
            std::string id;
            std::string name;
            std::string owner_id;
            std::string account_id;
            double amount = -1;
            std::string stage_id;
            std::string close_date;
        };
        auto opts = std::make_shared<UpdateOpts>();

        cmd->add_option("id", opts->id, "Deal ID")->required();
        cmd->add_option("--name", opts->name, "Deal name");
        cmd->add_option("--owner-id", opts->owner_id, "Owner user ID");
        cmd->add_option("--account-id", opts->account_id, "Account ID");
        cmd->add_option("--amount", opts->amount, "Deal amount");
        cmd->add_option("--stage-id", opts->stage_id, "Opportunity stage ID");
        cmd->add_option("--close-date", opts->close_date, "Close date (YYYY-MM-DD)");

        cmd->callback([opts]() {
            try {
                DealUpdateInput input;

                if (!opts->name.empty()) {
                    input.name = opts->name;
                }
                if (!opts->owner_id.empty()) {
                    input.owner_id = opts->owner_id;
                }
                if (!opts->account_id.empty()) {
                    input.account_id = opts->account_id;
                }
                if (opts->amount >= 0) {
                    input.amount = opts->amount;
                }
                if (!opts->stage_id.empty()) {
                    input.opportunity_stage_id = opts->stage_id;
                }
                if (!opts->close_date.empty()) {
                    input.closed_date = opts->close_date;
                }

                auto deal = deals_api::update_deal(opts->id, input);
                if (get_output_format() != OutputFormat::Json) {
                    print_success("Deal updated: " + deal.id);
                }
                render_deal_detail(deal);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
                throw;
            }
        });
    }
}
