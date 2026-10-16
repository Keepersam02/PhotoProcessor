#pragma once
#include "image_handler.hpp"
#include "sys/sysinfo.h"
#include "gtest/gtest_prod.h"
#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <linux/sysinfo.h>
#include <memory>
#include <mutex>
#include <stack>
#include <vector>

class file_pool {
private:
  FRIEND_TEST(pool_test, init);
  std::mutex mtx;
  std::condition_variable a_cv;
  std::condition_variable r_cv;

public:
  std::stack<size_t, std::vector<size_t>> avail_index;
  std::stack<size_t, std::vector<size_t>> ready_index;
  std::unique_ptr<std::byte[]> files;

  size_t capacity;
  size_t elem_size;

  /*
   *  Create file_pool based on a max file size and a percent of ram desired to
   * take up.
   */
  file_pool(size_t elem_size_, float percent_ram) {
    // get system info
    struct sysinfo s_info;
    int ret = sysinfo(&s_info);
    if (ret != 0) {
      // TODO learn how to get out of constructor when failing potential bad
      // design whould just pass in sysinfo
    }

    elem_size = elem_size_ + sizeof(uint64_t);
    // calc num elemnts to go into file pool and allocate mem
    float max_pool =
        static_cast<float>(s_info.totalram * s_info.mem_unit) * percent_ram;
    size_t max_elems = static_cast<size_t>(
        static_cast<size_t>(std::lround(max_pool)) / elem_size);
    size_t pool_size = max_elems * elem_size;
    files = std::make_unique<std::byte[]>(pool_size);

    // setting info and making indexes available
    capacity = max_elems;
    for (size_t i = 0; i < capacity; i++) {
      avail_index.push(i);
    }
  }

  /*
   *  Create a file_pool based on a fixed number of elements and a provided max
   * pool size.
   */
  file_pool(size_t elem_size_, size_t max_pool_elems) {
    // set file pool info
    capacity = max_pool_elems;
    elem_size = elem_size_ + sizeof(uint64_t);
    files = std::make_unique<std::byte[]>(elem_size * max_pool_elems);

    // make indexes available
    for (size_t i = 0; i < max_pool_elems; i++) {
      avail_index.push(i);
    }
  }

  /*
   *  Create file_pool based on information about given files and a max pool
   * size.
   */
  file_pool(files_info f_info, size_t max_pool_size) {
    // find number of elements pool holds, set pool info
    //  TODO capacity prob wrong here
    capacity = max_pool_size / f_info.max_size;
    elem_size = f_info.max_size + sizeof(uint64_t);
    size_t pool_remainder = max_pool_size % elem_size;
    size_t pool_size = max_pool_size - pool_remainder;
    files = std::make_unique<std::byte[]>(pool_size);
    // make indexes available
    for (size_t i = 0; i < capacity; i++) {
      avail_index.push(i);
    }
  }

  /*
   * Waits for a slot to become available and grabs it.
   */
  void *get_slot(uint64_t im_id) {
    std::unique_lock<std::mutex> lock(mtx);
    a_cv.wait(lock, [this] { return !avail_index.empty(); });
    const auto index = avail_index.top();
    avail_index.pop();

    void *slot = files.get() + elem_size * index;
    *static_cast<uint64_t *>(slot) = im_id;
    return static_cast<void *>(slot);
  }

  /*
   * Once a file has been loaded adds the pointer to that slot to a ready stack
   * that a later stage uses to pull from.
   */
  void slot_ready(void *file) {
    std::byte *file_ptr = static_cast<std::byte *>(file);
    const size_t raw_ptr = static_cast<size_t>(file_ptr - files.get());
    const size_t index = raw_ptr / elem_size;
    std::unique_lock<std::mutex> lock(mtx);
    ready_index.push(index);
    r_cv.notify_one();
  }

  /*
   *  What later stages use to get a ready file.
   */
  void *get_ready() {
    std::unique_lock<std::mutex> lock(mtx);
    r_cv.wait(lock, [this] { return !ready_index.empty(); });
    const size_t index = ready_index.top();
    ready_index.pop();

    void *slot = files.get() + elem_size * index;
    return slot;
  }

  /*
   * Once later stage is done with the loaded file use this to return slot to
   * avail_index and ready to reuse.
   */
  void free_slot(void *file) {
    std::byte *file_ptr = static_cast<std::byte *>(file);
    const size_t raw_ptr = static_cast<size_t>(file_ptr - files.get());
    const size_t index = raw_ptr / elem_size;
    std::unique_lock<std::mutex> lock(mtx);
    avail_index.push(index);
    a_cv.notify_one();
  }
};
