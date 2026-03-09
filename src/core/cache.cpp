#include "core/cache.h"

#include <sqlite3.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace cache {

namespace {

struct StmtGuard {
    sqlite3_stmt* stmt = nullptr;
    ~StmtGuard() { if (stmt) sqlite3_finalize(stmt); }
};

struct TransactionGuard {
    sqlite3* db;
    bool committed = false;
    explicit TransactionGuard(sqlite3* db) : db(db) {
        sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);
    }
    void commit() {
        sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
        committed = true;
    }
    ~TransactionGuard() {
        if (!committed) {
            sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        }
    }
};

} // namespace

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string col_text(sqlite3_stmt* stmt, int col) {
    const unsigned char* raw = sqlite3_column_text(stmt, col);
    return raw ? std::string(reinterpret_cast<const char*>(raw)) : std::string{};
}

// ---------------------------------------------------------------------------
// cache_path
// ---------------------------------------------------------------------------

std::string cache_path() {
    const char* home = std::getenv("HOME");
    if (home == nullptr) {
        throw std::runtime_error("HOME environment variable is not set");
    }
    std::string dir = std::string(home) + "/.config/apollo";
    std::filesystem::create_directories(dir);
    return dir + "/cache.db";
}

// ---------------------------------------------------------------------------
// Constructor / Destructor / Move
// ---------------------------------------------------------------------------

Cache::Cache() {
    int rc = sqlite3_open(cache_path().c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string msg = "Failed to open cache database: ";
        if (db_) {
            msg += sqlite3_errmsg(db_);
            sqlite3_close(db_);
            db_ = nullptr;
        } else {
            msg += "out of memory";
        }
        throw std::runtime_error(msg);
    }

    // WAL mode for better concurrency and performance
    exec("PRAGMA journal_mode=WAL");
    exec("PRAGMA synchronous=NORMAL");
    exec("PRAGMA foreign_keys=ON");

    create_tables();
}

Cache::~Cache() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

Cache::Cache(Cache&& other) noexcept : db_(other.db_) {
    other.db_ = nullptr;
}

Cache& Cache::operator=(Cache&& other) noexcept {
    if (this != &other) {
        if (db_) {
            sqlite3_close(db_);
        }
        db_ = other.db_;
        other.db_ = nullptr;
    }
    return *this;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void Cache::exec(const std::string& sql) {
    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK) {
        std::string msg = "SQLite error: ";
        if (errmsg) {
            msg += errmsg;
            sqlite3_free(errmsg);
        }
        throw std::runtime_error(msg);
    }
}

void Cache::create_tables() {
    exec(R"sql(
        CREATE TABLE IF NOT EXISTS cache_meta (
            table_name TEXT PRIMARY KEY,
            updated_at INTEGER NOT NULL
        )
    )sql");

    exec(R"sql(
        CREATE TABLE IF NOT EXISTS users (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            email TEXT NOT NULL
        )
    )sql");

    exec(R"sql(
        CREATE TABLE IF NOT EXISTS stages (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            type TEXT NOT NULL,
            display_order INTEGER DEFAULT 0
        )
    )sql");

    exec(R"sql(
        CREATE TABLE IF NOT EXISTS labels (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            modality TEXT NOT NULL
        )
    )sql");

    exec(R"sql(
        CREATE TABLE IF NOT EXISTS fields (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            field_type TEXT NOT NULL,
            modality TEXT NOT NULL
        )
    )sql");

    // Indices for frequent lookup patterns
    exec("CREATE INDEX IF NOT EXISTS idx_users_name ON users(name)");
    exec("CREATE INDEX IF NOT EXISTS idx_users_email ON users(email)");
    exec("CREATE INDEX IF NOT EXISTS idx_stages_type ON stages(type)");
    exec("CREATE INDEX IF NOT EXISTS idx_stages_name ON stages(name)");
    exec("CREATE INDEX IF NOT EXISTS idx_labels_name ON labels(name)");
    exec("CREATE INDEX IF NOT EXISTS idx_labels_modality ON labels(modality)");
    exec("CREATE INDEX IF NOT EXISTS idx_fields_name ON fields(name)");
    exec("CREATE INDEX IF NOT EXISTS idx_fields_modality ON fields(modality)");
}

void Cache::update_timestamp(const std::string& table) {
    auto now = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "INSERT OR REPLACE INTO cache_meta (table_name, updated_at) VALUES (?, ?)",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, table.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(guard.stmt, 2, static_cast<sqlite3_int64>(now));
    sqlite3_step(guard.stmt);
}

// ---------------------------------------------------------------------------
// is_open
// ---------------------------------------------------------------------------

bool Cache::is_open() const {
    return db_ != nullptr;
}

// ---------------------------------------------------------------------------
// Cache management
// ---------------------------------------------------------------------------

bool Cache::is_stale(const std::string& table, int max_age_minutes) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT updated_at FROM cache_meta WHERE table_name = ?",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, table.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        sqlite3_int64 updated = sqlite3_column_int64(guard.stmt, 0);

        auto now = std::chrono::system_clock::now();
        auto updated_time = std::chrono::system_clock::from_time_t(
            static_cast<std::time_t>(updated)
        );
        auto age = std::chrono::duration_cast<std::chrono::minutes>(
            now - updated_time);
        return age.count() >= max_age_minutes;
    }

    return true;  // No record means stale
}

void Cache::invalidate(const std::string& table) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "DELETE FROM cache_meta WHERE table_name = ?",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, table.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(guard.stmt);
}

void Cache::invalidate_all() {
    exec("DELETE FROM cache_meta");
}

// ---------------------------------------------------------------------------
// Users
// ---------------------------------------------------------------------------

void Cache::store_users(const std::vector<CachedUser>& users) {
    TransactionGuard txn(db_);
    exec("DELETE FROM users");

    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "INSERT INTO users (id, name, email) VALUES (?, ?, ?)",
        -1, &guard.stmt, nullptr
    );

    for (const auto& u : users) {
        sqlite3_bind_text(guard.stmt, 1, u.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 2, u.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 3, u.email.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_step(guard.stmt);
        sqlite3_reset(guard.stmt);
    }

    update_timestamp("users");
    txn.commit();
}

static CachedUser row_to_user(sqlite3_stmt* stmt) {
    CachedUser u;
    u.id    = col_text(stmt, 0);
    u.name  = col_text(stmt, 1);
    u.email = col_text(stmt, 2);
    return u;
}

std::vector<CachedUser> Cache::get_users() {
    std::vector<CachedUser> result;

    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, email FROM users ORDER BY name",
        -1, &guard.stmt, nullptr
    );

    while (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result.push_back(row_to_user(guard.stmt));
    }

    return result;
}

std::optional<CachedUser> Cache::find_user_by_name(const std::string& name) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, email FROM users WHERE name = ? COLLATE NOCASE",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedUser> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_user(guard.stmt);
    }

    return result;
}

std::optional<CachedUser> Cache::find_user_by_email(const std::string& email) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, email FROM users WHERE email = ? COLLATE NOCASE",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedUser> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_user(guard.stmt);
    }

    return result;
}

std::optional<CachedUser> Cache::find_user_by_id(const std::string& id) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, email FROM users WHERE id = ?",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedUser> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_user(guard.stmt);
    }

    return result;
}

// ---------------------------------------------------------------------------
// Stages
// ---------------------------------------------------------------------------

void Cache::store_stages(const std::vector<CachedStage>& stages,
                         const std::string& type) {
    TransactionGuard txn(db_);

    // Delete only stages of this type
    {
        StmtGuard del_guard;
        sqlite3_prepare_v2(
            db_,
            "DELETE FROM stages WHERE type = ?",
            -1, &del_guard.stmt, nullptr
        );
        sqlite3_bind_text(del_guard.stmt, 1, type.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(del_guard.stmt);
    }

    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "INSERT INTO stages (id, name, type, display_order) VALUES (?, ?, ?, ?)",
        -1, &guard.stmt, nullptr
    );

    for (const auto& s : stages) {
        sqlite3_bind_text(guard.stmt, 1, s.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 2, s.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 3, s.type.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(guard.stmt, 4, s.display_order);

        sqlite3_step(guard.stmt);
        sqlite3_reset(guard.stmt);
    }

    update_timestamp("stages_" + type);
    txn.commit();
}

static CachedStage row_to_stage(sqlite3_stmt* stmt) {
    CachedStage s;
    s.id            = col_text(stmt, 0);
    s.name          = col_text(stmt, 1);
    s.type          = col_text(stmt, 2);
    s.display_order = sqlite3_column_int(stmt, 3);
    return s;
}

std::vector<CachedStage> Cache::get_stages(const std::string& type) {
    std::vector<CachedStage> result;

    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, type, display_order FROM stages "
        "WHERE type = ? ORDER BY display_order",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, type.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result.push_back(row_to_stage(guard.stmt));
    }

    return result;
}

std::optional<CachedStage> Cache::find_stage_by_name(
    const std::string& name, const std::string& type) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, type, display_order FROM stages "
        "WHERE name = ? COLLATE NOCASE AND type = ?",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(guard.stmt, 2, type.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedStage> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_stage(guard.stmt);
    }

    return result;
}

std::optional<CachedStage> Cache::find_stage_by_id(const std::string& id) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, type, display_order FROM stages WHERE id = ?",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedStage> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_stage(guard.stmt);
    }

    return result;
}

// ---------------------------------------------------------------------------
// Labels
// ---------------------------------------------------------------------------

void Cache::store_labels(const std::vector<CachedLabel>& labels) {
    TransactionGuard txn(db_);
    exec("DELETE FROM labels");

    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "INSERT INTO labels (id, name, modality) VALUES (?, ?, ?)",
        -1, &guard.stmt, nullptr
    );

    for (const auto& l : labels) {
        sqlite3_bind_text(guard.stmt, 1, l.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 2, l.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 3, l.modality.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_step(guard.stmt);
        sqlite3_reset(guard.stmt);
    }

    update_timestamp("labels");
    txn.commit();
}

static CachedLabel row_to_label(sqlite3_stmt* stmt) {
    CachedLabel l;
    l.id       = col_text(stmt, 0);
    l.name     = col_text(stmt, 1);
    l.modality = col_text(stmt, 2);
    return l;
}

std::vector<CachedLabel> Cache::get_labels(const std::string& modality) {
    std::vector<CachedLabel> result;

    StmtGuard guard;
    if (modality.empty()) {
        sqlite3_prepare_v2(
            db_,
            "SELECT id, name, modality FROM labels ORDER BY name",
            -1, &guard.stmt, nullptr
        );
    } else {
        sqlite3_prepare_v2(
            db_,
            "SELECT id, name, modality FROM labels "
            "WHERE modality = ? ORDER BY name",
            -1, &guard.stmt, nullptr
        );
        sqlite3_bind_text(guard.stmt, 1, modality.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result.push_back(row_to_label(guard.stmt));
    }

    return result;
}

std::optional<CachedLabel> Cache::find_label_by_name(const std::string& name) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, modality FROM labels "
        "WHERE name = ? COLLATE NOCASE",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedLabel> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_label(guard.stmt);
    }

    return result;
}

std::optional<CachedLabel> Cache::find_label_by_id(const std::string& id) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, modality FROM labels WHERE id = ?",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedLabel> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_label(guard.stmt);
    }

    return result;
}

// ---------------------------------------------------------------------------
// Fields
// ---------------------------------------------------------------------------

void Cache::store_fields(const std::vector<CachedField>& fields) {
    TransactionGuard txn(db_);
    exec("DELETE FROM fields");

    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "INSERT INTO fields (id, name, field_type, modality) VALUES (?, ?, ?, ?)",
        -1, &guard.stmt, nullptr
    );

    for (const auto& f : fields) {
        sqlite3_bind_text(guard.stmt, 1, f.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 2, f.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 3, f.field_type.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(guard.stmt, 4, f.modality.c_str(), -1, SQLITE_TRANSIENT);

        sqlite3_step(guard.stmt);
        sqlite3_reset(guard.stmt);
    }

    update_timestamp("fields");
    txn.commit();
}

static CachedField row_to_field(sqlite3_stmt* stmt) {
    CachedField f;
    f.id         = col_text(stmt, 0);
    f.name       = col_text(stmt, 1);
    f.field_type = col_text(stmt, 2);
    f.modality   = col_text(stmt, 3);
    return f;
}

std::vector<CachedField> Cache::get_fields(const std::string& modality) {
    std::vector<CachedField> result;

    StmtGuard guard;
    if (modality.empty()) {
        sqlite3_prepare_v2(
            db_,
            "SELECT id, name, field_type, modality FROM fields ORDER BY name",
            -1, &guard.stmt, nullptr
        );
    } else {
        sqlite3_prepare_v2(
            db_,
            "SELECT id, name, field_type, modality FROM fields "
            "WHERE modality = ? ORDER BY name",
            -1, &guard.stmt, nullptr
        );
        sqlite3_bind_text(guard.stmt, 1, modality.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result.push_back(row_to_field(guard.stmt));
    }

    return result;
}

std::optional<CachedField> Cache::find_field_by_name(const std::string& name) {
    StmtGuard guard;
    sqlite3_prepare_v2(
        db_,
        "SELECT id, name, field_type, modality FROM fields "
        "WHERE name = ? COLLATE NOCASE",
        -1, &guard.stmt, nullptr
    );
    sqlite3_bind_text(guard.stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<CachedField> result;
    if (sqlite3_step(guard.stmt) == SQLITE_ROW) {
        result = row_to_field(guard.stmt);
    }

    return result;
}

// ---------------------------------------------------------------------------
// Singleton
// ---------------------------------------------------------------------------

Cache& get_cache() {
    static Cache instance;
    return instance;
}

}  // namespace cache
