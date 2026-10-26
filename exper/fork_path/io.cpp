#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

int import_image(char *path, void *data, size_t data_size) {
  int fd = open(path, O_RDONLY);
  if (fd <= 0) {
    return -1;
  }

  struct stat f_info;
  {
    int ret = fstat(fd, &f_info);
    if (ret <= 0) {
      return ret;
    }
  }
  if (static_cast<unsigned>(f_info.st_size) > data_size) {
    return -1;
  }

  ssize_t ret = read(fd, data, static_cast<unsigned>(f_info.st_size));
  if (ret != 0) {
    if (ret > 0) {
      return -1;
    }
    return ret;
  }
  close(fd);
  return 0;
}
