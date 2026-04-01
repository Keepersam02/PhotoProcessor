#include "io/image_io_error.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <expected>
#include <stdfloat>

std::expected<std::array<float, 250>, image_error>
linear_exposure(std::array<float, 250> input, float adjustment) {
  std::array<float, 250> adjusted;
  for (size_t i = 0; i < 250; i++) {
    adjusted.at(i) = std::exp2(adjustment) * input.at(i);
  }
  return adjusted;
}
