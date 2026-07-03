#pragma once
#ifndef DATABASE_H
#define DATABASE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <functional>
#include <mutex>
#include <memory>
#include <string>

class Database {
public:
    explicit Database(const std::string& path);

    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    void execute    (std::function<void(SQLite::Database&)> fn);
    void transaction(std::function<void(SQLite::Database&)> fn);

private:
    mutable std::mutex mutex_;
    SQLite::Database   db_;
};

#endif