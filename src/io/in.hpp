#include "types/file_slab.hpp"
#include "types/misc.hpp"

#include <cstddef>
enum err_sev { INFO, WARNING, ERROR, SEVERE };

struct io_err {
  char std_err_mess[200];
  char extra_mess[250];

  err_sev severity;
};

int import_file(const char *file_path, void *data);

void files_info(const char *dir_path, size_t &max_size, size_t &num_files);

void import_files(const file_exec files[], size_t batch_size, file_slab &slab);
