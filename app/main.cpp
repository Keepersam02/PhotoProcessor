#include "bpp_loop.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stdout, "Must give directory in and out");
    exit(EXIT_FAILURE);
  }

  int ret = open(argv[1], O_DIRECTORY);
  if (errno == ENOTDIR) {
    fprintf(stdout,
            "Proveded input path does not point to directory: path: %s\n",
            argv[1]);
    exit(EXIT_FAILURE);
  } else if (ret < 0) {
    fprintf(stdout, "Error with input dir\n");
    exit(EXIT_FAILURE);
  }

  int ret_d = open(argv[2], O_DIRECTORY);
  if (errno == ENOTDIR) {
    fprintf(stdout, "Provided output path not dir, path: %s\n", argv[2]);
    exit(EXIT_FAILURE);
  } else if (ret_d < 0) {
    fprintf(stdout, "Error with input dir\n");
    exit(EXIT_FAILURE);
  }

  bpp_loop(argv[1], argv[2]);

  exit(EXIT_SUCCESS);
}
