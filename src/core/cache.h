#pragma once

#include <optional>
#include <string>
#include <vector>

struct sqlite3;

namespace cache {

// Lightweight cache structs -- just what's needed for name/ID resolution

struct CachedUser {
    std::string id;
    std::string name;
    std::string email;
};

struct CachedStage {
    std::string id;
    std::string name;
    std::string type;          // "contact", "account", "deal"
    int display_order = 0;
};

struct CachedLabel {
    std::string id;
    std::string name;
    std::string modality;      // "contacts", "accounts", "emailer_campaigns"
};

struct CachedField {
    std::string id;
    std::string name;
    std::string field_type;
    std::string modality;
};

class Cache {
public:
    Cache();   // Opens/creates ~/.config/capollo/cache.db, runs migrations
    ~Cache();  // Closes db

    // Non-copyable, movable
    Cache(const Cache&) = delete;
    Cache& operator=(const Cache&) = delete;
    Cache(Cache&& other) noexcept;
    Cache& operator=(Cache&& other) noexcept;

    // Users
    void store_users(const std::vector<CachedUser>& users);
    std::vector<CachedUser> get_users();
    std::optional<CachedUser> find_user_by_name(const std::string& name);
    std::optional<CachedUser> find_user_by_email(const std::string& email);

    // Stages
    void store_stages(const std::vector<CachedStage>& stages, const std::string& type);
    std::vector<CachedStage> get_stages(const std::string& type);
    std::optional<CachedStage> find_stage_by_name(
        const std::string& name, const std::string& type);

    // Labels
    void store_labels(const std::vector<CachedLabel>& labels);
    std::vector<CachedLabel> get_labels(const std::string& modality = "");
    std::optional<CachedLabel> find_label_by_name(const std::string& name);

    // Fields
    void store_fields(const std::vector<CachedField>& fields);
    std::vector<CachedField> get_fields(const std::string& modality = "");
    std::optional<CachedField> find_field_by_name(const std::string& name);

    // Cache management
    bool is_stale(const std::string& table, int max_age_minutes = 60);
    void invalidate(const std::string& table);
    void invalidate_all();

    // Check if database is open
    bool is_open() const;

private:
    sqlite3* db_ = nullptr;

    void create_tables();
    void update_timestamp(const std::string& table);
    void exec(const std::string& sql);
};

// Get the singleton cache instance
Cache& get_cache();

// Get the cache database path (~/.config/capollo/cache.db)
std::string cache_path();

}  // namespace cache
