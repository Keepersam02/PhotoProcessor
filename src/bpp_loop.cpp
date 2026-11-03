#include "io/in.hpp"
#include "proc/proc.hpp"
#include "types/file_slab.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

char main_slab[2000000000];

int bpp_loop(const char in_dir_path[500], const char out_dir_path[500]) {
  size_t max_file_size = 0;

  files_info(in_dir_path, max_file_size);
  if (max_file_size == 0) {
    fprintf(stderr, "no files found\n");
    return -1;
  }
  if ((max_file_size * 2) > 2000000000) {
    fprintf(stderr, "files too big\n");
    return -1;
  }

  file_slab slab(max_file_size);

  DIR *dir = opendir(in_dir_path);
  struct dirent *dir_entry_1;
  struct dirent *dir_entry_2;
  char out_path_1[750];
  char f_path_1[750];

  char out_path_2[750];
  char f_path_2[750];

  int pid1;
  int pid2;

  dir_entry_1 = readdir(dir);
  if (dir_entry_1 == NULL) {
    return -1;
  }
  dir_entry_2 = readdir(dir);
  if (dir_entry_2 == NULL) {

    strncpy(out_path_1, out_dir_path, 500);
    strncat(out_path_1, dir_entry_1->d_name, 249);

    strncpy(f_path_1, in_dir_path, 500);
    strncpy(f_path_1, dir_entry_1->d_name, 249);

    import_file(f_path_1, main_slab + slab.slot_size * 1);

    struct stat f_stat;
    stat(f_path_1, &f_stat);
    debayer_write(out_path_1, main_slab + slab.slot_size * 1,
                  static_cast<size_t>(f_stat.st_size));

    return 0;
  }
  while (1) {

    if (dir_entry_1 == NULL && dir_entry_2 == NULL) {
      return 0;
    }

    if (dir_entry_2 == NULL) {

      strncpy(out_path_1, out_dir_path, 500);
      strncat(out_path_1, dir_entry_1->d_name, 249);

      strncpy(f_path_1, in_dir_path, 500);
      strncpy(f_path_1, dir_entry_1->d_name, 249);

      import_file(f_path_1, main_slab + slab.slot_size * 1);

      struct stat f_stat;
      stat(f_path_1, &f_stat);
      debayer_write(out_path_1, main_slab + slab.slot_size * 1,
                    static_cast<size_t>(f_stat.st_size));

      return 0;
    }
    strncpy(out_path_1, out_dir_path, 500);
    strncat(out_path_1, dir_entry_1->d_name, 249);

    strncpy(f_path_1, in_dir_path, 500);
    strncpy(f_path_1, dir_entry_1->d_name, 249);

    strncpy(out_path_2, out_dir_path, 500);
    strncat(out_path_2, dir_entry_2->d_name, 249);

    strncpy(f_path_2, in_dir_path, 500);
    strncpy(f_path_2, dir_entry_2->d_name, 249);

    struct stat f_stat1;
    struct stat f_stat2;
    stat(f_path_1, &f_stat1);
    stat(f_path_2, &f_stat2);

    import_file(f_path_1, main_slab + slab.slot_size * 1);
    import_file(f_path_2, main_slab + slab.slot_size * 2);

    debayer_write(out_path_1, main_slab + slab.slot_size * 1,
                  static_cast<size_t>(f_stat1.st_size));
    pid1 = fork();

    debayer_write(out_path_2, main_slab + slab.slot_size * 2,
                  static_cast<size_t>(f_stat2.st_size));
    pid2 = fork();

    int wstat1;
    int wstat2;
    wait(&wstat1);
    wait(&wstat2);
  }
  return 0;
}
