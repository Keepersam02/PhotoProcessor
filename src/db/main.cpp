#include "database.h"
#include <iostream>

int main() {
    Database db("app.sqlite");

    // Create table
    db.execute(R"(
        CREATE TABLE IF NOT EXISTS users (
            id    INTEGER PRIMARY KEY AUTOINCREMENT,
            name  TEXT    NOT NULL,
            email TEXT    UNIQUE,
            age   INTEGER
        );
    )");

    // ── Insert with prepared statement ──────────
    {
        auto stmt = db.prepare(
            "INSERT INTO users (name, email, age) VALUES (?, ?, ?);"
        );
        stmt.bindText(1, "Alice");
        stmt.bindText(2, "alice@example.com");
        stmt.bindInt (3, 30);
        stmt.step();
        std::cout << "Inserted row id: " << db.lastInsertRowId() << "\n";
    }

    // ── Bulk insert with transaction ─────────────
    db.beginTransaction();
    {
        auto stmt = db.prepare(
            "INSERT OR IGNORE INTO users (name, email, age) VALUES (?, ?, ?);"
        );
        std::vector<std::tuple<std::string,std::string,int>> records = {
            {"Bob",     "bob@example.com",     25},
            {"Charlie", "charlie@example.com", 35},
            {"Diana",   "diana@example.com",   28},
        };
        for (auto& [name, email, age] : records) {
            stmt.bindText(1, name);
            stmt.bindText(2, email);
            stmt.bindInt (3, age);
            stmt.step();
            stmt.reset();
        }
    }
    db.commit();

    // ── Query all rows ───────────────────────────
    std::cout << "\nAll users:\n";
    auto rows = db.queryAll("SELECT id, name, email, age FROM users;");
    for (auto& row : rows) {
        std::cout << row.at("id")    << " | "
                  << row.at("name")  << " | "
                  << row.at("email") << " | "
                  << row.at("age")   << "\n";
    }

    // ── Query with callback ──────────────────────
    std::cout << "\nUsers over 27:\n";
    db.query(
        "SELECT name, age FROM users WHERE age > 27 ORDER BY age;",
        [](const Row& row) {
            std::cout << row.at("name") << " (age " << row.at("age") << ")\n";
        }
    );

    // ── Check table exists ───────────────────────
    std::cout << "\ntable 'users' exists: "
              << db.tableExists("users") << "\n";

    return 0;
}