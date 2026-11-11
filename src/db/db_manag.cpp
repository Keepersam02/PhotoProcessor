
#include "sqlite3.h"
#include <cstdio>

int init_db(const char path[]) {
  struct sqlite3 *db;
  int val =
      sqlite3_open_v2(path, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
  if (val != SQLITE_OK) {
    fprintf(stderr, "failed to create sqlite db, err: %s, path: %s\n",
            sqlite3_errmsg(db), path);
    sqlite3_close(db);
    return -1;
  }
}

int open_db(const char path[], struct sqlite3 *db) {
  int ex = sqlite3_open_v2(path, &db, SQLITE_OPEN_READONLY, 0);
  if (ex != SQLITE_OK) {
    int in = init_db(path);
  }
}
