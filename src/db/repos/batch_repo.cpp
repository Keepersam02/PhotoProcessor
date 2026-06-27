#include "../models/batch.hpp"
#include "batch_repo.hpp"
#include <stdexcept>


BatchRepo::BatchRepo(SQLite::Database& db) : db_(db) {}

void BatchRepo::insert(const Batch& b) {
    SQLite::Statement q(db_,
        "INSERT INTO batch (name, date_created, date_modified, status) "
        "VALUES (?, ?, ?, ?)");
    q.bind(1, b.name);
    q.bind(2, b.dateCreated);
    q.bind(3, b.dateModified);
    q.bind(4, b.status);
    q.exec();
}

Batch BatchRepo::findById(int id) {
    SQLite::Statement q(db_,
        "SELECT id, name, date_created, date_modified, status "
        "FROM batch WHERE id = ?");
    q.bind(1, id);
    if (q.executeStep()) {
        return { q.getColumn(0), q.getColumn(1),
                 q.getColumn(2).getInt64(), q.getColumn(3).getInt64(), q.getColumn(4) };
    }
    throw std::runtime_error("Batch not found: " + std::to_string(id));
}

std::vector<Batch> BatchRepo::all() {
    SQLite::Statement q(db_,
        "SELECT id, name, date_created, date_modified, status FROM batch");
    std::vector<Batch> results;
    while (q.executeStep()) {
        results.push_back({ q.getColumn(0), q.getColumn(1),
                            q.getColumn(2).getInt64(), q.getColumn(3).getInt64(), q.getColumn(4) });
    }
    return results;
}

void BatchRepo::updateStatus(int id, BatchStatus status) {
    SQLite::Statement q(db_,
        "UPDATE batch SET status = ? WHERE id = ?");
    q.bind(1, static_cast<int>(status));
    q.bind(2, id);
    q.exec();
}

void BatchRepo::updateModified(int id, int64_t dateModified) {
    SQLite::Statement q(db_,
        "UPDATE batch SET date_modified = ? WHERE id = ?");
    q.bind(1, dateModified);
    q.bind(2, id);
    q.exec();
}

void BatchRepo::remove(int id) {
    SQLite::Statement q(db_, "DELETE FROM batch WHERE id = ?");
    q.bind(1, id);
    q.exec();
}