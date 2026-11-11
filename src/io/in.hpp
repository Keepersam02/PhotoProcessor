#include "types/file_slab.hpp"
#include "types/misc.hpp"
#include <fcntl.h>

#include <cstddef>

struct import_file_arg {
  file_exec *exec;
  void *data;
};

int import_file(void *arg);

int import_files_st(void *args);

void files_info(const char *dir_path, size_t &max_size, size_t &num_files);
