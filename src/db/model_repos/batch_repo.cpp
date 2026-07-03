#include "../models/batch.hpp"
#include "batch_repo.hpp"
#include <stdexcept>
#include <optional>

BatchRepo::BatchRepo(std::shared_ptr<Database> db) : db_(std::move(db)) {}

void BatchRepo::insert(const Batch& b) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "INSERT INTO batch (name, date_created, date_modified, status) "
            "VALUES (?, ?, ?, ?)");
        q.bind(1, b.name);
        q.bind(2, b.dateCreated);
        q.bind(3, b.dateModified);
        q.bind(4, b.status);
        q.exec();
    });
}

Batch BatchRepo::findById(int32_t id) {
    std::optional<Batch> result;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT id, name, date_created, date_modified, status "
            "FROM batch WHERE id = ?");
        q.bind(1, id);
        if (q.executeStep()) {
            result = { q.getColumn(0), q.getColumn(1),
                       q.getColumn(2).getInt64(), q.getColumn(3).getInt64(), q.getColumn(4) };
        }
    });
    if (!result) throw std::runtime_error("Batch not found: " + std::to_string(id));
    return *result;
}

Batch BatchRepo::findByName(std::string n) {
    std::optional<Batch> result;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT id, name, date_created, date_modified, status "
            "FROM batch WHERE name = ?");
        q.bind(1, n);
        if (q.executeStep()) {
            result = { q.getColumn(0), q.getColumn(1),
                       q.getColumn(2).getInt64(), q.getColumn(3).getInt64(), q.getColumn(4) };
        }
    });
    if (!result) throw std::runtime_error("Batch not found: " + n);
    return *result;
}

std::vector<Batch> BatchRepo::all() {
    std::vector<Batch> results;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT id, name, date_created, date_modified, status FROM batch");
        while (q.executeStep()) {
            results.push_back({ q.getColumn(0), q.getColumn(1),
                                q.getColumn(2).getInt64(), q.getColumn(3).getInt64(), q.getColumn(4) });
        }
    });
    return results;
}

void BatchRepo::updateStatus(int32_t id, BatchStatus status) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "UPDATE batch SET status = ? WHERE id = ?");
        q.bind(1, static_cast<int>(status));
        q.bind(2, id);
        q.exec();
    });
}

void BatchRepo::updateModified(int32_t id, int64_t dateModified) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "UPDATE batch SET date_modified = ? WHERE id = ?");
        q.bind(1, dateModified);
        q.bind(2, id);
        q.exec();
    });
}

void BatchRepo::updateName(int32_t id, std::string name){
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "UPDATE batch SET name = ? WHERE id = ?");
        q.bind(1, name);
        q.bind(2, id);
        q.exec();
    });
}

void BatchRepo::remove(int32_t id) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db, "DELETE FROM batch WHERE id = ?");
        q.bind(1, id);
        q.exec();
    });
}