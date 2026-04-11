#include "database.h"
#include <stdexcept>
#include <sstream>

// ─────────────────────────────────────────────
//  Helpers
// ─────────────────────────────────────────────

void Database::checkRC(int rc, const std::string& context) const {
    if (rc != SQLITE_OK && rc != SQLITE_ROW && rc != SQLITE_DONE) {
        std::string msg = context.empty() ? "" : "[" + context + "] ";
        msg += sqlite3_errmsg(db_);
        throw std::runtime_error(msg);
    }
}

// ─────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────

Database::Database(const std::string& path) : path_(path) {
    int rc = sqlite3_open(path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        throw std::runtime_error("Failed to open database: " + err);
    }
    // Recommended pragmas
    execute("PRAGMA journal_mode=WAL;");
    execute("PRAGMA foreign_keys=ON;");
}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

Database::Database(Database&& other) noexcept
    : db_(other.db_), path_(std::move(other.path_)) {
    other.db_ = nullptr;
}

Database& Database::operator=(Database&& other) noexcept {
    if (this != &other) {
        if (db_) sqlite3_close(db_);
        db_       = other.db_;
        path_     = std::move(other.path_);
        other.db_ = nullptr;
    }
    return *this;
}

// ─────────────────────────────────────────────
//  Core Execution
// ─────────────────────────────────────────────

void Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("execute() failed: " + err);
    }
}

void Database::query(const std::string& sql, RowCallback callback) {
    auto stmt = prepare(sql);
    int  cols = stmt.colCount();

    while (stmt.step()) {
        Row row;
        for (int i = 0; i < cols; ++i)
            row[stmt.colName(i)] = stmt.colText(i);
        callback(row);
    }
}

std::vector<Row> Database::queryAll(const std::string& sql) {
    std::vector<Row> results;
    query(sql, [&](const Row& row) { results.push_back(row); });
    return results;
}

// ─────────────────────────────────────────────
//  Transactions
// ─────────────────────────────────────────────

void Database::beginTransaction() { execute("BEGIN TRANSACTION;"); }
void Database::commit()           { execute("COMMIT;");            }
void Database::rollback()         { execute("ROLLBACK;");          }

// ─────────────────────────────────────────────
//  Prepared Statements
// ─────────────────────────────────────────────

Database::Statement Database::prepare(const std::string& sql) {
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
    checkRC(rc, "prepare");
    return Statement(stmt, this);
}

// --- Bind ---

void Database::Statement::bindInt(int idx, int value) {
    checkRC(sqlite3_bind_int(stmt, idx, value), "bindInt");  // calls db->checkRC via friend? no — inline below
    // Actually we call sqlite3 directly and throw manually:
    if (sqlite3_bind_int(stmt, idx, value) != SQLITE_OK)
        throw std::runtime_error("bindInt failed");
}

void Database::Statement::bindInt(int idx, int value) {
    if (sqlite3_bind_int(stmt, idx, value) != SQLITE_OK)
        throw std::runtime_error("bindInt failed at index " + std::to_string(idx));
}

void Database::Statement::bindInt64(int idx, int64_t value) {
    if (sqlite3_bind_int64(stmt, idx, value) != SQLITE_OK)
        throw std::runtime_error("bindInt64 failed at index " + std::to_string(idx));
}

void Database::Statement::bindDouble(int idx, double value) {
    if (sqlite3_bind_double(stmt, idx, value) != SQLITE_OK)
        throw std::runtime_error("bindDouble failed at index " + std::to_string(idx));
}

void Database::Statement::bindText(int idx, const std::string& value) {
    if (sqlite3_bind_text(stmt, idx, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
        throw std::runtime_error("bindText failed at index " + std::to_string(idx));
}

void Database::Statement::bindNull(int idx) {
    if (sqlite3_bind_null(stmt, idx) != SQLITE_OK)
        throw std::runtime_error("bindNull failed at index " + std::to_string(idx));
}

// --- Step / Column ---

bool Database::Statement::step() {
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)  return true;
    if (rc == SQLITE_DONE) return false;
    throw std::runtime_error("step() failed");
}

int         Database::Statement::colInt   (int idx) { return sqlite3_column_int   (stmt, idx); }
int64_t     Database::Statement::colInt64 (int idx) { return sqlite3_column_int64 (stmt, idx); }
double      Database::Statement::colDouble(int idx) { return sqlite3_column_double(stmt, idx); }
int         Database::Statement::colCount ()        { return sqlite3_column_count  (stmt);      }

std::string Database::Statement::colText(int idx) {
    const unsigned char* text = sqlite3_column_text(stmt, idx);
    return text ? reinterpret_cast<const char*>(text) : "";
}

std::string Database::Statement::colName(int idx) {
    const char* name = sqlite3_column_name(stmt, idx);
    return name ? name : "";
}

// --- Lifecycle ---

void Database::Statement::reset()    { if (stmt) sqlite3_reset(stmt);    }
void Database::Statement::finalize() { if (stmt) { sqlite3_finalize(stmt); stmt = nullptr; } }

// ─────────────────────────────────────────────
//  Utility
// ─────────────────────────────────────────────

int64_t Database::lastInsertRowId() const { return sqlite3_last_insert_rowid(db_); }
int     Database::changedRows()     const { return sqlite3_changes(db_);            }

bool Database::tableExists(const std::string& tableName) {
    auto stmt = prepare(
        "SELECT 1 FROM sqlite_master WHERE type='table' AND name=?;"
    );
    stmt.bindText(1, tableName);
    return stmt.step();
}