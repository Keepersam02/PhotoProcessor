#include "io/in.hpp"
#include "proc/proc.hpp"
#include "types/file_slab.hpp"
#include "types/misc.hpp"
#include <bits/types/idtype_t.h>
#include <cerrno>
#include <csignal>
#include <cstddef>
#include <cstdint>
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
    char *slot = main_slab + cur.index * slab.slot_size;
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

uint8_t valid_file(dirent *entry) {
  if (!strncmp(entry->d_name, ".", 1) || !strncmp(entry->d_name, "..", 2)) {
    return 0;
  }

  if (entry->d_type != DT_REG) {
    return 0;
  }
  return 1;
}

int bpp_loop(const char in_dir_path[500], const char out_dir_path[500],
             const size_t batch_size) {
  size_t max_size = 0;
  size_t num_files = 0;
  files_info(in_dir_path, max_size, num_files);
  file_slab slab(max_size);

  file_exec execs[2];

  int pids[2];

  DIR *dir = opendir(in_dir_path);
  if (dir == NULL) {
    fprintf(stderr, "invalid input dir, err: %s, path: %s\n", strerror(errno),
            in_dir_path);
    exit(-1);
  }

  dirent *cur;

  // find a regular file
  while ((cur = readdir(dir)) != NULL) {
    if (valid_file(cur)) {
      break;
    }
  }

  setup_paths(in_dir_path, out_dir_path, execs[0], cur);
  execs[0].index = 0;
  struct stat st;
  int ft = stat(execs[0].in_path, &st);
  if (ft == -1) {
    fprintf(stderr, "failed to get initial file stats, err: %s, path: %s\n",
            strerror(errno), execs[0].in_path);
    return -1;
  }
  execs[0].f_size = static_cast<size_t>(st.st_size);

  import_file_arg arg = {&execs[0], main_slab};
  int ret = import_file(&arg);
  if (ret != 0) {
    exit(-1); // function prints to standard error already
  }

  size_t num_loaded = 1;
  size_t num_failed = 0;
  int load_index = 1; // index where file will be loaded in that loop iteration
  while (1) {
    if ((num_loaded + num_failed) == num_files) {
      execute_set(1, &execs[!load_index], pids, slab);
    }

  load_redo:
    if ((num_loaded + num_failed) == num_files) {
      break;
    }
    uint8_t done = 0;
    while (1) {
      if ((cur = readdir(dir)) == NULL) {
        done = 1;
        break;
      }
      if (valid_file(cur)) {
        break;
      }
    }
    if (done) {
      break;
    }
    setup_paths(in_dir_path, out_dir_path, execs[load_index], cur);
    struct stat f_stat;
    int fi = stat(execs[load_index].in_path, &f_stat);
    if (fi == -1) {
      fprintf(stderr, "error reading file info, err: %s, path: %s\n",
              strerror(errno), execs[load_index].in_path);
      num_failed++;
      goto load_redo;
    }
    execs[load_index].f_size = f_stat.st_size;
    execs[load_index].index = load_index;

    import_file_arg args = {&execs[load_index],
                            main_slab + slab.slot_size * load_index};
    int t_id = clone(import_file, io_stack + 1024 * 1024,
                     CLONE_VM | CLONE_FILES | SIGCHLD, &args);
    if (t_id == -1) {
      fprintf(stderr, "error starting load thread: err: %s, path: %s",
              strerror(errno), execs[load_index].in_path);
      num_failed++;
      goto load_redo;
    }

    execute_set(1, &execs[!load_index], pids, slab);

    int w_stat;
    waitpid(t_id, &w_stat, 0);
    if (w_stat == -1) {
      fprintf(stderr, "error processing file, err: %s, path %s\n",
              strerror(errno), execs[!load_index].in_path);
      num_failed++;
    }
    load_index = !load_index;
  }
  if (num_failed != 0) {
    return -1;
  }
  return 0;
}
