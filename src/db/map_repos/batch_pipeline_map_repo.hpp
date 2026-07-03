#pragma once
#ifndef BATCH_PIPELINE_MAP_REPO_H
#define BATCH_PIPELINE_MAP_REPO_H


#include <memory>
#include <iostream>

#include "../database.hpp"
#include "../models/batch.hpp"
#include "../models/pipeline.hpp"
#include "../maps/batch_pipeline_map.hpp"

class BatchPipelineMapRepo {
public:
    explicit BatchPipelineMapRepo(std::shared_ptr<Database> db);

    void                          insert(int32_t b_id,int32_t p_id);
    std::vector<BatchPipelineMap> all();
    std::vector<Batch>            findAllBatchesByPipeline(int32_t p_id);
    std::vector<Pipeline>         findAllPipelinesByBatch(int32_t b_id);
    void                          remove(int32_t id);
    void                          removeByBatch(int32_t b_id);
    void                          removeByPipeline(int32_t p_id);
    

private:
    std::shared_ptr<Database> db_;
};

#endif