#include "types/file_slab.hpp"
#include "types/misc.hpp"
#include <fcntl.h>

#include <cstddef>

int import_file(const char *file_path, void *data);

int import_files_st(void *args);
