#include "database.hpp"

Database::Database(const std::string& path)
    : db_(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    db_.exec("PRAGMA journal_mode=WAL");
    db_.exec("PRAGMA foreign_keys=ON");
    db_.exec("PRAGMA synchronous=NORMAL");
}

void Database::execute(std::function<void(SQLite::Database&)> fn) {
    std::unique_lock<std::mutex> lock(mutex_);
    fn(db_);
}

void Database::transaction(std::function<void(SQLite::Database&)> fn) {
    std::unique_lock<std::mutex> lock(mutex_);
    SQLite::Transaction txn(db_);
    fn(db_);
    txn.commit();
}