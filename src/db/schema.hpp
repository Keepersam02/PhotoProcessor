#pragma once
#include <SQLiteCpp/SQLiteCpp.h>

class Schema {
public:
    static void initialize(SQLite::Database& db);
};