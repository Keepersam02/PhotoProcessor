#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>

struct Batch {
    int         id;
    std::string name;
    long        dateCreated;
    long        dateModified;
    int         status;
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