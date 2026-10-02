#ifndef IMAGE_FILES
#define IMAGE_FILES

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <sys/types.h>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

class image_file {
public:
  fs::path file_path_;
  void *file_data;
  int fd;

  image_file(fs::path file_path, uint64_t id) {
    file_path_ = std::move(file_path);
    id_ = id;
    file_data = nullptr;
  }
  image_file() : id_(0) {}

  uint64_t get_id() { return id_; }

private:
  uint64_t id_;
};

class image_files {
public:
  image_file *files;
  uint64_t num_files;
  std::vector<uint64_t> not_loaded;
  std::vector<uint64_t> loaded;
  std::vector<uint64_t> pre_processed;
  std::vector<uint64_t> processed;
  std::vector<uint64_t> done;
  std::vector<uint64_t> failed;
  uint64_t num_free;
  std::mutex not_loaded_lk;
  std::mutex loaded_lk;
  std::mutex pre_proc_lk;
  std::mutex proc_lk;
  std::mutex done_lk;
  std::mutex failed_lk;

  image_files(uint64_t num_files_) {
    num_files = num_files_;
    files = new image_file[num_files];
    not_loaded.reserve(num_files);
    loaded.reserve(num_files);
    pre_processed.reserve(num_files);
    processed.reserve(num_files);
    done.reserve(num_files);
    num_free = 0;
    for (uint64_t i = 0; i < num_files; i++) {
      not_loaded.push_back(i);
    }
  }
  ~image_files() { delete[] files; }

  uint64_t get_not_loaded() {
    std::unique_lock<std::mutex> lock(not_loaded_lk);
    if (not_loaded.empty()) {
      return UINT64_MAX;
    }
    uint64_t index = not_loaded.back();
    not_loaded.pop_back();
    return index;
  }

  uint64_t get_loaded() {
    std::unique_lock<std::mutex> lock(loaded_lk);
    if (loaded.empty()) {
      return UINT64_MAX;
    }
    uint64_t index = loaded.back();
    loaded.pop_back();
    return index;
  }

  size_t get_loaded_size() {
    std::unique_lock<std::mutex> lock(loaded_lk);
    return loaded.size();
  }

  void ins_loaded(uint64_t index) {
    std::unique_lock<std::mutex> lock(loaded_lk);
    loaded.push_back(index);
  }

  uint64_t get_pre_proc() {
    std::unique_lock<std::mutex> lock(pre_proc_lk);
    if (pre_processed.empty()) {
      return UINT64_MAX;
    }
    uint64_t index = pre_processed.back();
    pre_processed.pop_back();
    return index;
  }

  size_t get_pre_proc_size() {
    std::unique_lock<std::mutex> lock(pre_proc_lk);
    return pre_processed.size();
  }

  void inspro_proc(uint64_t index) {
    std::unique_lock<std::mutex> lock(pre_proc_lk);
    pre_processed.push_back(index);
  }

  uint64_t get_proc() {
    std::unique_lock<std::mutex> lock(proc_lk);
    if (processed.empty()) {
      return UINT64_MAX;
    }
    auto index = processed.back();
    processed.pop_back();
    return index;
  }

  void ins_proc(uint64_t index) {
    std::unique_lock<std::mutex> lock(proc_lk);
    processed.push_back(index);
  }

  uint64_t get_done() {
    std::unique_lock<std::mutex> lock(done_lk);
    if (done.empty()) {
      return UINT64_MAX;
    }
    auto index = done.back();
    done.pop_back();
    return index;
  }

  void ins_done(uint64_t index) {
    std::unique_lock<std::mutex> lock(done_lk);
    done.push_back(index);
  }

  uint64_t get_failed() {
    std::unique_lock<std::mutex> lock(failed_lk);
    if (failed.empty()) {
      return UINT64_MAX;
    }
    auto index = failed.back();
    done.pop_back();
    return index;
  }

  void ins_failed(uint64_t index) {
    std::unique_lock<std::mutex> lock(failed_lk);
    failed.push_back(index);
  }
};

#endif
