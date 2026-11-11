

#include <cstddef>
enum err_sev { INFO, WARNING, ERROR, SEVERE };

struct io_err {
  char std_err_mess[200];
  char extra_mess[250];

  err_sev severity;
};

int import_file(const char *file_path, void *data);

void files_info(const char *dir_path, size_t &max_size, size_t &num_files);
