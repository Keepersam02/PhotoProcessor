#include "io.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

const size_t max_buff_size = 1000000000;
char file_buff[max_buff_size];

char file_paths[1000][2000];

int main(int argc, char **argv) {
  if (argc > 2) {
    exit(-1);
  }
  char dir_path[500];
  if (strlen(argv[1]) > 500) {
    exit(-1);
  }
  strncpy(dir_path, argv[1], 500);

  size_t index = 0;

  DIR *dir = opendir(dir_path);
  struct dirent *dir_entry;
  while ((dir_entry = readdir(dir)) != NULL) {
    if (dir_entry->d_type != DT_REG) {
      continue;
    }
    char file_path[2000] = {};
    strncpy(file_path, dir_path, strlen(dir_path));
    strncat(file_path, "/", 2);
    strncat(file_path, dir_entry->d_name, 256);
    strncat(file_paths[index], file_path, 2000);
    index++;
    if (index >= 1000) {
      break;
    }
  }

  for (size_t k = 0; k < index; k++) {
    printf("%s\n", file_paths[k]);
  }
  return 0;
}
