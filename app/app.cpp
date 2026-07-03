#include "app.hpp"
#include "screens/main_screen.hpp"
#include "screens/batch_screen.hpp"
#include "screens/batch_create_screen.hpp"
#include "screens/pipeline_screen.hpp"
#include "screens/pipeline_create_screen.hpp"
#include "screens/run_screen.hpp"

#include "helpers/helper.hpp"

#include <chrono>
#include <ctime>


/*
OPTIMIZE:
 - store selected batches in stead of searching through all of them?

*/

//Constructor
App::App(std::shared_ptr<BatchRepo>    batchRepo,
         std::shared_ptr<PipelineRepo> pipelineRepo,
         std::shared_ptr<OperationRepo> operationRepo,
         std::shared_ptr<BatchPipelineMapRepo> batchPipelineMapRepo,
         std::shared_ptr<PipelineOperationMapRepo> pipelineOperationMapRepo)
    : app_output("app/app_error_log.txt"),/*, std::ios::app)*/
    batchRepo_ (std::move(batchRepo)),
    pipelineRepo_ (std::move(pipelineRepo)),
    operationRepo_ (std::move(operationRepo)),
    batchPipelineMapRepo_ (std::move(batchPipelineMapRepo)),
    pipelineOperationMapRepo_ (std::move(pipelineOperationMapRepo))
{
    refresh(); // populate state_ before first render
}

//General:

int App::run() {
    Component main_screen         = MainScreen        (*this, screen_.ExitLoopClosure());
    Component batch_screen        = BatchScreen       (*this);
    Component pipeline_screen     = PipelineScreen    (*this);
    Component run_screen          = RunScreen         (*this);
    Component batch_create_screen = BatchCreateScreen (*this);
    Component pipeline_create_screen = PipelineCreateScreen (*this);

    /*
     * 0 - Main
     * 1 = Batches
     * 2 = Pipelines
     * 3 = Run
     * 4 = Batch Create
     * 5 = Pipeline Create
     * 
    */
    auto root = Container::Tab({
        main_screen,
        batch_screen,
        pipeline_screen,
        run_screen,
        batch_create_screen,
        pipeline_create_screen
    }, &active_tab_);

    try {
        app_output << "Starting App..." << std::endl;
        screen_.Loop(root);
    }
    catch(const std::exception& e) {

        writeError(e);
        return 1;

    }
    return 0;
}

/* 
 * Refreshes appstate data
 * TODO:
 * - smarter refresh, only get changed data (maybe multiple refreshes?)
 * - different display structs, for the CLI data
 * - !!!refresh currently deselects all
 * 
 * 
 */
void App::refresh() {
    state_.batches    = batchRepo_->all();
    state_.pipelines  = pipelineRepo_->all();
    state_.operations = operationRepo_->all();
}

void App::writeError(const std::exception& e){
    time_t t = std::chrono::system_clock::to_time_t(
                   std::chrono::system_clock::now());
    app_output  << "❚ "<< epochToReadable(t) << " ❚ ――― ❚ " << e.what() << " ❚" <<std::endl;
}

// Batch:

/*
 * TODO:
 * validate by name without error
 * validate pipelines exist?
 * warning if no pipeline selected?
 * 
 * Save current input batch to db. 
 * @return true if success
 */
bool App::saveNewBatch() {

    time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    state_.input_batch.name = state_.input_string;
    state_.input_batch.dateCreated  = static_cast<int64_t>(t);
    state_.input_batch.dateModified = static_cast<int64_t>(t);
    state_.input_batch.status       = 0;

    // unique name check
    try{
        batchRepo_->insert(state_.input_batch);
    }
    catch(const std::exception& e) {
        writeError(e);
        return 0;
    }

    for (auto& pipeline : state_.pipelines){
        if (pipeline.is_selected){
            try{
                batchPipelineMapRepo_->insert(batchRepo_->findByName(state_.input_string).id, pipeline.id);
            }
            catch(const std::exception& e) {
                writeError(e);
                return 0;
            }
        }
    }
    // clear transient input state
    state_.clearInput();
    refresh();

    active_tab_ = 1;
    return 1;
}

void App::deleteBatch(int b_id) {
    batchRepo_->remove(b_id);
    refresh();
}

void App::deleteSelectedBatches() {

    for (auto& batch : state_.batches) {
        if (batch.is_selected) {
            batchPipelineMapRepo_->removeByBatch(batch.id);
            batchRepo_->remove(batch.id);
        }
    }
    refresh();
}

void App::selectAllBatches(){
    for (auto& batch : state_.batches) {
        if (!batch.is_selected) {
            batch.is_selected = true;
        }
    }
}

void App::deSelectAllBatches(){
    for (auto& batch : state_.batches) {
        if (batch.is_selected) {
            batch.is_selected = false;
        }
    }
}

bool App::anySelectedBatches(){
    for (auto& batch : state_.batches) {
        if (batch.is_selected) {
            return true;
        }
    }
    return false;
}

// Pipeline:

bool App::saveNewPipeline(){

    time_t t = std::chrono::system_clock::to_time_t(
                   std::chrono::system_clock::now());

    if (!state_.input_pipeline.dateCreated){
        state_.input_pipeline.dateCreated  = static_cast<int64_t>(t);
    }
    state_.input_pipeline.dateModified = static_cast<int64_t>(t);
    state_.input_pipeline.name         = state_.input_string;

    // unique name check
    try{
        pipelineRepo_->insert(state_.input_pipeline);
    }
    catch(const std::exception& e) {
        writeError(e);
        return 0;
    }
    // clear transient input state
    state_.clearInput();

    refresh();
    active_tab_ = 2;

    return 1;
}

void App::deletePipeline(int p_id){
    batchPipelineMapRepo_->removeByPipeline(p_id);
    pipelineRepo_->remove(p_id);
    refresh();
}

void App::deleteSelectedPipelines(){
    for (auto& pipeline : state_.pipelines) {
        if (pipeline.is_selected) {
            batchPipelineMapRepo_->removeByPipeline(pipeline.id);
            pipelineRepo_->remove(pipeline.id);
        }
    }
    refresh();
}

bool App::anySelectedPipelines(){
    for (auto& pipeline : state_.pipelines) {
        if (pipeline.is_selected) {

            return true;
        }
    }
    return false;
}


// Batch pipeline Map
bool App::anyConnectedPipelines(Batch& b){
    return batchPipelineMapRepo_->findAllPipelinesByBatch(b.id).size();
}
