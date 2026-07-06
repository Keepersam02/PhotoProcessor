#include "app.hpp"
#include "screens/main_screen.hpp"
#include "screens/batch_screen.hpp"
#include "screens/batch_create_screen.hpp"
#include "screens/batch_edit_screen.hpp"
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
    Component main_screen            = MainScreen        (*this, screen_.ExitLoopClosure());
    Component batch_screen           = BatchScreen       (*this);
    Component pipeline_screen        = PipelineScreen    (*this);
    Component run_screen             = RunScreen         (*this);
    Component batch_create_screen    = BatchCreateScreen (*this);
    Component batch_edit_screen      = BatchEditScreen (*this);
    Component pipeline_create_screen = PipelineCreateScreen (*this);

    /*
     * 0 - Main
     * 1 = Batches
     * 2 = Pipelines
     * 3 = Run
     * 4 = Batch Create
     * 5 = Batch Edit Screen
     * 6 = Pipeline Create
    */
    auto root = Container::Tab({
        main_screen,
        batch_screen,
        pipeline_screen,
        run_screen,
        batch_create_screen,
        batch_edit_screen,
        pipeline_create_screen
    }, &active_tab_);
    // root = root | bgcolor(Color::White) | color(Color::Black);

    app_output << "\nStarting App..." << std::endl;

    try {
        screen_.Loop(root);
    }
    catch (const std::filesystem::filesystem_error& e) {
        writeError(std::string("Filesystem error: ") + e.what());
        // Belt-and-braces: force-restore the terminal even if FTXUI's own
        // cleanup didn't run (see FTXUI issue #873).
        std::cout << "\x1b[?1000l\x1b[?1002l\x1b[?1003l\x1b[?1006l" // disable mouse tracking
                    "\x1b[?25h"                                     // show cursor
                    "\x1b[?1049l"                                   // leave alt screen
                << std::flush;
        return 1;
    }
    catch (const std::runtime_error& e) {
        writeError(std::string("Runtime error: ") + e.what());
        // Belt-and-braces: force-restore the terminal even if FTXUI's own
        // cleanup didn't run (see FTXUI issue #873).
        std::cout << "\x1b[?1000l\x1b[?1002l\x1b[?1003l\x1b[?1006l" // disable mouse tracking
                    "\x1b[?25h"                                     // show cursor
                    "\x1b[?1049l"                                   // leave alt screen
                << std::flush;
        return 1;
    }
    catch (const std::logic_error& e) {
        writeError(std::string("Logic error: ") + e.what());
        // Belt-and-braces: force-restore the terminal even if FTXUI's own
        // cleanup didn't run (see FTXUI issue #873).
        std::cout << "\x1b[?1000l\x1b[?1002l\x1b[?1003l\x1b[?1006l" // disable mouse tracking
                    "\x1b[?25h"                                     // show cursor
                    "\x1b[?1049l"                                   // leave alt screen
                << std::flush;
        return 1;
    }
    catch (const std::exception& e) {          // catch-all for other std exceptions
        writeError(e);
        // Belt-and-braces: force-restore the terminal even if FTXUI's own
        // cleanup didn't run (see FTXUI issue #873).
        std::cout << "\x1b[?1000l\x1b[?1002l\x1b[?1003l\x1b[?1006l" // disable mouse tracking
                    "\x1b[?25h"                                     // show cursor
                    "\x1b[?1049l"                                   // leave alt screen
                << std::flush;
        return 1;
    }
    catch (...) {                              // catches non-std::exception throws too
        writeError("Unknown non-standard exception caught");
        // Belt-and-braces: force-restore the terminal even if FTXUI's own
        // cleanup didn't run (see FTXUI issue #873).
        std::cout << "\x1b[?1000l\x1b[?1002l\x1b[?1003l\x1b[?1006l" // disable mouse tracking
                    "\x1b[?25h"                                     // show cursor
                    "\x1b[?1049l"                                   // leave alt screen
                << std::flush;
        return 1;
    }
    app_output << "Exited normally...\n" << std::endl;
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

void App::writeError(const std::string& message) {
    time_t t = std::chrono::system_clock::to_time_t(
                   std::chrono::system_clock::now());
    app_output << "❚ " << epochToReadable(t) << " ❚ ――― ❚ " << message << " ❚" << std::endl;
}

void App::writeError(const std::exception& e) {
    writeError(std::string(e.what()));
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
    state_.input_batch.status       = BatchStatus::Pending;

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
    return 1;
}

/* 
 * Edit a current batch in db
 * 
 * 
 * 
 */
bool App::editBatch(){
    time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // unique name check
    try{
        batchRepo_->updateName(state_.input_batch.id, state_.input_batch.name);
    }
    catch(const std::exception& e) {
        writeError(e);
        return 0;
    }
    batchRepo_->updateModified(state_.input_batch.id, t);
    batchRepo_->updateStatus(state_.input_batch.id, BatchStatus::Pending);

    // update pipeline maps
    for (auto& pipeline : state_.pipelines){
        if (pipeline.is_selected){
            try{
                batchPipelineMapRepo_->insert(state_.input_batch.id,pipeline.id);
            }
            catch(const std::exception& e) {
                writeError(e);
            }
        }
        else{
            try{
                batchPipelineMapRepo_->removeByBoth(state_.input_batch.id, pipeline.id);
            }
            catch(const std::exception& e){
                writeError(e);
            }
        }
    }

    return true;
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

/*
 * populate appstate with the first selected batch 
 * 
 */
void App::selectBatch(){
    for (auto& batch : state_.batches){
        if (batch.is_selected){
            state_.input_batch = batch;
        }
    }

    std::vector<Pipeline> selectedBatchPipelines = batchPipelineMapRepo_->findAllPipelinesByBatch(state_.input_batch.id);
    deSelectAllBatches();
    for (auto& b_pipeline : selectedBatchPipelines){
        for (auto& pipeline : state_.pipelines) {
            if (b_pipeline.id == pipeline.id){
                pipeline.is_selected = true;
                continue;
            }
        }
    }
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

void App::selectAllPipelines(){
    for (auto& pipeline : state_.pipelines) {
        if (!pipeline.is_selected) {
            pipeline.is_selected = true;
        }
    }
}

void App::deSelectAllPipelines(){
    for (auto& pipeline : state_.pipelines) {
        if (pipeline.is_selected) {
            pipeline.is_selected = false;
        }
    }
}


// Batch pipeline Map
bool App::anyConnectedPipelines(Batch& b){
    return batchPipelineMapRepo_->findAllPipelinesByBatch(b.id).size();
}
