#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace resolve {

class Resolver {
public:
    Resolver();

    // Resolve IDs to human-readable names. Returns raw ID if not found.
    std::string user_name(const std::string& id) const;
    std::string stage_name(const std::string& id) const;
    std::string label_name(const std::string& id) const;
    std::string label_names(const std::vector<std::string>& ids) const;

private:
    std::unordered_map<std::string, std::string> users_;
    std::unordered_map<std::string, std::string> stages_;
    std::unordered_map<std::string, std::string> labels_;

    void load_users();
    void load_stages();
    void load_labels();
};

// Singleton accessor -- lazy initialized on first call
const Resolver& get_resolver();

}  // namespace resolve
