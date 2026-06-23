#pragma once
#ifndef BATCH_H
#define BATCH_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>


struct Batch {
    // DB variables
    int         id;
    std::string name;
    int64_t     dateCreated;
    int64_t     dateModified;
    int         status;

    // CLI variables
    bool        is_selected;
    std::string dateCreatedFormatted;
    std::string dateModifiedFormatted;
};

enum class BatchStatus {
    Pending    = 0,
    Processing = 1,
    Complete   = 2,
    Failed     = 3
};


// Wrapper for DB, executes SQLite queries
class BatchRepo {
public:
    BatchRepo(SQLite::Database& db);

    void              insert(const Batch& b);
    Batch             findById(int id);
    std::vector<Batch> all();
    void              updateStatus(int id, BatchStatus status);
    void              updateModified(int id, long dateModified);
    void              remove(int id);

private:
    SQLite::Database& db_;
};

#endif