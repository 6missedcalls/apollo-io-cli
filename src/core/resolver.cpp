#include "core/resolver.h"

#include <string>
#include <vector>

#include "core/cache.h"
#include "core/error.h"
#include "core/output.h"
#include "modules/labels/api.h"
#include "modules/stages/api.h"
#include "modules/users/api.h"

namespace resolve {

// ---------------------------------------------------------------------------
// Helpers: convert API models to cache structs
// ---------------------------------------------------------------------------

static cache::CachedUser to_cached(const ApolloUser& u) {
    return {u.id, u.name, u.email};
}

static cache::CachedStage to_cached(const Stage& s, const std::string& type) {
    return {
        s.id,
        s.display_name.empty() ? s.name : s.display_name,
        type,
        s.display_order
    };
}

static cache::CachedLabel to_cached(const Label& l) {
    return {l.id, l.name, l.modality};
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

Resolver::Resolver() {
    load_users();
    load_stages();
    load_labels();
}

// ---------------------------------------------------------------------------
// load_users
// ---------------------------------------------------------------------------

void Resolver::load_users() {
    auto& c = cache::get_cache();

    if (c.is_stale("users")) {
        try {
            auto api_users = users_api::list_users();
            std::vector<cache::CachedUser> cached;
            cached.reserve(api_users.size());
            for (const auto& u : api_users) {
                cached.push_back(to_cached(u));
            }
            c.store_users(cached);
        } catch (const ApolloError& e) {
            print_verbose("resolver: failed to refresh users cache: " + std::string(e.what()));
        }
    }

    auto users = c.get_users();
    for (const auto& u : users) {
        users_.emplace(u.id, u.name);
    }
}

// ---------------------------------------------------------------------------
// load_stages
// ---------------------------------------------------------------------------

void Resolver::load_stages() {
    auto& c = cache::get_cache();

    // Refresh each stage type independently
    struct StageType {
        std::string type;
        std::vector<Stage> (*fetch)();
    };

    const StageType stage_types[] = {
        {"contact", stages_api::list_contact_stages},
        {"account", stages_api::list_account_stages},
        {"deal",    stages_api::list_deal_stages},
    };

    for (const auto& st : stage_types) {
        if (c.is_stale("stages_" + st.type)) {
            try {
                auto api_stages = st.fetch();
                std::vector<cache::CachedStage> cached;
                cached.reserve(api_stages.size());
                for (const auto& s : api_stages) {
                    cached.push_back(to_cached(s, st.type));
                }
                c.store_stages(cached, st.type);
            } catch (const ApolloError& e) {
                print_verbose("resolver: failed to refresh " + st.type +
                              " stages cache: " + std::string(e.what()));
            }
        }
    }

    // Build one unified map from all stage types
    for (const auto& type : {"contact", "account", "deal"}) {
        auto stages = c.get_stages(type);
        for (const auto& s : stages) {
            stages_.emplace(s.id, s.name);
        }
    }
}

// ---------------------------------------------------------------------------
// load_labels
// ---------------------------------------------------------------------------

void Resolver::load_labels() {
    auto& c = cache::get_cache();

    if (c.is_stale("labels")) {
        try {
            auto api_labels = labels_api::list_labels();
            std::vector<cache::CachedLabel> cached;
            cached.reserve(api_labels.size());
            for (const auto& l : api_labels) {
                cached.push_back(to_cached(l));
            }
            c.store_labels(cached);
        } catch (const ApolloError& e) {
            print_verbose("resolver: failed to refresh labels cache: " + std::string(e.what()));
        }
    }

    auto labels = c.get_labels();
    for (const auto& l : labels) {
        labels_.emplace(l.id, l.name);
    }
}

// ---------------------------------------------------------------------------
// Resolution methods
// ---------------------------------------------------------------------------

std::string Resolver::user_name(const std::string& id) const {
    if (id.empty()) {
        return "";
    }
    auto it = users_.find(id);
    return it != users_.end() ? it->second : id;
}

std::string Resolver::stage_name(const std::string& id) const {
    if (id.empty()) {
        return "";
    }
    auto it = stages_.find(id);
    return it != stages_.end() ? it->second : id;
}

std::string Resolver::label_name(const std::string& id) const {
    if (id.empty()) {
        return "";
    }
    auto it = labels_.find(id);
    return it != labels_.end() ? it->second : id;
}

std::string Resolver::label_names(const std::vector<std::string>& ids) const {
    if (ids.empty()) {
        return "";
    }
    std::string result;
    for (size_t i = 0; i < ids.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += label_name(ids[i]);
    }
    return result;
}

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

const Resolver& get_resolver() {
    static Resolver instance;
    return instance;
}

}  // namespace resolve
