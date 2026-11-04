
#pragma once
#ifndef deb

#include <cstddef>
#include <cstdint>
class slot_stack {
public:
  size_t slots[2];
  size_t capacity;
  size_t size;

  slot_stack(int init_full) {
    if (init_full == 1) {
      capacity = 2;
      size = 2;
      for (size_t i = 0; i < capacity; i++) {
        slots[i] = i;
      }
    } else {
      capacity = 2;
      size = 0;
    }
  }

  size_t pop() {
    if (size == 0) {
      return SIZE_MAX;
    }
    size--;
    return slots[size];
  }

  size_t push(size_t slot) {
    if (size == capacity) {
      return SIZE_MAX;
    }
    slots[size] = slot;
    size++;
    return slot;
  }
};

class file_slab {
public:
  size_t num_slots;
  size_t slot_size;

  slot_stack free_s;
  slot_stack loaded;

  file_slab(size_t max_file_size) : free_s(1), loaded(0) {
    num_slots = 2;
    slot_size = max_file_size + sizeof(size_t) + 750;
  }
};
#endif // DEBUG
