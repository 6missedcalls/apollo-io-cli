#include "modules/enrichment/commands.h"

#include <iostream>
#include <memory>
#include <string>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/output.h"
#include "modules/enrichment/api.h"
#include "modules/enrichment/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_person_detail(const PersonMatch& person) {
    if (get_output_format() == OutputFormat::Json) {
        output_json(person.raw);
        return;
    }

    DetailRenderer detail;
    detail.add_section(person.name);

    if (person.email.has_value()) {
        detail.add_field("Email", person.email.value());
    }
    if (person.title.has_value()) {
        detail.add_field("Title", person.title.value());
    }
    if (person.headline.has_value()) {
        detail.add_field("Headline", person.headline.value());
    }
    if (person.organization_name.has_value()) {
        detail.add_field("Organization", person.organization_name.value());
    }
    if (person.seniority.has_value()) {
        detail.add_field("Seniority", person.seniority.value());
    }
    if (person.linkedin_url.has_value()) {
        detail.add_field("LinkedIn", person.linkedin_url.value());
    }

    // Location
    std::string location;
    if (person.city.has_value()) location += person.city.value();
    if (person.state.has_value()) {
        if (!location.empty()) location += ", ";
        location += person.state.value();
    }
    if (person.country.has_value()) {
        if (!location.empty()) location += ", ";
        location += person.country.value();
    }
    if (!location.empty()) {
        detail.add_field("Location", location);
    }

    detail.add_field("ID", person.id);

    detail.render(std::cout);
}

void render_org_detail(const OrgEnrichment& org) {
    if (get_output_format() == OutputFormat::Json) {
        output_json(org.raw);
        return;
    }

    DetailRenderer detail;
    detail.add_section(org.name);

    if (org.domain.has_value()) {
        detail.add_field("Domain", org.domain.value());
    }
    if (org.industry.has_value()) {
        detail.add_field("Industry", org.industry.value());
    }
    if (org.employee_count.has_value()) {
        detail.add_field("Employees", std::to_string(org.employee_count.value()));
    }
    if (org.founded_year.has_value()) {
        detail.add_field("Founded", std::to_string(org.founded_year.value()));
    }
    if (org.annual_revenue_printed.has_value()) {
        detail.add_field("Revenue", org.annual_revenue_printed.value());
    }

    if (org.phone.has_value()) {
        detail.add_field("Phone", org.phone.value());
    }
    if (org.linkedin_url.has_value()) {
        detail.add_field("LinkedIn", org.linkedin_url.value());
    }
    if (org.twitter_url.has_value()) {
        detail.add_field("Twitter", org.twitter_url.value());
    }

    // Location
    std::string location;
    if (org.city.has_value()) location += org.city.value();
    if (org.state.has_value()) {
        if (!location.empty()) location += ", ";
        location += org.state.value();
    }
    if (org.country.has_value()) {
        if (!location.empty()) location += ", ";
        location += org.country.value();
    }
    if (!location.empty()) {
        detail.add_field("Location", location);
    }

    if (org.short_description.has_value() && !org.short_description.value().empty()) {
        detail.add_blank_line();
        detail.add_field("Description", org.short_description.value());
    } else if (org.description.has_value() && !org.description.value().empty()) {
        detail.add_blank_line();
        detail.add_field("Description", org.description.value());
    }

    // Funding
    if (org.funding_data.is_array() && !org.funding_data.empty()) {
        detail.add_blank_line();
        detail.add_section("Funding");
        for (const auto& round : org.funding_data) {
            std::string round_str;
            if (round.contains("type") && !round["type"].is_null()) {
                round_str += round["type"].get<std::string>();
            }
            if (round.contains("amount") && !round["amount"].is_null()) {
                round_str += " - $" + round["amount"].get<std::string>();
            }
            if (round.contains("date") && !round["date"].is_null()) {
                std::string date = round["date"].get<std::string>();
                if (date.size() >= 10) {
                    round_str += " (" + date.substr(0, 10) + ")";
                }
            }
            if (!round_str.empty()) {
                detail.add_field("", round_str);
            }
        }
    }

    detail.add_blank_line();
    detail.add_field("ID", org.id);

    detail.render(std::cout);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void enrichment_commands::register_commands(CLI::App& app) {
    auto* enrichment = app.add_subcommand("enrichment", "Enrich people and organizations");
    enrichment->require_subcommand(1);

    // -----------------------------------------------------------------------
    // enrichment people
    // -----------------------------------------------------------------------
    {
        auto* cmd = enrichment->add_subcommand("people", "Look up a person by email, name, or LinkedIn");

        struct PeopleOpts {
            std::string email;
            std::string first_name;
            std::string last_name;
            std::string org;
            std::string domain;
            std::string linkedin;
        };
        auto opts = std::make_shared<PeopleOpts>();

        cmd->add_option("--email", opts->email, "Email address");
        cmd->add_option("--first-name", opts->first_name, "First name");
        cmd->add_option("--last-name", opts->last_name, "Last name");
        cmd->add_option("--org", opts->org, "Organization name");
        cmd->add_option("--domain", opts->domain, "Company domain");
        cmd->add_option("--linkedin", opts->linkedin, "LinkedIn URL");

        cmd->callback([opts]() {
            try {
                json body = json::object();
                if (!opts->email.empty()) body["email"] = opts->email;
                if (!opts->first_name.empty()) body["first_name"] = opts->first_name;
                if (!opts->last_name.empty()) body["last_name"] = opts->last_name;
                if (!opts->org.empty()) body["organization_name"] = opts->org;
                if (!opts->domain.empty()) body["domain"] = opts->domain;
                if (!opts->linkedin.empty()) body["linkedin_url"] = opts->linkedin;

                if (body.empty()) {
                    print_error("At least one identifier is required (--email, --first-name + --last-name, --linkedin, etc.)");
                    return;
                }

                auto person = enrichment_api::people_match(body);
                render_person_detail(person);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -----------------------------------------------------------------------
    // enrichment organizations
    // -----------------------------------------------------------------------
    {
        auto* cmd = enrichment->add_subcommand("organizations", "Look up an organization by domain");

        auto domain = std::make_shared<std::string>();
        cmd->add_option("--domain", *domain, "Company domain (e.g., apollo.io)")->required();

        cmd->callback([domain]() {
            try {
                auto org = enrichment_api::org_enrich(*domain);
                render_org_detail(org);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }
}
