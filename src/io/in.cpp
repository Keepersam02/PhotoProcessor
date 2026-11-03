#include "in.hpp"
#include "fcntl.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int import_file(const char *file_path, void *data) {
  strncpy(static_cast<char *>(data) + sizeof(size_t), file_path, 749);

  char *file_load = static_cast<char *>(data) + 751;

  int fd = open(file_path, O_RDONLY);
  if (fd < 0) {
    return fd;
  }
  struct stat f_stat;
  int ret = fstat(fd, &f_stat);
  if (ret != 0) {
    return ret;
  }
  size_t file_size = static_cast<size_t>(f_stat.st_size);
  ssize_t byt_read = read(fd, file_load, file_size);
  if (byt_read != 0) {
    return static_cast<int>(byt_read);
  }
  return 0;
}

void files_info(const char *dir_path, size_t &max_size, size_t &num_files) {
  DIR *dir = opendir(dir_path);
  if (dir == NULL) {
    printf("dir not found is null\n");
    return;
  }
  struct dirent *dir_entry;
  char file_path[750];
  while ((dir_entry = readdir(dir)) != NULL) {
    strncpy(file_path, dir_path, 500);
    strcat(file_path, "/");
    strncat(file_path, dir_entry->d_name, 250);

    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {

      continue;
    }

    struct stat f_stat;
    int ret = fstat(fd, &f_stat);
    if (ret != 0) {
      // todo
      continue;
    }
    if (static_cast<size_t>(f_stat.st_size) > max_size) {
      max_size = static_cast<size_t>(f_stat.st_size);
    }
  }
  closedir(dir);
}
