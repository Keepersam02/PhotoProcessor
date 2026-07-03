#pragma once
#ifndef APP_H
#define APP_H

#include <memory>
#include <fstream>

#include <ftxui/component/screen_interactive.hpp>

#include "app_state.hpp"

#include "../src/db/model_repos/batch_repo.hpp"
#include "../src/db/model_repos/pipeline_repo.hpp"
#include "../src/db/model_repos/operation_repo.hpp"
#include "../src/db/map_repos/batch_pipeline_map_repo.hpp"
#include "../src/db/map_repos/pipeline_operation_map_repo.hpp"


using namespace ftxui;

class App {
public:
    App(std::shared_ptr<BatchRepo>                batchRepo,
        std::shared_ptr<PipelineRepo>             pipelineRepo,
        std::shared_ptr<OperationRepo>            operationRepo,
        std::shared_ptr<BatchPipelineMapRepo>     batchPipelineMapRepo,
        std::shared_ptr<PipelineOperationMapRepo> pipelineOperationMapRepo);

    
    // General
    int run();
    void refresh();
    void writeError(const std::exception& e);

    // Batch actions
    bool saveNewBatch();
    void deleteBatch(int id);
    void deleteSelectedBatches();
    void selectAllBatches();
    void deSelectAllBatches();
    bool anySelectedBatches();

    // Pipeline actions
    bool saveNewPipeline();
    void deletePipeline(int id);
    void deleteSelectedPipelines();
    bool anySelectedPipelines();

    // Batch Pipeline Map Actions
    bool anyConnectedPipelines(Batch& b);
    bool anyConnectedBatches(Pipeline& p);
    bool anyConnectedPipelines(Operation& o);


    // State for display data
    AppState state_;

    std::ofstream app_output;
    
    int active_tab_ = 0;

private:
    std::shared_ptr<BatchRepo>    batchRepo_;
    std::shared_ptr<PipelineRepo> pipelineRepo_;
    std::shared_ptr<OperationRepo> operationRepo_;
    std::shared_ptr<BatchPipelineMapRepo> batchPipelineMapRepo_;
    std::shared_ptr<PipelineOperationMapRepo> pipelineOperationMapRepo_;

    
    ScreenInteractive screen_ = ScreenInteractive::Fullscreen();
};

#endif