#include "commands.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "../../core/color.h"
#include "../../core/error.h"
#include "../../core/output.h"
#include "../../core/paginator.h"
#include "api.h"
#include "model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_contact_table(const std::vector<Contact>& contacts) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "NAME", "EMAIL", "STATUS", "TITLE", "ORG", "OWNER"});
        for (const auto& c : contacts) {
            output_csv_row({
                c.id,
                c.name,
                c.email,
                c.email_status,
                c.title.value_or(""),
                c.organization_name.value_or(""),
                c.owner_id.value_or("")
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",     6, 12, false},
        {"NAME",   8, 20, false},
        {"EMAIL", 10, 25, false},
        {"STATUS", 4, 10, false},
        {"TITLE",  6, 20, false},
        {"ORG",    6, 20, false},
        {"OWNER",  6, 12, false}
    });

    for (const auto& c : contacts) {
        std::string status_str = color::email_status(c.email_status, c.email_status);

        table.add_row({
            c.id,
            c.name,
            c.email,
            status_str,
            c.title.value_or(""),
            c.organization_name.value_or(""),
            c.owner_id.value_or("")
        });
    }

    if (table.empty()) {
        print_warning("No contacts found.");
        return;
    }

    table.render(std::cout);
}

void render_contact_json(const std::vector<Contact>& contacts) {
    json arr = json::array();
    for (const auto& c : contacts) {
        json j;
        j["id"] = c.id;
        j["first_name"] = c.first_name;
        j["last_name"] = c.last_name;
        j["name"] = c.name;
        j["email"] = c.email;
        j["email_status"] = c.email_status;
        j["title"] = c.title.value_or("");
        j["organization_name"] = c.organization_name.value_or("");
        j["owner_id"] = c.owner_id.value_or("");
        j["linkedin_url"] = c.linkedin_url.value_or("");
        j["account_id"] = c.account_id.value_or("");
        j["contact_stage_id"] = c.contact_stage_id.value_or("");
        j["source"] = c.source.value_or("");
        j["created_at"] = c.created_at;
        j["updated_at"] = c.updated_at;
        arr.push_back(j);
    }
    output_json(arr);
}

void render_contact_detail(const Contact& c) {
    if (get_output_format() == OutputFormat::Json) {
        json j;
        j["id"] = c.id;
        j["first_name"] = c.first_name;
        j["last_name"] = c.last_name;
        j["name"] = c.name;
        j["email"] = c.email;
        j["email_status"] = c.email_status;
        j["title"] = c.title.value_or("");
        j["headline"] = c.headline.value_or("");
        j["organization_name"] = c.organization_name.value_or("");
        j["organization_id"] = c.organization_id.value_or("");
        j["owner_id"] = c.owner_id.value_or("");
        j["account_id"] = c.account_id.value_or("");
        j["contact_stage_id"] = c.contact_stage_id.value_or("");
        j["linkedin_url"] = c.linkedin_url.value_or("");
        j["photo_url"] = c.photo_url.value_or("");
        j["twitter_url"] = c.twitter_url.value_or("");
        j["facebook_url"] = c.facebook_url.value_or("");
        j["github_url"] = c.github_url.value_or("");
        j["seniority"] = c.seniority.value_or("");
        j["present_raw_address"] = c.present_raw_address.value_or("");
        j["city"] = c.city.value_or("");
        j["state"] = c.state.value_or("");
        j["country"] = c.country.value_or("");
        j["postal_code"] = c.postal_code.value_or("");
        j["source"] = c.source.value_or("");
        j["original_source"] = c.original_source.value_or("");
        j["existence_level"] = c.existence_level.value_or("");
        j["label_ids"] = c.label_ids;
        j["departments"] = c.departments;
        j["typed_custom_fields"] = c.typed_custom_fields;
        j["created_at"] = c.created_at;
        j["updated_at"] = c.updated_at;

        json phones = json::array();
        for (const auto& pn : c.phone_numbers) {
            json p;
            p["raw_number"] = pn.raw_number;
            p["type"] = pn.type;
            p["sanitized_number"] = pn.sanitized_number;
            phones.push_back(p);
        }
        j["phone_numbers"] = phones;

        output_json(j);
        return;
    }

    DetailRenderer detail;
    detail.add_section(c.name.empty() ? c.id : c.name);

    detail.add_field("ID", c.id);
    detail.add_field("Email", c.email);
    detail.add_field("Email Status", color::email_status(c.email_status, c.email_status));

    if (c.title.has_value() && !c.title->empty()) {
        detail.add_field("Title", c.title.value());
    }
    if (c.headline.has_value() && !c.headline->empty()) {
        detail.add_field("Headline", c.headline.value());
    }
    if (c.organization_name.has_value() && !c.organization_name->empty()) {
        detail.add_field("Organization", c.organization_name.value());
    }
    if (c.seniority.has_value() && !c.seniority->empty()) {
        detail.add_field("Seniority", c.seniority.value());
    }
    if (!c.departments.empty()) {
        std::string deps;
        for (size_t i = 0; i < c.departments.size(); ++i) {
            if (i > 0) deps += ", ";
            deps += c.departments[i];
        }
        detail.add_field("Departments", deps);
    }

    if (c.owner_id.has_value() && !c.owner_id->empty()) {
        detail.add_field("Owner", c.owner_id.value());
    }
    if (c.account_id.has_value() && !c.account_id->empty()) {
        detail.add_field("Account", c.account_id.value());
    }
    if (c.contact_stage_id.has_value() && !c.contact_stage_id->empty()) {
        detail.add_field("Stage", c.contact_stage_id.value());
    }

    // Phone numbers
    if (!c.phone_numbers.empty()) {
        detail.add_blank_line();
        detail.add_section("Phone Numbers");
        for (const auto& pn : c.phone_numbers) {
            std::string label = pn.type.empty() ? "Phone" : pn.type;
            std::string number = pn.sanitized_number.empty() ? pn.raw_number : pn.sanitized_number;
            detail.add_field(label, number);
        }
    }

    // Links
    if (c.linkedin_url.has_value() && !c.linkedin_url->empty()) {
        detail.add_field("LinkedIn", c.linkedin_url.value());
    }
    if (c.twitter_url.has_value() && !c.twitter_url->empty()) {
        detail.add_field("Twitter", c.twitter_url.value());
    }
    if (c.facebook_url.has_value() && !c.facebook_url->empty()) {
        detail.add_field("Facebook", c.facebook_url.value());
    }
    if (c.github_url.has_value() && !c.github_url->empty()) {
        detail.add_field("GitHub", c.github_url.value());
    }

    // Address
    if (c.present_raw_address.has_value() && !c.present_raw_address->empty()) {
        detail.add_field("Address", c.present_raw_address.value());
    } else {
        std::string location;
        if (c.city.has_value() && !c.city->empty()) location += c.city.value();
        if (c.state.has_value() && !c.state->empty()) {
            if (!location.empty()) location += ", ";
            location += c.state.value();
        }
        if (c.country.has_value() && !c.country->empty()) {
            if (!location.empty()) location += ", ";
            location += c.country.value();
        }
        if (!location.empty()) {
            detail.add_field("Location", location);
        }
    }

    if (!c.label_ids.empty()) {
        std::string labels;
        for (size_t i = 0; i < c.label_ids.size(); ++i) {
            if (i > 0) labels += ", ";
            labels += c.label_ids[i];
        }
        detail.add_field("Labels", labels);
    }

    if (c.source.has_value() && !c.source->empty()) {
        detail.add_field("Source", c.source.value());
    }
    if (c.existence_level.has_value() && !c.existence_level->empty()) {
        detail.add_field("Existence", c.existence_level.value());
    }

    detail.add_field("Created", c.created_at);
    detail.add_field("Updated", c.updated_at);

    detail.render(std::cout);
}

json read_json_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw ApolloError(ErrorKind::Internal, "Could not open file: " + path);
    }

    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        throw ApolloError(ErrorKind::Validation, "Invalid JSON in file: " + std::string(e.what()));
    }

    return data;
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void contacts_commands::register_commands(CLI::App& app) {
    auto* contacts = app.add_subcommand("contacts", "Manage contacts");
    contacts->require_subcommand(1);

    // -----------------------------------------------------------------------
    // contacts list
    // -----------------------------------------------------------------------
    {
        auto* cmd = contacts->add_subcommand("list", "List contacts");

        struct ListOpts {
            std::string query;
            std::string stage;
            std::string owner;
            std::string account;
            std::string label;
            int page = 1;
            int per_page = 25;
            bool all = false;
            int limit = 0;
            std::string sort;
        };
        auto opts = std::make_shared<ListOpts>();

        cmd->add_option("--query,-q", opts->query, "Search keywords");
        cmd->add_option("--stage", opts->stage, "Filter by contact stage ID");
        cmd->add_option("--owner", opts->owner, "Filter by owner user ID");
        cmd->add_option("--account", opts->account, "Filter by account ID");
        cmd->add_option("--label", opts->label, "Filter by label ID");
        cmd->add_option("--page", opts->page, "Page number")->default_val(1);
        cmd->add_option("--per-page", opts->per_page, "Results per page (max 100)")->default_val(25);
        cmd->add_flag("--all,-a", opts->all, "Fetch all pages");
        cmd->add_option("--limit,-n", opts->limit, "Maximum number of contacts to return")->default_val(0);
        cmd->add_option("--sort", opts->sort, "Sort field (contact_created_at, contact_updated_at, first_name, last_name, email)");

        cmd->callback([opts]() {
            try {
                json search_body = json::object();

                if (!opts->query.empty()) {
                    search_body["q_keywords"] = opts->query;
                }
                if (!opts->stage.empty()) {
                    search_body["contact_stage_ids"] = json::array({opts->stage});
                }
                if (!opts->owner.empty()) {
                    search_body["owner_id"] = opts->owner;
                }
                if (!opts->account.empty()) {
                    search_body["account_ids"] = json::array({opts->account});
                }
                if (!opts->label.empty()) {
                    search_body["contact_label_ids"] = json::array({opts->label});
                }
                if (!opts->sort.empty()) {
                    search_body["sort_by_field"] = opts->sort;
                }

                if (opts->all || opts->limit > 0) {
                    PaginationOptions pag_opts;
                    pag_opts.per_page = opts->per_page;
                    pag_opts.page = opts->page;
                    pag_opts.fetch_all = opts->all;
                    pag_opts.limit = opts->limit;

                    Paginator<Contact> paginator(
                        [&](int page, int per_page) -> OffsetPage<Contact> {
                            json body = search_body;
                            body["page"] = page;
                            body["per_page"] = per_page;
                            return contacts_api::search_contacts(body);
                        },
                        pag_opts
                    );

                    auto all_contacts = paginator.fetch_all();
                    if (get_output_format() == OutputFormat::Json) {
                        render_contact_json(all_contacts);
                    } else {
                        render_contact_table(all_contacts);
                    }
                } else {
                    search_body["page"] = opts->page;
                    search_body["per_page"] = opts->per_page;

                    auto result = contacts_api::search_contacts(search_body);
                    if (get_output_format() == OutputFormat::Json) {
                        render_contact_json(result.items);
                    } else {
                        render_contact_table(result.items);
                        if (result.page_info.page < result.page_info.total_pages) {
                            print_warning("Page " + std::to_string(result.page_info.page) +
                                " of " + std::to_string(result.page_info.total_pages) +
                                " (" + std::to_string(result.page_info.total_entries) +
                                " total). Use --all to fetch all pages.");
                        }
                    }
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // contacts show <id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = contacts->add_subcommand("show", "Show contact details");
        auto contact_id = std::make_shared<std::string>();
        cmd->add_option("id", *contact_id, "Contact ID")->required();

        cmd->callback([contact_id]() {
            try {
                auto contact = contacts_api::get_contact(*contact_id);
                render_contact_detail(contact);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // contacts create
    // -----------------------------------------------------------------------
    {
        auto* cmd = contacts->add_subcommand("create", "Create a new contact");

        struct CreateOpts {
            std::string first_name;
            std::string last_name;
            std::string email;
            std::string title;
            std::string org;
            std::string account_id;
            std::string owner_id;
            std::string linkedin;
            std::vector<std::string> phones;
            std::string address;
            std::string stage;
            std::vector<std::string> labels;
        };
        auto opts = std::make_shared<CreateOpts>();

        cmd->add_option("--first-name", opts->first_name, "First name");
        cmd->add_option("--last-name", opts->last_name, "Last name");
        cmd->add_option("--email", opts->email, "Email address");
        cmd->add_option("--title", opts->title, "Job title");
        cmd->add_option("--org", opts->org, "Organization name");
        cmd->add_option("--account-id", opts->account_id, "Account ID");
        cmd->add_option("--owner-id", opts->owner_id, "Owner user ID");
        cmd->add_option("--linkedin", opts->linkedin, "LinkedIn URL");
        cmd->add_option("--phone", opts->phones, "Phone number (repeatable, format: type:number e.g. mobile:555-1234)");
        cmd->add_option("--address", opts->address, "Full address");
        cmd->add_option("--stage", opts->stage, "Contact stage ID");
        cmd->add_option("--label", opts->labels, "Label name (repeatable)");

        cmd->callback([opts]() {
            try {
                ContactCreateInput input;

                if (!opts->first_name.empty()) input.first_name = opts->first_name;
                if (!opts->last_name.empty()) input.last_name = opts->last_name;
                if (!opts->email.empty()) input.email = opts->email;
                if (!opts->title.empty()) input.title = opts->title;
                if (!opts->org.empty()) input.organization_name = opts->org;
                if (!opts->account_id.empty()) input.account_id = opts->account_id;
                if (!opts->owner_id.empty()) input.owner_id = opts->owner_id;
                if (!opts->linkedin.empty()) input.linkedin_url = opts->linkedin;
                if (!opts->address.empty()) input.present_raw_address = opts->address;
                if (!opts->stage.empty()) input.contact_stage_id = opts->stage;

                input.label_names = opts->labels;

                // Parse phone numbers: "type:number" or just "number"
                for (const auto& phone_str : opts->phones) {
                    PhoneNumber pn;
                    auto colon_pos = phone_str.find(':');
                    if (colon_pos != std::string::npos) {
                        pn.type = phone_str.substr(0, colon_pos);
                        pn.raw_number = phone_str.substr(colon_pos + 1);
                    } else {
                        pn.raw_number = phone_str;
                    }
                    input.phone_numbers.push_back(pn);
                }

                auto contact = contacts_api::create_contact(input);
                print_success("Created contact " + contact.id + " (" + contact.name + ")");
                render_contact_detail(contact);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // contacts update <id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = contacts->add_subcommand("update", "Update a contact");

        struct UpdateOpts {
            std::string id;
            std::string first_name;
            std::string last_name;
            std::string email;
            std::string title;
            std::string org;
            std::string account_id;
            std::string owner_id;
            std::string linkedin;
            std::vector<std::string> phones;
            std::string address;
            std::string stage;
            std::vector<std::string> labels;
        };
        auto opts = std::make_shared<UpdateOpts>();

        cmd->add_option("id", opts->id, "Contact ID")->required();
        cmd->add_option("--first-name", opts->first_name, "First name");
        cmd->add_option("--last-name", opts->last_name, "Last name");
        cmd->add_option("--email", opts->email, "Email address");
        cmd->add_option("--title", opts->title, "Job title");
        cmd->add_option("--org", opts->org, "Organization name");
        cmd->add_option("--account-id", opts->account_id, "Account ID");
        cmd->add_option("--owner-id", opts->owner_id, "Owner user ID");
        cmd->add_option("--linkedin", opts->linkedin, "LinkedIn URL");
        cmd->add_option("--phone", opts->phones, "Phone number (repeatable, format: type:number)");
        cmd->add_option("--address", opts->address, "Full address");
        cmd->add_option("--stage", opts->stage, "Contact stage ID");
        cmd->add_option("--label", opts->labels, "Label name (repeatable)");

        cmd->callback([opts]() {
            try {
                ContactUpdateInput input;

                if (!opts->first_name.empty()) input.first_name = opts->first_name;
                if (!opts->last_name.empty()) input.last_name = opts->last_name;
                if (!opts->email.empty()) input.email = opts->email;
                if (!opts->title.empty()) input.title = opts->title;
                if (!opts->org.empty()) input.organization_name = opts->org;
                if (!opts->account_id.empty()) input.account_id = opts->account_id;
                if (!opts->owner_id.empty()) input.owner_id = opts->owner_id;
                if (!opts->linkedin.empty()) input.linkedin_url = opts->linkedin;
                if (!opts->address.empty()) input.present_raw_address = opts->address;
                if (!opts->stage.empty()) input.contact_stage_id = opts->stage;

                if (!opts->labels.empty()) {
                    input.label_names = opts->labels;
                }

                // Parse phone numbers
                for (const auto& phone_str : opts->phones) {
                    PhoneNumber pn;
                    auto colon_pos = phone_str.find(':');
                    if (colon_pos != std::string::npos) {
                        pn.type = phone_str.substr(0, colon_pos);
                        pn.raw_number = phone_str.substr(colon_pos + 1);
                    } else {
                        pn.raw_number = phone_str;
                    }
                    input.phone_numbers.push_back(pn);
                }

                auto contact = contacts_api::update_contact(opts->id, input);
                print_success("Updated contact " + contact.id);
                render_contact_detail(contact);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // contacts delete <id>
    // -----------------------------------------------------------------------
    {
        auto* cmd = contacts->add_subcommand("delete", "Delete a contact");

        struct DeleteOpts {
            std::string id;
            bool yes = false;
        };
        auto opts = std::make_shared<DeleteOpts>();

        cmd->add_option("id", opts->id, "Contact ID")->required();
        cmd->add_flag("--yes,-y", opts->yes, "Skip confirmation");

        cmd->callback([opts]() {
            try {
                if (!opts->yes) {
                    std::cerr << "Are you sure you want to delete contact "
                              << opts->id << "? [y/N] ";
                    std::string confirm;
                    std::getline(std::cin, confirm);
                    if (confirm != "y" && confirm != "Y" && confirm != "yes") {
                        print_warning("Aborted.");
                        return;
                    }
                }

                contacts_api::delete_contact(opts->id);
                print_success("Deleted contact " + opts->id);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // contacts bulk-create --file <path>
    // -----------------------------------------------------------------------
    {
        auto* cmd = contacts->add_subcommand("bulk-create", "Bulk create contacts from JSON file");

        auto file_path = std::make_shared<std::string>();
        cmd->add_option("--file,-f", *file_path, "Path to JSON file with contacts array")->required();

        cmd->callback([file_path]() {
            try {
                auto data = read_json_file(*file_path);

                // Accept either {"contacts": [...]} or a bare array [...]
                json contacts_arr;
                if (data.is_array()) {
                    contacts_arr = data;
                } else if (data.contains("contacts") && data["contacts"].is_array()) {
                    contacts_arr = data["contacts"];
                } else {
                    throw ApolloError(ErrorKind::Validation,
                        "JSON file must contain a contacts array or a top-level array of contact objects");
                }

                // Parse each item into ContactCreateInput
                std::vector<ContactCreateInput> inputs;
                inputs.reserve(contacts_arr.size());
                for (const auto& item : contacts_arr) {
                    ContactCreateInput input;
                    if (item.contains("first_name") && !item["first_name"].is_null()) {
                        input.first_name = item["first_name"].get<std::string>();
                    }
                    if (item.contains("last_name") && !item["last_name"].is_null()) {
                        input.last_name = item["last_name"].get<std::string>();
                    }
                    if (item.contains("email") && !item["email"].is_null()) {
                        input.email = item["email"].get<std::string>();
                    }
                    if (item.contains("title") && !item["title"].is_null()) {
                        input.title = item["title"].get<std::string>();
                    }
                    if (item.contains("organization_name") && !item["organization_name"].is_null()) {
                        input.organization_name = item["organization_name"].get<std::string>();
                    }
                    if (item.contains("account_id") && !item["account_id"].is_null()) {
                        input.account_id = item["account_id"].get<std::string>();
                    }
                    if (item.contains("owner_id") && !item["owner_id"].is_null()) {
                        input.owner_id = item["owner_id"].get<std::string>();
                    }
                    if (item.contains("linkedin_url") && !item["linkedin_url"].is_null()) {
                        input.linkedin_url = item["linkedin_url"].get<std::string>();
                    }
                    if (item.contains("present_raw_address") && !item["present_raw_address"].is_null()) {
                        input.present_raw_address = item["present_raw_address"].get<std::string>();
                    }
                    if (item.contains("contact_stage_id") && !item["contact_stage_id"].is_null()) {
                        input.contact_stage_id = item["contact_stage_id"].get<std::string>();
                    }
                    if (item.contains("label_names") && item["label_names"].is_array()) {
                        for (const auto& lbl : item["label_names"]) {
                            if (lbl.is_string()) input.label_names.push_back(lbl.get<std::string>());
                        }
                    }
                    if (item.contains("typed_custom_fields") && !item["typed_custom_fields"].is_null()) {
                        input.typed_custom_fields = item["typed_custom_fields"];
                    }
                    inputs.push_back(std::move(input));
                }

                auto contacts = contacts_api::bulk_create(inputs);
                print_success("Created " + std::to_string(contacts.size()) + " contact(s)");

                if (get_output_format() == OutputFormat::Json) {
                    render_contact_json(contacts);
                } else {
                    render_contact_table(contacts);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // contacts bulk-update --file <path>
    // -----------------------------------------------------------------------
    {
        auto* cmd = contacts->add_subcommand("bulk-update", "Bulk update contacts from JSON file");

        auto file_path = std::make_shared<std::string>();
        cmd->add_option("--file,-f", *file_path, "Path to JSON file with contacts array (each must have 'id')")->required();

        cmd->callback([file_path]() {
            try {
                auto data = read_json_file(*file_path);

                // Accept either {"contacts": [...]} or a bare array [...]
                json contacts_arr;
                if (data.is_array()) {
                    contacts_arr = data;
                } else if (data.contains("contacts") && data["contacts"].is_array()) {
                    contacts_arr = data["contacts"];
                } else {
                    throw ApolloError(ErrorKind::Validation,
                        "JSON file must contain a contacts array or a top-level array of contact objects");
                }

                // Validate each item has an id
                for (const auto& item : contacts_arr) {
                    if (!item.contains("id") || item["id"].is_null() || !item["id"].is_string()) {
                        throw ApolloError(ErrorKind::Validation,
                            "Each contact in bulk update must have a string 'id' field");
                    }
                }

                auto response = contacts_api::bulk_update(contacts_arr);

                // Parse response contacts
                std::vector<Contact> contacts;
                if (response.contains("contacts") && response["contacts"].is_array()) {
                    for (const auto& item : response["contacts"]) {
                        Contact contact;
                        from_json(item, contact);
                        contacts.push_back(std::move(contact));
                    }
                }

                print_success("Updated " + std::to_string(contacts.size()) + " contact(s)");

                if (get_output_format() == OutputFormat::Json) {
                    output_json(response);
                } else {
                    render_contact_table(contacts);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }
}
