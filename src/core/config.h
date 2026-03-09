#pragma once

#include <string>

struct Config {
    std::string api_key;

    struct Defaults {
        std::string output = "table";
        int per_page = 25;
    } defaults;

    struct Display {
        bool color = true;
        int page_size = 25;
        std::string date_format = "iso";  // "iso" or "relative"
    } display;
};

// Load config from ~/.config/apollo/config.toml. Returns default Config if file doesn't exist.
Config load_config();

// Save config to ~/.config/apollo/config.toml. Creates directories if needed.
// Sets file permissions to 600 (owner read/write only).
void save_config(const Config& config);

// Returns the config file path: ~/.config/apollo/config.toml
std::string config_path();

// Returns the config directory: ~/.config/apollo/
std::string config_dir();

// Returns true if the config file exists on disk.
bool config_exists();
