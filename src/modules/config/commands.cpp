#include "modules/config/commands.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>

#include <CLI/CLI.hpp>

#include "core/auth.h"
#include "core/config.h"
#include "core/output.h"

// ---------------------------------------------------------------------------
// Valid key constants
// ---------------------------------------------------------------------------

namespace {

const std::set<std::string> VALID_KEYS = {
    "api_key",
    "defaults.output",
    "defaults.per_page",
    "display.color",
    "display.page_size",
    "display.date_format"
};

const std::set<std::string> VALID_OUTPUT_FORMATS = {"table", "json", "csv"};
const std::set<std::string> VALID_DATE_FORMATS = {"iso", "relative", "short"};

// Returns a human-readable string for the given config key's value.
std::string get_value_for_key(const Config& cfg, const std::string& key) {
    if (key == "api_key") {
        if (cfg.api_key.empty()) return "(not set)";
        // Mask the key for security
        if (cfg.api_key.size() > 8) {
            return cfg.api_key.substr(0, 4) + "..." + cfg.api_key.substr(cfg.api_key.size() - 4);
        }
        return cfg.api_key;
    }
    if (key == "defaults.output") {
        return cfg.defaults.output;
    }
    if (key == "defaults.per_page") {
        return std::to_string(cfg.defaults.per_page);
    }
    if (key == "display.color") {
        return cfg.display.color ? "true" : "false";
    }
    if (key == "display.page_size") {
        return std::to_string(cfg.display.page_size);
    }
    if (key == "display.date_format") {
        return cfg.display.date_format;
    }
    return "";
}

// Apply a string value to the given config key. Returns updated Config.
// Throws std::invalid_argument on validation failure.
Config apply_key_value(const Config& cfg, const std::string& key, const std::string& value) {
    Config updated = cfg;

    if (key == "api_key") {
        if (!validate_api_key(value)) {
            throw std::invalid_argument("Invalid API key. Keys must be non-empty.");
        }
        updated.api_key = value;
        return updated;
    }

    if (key == "defaults.output") {
        if (VALID_OUTPUT_FORMATS.find(value) == VALID_OUTPUT_FORMATS.end()) {
            throw std::invalid_argument(
                "Invalid output format '" + value + "'. Valid values: table, json, csv"
            );
        }
        updated.defaults.output = value;
        return updated;
    }

    if (key == "defaults.per_page") {
        try {
            int per_page = std::stoi(value);
            if (per_page < 1 || per_page > 100) {
                throw std::invalid_argument("per_page must be between 1 and 100.");
            }
            updated.defaults.per_page = per_page;
        } catch (const std::out_of_range&) {
            throw std::invalid_argument("per_page value out of range.");
        } catch (const std::invalid_argument& e) {
            std::string msg = e.what();
            if (msg.find("per_page") != std::string::npos) {
                throw;
            }
            throw std::invalid_argument(
                "Invalid per_page '" + value + "'. Must be an integer."
            );
        }
        return updated;
    }

    if (key == "display.color") {
        if (value == "true" || value == "1" || value == "yes") {
            updated.display.color = true;
        } else if (value == "false" || value == "0" || value == "no") {
            updated.display.color = false;
        } else {
            throw std::invalid_argument(
                "Invalid color value '" + value + "'. Valid values: true, false"
            );
        }
        return updated;
    }

    if (key == "display.page_size") {
        try {
            int page_size = std::stoi(value);
            if (page_size < 1 || page_size > 250) {
                throw std::invalid_argument("page_size must be between 1 and 250.");
            }
            updated.display.page_size = page_size;
        } catch (const std::out_of_range&) {
            throw std::invalid_argument("page_size value out of range.");
        } catch (const std::invalid_argument& e) {
            std::string msg = e.what();
            if (msg.find("page_size") != std::string::npos) {
                throw;
            }
            throw std::invalid_argument(
                "Invalid page_size '" + value + "'. Must be an integer."
            );
        }
        return updated;
    }

    if (key == "display.date_format") {
        if (VALID_DATE_FORMATS.find(value) == VALID_DATE_FORMATS.end()) {
            throw std::invalid_argument(
                "Invalid date_format '" + value + "'. Valid values: iso, relative, short"
            );
        }
        updated.display.date_format = value;
        return updated;
    }

    throw std::invalid_argument("Unknown config key: " + key);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void config_commands::register_commands(CLI::App& app) {
    auto* config = app.add_subcommand("config", "Manage capollo CLI configuration");
    config->require_subcommand(1);

    // -----------------------------------------------------------------------
    // config init
    // -----------------------------------------------------------------------
    {
        auto* cmd = config->add_subcommand("init", "Interactive setup: configure capollo with your Apollo API key");

        cmd->callback([]() {
            try {
                std::cout << "Enter your Apollo API key: ";
                std::string key;
                std::getline(std::cin, key);

                if (key.empty()) {
                    print_error("No API key provided. Setup aborted.");
                    return;
                }

                if (!validate_api_key(key)) {
                    print_error(
                        "Invalid API key. Keys must be non-empty. "
                        "You can find your API key at https://app.apollo.io/#/settings/integrations/api"
                    );
                    return;
                }

                Config cfg = load_config();
                cfg.api_key = key;
                save_config(cfg);

                print_success("Configuration saved to " + config_path());
                print_success("API key set successfully.");
            } catch (const std::exception& e) {
                print_error(std::string("Failed to save configuration: ") + e.what());
                throw std::runtime_error(std::string("Configuration setup failed: ") + e.what());
            }
        });
    }

    // -----------------------------------------------------------------------
    // config set <key> <value>
    // -----------------------------------------------------------------------
    {
        auto* cmd = config->add_subcommand("set", "Set a configuration value");

        auto opts = std::make_shared<std::pair<std::string, std::string>>();
        cmd->add_option("key", opts->first, "Config key (e.g., defaults.output)")->required();
        cmd->add_option("value", opts->second, "Value to set")->required();

        cmd->callback([opts]() {
            try {
                const std::string& key = opts->first;
                const std::string& value = opts->second;

                if (VALID_KEYS.find(key) == VALID_KEYS.end()) {
                    print_error(
                        "Unknown config key: '" + key + "'. Valid keys:\n"
                        "  api_key\n"
                        "  defaults.output\n"
                        "  defaults.per_page\n"
                        "  display.color\n"
                        "  display.page_size\n"
                        "  display.date_format"
                    );
                    return;
                }

                Config cfg = load_config();
                Config updated = apply_key_value(cfg, key, value);
                save_config(updated);

                print_success("Set " + key + " = " + value);
            } catch (const std::invalid_argument& e) {
                print_error(e.what());
            } catch (const std::exception& e) {
                print_error(std::string("Failed to update configuration: ") + e.what());
                throw std::runtime_error(std::string("Configuration update failed: ") + e.what());
            }
        });
    }

    // -----------------------------------------------------------------------
    // config get <key>
    // -----------------------------------------------------------------------
    {
        auto* cmd = config->add_subcommand("get", "Get a configuration value");

        auto key = std::make_shared<std::string>();
        cmd->add_option("key", *key, "Config key to retrieve")->required();

        cmd->callback([key]() {
            try {
                if (VALID_KEYS.find(*key) == VALID_KEYS.end()) {
                    print_error(
                        "Unknown config key: '" + *key + "'. Valid keys:\n"
                        "  api_key\n"
                        "  defaults.output\n"
                        "  defaults.per_page\n"
                        "  display.color\n"
                        "  display.page_size\n"
                        "  display.date_format"
                    );
                    return;
                }

                Config cfg = load_config();
                std::string value = get_value_for_key(cfg, *key);
                std::cout << value << std::endl;
            } catch (const std::exception& e) {
                print_error(std::string("Failed to read configuration: ") + e.what());
                throw std::runtime_error(std::string("Configuration read failed: ") + e.what());
            }
        });
    }

    // -----------------------------------------------------------------------
    // config list
    // -----------------------------------------------------------------------
    {
        auto* cmd = config->add_subcommand("list", "List all configuration key-value pairs");

        cmd->callback([]() {
            try {
                Config cfg = load_config();

                TableRenderer table({
                    {"KEY",   8, 30, false},
                    {"VALUE", 4, 60, false}
                });

                // Mask API key
                std::string masked_key;
                if (cfg.api_key.empty()) {
                    masked_key = "(not set)";
                } else if (cfg.api_key.size() > 8) {
                    masked_key = cfg.api_key.substr(0, 4) + "..."
                        + cfg.api_key.substr(cfg.api_key.size() - 4);
                } else {
                    masked_key = cfg.api_key;
                }

                table.add_row({"api_key",             masked_key});
                table.add_row({"defaults.output",      cfg.defaults.output});
                table.add_row({"defaults.per_page",    std::to_string(cfg.defaults.per_page)});
                table.add_row({"display.color",        cfg.display.color ? "true" : "false"});
                table.add_row({"display.page_size",    std::to_string(cfg.display.page_size)});
                table.add_row({"display.date_format",  cfg.display.date_format});

                table.render(std::cout);
            } catch (const std::exception& e) {
                print_error(std::string("Failed to read configuration: ") + e.what());
                throw std::runtime_error(std::string("Configuration list failed: ") + e.what());
            }
        });
    }
}
