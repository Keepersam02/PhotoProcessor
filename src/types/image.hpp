#include <cstdint>
#include <stdfloat>
#include <string>
#include <vector>

enum color_space {

}

struct Image {
  std::string file_name;
  std::string file_path;

  int height;
  int width;
  int channels;
}
