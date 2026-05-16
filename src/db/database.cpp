#include "db/database.hpp"

Database::Database() {
    db_ = std::make_unique<SQLite::Database>(
        "BPP.db",
        SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
    );
    db_->exec("PRAGMA foreign_keys = ON;"); // enforce FK constraints
}

// Pointer to DB object
Database& Database::getInstance() {
    static Database instance;
    return instance;
}

// Smart pointer to SQL DB object
SQLite::Database& Database::get() {
    return *db_;
}