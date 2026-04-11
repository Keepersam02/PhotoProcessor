#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <unordered_map>

// A single result row: column name -> value (as string)
using Row = std::unordered_map<std::string, std::string>;

// Callback type for query results
using RowCallback = std::function<void(const Row&)>;

class Database {
public:
    // Constructor / Destructor
    explicit Database(const std::string& path);
    ~Database();

    // Non-copyable, movable
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&& other) noexcept;
    Database& operator=(Database&& other) noexcept;

    // DDL / DML with no result rows
    void execute(const std::string& sql);

    // Query with a per-row callback
    void query(const std::string& sql, RowCallback callback);

    // Query returning all rows at once
    std::vector<Row> queryAll(const std::string& sql);

    // Transactions
    void beginTransaction();
    void commit();
    void rollback();

    // Prepared statement helpers
    struct Statement {
        sqlite3_stmt* stmt = nullptr;
        Database*     db   = nullptr;

        Statement() = default;
        Statement(sqlite3_stmt* s, Database* d) : stmt(s), db(d) {}
        ~Statement() { finalize(); }

        Statement(const Statement&)            = delete;
        Statement& operator=(const Statement&) = delete;
        Statement(Statement&& o) noexcept : stmt(o.stmt), db(o.db) { o.stmt = nullptr; }

        void bindInt   (int idx, int value);
        void bindInt64 (int idx, int64_t value);
        void bindDouble(int idx, double value);
        void bindText  (int idx, const std::string& value);
        void bindNull  (int idx);

        bool        step();       // true = row available, false = done
        int         colInt   (int idx);
        int64_t     colInt64 (int idx);
        double      colDouble(int idx);
        std::string colText  (int idx);
        std::string colName  (int idx);
        int         colCount ();

        void reset();
        void finalize();
    };

    Statement prepare(const std::string& sql);

    // Utility
    int64_t lastInsertRowId() const;
    int     changedRows()     const;
    bool    tableExists(const std::string& tableName);

private:
    sqlite3*    db_   = nullptr;
    std::string path_;

    void checkRC(int rc, const std::string& context = "") const;
};