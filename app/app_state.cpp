
#include <vector>
#include <string>

#include "app_state.hpp"

#include "../src/db/models/batch.hpp"
#include "../src/db/models/pipeline.hpp"
#include "../src/db/models/operation.hpp"



void AppState::clearInput(){
    input_string = "";
    input_batch = {};
    input_pipeline = {};
}