#pragma once
#ifndef SCHEMA_H
#define SCHEMA_H

#include <SQLiteCpp/SQLiteCpp.h>

class Schema {
public:
    static void initialize(SQLite::Database& db);
};


#endif