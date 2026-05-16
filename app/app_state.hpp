#pragma once
#include <vector>
#include "db/database.hpp"
#include "db/schema.hpp"
#include "db/models/batch.hpp"

/*
    NEED FROM DB
1. Batches
2. Pipelines
3. Operations
4. Files
*/
struct AppState {
    BatchRepo* batchRepo = nullptr;
    std::vector<Batch> batches;

    int selected_batch    = 0;
    int selected_pipeline = 0;

    void refresh() {
        if (batchRepo) batches = batchRepo->all();
    }
};