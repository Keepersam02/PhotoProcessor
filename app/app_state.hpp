#pragma once
#ifndef APP_STATE_H
#define APP_STATE_H

#include <vector>
#include <string>

#include "../src/db/database.hpp"
#include "../src/db/models/batch.hpp"
#include "../src/db/repos/batch_repo.hpp"
#include "../src/db/models/pipeline.hpp"
#include "../src/db/repos/pipeline_repo.hpp"

/*
    data needs to be displayed:
    from db:
 - Batches
 - Pipelines
 - Operations


    from User:
 - Files
*/

/*
 * App State:
 * Holds mutable data to be displayed
 * 
 */
struct AppState {

    // Temp data:
    std::string input_string;
    Batch input_batch;
    Pipeline input_pipeline;

    // DB Repos:
    BatchRepo batchRepo = Database::getInstance().get();
    // PipelineRepo pipelineRepo= 

    // Display data:
    std::vector<Batch> batches = batchRepo.all();

    void refresh() {
        batches = batchRepo.all();
    }
};

#endif