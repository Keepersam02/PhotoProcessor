#include "db/schema.hpp"

/*
 * Todo: 
 * unique names (for batches)
 * 
*/

void Schema::initialize(SQLite::Database& db) {
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS batch (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            name          TEXT    NOT NULL UNIQUE,
            date_created  INTEGER NOT NULL,
            date_modified INTEGER NOT NULL,
            status        INTEGER NOT NULL DEFAULT 0
        );
        
        CREATE TABLE IF NOT EXISTS pipeline (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            name          TEXT    NOT NULL UNIQUE,
            date_created  INTEGER NOT NULL,
            date_modified INTEGER NOT NULL
        );

        CREATE TABLE IF NOT EXISTS operation (
            id   INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT    NOT NULL UNIQUE
        );

        CREATE TABLE IF NOT EXISTS file (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            source_id   INTEGER NOT NULL,
            batch_id    INTEGER NOT NULL REFERENCES batch(id),
            name        TEXT    NOT NULL UNIQUE, 
            path        TEXT    NOT NULL,
            db_created  INTEGER NOT NULL,
            db_modified INTEGER NOT NULL
        );

        CREATE TABLE IF NOT EXISTS batch_pipeline_map (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            batch_id    INTEGER NOT NULL REFERENCES batch(id),
            pipeline_id INTEGER NOT NULL REFERENCES pipeline(id),
            UNIQUE(batch_id, pipeline_id)
        );

        CREATE TABLE IF NOT EXISTS pipeline_operation_map (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            pipeline_id  INTEGER NOT NULL REFERENCES pipeline(id),
            operation_id INTEGER NOT NULL REFERENCES operation(id),
            position     INTEGER NOT NULL UNIQUE,
            UNIQUE(pipeline_id, operation_id)
        );
    )");
}
