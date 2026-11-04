#include "io/in.hpp"
#include "proc/proc.hpp"
#include "types/file_slab.hpp"
#include "types/misc.hpp"
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

char main_slab[2000000000];

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
             const size_t batch_size) {
  size_t max_file_size = 0;
  size_t num_files = 0;

  files_info(in_dir_path, max_file_size, num_files);
  if (max_file_size == 0) {
    fprintf(stderr, "no files found\n");
    return -1;
  }
  if ((max_file_size * batch_size * 2) > 2000000000) {
    fprintf(stderr, "files too big\n");
    return -1;
  }

  file_slab slab(max_file_size);

  size_t num_proc = 0;

  file_exec files_e[32];

  DIR *dir = opendir(in_dir_path);
  if (dir == NULL) {
    return -1;
  }
  int pids[20];
  uint8_t to_proc = 0;
  // setup first set of images to process
  size_t file_off = batch_size * to_proc;
  for (size_t i = 0; i < batch_size; i++) {
    size_t index = i + file_off;
    dirent *cur = readdir(dir);
    setup_paths(in_dir_path, out_dir_path, files_e[index], cur);

    int fd = open(files_e[index].in_path, O_RDONLY);
    if (fd == -1) {
      continue;
    }
    struct stat f_stat;
    int st = fstat(fd, &f_stat);
    files_e[index].f_size = static_cast<size_t>(f_stat.st_size);
  }
  import_files_arg args;
  *args.files = files_e;
  args.batch_size = batch_size;
  args.slab = slab;

  import_files(&args);
  while (1) {
    if ((num_files - num_proc) == 0) {
      break;
    } else if ((num_files - num_proc) < batch_size) {
      for (size_t i = 0; i < (num_files - num_proc); i++) {
      }
      break;
    }

    size_t proc_off = batch_size * to_proc;
    size_t import_off = batch_size * (!to_proc);
    for (size_t i = 0; i < batch_size; i++) {
      size_t index = i + file_off;
      dirent *cur = readdir(dir);
      setup_paths(in_dir_path, out_dir_path, files_e[index], cur);

      int fd = open(files_e[index].in_path, O_RDONLY);
      if (fd == -1) {
        continue;
      }
      struct stat f_stat;
      int st = fstat(fd, &f_stat);
      files_e[index].f_size = static_cast<size_t>(f_stat.st_size);
    }
    import_files_arg args;
    *args.files = &files_e[import_off];
    args.batch_size = batch_size;
    args.slab = slab;
    char child_stack[2000];
    // todo find why not work
    int t_id = clone(import_files, child_stack + 2000,
                     CLONE_FILES | CLONE_VM | SIGCHLD, &args);

    execute_set(batch_size, &files_e[proc_off], pids, slab);

    int wstat;
    waitpid(t_id, &wstat, 0);

    to_proc = !to_proc;
  }
  return 0;
}
