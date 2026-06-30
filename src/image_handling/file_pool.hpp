#pragma once
#include "gtest/gtest_prod.h"
#include <condition_variable>
#include <cstddef>
#include <cstring>
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

  std::stack<size_t, std::vector<size_t>> avail_index;
  std::stack<size_t, std::vector<size_t>> ready_index;
  std::unique_ptr<std::byte[]> files;

  size_t capacity_;
  size_t elem_size_;

public:
  file_pool(size_t num_elems, size_t elem_size) {
    capacity_ = num_elems;
    elem_size_ = elem_size;

    files = std::make_unique<std::byte[]>(capacity_ * elem_size_);
    for (size_t i = 0; i < capacity_; i++) {
      avail_index.push(i);
    }
  }

  void *get_slot() {
    std::unique_lock<std::mutex> lock(mtx);
    a_cv.wait(lock, [this] { return !avail_index.empty(); });
    const auto index = avail_index.top();
    avail_index.pop();

    void *slot = files.get() + elem_size_ * index;
    return slot;
  }

  void slot_ready(void *file) {
    std::byte *file_ptr = static_cast<std::byte *>(file);
    const size_t raw_ptr = static_cast<size_t>(file_ptr - files.get());
    const size_t index = raw_ptr / elem_size_;
    std::unique_lock<std::mutex> lock(mtx);
    ready_index.push(index);
    r_cv.notify_one();
  }

  void *get_ready() {
    std::unique_lock<std::mutex> lock(mtx);
    r_cv.wait(lock, [this] { return !ready_index.empty(); });
    const size_t index = ready_index.top();
    ready_index.pop();

    void *slot = files.get() + elem_size_ * index;
    return slot;
  }

  void free_slot(void *file) {
    std::byte *file_ptr = static_cast<std::byte *>(file);
    const size_t raw_ptr = static_cast<size_t>(file_ptr - files.get());
    const size_t index = raw_ptr / elem_size_;
    std::unique_lock<std::mutex> lock(mtx);
    avail_index.push(index);
    a_cv.notify_one();
  }
};
