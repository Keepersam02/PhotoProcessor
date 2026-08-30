#include <algorithm>
#include <array>
#include <cstdlib>
#include <filesystem>
#include <stack>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

class image_file {
public:
  fs::path file_path_;
  void *data_;

  image_file(fs::path file_path, uint64_t id) {
    file_path_ = file_path;
    id_ = id;
    data_ = NULL;
  }

  uint64_t get_id() { return id_; }

private:
  uint64_t id_;
};

class image_files {
public:
  image_file *files_;
  uint64_t num_files_;
  std::vector<uint64_t> not_loaded_;
  std::vector<uint64_t> loaded_;
  std::vector<uint64_t> finished_;
  std::vector<uint64_t> failed;
  uint64_t num_free_;

  image_files(uint64_t num_files) {
    num_files_ = num_files;
    files_ = static_cast<image_file *>(malloc(sizeof(uint64_t) * num_files));

    not_loaded_.reserve(num_files);
    loaded_.reserve(num_files);
    finished_.reserve(num_files);

    num_free_ = 0;
  }
};
