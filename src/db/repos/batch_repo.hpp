#pragma once
#ifndef BATCH_REPO_H
#define BATCH_REPO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "../models/batch.hpp"


// Wrapper for DB, executes SQLite queries
class BatchRepo {
public:
    BatchRepo(SQLite::Database& db);

    void               insert(const Batch& b);
    Batch              findById(int id);
    std::vector<Batch> all();
    void               updateStatus(int id, BatchStatus status);
    void               updateModified(int id, int64_t dateModified);
    void               remove(int id);

private:
    SQLite::Database& db_;
};


#endif
