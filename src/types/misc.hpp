#pragma once
#ifndef deb

#include <cstddef>
struct file_exec {
  char in_path[750];
  char out_path[750];
  size_t f_size;
  size_t index;
};

#endif // deb
