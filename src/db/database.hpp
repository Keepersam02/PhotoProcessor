#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <string>

class Database {
public:
    static Database& getInstance();
    SQLite::Database& get();

    // Delete copy/move
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

private:
    Database();
    std::unique_ptr<SQLite::Database> db_;
};