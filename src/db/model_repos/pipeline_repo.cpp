#include "../models/pipeline.hpp"
#include "pipeline_repo.hpp"
#include <stdexcept>

#include <stdexcept>
#include <optional>

PipelineRepo::PipelineRepo(std::shared_ptr<Database> db) : db_(std::move(db)) {}

void PipelineRepo::insert(const Pipeline& p) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "INSERT INTO pipeline (name, date_created, date_modified) "
            "VALUES (?, ?, ?)");
        q.bind(1, p.name);
        q.bind(2, p.dateCreated);
        q.bind(3, p.dateModified);
        q.exec();
    });
}

Pipeline PipelineRepo::findById(int32_t id) {
    Pipeline result;
    bool     found = false;

    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT id, name, date_created, date_modified "
            "FROM pipeline WHERE id = ?");
        q.bind(1, id);
        if (q.executeStep()) {
            result = { q.getColumn(0), q.getColumn(1),
                       q.getColumn(2).getInt64(), q.getColumn(3).getInt64() };
            found = true;
        }
    });

    if (!found) throw std::runtime_error("Pipeline not found: " + std::to_string(id));
    return result;
}

std::vector<Pipeline> PipelineRepo::all() {
    std::vector<Pipeline> results;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT id, name, date_created, date_modified FROM pipeline");
        while (q.executeStep()) {
            results.push_back({ q.getColumn(0), q.getColumn(1),
                                q.getColumn(2).getInt64(), q.getColumn(3).getInt64() });
        }
    });
    return results;
}

void PipelineRepo::updateModified(int32_t id, int64_t dateModified) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "UPDATE pipeline SET date_modified = ? WHERE id = ?");
        q.bind(1, dateModified);
        q.bind(2, id);
        q.exec();
    });
}

void PipelineRepo::updateName(int32_t id, std::string name ){
        db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "UPDATE pipeline SET name = ? WHERE id = ?");
        q.bind(1, name);
        q.bind(2, id);
        q.exec();
    });
}

void PipelineRepo::remove(int32_t id) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db, "DELETE FROM pipeline WHERE id = ?");
        q.bind(1, id);
        q.exec();
    });
}