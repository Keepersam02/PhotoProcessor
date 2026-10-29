#include <cstddef>
#include <cstdlib>

int import_image(char *path, void *data, size_t data_size);

class raw_file_buf {
  size_t slot_size;
  size_t free_slot;

  void *data;

  raw_file_buf(size_t slot_size_, size_t &init_free) {
    slot_size = slot_size_;
    free_slot = 0;
    init_free = 1;

    data = malloc(slot_size * 2);
  }

  ~raw_file_buf() { free(data); }
};
