#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <set>
#include <vector>

#include "color_format.hpp"

namespace palette {

std::vector<std::array<float, 4>> extract(const std::vector<color_format::component_type>& format, const std::vector<float>& image, int width, int height) noexcept;
std::vector<std::array<float, 4>> quantize(const std::vector<std::array<float, 4>>& palette, int colors) noexcept;
std::vector<std::array<float, 4>> gpl_load(const char* path, std::string& name, int& columns) noexcept;
std::vector<std::array<float, 4>> binary_load(const char* path, const std::vector<color_format::component_type>& format) noexcept;
std::string to_gpl(const std::vector<std::array<float, 4>>& palette, float pow) noexcept;

} // palette
