#include "../src/db/database.hpp"
#include "../src/db/schema.hpp"
#include "../src/db/model_repos/batch_repo.hpp"
#include "../src/db/model_repos/pipeline_repo.hpp"
#include "../src/db/map_repos/batch_pipeline_map_repo.hpp"
#include "../src/db/map_repos/pipeline_operation_map_repo.hpp"


#include <fstream>
#include "app.hpp"

int main() {
    // 1. Database
    auto db = std::make_shared<Database>("src/db/BPP.db");

    // 2. Schema
    db->execute([](SQLite::Database& d){ Schema::initialize(d); });

    // 3. Repos
    auto batchRepo    = std::make_shared<BatchRepo>(db);
    auto pipelineRepo = std::make_shared<PipelineRepo>(db);
    auto operationRepo = std::make_shared<OperationRepo>(db);
    auto batchPipelineMapRepo = std::make_shared<BatchPipelineMapRepo>(db);
    auto pipelineOperationMapRepo = std::make_shared<PipelineOperationMapRepo>(db);

    // 4. App
    App app(batchRepo, pipelineRepo, operationRepo,batchPipelineMapRepo,pipelineOperationMapRepo);
    return app.run();


}