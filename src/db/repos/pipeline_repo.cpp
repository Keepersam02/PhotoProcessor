#include "../models/pipeline.hpp"
#include "pipeline_repo.hpp"
#include <stdexcept>


PipelineRepo::PipelineRepo(SQLite::Database& db) : db_(db) {}

void PipelineRepo::insert(const Pipeline& p) {
    SQLite::Statement q(db_,
        "INSERT INTO pipeline (name, date_created, date_modified, status) "
        "VALUES (?, ?, ?, ?)");
    q.bind(1, p.name);
    q.bind(2, p.dateCreated);
    q.bind(3, p.dateModified);
    q.bind(4, p.status);
    q.exec();
}

Pipeline PipelineRepo::findById(int id) {
    SQLite::Statement q(db_,
        "SELECT id, name, date_created, date_modified "
        "FROM pipeline WHERE id = ?");
    q.bind(1, id);
    if (q.executeStep()) {
        return { q.getColumn(0), q.getColumn(1),
                 q.getColumn(2).getInt64(), q.getColumn(3).getInt64() };
    }
    throw std::runtime_error("Pipeline not found: " + std::to_string(id));
}

std::vector<Pipeline> PipelineRepo::all() {
    SQLite::Statement q(db_,
        "SELECT id, name, date_created, date_modified FROM pipeline");
    std::vector<Pipeline> results;
    while (q.executeStep()) {
        results.push_back({ q.getColumn(0), q.getColumn(1),
                            q.getColumn(2).getInt64(), q.getColumn(3).getInt64() });
    }
    return results;
}


void PipelineRepo::updateModified(int id, int64_t dateModified) {
    SQLite::Statement q(db_,
        "UPDATE pipeline SET date_modified = ? WHERE id = ?");
    q.bind(1, dateModified);
    q.bind(2, id);
    q.exec();
}

void PipelineRepo::remove(int id) {
    SQLite::Statement q(db_, "DELETE FROM pipeline WHERE id = ?");
    q.bind(1, id);
    q.exec();
}