#include "sqlite3.h"
#include <expected>
#include <filesystem>

namespace fs = std::filesystem;

bool init_db(fs::path new_db_path, sqlite3 *db);

bool db_exist(dberr *err);

bool db_valid(dberr *err);
