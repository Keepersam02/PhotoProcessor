#include "../models/operation.hpp"
#include "operation_repo.hpp"
#include <stdexcept>
#include <stdexcept>
#include <optional>

OperationRepo::OperationRepo(std::shared_ptr<Database> db) : db_(std::move(db)) {}

void OperationRepo::insert(const Operation& o) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "INSERT INTO operation (name) "
            "VALUES (?)");
        q.bind(1, o.name);
        q.exec();
    });
}

Operation OperationRepo::findById(int id) {
    Operation result;
    bool     found = false;

    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT id, name "
            "FROM operation WHERE id = ?");
        q.bind(1, id);
        if (q.executeStep()) {
            result = { q.getColumn(0), q.getColumn(1) };
            found = true;
        }
    });

    if (!found) throw std::runtime_error("Operation not found: " + std::to_string(id));
    return result;
}

std::vector<Operation> OperationRepo::all() {
    std::vector<Operation> results;
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db,
            "SELECT id, name FROM operation");
        while (q.executeStep()) {
            results.push_back({ q.getColumn(0), q.getColumn(1) });
        }
    });
    return results;
}

void OperationRepo::remove(int id) {
    db_->execute([&](SQLite::Database& db) {
        SQLite::Statement q(db, "DELETE FROM operation WHERE id = ?");
        q.bind(1, id);
        q.exec();
    });
}