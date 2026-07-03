#include "batch_pipeline_map_repo.hpp"

#include "../models/batch.hpp"
#include "../models/pipeline.hpp"

#include <stdexcept>
#include <optional>
#include <iostream>




BatchPipelineMapRepo::BatchPipelineMapRepo(std::shared_ptr<Database> db) : db_(std::move(db)) {}

void BatchPipelineMapRepo::insert(int32_t b_id, int32_t p_id){
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db,
        "INSERT INTO batch_pipeline_map (batch_id, pipeline_id) "
        "VALUES (?, ?)");
    q.bind(1, b_id);
    q.bind(2, p_id);
    q.exec();
    });
}

std::vector<BatchPipelineMap> BatchPipelineMapRepo::all(){
    std::vector<BatchPipelineMap> results;
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db,
        "SELECT id, id_batch, id_pipeline FROM batch_pipeline_map");
        while (q.executeStep()) {
        results.push_back({ q.getColumn(0), q.getColumn(1),q.getColumn(2) });
    }
    });
    return results;
}

std::vector<Batch> BatchPipelineMapRepo::findAllBatchesByPipeline(int32_t p_id) {
    std::vector<Batch> results;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT b.id, b.name, b.date_created, b.date_modified, b.status "
            "FROM batch b "
            "JOIN batch_pipeline_map m ON b.id = m.batch_id "
            "WHERE m.pipeline_id = ?");
        q.bind(1, p_id);
        while (q.executeStep()) {
            results.push_back({ q.getColumn(0), q.getColumn(1),
                                q.getColumn(2).getInt64(), q.getColumn(3).getInt64(),
                                q.getColumn(4) });
        }
    });
    return results;
}

std::vector<Pipeline> BatchPipelineMapRepo::findAllPipelinesByBatch(int32_t b_id){
    std::vector<Pipeline> results;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT p.id, p.name, p.date_created, p.date_modified "
            "FROM pipeline p "
            "JOIN batch_pipeline_map m ON p.id = m.pipeline_id "
            "WHERE m.batch_id = ?");
        q.bind(1, b_id);
        while (q.executeStep()) {
            results.push_back({ q.getColumn(0), q.getColumn(1),
                                q.getColumn(2).getInt64(), q.getColumn(3).getInt64()});
        }
    });
    return results;
}


void BatchPipelineMapRepo::remove(int32_t id){
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db, "DELETE FROM batch_pipeline_map WHERE id = ?");
    q.bind(1, id);
    q.exec();
    });
}

void BatchPipelineMapRepo::removeByBatch(int32_t b_id){
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db, "DELETE FROM batch_pipeline_map WHERE batch_id = ?");
    q.bind(1, b_id);
    q.exec();
    });
}

void BatchPipelineMapRepo::removeByPipeline(int32_t p_id){
    db_->execute([&](SQLite::Database& db) {
    SQLite::Statement q(db, "DELETE FROM batch_pipeline_map WHERE pipeline_id = ?");
    q.bind(1, p_id);
    q.exec();
    });
}