

enum err_sev { INFO, WARNING, ERROR, SEVERE };

struct io_err {
  char std_err_mess[200];
  char extra_mess[250];

  err_sev severity;
};

void import_file(const char *file_path, void *data, io_err *error);
