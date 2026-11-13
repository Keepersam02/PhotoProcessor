#include "db_manag.hpp"
#include "sqlite3.h"
#include <cstdio>
#include <cstring>

const char *table_inits[NUMBER_TABLES] = {
    "CREATE TABLE IF NOT EXISTS log ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " severity INTEGER NOT NULL,"
    " category INTEGER NOT NULL,"
    " time LONG INTEGER NOT NULL,"
    " primary_info TEXT NOT NULL,"
    " secondary_info STRING,"
    " extra_info STRING"};

int init_db(const char *path);

int init_tables(sqlite3 *db, const char *table_names[NUMBER_TABLES]);

int open_db(const char *path, sqlite3 *db);

int init_db(const char *path) {
  sqlite3 *db;
  int ret =
      sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if (ret != 0) {
    fprintf(stderr, "error creating sqlite db, err: %s, path: %s\n",
            sqlite3_errstr(ret), path);
    return -1;
  }
  return 0;
}

int init_tables(sqlite3 *db, const char *table_names[NUMBER_TABLES]) {
  sqlite3_stmt *stmt;
  for (int i = 0; i < NUMBER_TABLES; i++) {
    {
      int ret = sqlite3_prepare_v2(db, table_names[i], -1, &stmt, 0);
      if (ret != SQLITE_OK) {
        fprintf(stderr,
                "error creating table, prepare, err: %s, table name: %s\n",
                sqlite3_errstr(ret), table_names[i]);
        sqlite3_finalize(stmt);
        return -1;
      }
    }
    {
      int ret = sqlite3_step(stmt);
      if (ret != SQLITE_DONE) {
        fprintf(stderr,
                "error in createing table, step, err: %s, table name: %s\n",
                sqlite3_errstr(ret), table_names[i]);
        sqlite3_finalize(stmt);
        return -1;
      }
    }
  }
  fprintf(stderr, "all tables init success\n");
  return 0;
}
