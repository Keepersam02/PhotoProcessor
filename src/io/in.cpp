#include "in.hpp"
#include "fcntl.h"
#include "types/file_slab.hpp"
#include "types/misc.hpp"
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

struct import_fd_args {
  int fd;
  struct stat f_stat;
  void *data;
};

int import_file_fd(void *arg) {
  import_fd_args *args = static_cast<import_fd_args *>(arg);
  ssize_t rd =
      read(args->fd, args->data, static_cast<size_t>(args->f_stat.st_size));
  if (rd != args->f_stat.st_size) {
    return -1;
  }
  return 0;
}

void import_files(const file_exec files[], size_t batch_size, file_slab &slab) {
  int t_id[20];
  import_fd_args args[20];
  for (size_t i = 0; i < batch_size; i++) {
    int fd = open(files[i].in_path, O_RDONLY);
    if (fd == -1) {
      continue;
    }
    struct stat f_stat;
    int st = fstat(fd, &f_stat);
    char *slot = main_slab + i * slab.slot_size;
    args[i] = {fd, f_stat, slot};
    t_id[i] = clone(import_file_fd, clone_stacks + stacks_size * i,
                    CLONE_VM | CLONE_FILES, &args[i]);
  }

  for (size_t i = 0; i < batch_size; i++) {
    int wstat;
    wait(&wstat);
  }
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
    num_files++;
  }
  closedir(dir);
}
