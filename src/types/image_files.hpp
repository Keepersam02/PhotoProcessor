#ifndef IMAGE_FILES
#define IMAGE_FILES

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <sys/types.h>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

class image_file {
public:
  char path[500];
  void *file_data;
  int fd;

  image_file(char *file_path, uint64_t id_, size_t pipe_id_) {
    memccpy(path, file_path, '\0', 500);
    id = id_;
    file_data = nullptr;
    pipe_id = pipe_id_;
  }
  image_file() : id(0) {}

  uint64_t get_id() { return id; }
  size_t get_pipeline_id() { return pipe_id; }

private:
  uint64_t id;
  size_t pipe_id;
};

class image_files {
public:
  image_file *files;
  size_t num_files;
  std::vector<size_t> not_loaded;
  std::vector<size_t> loaded;
  std::vector<size_t> pre_processed;
  std::vector<size_t> processed;
  std::vector<size_t> done;
  std::vector<size_t> failed;
  uint64_t num_free;
  std::mutex not_loaded_lk;
  std::mutex loaded_lk;
  std::mutex pre_proc_lk;
  std::mutex proc_lk;
  std::mutex done_lk;
  std::mutex failed_lk;

  image_files(size_t num_files_) {
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

  image_files(image_files &im_files) {
    num_files = im_files.num_files;
    files = new image_file[num_files];
    std::memcpy(&files, &im_files.files, sizeof(files) * num_files);

    num_free = im_files.num_free;

    not_loaded = im_files.not_loaded;
    loaded = im_files.loaded;
    pre_processed = im_files.pre_processed;
    processed = im_files.processed;
    done = im_files.done;
    failed = im_files.failed;
  }

  ~image_files() { delete[] files; }

  uint64_t get_not_loaded() {
    std::unique_lock<std::mutex> lock(not_loaded_lk);
    if (not_loaded.empty()) {
      return UINT64_MAX;
    }
    size_t index = not_loaded.back();
    not_loaded.pop_back();
    return index;
  }

  size_t get_loaded() {
    std::unique_lock<std::mutex> lock(loaded_lk);
    if (loaded.empty()) {
      return SIZE_MAX;
    }
    size_t index = loaded.back();
    loaded.pop_back();
    return index;
  }

  size_t get_loaded_size() {
    std::unique_lock<std::mutex> lock(loaded_lk);
    return loaded.size();
  }

  void ins_loaded(size_t index) {
    std::unique_lock<std::mutex> lock(loaded_lk);
    loaded.push_back(index);
  }

  size_t get_pre_proc() {
    std::unique_lock<std::mutex> lock(pre_proc_lk);
    if (pre_processed.empty()) {
      return SIZE_MAX;
    }
    size_t index = pre_processed.back();
    pre_processed.pop_back();
    return index;
  }

  size_t get_pre_proc_size() {
    std::unique_lock<std::mutex> lock(pre_proc_lk);
    return pre_processed.size();
  }

  void inspro_proc(size_t index) {
    std::unique_lock<std::mutex> lock(pre_proc_lk);
    pre_processed.push_back(index);
  }

  size_t get_proc() {
    std::unique_lock<std::mutex> lock(proc_lk);
    if (processed.empty()) {
      return SIZE_MAX;
    }
    auto index = processed.back();
    processed.pop_back();
    return index;
  }

  void ins_proc(size_t index) {
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

  void ins_done(size_t index) {
    std::unique_lock<std::mutex> lock(done_lk);
    done.push_back(index);
  }

  size_t get_failed() {
    std::unique_lock<std::mutex> lock(failed_lk);
    if (failed.empty()) {
      return SIZE_MAX;
    }
    auto index = failed.back();
    done.pop_back();
    return index;
  }

  void ins_failed(size_t index) {
    std::unique_lock<std::mutex> lock(failed_lk);
    failed.push_back(index);
  }
  size_t get_failed_size() {
    std::unique_lock<std::mutex> lock(failed_lk);
    return failed.size();
  }
};

#endif
