#pragma once
#ifndef PIPELINE_REPO_H
#define PIPELINE_REPO_H

#include <memory>
#include <vector>

#include "../database.hpp"
#include "../models/pipeline.hpp"

class PipelineRepo {
public:
    explicit PipelineRepo(std::shared_ptr<Database> db);

    void                  insert(const Pipeline& p);
    Pipeline              findById(int32_t id);
    std::vector<Pipeline> all();
    void                  updateModified(int32_t id, int64_t dateModified);
    void                  updateName(int32_t id, std::string name);
    void                  remove(int32_t id);

private:
    std::shared_ptr<Database> db_;
};

#endif