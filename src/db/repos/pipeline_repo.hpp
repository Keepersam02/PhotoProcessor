#pragma once
#ifndef PIPELINE_REPO_H
#define PIPELINE_REPO_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>

#include "../models/pipeline.hpp"


// Wrapper for DB, executes SQLite queries
class PipelineRepo {
public:
    PipelineRepo(SQLite::Database& db);

    void                  insert(const Pipeline& p);
    Pipeline              findById(int id);
    std::vector<Pipeline> all();
    void                  updateModified(int id, int64_t dateModified);
    void                  remove(int id);

private:
    SQLite::Database& db_;
};


#endif
