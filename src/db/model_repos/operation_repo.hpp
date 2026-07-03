#pragma once
#ifndef OPERATION_REPO_H
#define OPERATION_REPO_H

#include <memory>
#include <vector>

#include "../database.hpp"
#include "../models/operation.hpp"

class OperationRepo {
public:
    explicit OperationRepo(std::shared_ptr<Database> db);

    void               insert(const Operation& o);
    Operation              findById(int id);
    std::vector<Operation> all();
    void               remove(int id);

private:
    std::shared_ptr<Database> db_;
};

#endif