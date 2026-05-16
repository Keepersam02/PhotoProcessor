#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <string>

class Database {
public:
    // reference to DB Class object
    static Database& getInstance();

    // reference to SQL database object
    SQLite::Database& get();

    // Prevents multiple DB objects
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

private:
    Database();
    // unique pointer auto deletes when it 
    // maybe change to normal pointer?
    std::unique_ptr<SQLite::Database> db_; // DB reference
};