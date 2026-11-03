#include "io/in.hpp"
#include "proc/proc.hpp"
#include "types/file_slab.hpp"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

char main_slab[2000000000];

struct file_exec {
  char in_path[750];
  char out_path[750];
  size_t f_size;
  size_t index;
};

void setup_paths(const char in_dir_path[500], const char out_dir_path[500],
                 file_exec &f_exec, dirent *file) {
  strncpy(f_exec.in_path, in_dir_path, 500);
  strcat(f_exec.in_path, "/");
  strncat(f_exec.in_path, file->d_name, 245);

  strncpy(f_exec.out_path, out_dir_path, 500);
  strcat(f_exec.out_path, "/");
  strncat(f_exec.out_path, file->d_name, 245);
}

void execute_set(size_t set_size, file_exec execs[], int pids[],
                 file_slab &slab) {
  for (size_t i = 0; i < set_size; i++) {
    file_exec cur = execs[i];
    int fd = open(cur.in_path, O_RDONLY);
    if (fd == -1) {
      continue;
    }
    struct stat f_stat;
    int ret = fstat(fd, &f_stat);
    if (ret == -1) {
      continue;
    }
    char *slot = main_slab + slab.slot_size * i;
    ssize_t rd = read(fd, slot, static_cast<size_t>(f_stat.st_size));
    if (rd != 0 && rd != static_cast<ssize_t>(f_stat.st_size)) {
      continue;
    }

    pids[i] = fork();
    if (pids[i] == 0) {
      debayer_write(cur.out_path, slot, static_cast<size_t>(f_stat.st_size));
      _exit(0);
    }
  }

  for (size_t i = 0; i < set_size; i++) {
    int status;
    int pid = pids[i];
    waitpid(pid, &status, 0);
    if (status == -1) {
      printf("error occured in processing\n");
    }
  }
}

int bpp_loop(const char in_dir_path[500], const char out_dir_path[500]) {
  size_t max_file_size = 0;
  size_t num_files = 0;

  files_info(in_dir_path, max_file_size, num_files);
  if (max_file_size == 0) {
    fprintf(stderr, "no files found\n");
    return -1;
  }
  if ((max_file_size * 2) > 2000000000) {
    fprintf(stderr, "files too big\n");
    return -1;
  }

  file_slab slab(max_file_size);

  size_t num_proc = 0;
  const size_t batch_size = 2;

  file_exec files_e[2];

  DIR *dir = opendir(in_dir_path);
  if (dir == NULL) {
    return -1;
  }
  int pids[2];
  while (1) {
    if ((num_files - num_proc) == 0) {
      break;
    } else if ((num_files - num_proc) < batch_size) {
      for (size_t i = 0; i < (num_files - num_proc); i++) {
        dirent *cur = readdir(dir);
        if (cur == NULL) {
          continue;
        }
        setup_paths(in_dir_path, out_dir_path, files_e[i], cur);
        int fd = open(files_e[i].in_path, O_RDONLY);
        if (fd == -1) {
          continue;
        }
        struct stat f_stat;
        int ret = fstat(fd, &f_stat);
        if (ret == -1) {
          close(fd);
          continue;
        }
        files_e[i].f_size = static_cast<size_t>(f_stat.st_size);
        files_e[i].index = i;
      }
      execute_set(num_files - num_proc, files_e, pids, slab);
      num_proc += (num_files - num_proc);
      continue;
    }

    for (size_t i = 0; i < batch_size; i++) {
      dirent *cur = readdir(dir);
      if (cur == NULL) {
        continue;
      }
      while (!strcmp("..", cur->d_name) || !strcmp(".", cur->d_name)) {
        cur = readdir(dir);
      }
      setup_paths(in_dir_path, out_dir_path, files_e[i], cur);
      int fd = open(files_e[i].in_path, O_RDONLY);
      if (fd == -1) {
        continue;
      }
      struct stat f_stat;
      int ret = fstat(fd, &f_stat);
      if (ret == -1) {
        close(fd);
        continue;
      }
      files_e[i].f_size = static_cast<size_t>(f_stat.st_size);
      files_e[i].index = i;
      close(fd);
    }
    execute_set(batch_size, files_e, pids, slab);
    num_proc += batch_size;
    continue;
  }

  return 0;
}
