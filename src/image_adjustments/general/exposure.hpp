#include "../../io/image_io_error.hpp"
#include <array>
#include <expected>

std::expected<std::array<float, 250>, image_error>
linear_exposure(std::array<float, 250> input, float adjustment);
