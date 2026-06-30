#include "image_handling/file_pool.hpp"
#include "gtest/gtest.h"
#include <atomic>

TEST(pool_test, init) {
  size_t capacity = 5;
  size_t elem_size = sizeof(int);

  file_pool pool(5, elem_size);

  EXPECT_EQ(pool.capacity_, capacity);
  EXPECT_EQ(pool.elem_size_, elem_size);

  EXPECT_EQ(pool.avail_index.size(), 5);
  EXPECT_TRUE(pool.ready_index.empty());
}

TEST(pool_test, single_th) {
  size_t capacity = 2;
  size_t elem_size = sizeof(int);
  file_pool pool(capacity, elem_size);

  auto ptr_1 = pool.get_slot();
  std::byte *b_ptr_1 = static_cast<std::byte *>(ptr_1);
  auto ptr_2 = pool.get_slot();
  std::byte *b_ptr_2 = static_cast<std::byte *>(ptr_2);

  auto ptr_diff = std::abs(b_ptr_2 - b_ptr_1);
  EXPECT_EQ(ptr_diff, sizeof(int));

  pool.slot_ready(ptr_1);
  pool.slot_ready(ptr_2);

  void *ready = pool.get_ready();
  EXPECT_NE(ready, nullptr);
}

TEST(pool_test, multi_th) {
  const int NUM_IMAGES = 10000;
  const int CAPACITY = 16;

  file_pool pool(CAPACITY, sizeof(int));

  std::atomic<int> im_produced{0};
  std::atomic<int> im_consumed{0};
  std::atomic<int> val_failures{0};

  auto produce = [&]() {
    while (true) {
      int im_id = im_produced.fetch_add(1);
      if (im_id >= NUM_IMAGES) {
        break;
      }

      void *mem = pool.get_slot();
      *static_cast<int *>(mem) = im_id;

      pool.slot_ready(mem);
    }
  };

  auto consume = [&]() {
    while (true) {
      int task_id = im_consumed.fetch_add(1);
      if (task_id >= NUM_IMAGES) {
        break;
      }

      void *mem = pool.get_ready();

      int id = *static_cast<int *>(mem);

      if (id < 0 || id > NUM_IMAGES) {
        val_failures.fetch_add(1);
      }
      pool.free_slot(mem);
    }
  };
  std::vector<std::thread> threads;
  for (int i = 0; i < 4; i++)
    threads.emplace_back(produce);
  for (int i = 0; i < 4; i++)
    threads.emplace_back(consume);

  for (auto &t : threads) {
    t.join();
  }
  EXPECT_EQ(val_failures.load(), 0);
}
