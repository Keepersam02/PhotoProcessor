
#include <cstdint>
enum broad_category {
  SYS_IO_ERR,
  USR_IO_ERR,
  PROC_ERR,

};

enum severity { INFO, DEBUG, WARNING, ERROR, SEVERE };

class log_interface {
  broad_category err_category;
  severity err_serverity;

  uint64_t time_abs; // find actual name for time since unix founding

  virtual void print_primary_info(char *info);
  virtual void print_secondary_info(char *info);
  virtual void print_extra_info(char *info);

  virtual ~log_interface() = default;
};
