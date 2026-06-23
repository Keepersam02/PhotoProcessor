#pragma once
#ifndef APP_STATE_H
#define APP_STATE_H

#include <vector>
#include <string>


#include "db/database.hpp"
#include "db/schema.hpp"
#include "db/models/batch.hpp"
#include "db/database.hpp"

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

    // Batch Table (connector to DB)
    BatchRepo batchRepo = Database::getInstance().get();

    // vector of all batches
    std::vector<Batch> batches = batchRepo.all();

    // string for input from user
    std::string input_string;

    // Batch struct for user to edit
    Batch input_batch;

    void refresh() {
        batches = batchRepo.all();
    }
};

#endif