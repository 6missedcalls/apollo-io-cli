#include "modules/accounts/commands.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/filter.h"
#include "core/output.h"
#include "core/paginator.h"
#include "core/rest_client.h"
#include "modules/accounts/api.h"
#include "modules/accounts/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_account_table(const std::vector<Account>& accounts) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "NAME", "DOMAIN", "PHONE", "STAGE", "OWNER"});
        for (const auto& acct : accounts) {
            output_csv_row({
                acct.id,
                acct.name,
                acct.domain.value_or(""),
                acct.phone.value_or(""),
                acct.account_stage_id.value_or(""),
                acct.owner_id.value_or("")
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",     4, 12, false},
        {"NAME",   8, 25, false},
        {"DOMAIN", 4, 20, false},
        {"PHONE",  4, 15, false},
        {"STAGE",  4, 15, false},
        {"OWNER",  4, 15, false}
    });

    for (const auto& acct : accounts) {
        table.add_row({
            acct.id,
            acct.name,
            acct.domain.value_or(""),
            acct.phone.value_or(""),
            acct.account_stage_id.value_or(""),
            acct.owner_id.value_or("")
        });
    }

    if (table.empty()) {
        print_warning("No accounts found.");
        return;
    }

    table.render(std::cout);
}

void render_account_json(const std::vector<Account>& accounts) {
    json arr = json::array();
    for (const auto& acct : accounts) {
        json j;
        j["id"] = acct.id;
        j["name"] = acct.name;
        j["domain"] = acct.domain.value_or("");
        j["phone"] = acct.phone.value_or("");
        j["phoneStatus"] = acct.phone_status.value_or("");
        j["sanitizedPhone"] = acct.sanitized_phone.value_or("");
        j["ownerId"] = acct.owner_id.value_or("");
        j["accountStageId"] = acct.account_stage_id.value_or("");
        j["source"] = acct.source.value_or("");
        j["linkedinUrl"] = acct.linkedin_url.value_or("");
        j["existenceLevel"] = acct.existence_level.value_or("");
        j["labelIds"] = acct.label_ids;
        j["createdAt"] = acct.created_at;
        j["updatedAt"] = acct.updated_at;
        arr.push_back(j);
    }
    output_json(arr);
}

void render_account_detail(const Account& acct) {
    if (get_output_format() == OutputFormat::Json) {
        json j;
        j["id"] = acct.id;
        j["name"] = acct.name;
        j["domain"] = acct.domain.value_or("");
        j["phone"] = acct.phone.value_or("");
        j["phoneStatus"] = acct.phone_status.value_or("");
        j["sanitizedPhone"] = acct.sanitized_phone.value_or("");
        j["ownerId"] = acct.owner_id.value_or("");
        j["creatorId"] = acct.creator_id.value_or("");
        j["accountStageId"] = acct.account_stage_id.value_or("");
        j["teamId"] = acct.team_id.value_or("");
        j["organizationId"] = acct.organization_id.value_or("");
        j["source"] = acct.source.value_or("");
        j["originalSource"] = acct.original_source.value_or("");
        j["linkedinUrl"] = acct.linkedin_url.value_or("");
        j["hubspotId"] = acct.hubspot_id.value_or("");
        j["salesforceId"] = acct.salesforce_id.value_or("");
        j["crmOwnerId"] = acct.crm_owner_id.value_or("");
        j["parentAccountId"] = acct.parent_account_id.value_or("");
        j["existenceLevel"] = acct.existence_level.value_or("");
        j["labelIds"] = acct.label_ids;
        j["typedCustomFields"] = acct.typed_custom_fields;
        j["modality"] = acct.modality;
        j["createdAt"] = acct.created_at;
        j["updatedAt"] = acct.updated_at;
        output_json(j);
        return;
    }

    DetailRenderer detail;
    detail.add_section(acct.name);

    detail.add_field("ID", acct.id);
    if (acct.domain.has_value()) {
        detail.add_field("Domain", acct.domain.value());
    }
    if (acct.phone.has_value()) {
        detail.add_field("Phone", acct.phone.value());
    }
    if (acct.phone_status.has_value()) {
        detail.add_field("Phone Status", acct.phone_status.value());
    }
    if (acct.sanitized_phone.has_value()) {
        detail.add_field("Sanitized Phone", acct.sanitized_phone.value());
    }

    detail.add_blank_line();

    if (acct.owner_id.has_value()) {
        detail.add_field("Owner ID", acct.owner_id.value());
    }
    if (acct.creator_id.has_value()) {
        detail.add_field("Creator ID", acct.creator_id.value());
    }
    if (acct.account_stage_id.has_value()) {
        detail.add_field("Account Stage", acct.account_stage_id.value());
    }
    if (acct.team_id.has_value()) {
        detail.add_field("Team ID", acct.team_id.value());
    }
    if (acct.organization_id.has_value()) {
        detail.add_field("Organization ID", acct.organization_id.value());
    }

    detail.add_blank_line();

    if (acct.source.has_value()) {
        detail.add_field("Source", acct.source.value());
    }
    if (acct.original_source.has_value()) {
        detail.add_field("Original Source", acct.original_source.value());
    }
    if (acct.linkedin_url.has_value()) {
        detail.add_field("LinkedIn", acct.linkedin_url.value());
    }
    if (acct.hubspot_id.has_value()) {
        detail.add_field("HubSpot ID", acct.hubspot_id.value());
    }
    if (acct.salesforce_id.has_value()) {
        detail.add_field("Salesforce ID", acct.salesforce_id.value());
    }
    if (acct.crm_owner_id.has_value()) {
        detail.add_field("CRM Owner ID", acct.crm_owner_id.value());
    }
    if (acct.parent_account_id.has_value()) {
        detail.add_field("Parent Account", acct.parent_account_id.value());
    }
    if (acct.existence_level.has_value()) {
        detail.add_field("Existence Level", acct.existence_level.value());
    }

    if (!acct.label_ids.empty()) {
        std::string labels;
        for (size_t i = 0; i < acct.label_ids.size(); ++i) {
            if (i > 0) labels += ", ";
            labels += acct.label_ids[i];
        }
        detail.add_field("Labels", labels);
    }

    if (!acct.typed_custom_fields.is_null() && !acct.typed_custom_fields.empty()) {
        detail.add_blank_line();
        detail.add_section("Custom Fields");
        for (auto it = acct.typed_custom_fields.begin();
             it != acct.typed_custom_fields.end(); ++it) {
            detail.add_field(it.key(), it.value().dump());
        }
    }

    detail.add_blank_line();
    detail.add_field("Created", acct.created_at);
    detail.add_field("Updated", acct.updated_at);

    detail.render(std::cout);
}

std::vector<std::string> split_comma(const std::string& input) {
    std::vector<std::string> result;
    std::istringstream stream(input);
    std::string token;
    while (std::getline(stream, token, ',')) {
        // Trim whitespace
        size_t start = token.find_first_not_of(' ');
        size_t end = token.find_last_not_of(' ');
        if (start != std::string::npos) {
            result.push_back(token.substr(start, end - start + 1));
        }
    }
    return result;
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void accounts_commands::register_commands(CLI::App& app) {
    auto* accounts = app.add_subcommand("accounts", "Manage accounts");
    accounts->require_subcommand(1);

    // -----------------------------------------------------------------------
    // accounts list
    // -----------------------------------------------------------------------
    {
        auto* cmd = accounts->add_subcommand("list", "List/search accounts");

        struct ListOpts {
            std::string query;
            std::string stage;
            std::string owner;
            int page = 1;
            int per_page = 25;
            bool all = false;
            int limit = 0;
            std::string sort;
        };
        auto opts = std::make_shared<ListOpts>();

        cmd->add_option("--query,-q", opts->query, "Search by organization name");
        cmd->add_option("--stage", opts->stage, "Filter by account stage ID");
        cmd->add_option("--owner", opts->owner, "Filter by owner user ID");
        cmd->add_option("--page", opts->page, "Page number (1-indexed)")->default_val(1);
        cmd->add_option("--per-page", opts->per_page, "Results per page (max 100)")->default_val(25);
        cmd->add_flag("--all,-a", opts->all, "Fetch all pages");
        cmd->add_option("--limit,-n", opts->limit, "Maximum number of accounts to return")->default_val(0);
        cmd->add_option("--sort", opts->sort, "Sort field (account_last_activity_date, account_created_at, account_name, owner_name)");

        cmd->callback([opts]() {
            try {
                std::optional<std::string> q_name = opts->query.empty()
                    ? std::nullopt : std::make_optional(opts->query);
                std::optional<std::string> owner_id = opts->owner.empty()
                    ? std::nullopt : std::make_optional(opts->owner);

                std::optional<std::vector<std::string>> stage_ids;
                if (!opts->stage.empty()) {
                    stage_ids = std::vector<std::string>{opts->stage};
                }

                std::optional<std::string> sort_field = opts->sort.empty()
                    ? std::nullopt : std::make_optional(opts->sort);

                if (sort_field.has_value() && !is_valid_accounts_sort_field(sort_field.value())) {
                    print_error("Invalid sort field: " + sort_field.value() +
                        ". Use: account_last_activity_date, account_created_at, account_name, owner_name");
                    return;
                }

                PaginationOptions pag_opts;
                pag_opts.page = opts->page;
                pag_opts.per_page = opts->per_page;
                pag_opts.fetch_all = opts->all;
                pag_opts.limit = opts->limit;

                Paginator<Account> paginator(
                    [&](int page, int per_page) -> OffsetPage<Account> {
                        auto search_body = build_accounts_search(
                            std::nullopt,       // q_keywords
                            stage_ids,          // account_stage_ids
                            sort_field,         // sort_by_field
                            false,              // sort_ascending
                            page,
                            per_page,
                            q_name,             // q_organization_name
                            owner_id,           // owner_id
                            std::nullopt,       // label_ids
                            std::nullopt,       // q_organization_domains
                            std::nullopt        // organization_locations
                        );
                        return accounts_api::search_accounts(search_body);
                    },
                    pag_opts
                );

                auto all_accounts = paginator.fetch_all();

                if (get_output_format() == OutputFormat::Json) {
                    render_account_json(all_accounts);
                } else {
                    render_account_table(all_accounts);
                    if (paginator.has_next() && !opts->all) {
                        print_warning("More accounts available. Use --all to fetch all pages.");
                    }
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // accounts show <id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = accounts->add_subcommand("show", "Show account details");
        auto account_id = std::make_shared<std::string>();
        cmd->add_option("id", *account_id, "Account ID")->required();

        cmd->callback([account_id]() {
            try {
                auto account = accounts_api::get_account(*account_id);
                render_account_detail(account);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // accounts create
    // -----------------------------------------------------------------------
    {
        auto* cmd = accounts->add_subcommand("create", "Create a new account");

        struct CreateOpts {
            std::string name;
            std::string domain;
            std::string phone;
            std::string owner_id;
            std::string stage;
            std::string linkedin;
            std::vector<std::string> labels;
        };
        auto opts = std::make_shared<CreateOpts>();

        cmd->add_option("--name", opts->name, "Account name")->required();
        cmd->add_option("--domain", opts->domain, "Company domain (without protocol)");
        cmd->add_option("--phone", opts->phone, "Phone number");
        cmd->add_option("--owner-id", opts->owner_id, "Owner user ID");
        cmd->add_option("--stage", opts->stage, "Account stage ID");
        cmd->add_option("--linkedin", opts->linkedin, "LinkedIn company URL");
        cmd->add_option("--label,-l", opts->labels, "Label name to append (repeatable)");

        cmd->callback([opts]() {
            try {
                AccountCreateInput input;
                input.name = opts->name;

                if (!opts->domain.empty()) input.domain = opts->domain;
                if (!opts->phone.empty()) input.phone = opts->phone;
                if (!opts->owner_id.empty()) input.owner_id = opts->owner_id;
                if (!opts->stage.empty()) input.account_stage_id = opts->stage;
                if (!opts->linkedin.empty()) input.linkedin_url = opts->linkedin;
                input.append_label_names = opts->labels;

                auto account = accounts_api::create_account(input);
                print_success("Created account: " + account.name + " (" + account.id + ")");
                render_account_detail(account);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // accounts update <id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = accounts->add_subcommand("update", "Update an existing account");

        struct UpdateOpts {
            std::string id;
            std::string name;
            std::string domain;
            std::string phone;
            std::string owner_id;
            std::string stage;
            std::string linkedin;
            std::vector<std::string> labels;
        };
        auto opts = std::make_shared<UpdateOpts>();

        cmd->add_option("id", opts->id, "Account ID")->required();
        cmd->add_option("--name", opts->name, "Account name");
        cmd->add_option("--domain", opts->domain, "Company domain");
        cmd->add_option("--phone", opts->phone, "Phone number");
        cmd->add_option("--owner-id", opts->owner_id, "Owner user ID");
        cmd->add_option("--stage", opts->stage, "Account stage ID");
        cmd->add_option("--linkedin", opts->linkedin, "LinkedIn company URL");
        cmd->add_option("--label,-l", opts->labels, "Label name to append (repeatable)");

        cmd->callback([opts]() {
            try {
                AccountUpdateInput input;

                if (!opts->name.empty()) input.name = opts->name;
                if (!opts->domain.empty()) input.domain = opts->domain;
                if (!opts->phone.empty()) input.phone = opts->phone;
                if (!opts->owner_id.empty()) input.owner_id = opts->owner_id;
                if (!opts->stage.empty()) input.account_stage_id = opts->stage;
                if (!opts->linkedin.empty()) input.linkedin_url = opts->linkedin;
                input.append_label_names = opts->labels;

                auto account = accounts_api::update_account(opts->id, input);
                print_success("Updated account: " + account.name + " (" + account.id + ")");
                render_account_detail(account);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // accounts delete <id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = accounts->add_subcommand("delete", "Delete an account");
        auto account_id = std::make_shared<std::string>();
        cmd->add_option("id", *account_id, "Account ID")->required();

        cmd->callback([account_id]() {
            try {
                accounts_api::delete_account(*account_id);
                print_success("Deleted account: " + *account_id);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // accounts bulk-create --file <path>
    // -----------------------------------------------------------------------
    {
        auto* cmd = accounts->add_subcommand("bulk-create", "Bulk create accounts from JSON file");

        struct BulkOpts {
            std::string file;
            bool dedupe = false;
        };
        auto opts = std::make_shared<BulkOpts>();

        cmd->add_option("--file,-f", opts->file, "Path to JSON file with accounts array")->required();
        cmd->add_flag("--dedupe", opts->dedupe, "Enable deduplication against existing accounts");

        cmd->callback([opts]() {
            try {
                std::ifstream ifs(opts->file);
                if (!ifs.is_open()) {
                    throw ApolloError(ErrorKind::Internal, "Cannot open file: " + opts->file);
                }

                json file_data;
                try {
                    file_data = json::parse(ifs);
                } catch (const json::parse_error& e) {
                    throw ApolloError(ErrorKind::Validation, "Invalid JSON in file: " + std::string(e.what()));
                }

                // Accept either a bare array or {"accounts": [...]}
                json accounts_arr;
                if (file_data.is_array()) {
                    accounts_arr = file_data;
                } else if (file_data.contains("accounts") && file_data["accounts"].is_array()) {
                    accounts_arr = file_data["accounts"];
                } else {
                    throw ApolloError(ErrorKind::Validation,
                        "Expected JSON array or object with \"accounts\" key");
                }

                if (accounts_arr.size() > 100) {
                    throw ApolloError(ErrorKind::Validation,
                        "Maximum 100 accounts per bulk create request. Got: " +
                        std::to_string(accounts_arr.size()));
                }

                std::vector<AccountCreateInput> inputs;
                inputs.reserve(accounts_arr.size());

                for (const auto& item : accounts_arr) {
                    AccountCreateInput input;
                    input.name = item.value("name", "");

                    if (item.contains("domain") && !item["domain"].is_null()) {
                        input.domain = item["domain"].get<std::string>();
                    }
                    if (item.contains("phone") && !item["phone"].is_null()) {
                        input.phone = item["phone"].get<std::string>();
                    }
                    if (item.contains("owner_id") && !item["owner_id"].is_null()) {
                        input.owner_id = item["owner_id"].get<std::string>();
                    }
                    if (item.contains("account_stage_id") && !item["account_stage_id"].is_null()) {
                        input.account_stage_id = item["account_stage_id"].get<std::string>();
                    }
                    if (item.contains("linkedin_url") && !item["linkedin_url"].is_null()) {
                        input.linkedin_url = item["linkedin_url"].get<std::string>();
                    }
                    if (item.contains("append_label_names") && item["append_label_names"].is_array()) {
                        input.append_label_names = item["append_label_names"].get<std::vector<std::string>>();
                    }
                    if (item.contains("typed_custom_fields") && !item["typed_custom_fields"].is_null()) {
                        input.typed_custom_fields = item["typed_custom_fields"];
                    }

                    inputs.push_back(input);
                }

                auto created = accounts_api::bulk_create(inputs, opts->dedupe);

                print_success("Created " + std::to_string(created.size()) + " account(s)");

                if (get_output_format() == OutputFormat::Json) {
                    render_account_json(created);
                } else {
                    render_account_table(created);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // accounts update-owners
    // -----------------------------------------------------------------------
    {
        auto* cmd = accounts->add_subcommand("update-owners", "Transfer account ownership");

        struct OwnerOpts {
            std::string owner_id;
            std::string account_ids_str;
        };
        auto opts = std::make_shared<OwnerOpts>();

        cmd->add_option("--owner-id", opts->owner_id, "New owner user ID")->required();
        cmd->add_option("--account-ids", opts->account_ids_str, "Comma-separated account IDs")->required();

        cmd->callback([opts]() {
            try {
                auto ids = split_comma(opts->account_ids_str);
                if (ids.empty()) {
                    throw ApolloError(ErrorKind::Validation, "No account IDs provided");
                }

                OwnershipUpdate update;
                update.account_ids = ids;
                update.owner_id = opts->owner_id;

                auto response = accounts_api::update_owners(update);

                int count = 0;
                if (response.contains("accounts") && response["accounts"].is_array()) {
                    count = static_cast<int>(response["accounts"].size());
                }

                print_success("Updated ownership for " + std::to_string(count) + " account(s)");

                if (get_output_format() == OutputFormat::Json) {
                    output_json(response);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }
}
