#include "io/image_io.hpp"
#include "io/image_io_error.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <expected>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <openssl/evp.h>
#include <sys/mman.h>
#include <system_error>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
/*
void copy_files(std::vector<fs::path> &file_paths,
                std::vector<fs::path> &dest_dirs, uint8_t max_threads) {}
*/
std::expected<bool, image_error> file_copier(void *file, size_t file_size,
                                             fs::path src, fs::path dest_dir) {
  const auto file_name = src.filename();
  fs::path file_dest = dest_dir / file_name;

  auto dest_file = fopen(file_dest.c_str(), "wb");
  if (dest_file == NULL) {
    std::string err = std::system_category().message(errno);
    return std::unexpected(
        image_error::IO(-1, err_severity::ERROR, err,
                        std::format("src path:{}, dest path: {}", src.c_str(),
                                    file_dest.c_str())));
  }
  auto written = fwrite(file, file_size, 1, dest_file);
  fclose(dest_file);
  if (written == 0) {
    return std::unexpected(image_error::IO(
        -1, err_severity::ERROR, "failed to write to destination",
        std::format("src:{}, dest:{}", src.c_str(), file_dest.c_str())));
  }
  return true;
}

namespace {
void file_hash(void *file, size_t size, unsigned char raw_hash[EVP_MAX_MD_SIZE],
               unsigned int *hash_size);
}

bool file_copy_ver(std::vector<std::pair<fs::path, fs::path>> &files,
                   std::vector<std::pair<fs::path, fs::path>> &failed) {
  std::vector<std::pair<fs::path, fs::path>> succ;
  succ.reserve(files.size());
  for (const auto &file : files) {
    auto file_1 = load_file(file.first);
    if (!file_1) {
      failed.push_back({file.first, file.second});
      continue;
    }
    auto file_2 = load_file(file.second);
    if (!file_2) {
      failed.push_back({file.second, file.second});
      continue;
    }

    unsigned char f1_hash[EVP_MAX_MD_SIZE];
    unsigned int f1_hash_size;
    file_hash(file_1->first, file_1->second, f1_hash, &f1_hash_size);

    unsigned char f2_hash[EVP_MAX_MD_SIZE];
    unsigned int f2_hash_size;
    file_hash(file_2->first, file_2->second, f2_hash, &f2_hash_size);
    if (f1_hash_size != f2_hash_size) {
      failed.push_back(std::make_pair(file.first, file.second));
      continue;
    }
    for (unsigned int i = 0; i < f1_hash_size; i++) {
      if (f1_hash[i] != f2_hash[i]) {
        failed.push_back(std::make_pair(file.first, file.second));
        continue;
      }
    }
    succ.push_back(file);
  }
  files = std::move(succ);
  if (failed.size() == 0) {
    return true;
  }
  return false;
}

namespace {
void file_hash(void *file, size_t size, unsigned char raw_hash[EVP_MAX_MD_SIZE],
               unsigned int *hash_size) {
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
  EVP_DigestUpdate(ctx, file, size);
  hash_size = 0;
  EVP_DigestFinal_ex(ctx, raw_hash, hash_size);
  EVP_MD_CTX_free(ctx);
}
} // namespace
