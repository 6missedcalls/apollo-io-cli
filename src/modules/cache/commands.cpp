#include "modules/cache/commands.h"

#include <iostream>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/cache.h"
#include "core/error.h"
#include "core/output.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

struct TableStatus {
    std::string name;
    size_t record_count = 0;
    bool is_stale = false;
    bool is_empty = false;
};

void render_status_table(const std::vector<TableStatus>& statuses) {
    TableRenderer table({
        {"TABLE",   5, 20, false},
        {"RECORDS", 4, 10, true},
        {"STALE",   4,  8, false}
    });

    for (const auto& s : statuses) {
        table.add_row({
            s.name,
            s.is_empty ? "0" : std::to_string(s.record_count),
            s.is_empty ? "never populated" : (s.is_stale ? "yes" : "no")
        });
    }

    table.render(std::cout);
}

void render_status_json(const std::vector<TableStatus>& statuses) {
    json arr = json::array();
    for (const auto& s : statuses) {
        json j;
        j["table"] = s.name;
        j["records"] = static_cast<int>(s.record_count);
        j["stale"] = s.is_stale;
        j["neverPopulated"] = s.is_empty;
        arr.push_back(j);
    }
    output_json(arr);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void cache_commands::register_commands(CLI::App& app) {
    auto* cache_cmd = app.add_subcommand("cache", "Manage local cache of Apollo data");
    cache_cmd->require_subcommand(1);

    // -----------------------------------------------------------------------
    // cache clear
    // -----------------------------------------------------------------------
    {
        auto* cmd = cache_cmd->add_subcommand("clear", "Clear all cached data");

        cmd->callback([]() {
            try {
                cache::get_cache().invalidate_all();
                print_success("Cache cleared.");
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            } catch (const std::exception& e) {
                print_error(std::string("Failed to clear cache: ") + e.what());
                throw std::runtime_error(std::string("Cache clear failed: ") + e.what());
            }
        });
    }

    // -----------------------------------------------------------------------
    // cache status
    // -----------------------------------------------------------------------
    {
        auto* cmd = cache_cmd->add_subcommand("status", "Show staleness status for all cache tables");

        cmd->callback([]() {
            try {
                auto& c = cache::get_cache();

                const std::vector<std::string> table_names = {
                    "users", "labels", "fields"
                };

                std::vector<TableStatus> statuses;
                statuses.reserve(table_names.size());

                for (const auto& name : table_names) {
                    TableStatus status;
                    status.name = name;
                    status.is_stale = c.is_stale(name);

                    if (name == "users") {
                        auto records = c.get_users();
                        status.record_count = records.size();
                        status.is_empty = records.empty();
                    } else if (name == "labels") {
                        auto records = c.get_labels();
                        status.record_count = records.size();
                        status.is_empty = records.empty();
                    } else if (name == "fields") {
                        auto records = c.get_fields();
                        status.record_count = records.size();
                        status.is_empty = records.empty();
                    }

                    statuses.push_back(std::move(status));
                }

                if (get_output_format() == OutputFormat::Json) {
                    render_status_json(statuses);
                } else {
                    render_status_table(statuses);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            } catch (const std::exception& e) {
                print_error(std::string("Failed to read cache status: ") + e.what());
                throw std::runtime_error(std::string("Cache status failed: ") + e.what());
            }
        });
    }
}
