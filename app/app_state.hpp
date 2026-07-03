#pragma once
#ifndef APP_STATE_H
#define APP_STATE_H

#include <vector>
#include <string>

#include "../src/db/models/batch.hpp"
#include "../src/db/models/pipeline.hpp"
#include "../src/db/models/operation.hpp"



/*
TODO:
 - different viewable batch/pipe/... structs for cli data, and only view a certain amount

*/
class AppState {
public:

    void clearInput();

    // transient input — cleared after each action
    std::string input_string;
    Batch       input_batch{};
    Pipeline    input_pipeline{};


    // display data — populated by App::refresh()
    std::vector<Batch>    batches;
    std::vector<Pipeline> pipelines;
    std::vector<Operation> operations;
};

#endif