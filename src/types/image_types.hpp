#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <linux/sysinfo.h>
#include <memory>
#include <mutex>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

enum class color_space {
  RAW,
  SRGB,
  ADOBE,
  WIDE,
  PROPHOTO,
  XYZ,
  ACES,
  DCI_P3,
  REC_2020,
  DWG
};
enum special_gamma { NONE, DI };

struct im_info {
  size_t width;
  size_t height;

  color_space c_space;
  float chromaticities[10]; // white, red, gree, blue
  float gamma[2];           // inverted power, slope
  special_gamma spec_gamm;

  size_t channels;
};

struct exif_data {};

class image_file {
public:
  char path[500];
  size_t raw_file_size;
  void *file_data;
  int fd;

  im_info im_info;
  exif_data exif_data;

  image_file(char *file_path, uint64_t id_, size_t pipe_id_,
             struct im_info *im_info_, struct exif_data *exif_data_) {
    int fd_ = open(file_path, O_RDONLY);
    struct stat stat_buf;
    fstat(fd_, &stat_buf);
    close(fd_);

    raw_file_size = static_cast<size_t>(stat_buf.st_size);

    memccpy(path, file_path, '\0', 500);
    id = id_;
    file_data = nullptr;
    pipe_id = pipe_id_;
    memcpy(im_info_, &im_info, sizeof(im_info));
    memcpy(exif_data_, &exif_data, sizeof(exif_data));
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

  size_t get_done_size() {
    std::unique_lock<std::mutex> lock(done_lk);
    return done.size();
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
class image_tile {
public:
  size_t image_id;
  size_t pipe_id;
  size_t tile_id;
  size_t cord_x;
  size_t cord_y;

  im_info info;
  size_t zero_width;
  size_t zero_height;

  float *data;

  image_tile(size_t image_id_, size_t tile_id_, size_t cord_x_, size_t cord_y_,
             im_info &info_, float *data_) {
    image_id = image_id_;
    tile_id = tile_id_;
    cord_x = cord_x_;
    cord_y = cord_y_;

    info = info_;
    data = data_;
  };

  ~image_tile();

private:
};

class tile_pool {
public:
  size_t num_elems;
  size_t elem_size;
  size_t tile_size;

  std::vector<size_t> avail_index;
  std::mutex avail_index_mtx;
  std::condition_variable avail_cv;
  std::vector<size_t> ready_index;
  std::mutex ready_inndex_mtx;
  std::condition_variable ready_cv;

  std::unique_ptr<std::byte[]> tiles;

  tile_pool(size_t tile_size_, float percent_ram) {
    struct sysinfo s_info;
    int ret = sysinfo(&s_info);
    if (ret != 0) {
    }
    size_t pool_size = static_cast<size_t>(std::lround(
        percent_ram *
        static_cast<float>(
            s_info.totalram *
            s_info.mem_unit))); // reserve space at top for image and tile id
    tiles = std::make_unique<std::byte[]>(pool_size);
    elem_size = tile_size_ + sizeof(image_tile);
    num_elems = pool_size / elem_size;
    tile_size = tile_size_;
  }

  void *get_free_slot() {
    std::unique_lock<std::mutex> lock(avail_index_mtx);
    avail_cv.wait(lock, [this] { return !avail_index.empty(); });

    size_t index = avail_index.back();
    avail_index.pop_back();

    return static_cast<void *>(&(tiles[elem_size * index]));
  }

  void slot_ready(void *slot) {
    std::byte *slot_ptr = static_cast<std::byte *>(slot);
    size_t raw_ptr = static_cast<size_t>(slot_ptr - tiles.get());
    size_t index = raw_ptr / elem_size;

    std::unique_lock<std::mutex> lock(ready_inndex_mtx);
    ready_cv.wait(lock, [this] { return ready_index.size() < num_elems; });

    ready_index.push_back(index);
  }

  void *get_ready() {
    std::unique_lock<std::mutex> lock(ready_inndex_mtx);
    ready_cv.wait(lock, [this] { return !ready_index.empty(); });

    size_t index = ready_index.back();
    ready_index.pop_back();

    return static_cast<void *>(&tiles[elem_size * index]);
  }

  void free_slot(void *slot) {
    std::byte *slot_ptr = static_cast<std::byte *>(slot);
    size_t raw_ptr = static_cast<size_t>(slot_ptr - tiles.get());
    size_t index = raw_ptr / elem_size;

    std::unique_lock<std::mutex> lock(avail_index_mtx);
    avail_cv.wait(lock, [this] { return avail_index.size() != num_elems; });
    avail_index.push_back(index);
  }
};

class image_tiles {
public:
  size_t max_num_tiles;
  std::vector<size_t> not_proc;
  std::mutex not_proc_mtx;
  std::vector<size_t> pre_proced;
  std::mutex pre_proced_mtx;
  std::vector<size_t> processed;
  std::mutex processed_mtx;

  size_t get_not_proc_size() {
    std::unique_lock<std::mutex> lock(not_proc_mtx);
    return not_proc.size();
  }

  size_t get_not_proc() {
    std::unique_lock<std::mutex> lock(not_proc_mtx);
    return not_proc.back();
  }
};
