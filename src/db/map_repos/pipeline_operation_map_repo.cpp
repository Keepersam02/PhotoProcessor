#include "pipeline_operation_map_repo.hpp"

#include "../models/pipeline.hpp"
#include "../models/operation.hpp"
#include "../maps/pipeline_operation_map.hpp"

#include <stdexcept>
#include <optional>
#include <iostream>




PipelineOperationMapRepo::PipelineOperationMapRepo(std::shared_ptr<Database> db) : db_(std::move(db)) {}

void PipelineOperationMapRepo::insert(int32_t p_id, int32_t o_id, int32_t pos){
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db,
        "INSERT INTO pipeline_operation_map (batch_id, pipeline_id, position) "
        "VALUES (?, ?, ?)");
    q.bind(1, p_id);
    q.bind(2, o_id);
    q.bind(3, pos);
    q.exec();
    });
}

std::vector<PipelineOperationMap> PipelineOperationMapRepo::all(){
    std::vector<PipelineOperationMap> results;
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db,
        "SELECT id, id_pipeline, id_operation, position FROM pipeline_operation_map");
        while (q.executeStep()) {
        results.push_back({ q.getColumn(0), q.getColumn(1),q.getColumn(2), q.getColumn(3) });
    }
    });
    return results;
}

/*
TODO:
NOT ORDERED!
*/
std::vector<Operation> PipelineOperationMapRepo::findAllOperationByPipeline(int32_t p_id) {
    std::vector<Operation> results;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT o.id, o.name "
            "FROM operation o "
            "JOIN pipeline_operation_map m ON o.id = m.operation_id "
            "WHERE m.pipeline_id = ?");
        q.bind(1, p_id);
        while (q.executeStep()) {
            results.push_back({ q.getColumn(0), q.getColumn(1)});
        }
    });
    return results;
}

std::vector<Pipeline> PipelineOperationMapRepo::findAllPipelinesByOperation(int32_t o_id){
    std::vector<Pipeline> results;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT p.id, p.name, p.date_created, p.date_modified "
            "FROM pipeline p "
            "JOIN pipeline_operation_map m ON p.id = m.pipeline_id "
            "WHERE m.batch_id = ?");
        q.bind(1, o_id);
        while (q.executeStep()) {
            results.push_back({ q.getColumn(0), q.getColumn(1),
                                q.getColumn(2).getInt64(), q.getColumn(3).getInt64()});
        }
    });
    return results;
}

void PipelineOperationMapRepo::remove(int32_t id){
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db, "DELETE FROM pipeline_operation_map WHERE id = ?");
    q.bind(1, id);
    q.exec();
    });
}
