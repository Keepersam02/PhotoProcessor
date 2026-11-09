#include "io/in.hpp"
#include "proc/proc.hpp"
#include "types/file_slab.hpp"
#include "types/misc.hpp"
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

char main_slab[2000000000];

char io_stack[1024 * 1024];

void change_extension(char path[750], char ext[15]) {
  for (size_t i = strlen(path); i > 0; i--) {
    if (path[i] == '.') {
      strncpy(&path[i], ext, 15);
      break;
    }
  }
}

void setup_paths(const char in_dir_path[500], const char out_dir_path[500],
                 file_exec &f_exec, dirent *file) {
  strncpy(f_exec.in_path, in_dir_path, 500);
  strcat(f_exec.in_path, "/");
  strncat(f_exec.in_path, file->d_name, 245);

  strncpy(f_exec.out_path, out_dir_path, 500);
  strcat(f_exec.out_path, "/");
  strncat(f_exec.out_path, file->d_name, 240);
  char ext[15] = ".tiff";
  change_extension(f_exec.out_path, ext);
}

void execute_set(size_t set_size, file_exec execs[], int pids[],
                 file_slab &slab) {
  for (size_t i = 0; i < set_size; i++) {
    file_exec cur = execs[i];
    char *slot = main_slab + i * slab.slot_size;
    pids[i] = fork();
    if (pids[i] == 0) {
      debayer_write(cur.out_path, slot, static_cast<size_t>(cur.f_size));
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

int bpp_loop(const char in_dir_path[500], const char out_dir_path[500],
             const size_t batch_size) {}
