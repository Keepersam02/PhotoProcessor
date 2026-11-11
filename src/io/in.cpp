#include "in.hpp"
#include "fcntl.h"
#include "types/misc.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

extern char main_slab[2000000000];

char clone_stacks[2000000];

const size_t stacks_size = 8000;

int import_file(void *arg) {
  import_file_arg *args = static_cast<import_file_arg *>(arg);
  void *data = args->data;
  file_exec exec = *args->exec;

  int fd = open(exec.in_path, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "error opening file, err: %s, path: %s\n", strerror(errno),
            exec.in_path);
    close(fd);
    return -1;
  }
  struct stat f_stat;
  int ret = fstat(fd, &f_stat);
  if (ret != 0) {
    fprintf(stderr, "error reading file stats, err: %s, path %s\n",
            strerror(errno), exec.in_path);
    close(fd);
    return -1;
  }
  size_t file_size = static_cast<size_t>(f_stat.st_size);
  ssize_t byt_read = read(fd, data, file_size);
  if (byt_read != 0 && byt_read != static_cast<ssize_t>(f_stat.st_size)) {
    fprintf(stderr, "unexpected read, path: %s, expeced: %ld, got: %ld\n",
            exec.in_path, file_size, byt_read);
    close(fd);
    return -1;
  }
  close(fd);
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
      close(fd);
      continue;
    }

    struct stat f_stat;
    int ret = fstat(fd, &f_stat);
    if (ret != 0) {
      // todo
      close(fd);
      continue;
    }
    if (static_cast<size_t>(f_stat.st_size) > max_size) {
      max_size = static_cast<size_t>(f_stat.st_size);
    }
    close(fd);
    num_files++;
  }
  closedir(dir);
}
