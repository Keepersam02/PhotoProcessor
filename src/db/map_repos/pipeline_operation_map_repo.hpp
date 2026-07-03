#pragma once
#ifndef PIPELINE_OPERATION_MAP_REPO_H
#define PIPELINE_OPERATION_MAP_REPO_H


#include <memory>
#include <iostream>

#include "../database.hpp"
#include "../models/pipeline.hpp"
#include "../models/operation.hpp"

#include "../maps/pipeline_operation_map.hpp"


class PipelineOperationMapRepo {
public:
    explicit PipelineOperationMapRepo(std::shared_ptr<Database> db);

    void                              insert(int32_t p_id, int32_t o_id, int32_t pos);
    std::vector<PipelineOperationMap> all();
    std::vector<Operation>            findAllOperationByPipeline(int32_t p_id);
    std::vector<Pipeline>             findAllPipelinesByOperation(int32_t o_id);
    void                              remove(int32_t id);

private:
    std::shared_ptr<Database> db_;
};

#endif