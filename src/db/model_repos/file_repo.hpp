#pragma once
#ifndef FILE_REPO_H
#define FILE_REPO_H

#include <memory>
#include <vector>

#include "../database.hpp"
#include "../models/file.hpp"
#include "../models/batch.hpp"


class FileRepo {
public:
    explicit FileRepo(std::shared_ptr<Database> db);

    void insert(const File& b);
    
    std::vector<File> getFilesFromBatchID(const Batch& b);

private:
    std::shared_ptr<Database> db_;
};

#endif