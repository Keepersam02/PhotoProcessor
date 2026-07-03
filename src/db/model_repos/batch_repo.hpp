#pragma once
#ifndef BATCH_REPO_H
#define BATCH_REPO_H

#include <memory>
#include <vector>

#include "../database.hpp"
#include "../models/batch.hpp"

class BatchRepo {
public:
    explicit BatchRepo(std::shared_ptr<Database> db);

    void               insert(const Batch& b);
    Batch              findById(int32_t id);
    Batch              findByName(std::string n);
    std::vector<Batch> all();
    void               updateStatus(int32_t id, BatchStatus status);
    void               updateModified(int32_t id, int64_t dateModified);
    void               updateName(int32_t id, std::string name);
    void               remove(int32_t id);

private:
    std::shared_ptr<Database> db_;
};

#endif