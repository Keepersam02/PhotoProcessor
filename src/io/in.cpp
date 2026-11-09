#include "in.hpp"
#include "fcntl.h"
#include "types/file_slab.hpp"
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

int import_file_fd(void *arg) {
  import_fd_args *args = static_cast<import_fd_args *>(arg);
  ssize_t rd =
      read(args->fd, args->data, static_cast<size_t>(args->f_stat.st_size));
  if (rd != args->f_stat.st_size) {
    return -1;
    close(args->fd);
  }
  return 0;
}

int import_files_st(void *args) {
  import_files_arg *arg = static_cast<import_files_arg *>(args);
  for (size_t i = 0; i < arg->batch_size; i++) {
    int fd = open(arg->files[i].in_path, O_RDONLY);
    if (fd == -1) {
      fprintf(stderr, "error opening file, err: %s, path: %s\n",
              strerror(errno), arg->files[i].in_path);
      continue;
    }
    struct stat f_stat;
    int st = fstat(fd, &f_stat);
    if (st == -1) {
      fprintf(stderr, "error getting file info err: %s, path: %s\n",
              strerror(errno), arg->files[i].in_path);
      continue;
    }
    ssize_t rd = read(fd, &arg->slab + i * arg->batch_size, f_stat.st_size);
    if (rd != 0) {
      fprintf(stderr, "failed to read entire file, err: %s, path: %s\n",
              strerror(errno), arg->files[i].in_path);
      continue;
    }
  }
  return 0;
}

void import_files_mt(void *arg) {
  import_files_arg *args = static_cast<import_files_arg *>(arg);
  int t_id[20];
  import_fd_args argss[20];
  for (size_t i = 0; i < args->batch_size; i++) {
    int fd = open(args->files[i].in_path, O_RDONLY);
    if (fd == -1) {
      continue;
    }
    struct stat f_stat;
    fstat(fd, &f_stat);
    char *slot = main_slab + i * args->slab->slot_size;
    argss[i] = {fd, f_stat, slot};
    t_id[i] =
        clone(import_file_fd, clone_stacks + stacks_size * i + stacks_size,
              CLONE_VM | CLONE_FILES | SIGCHLD, &args[i]);
  }

  for (size_t i = 0; i < args->batch_size; i++) {
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
