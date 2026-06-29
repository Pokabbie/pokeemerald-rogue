#pragma once

#include <memory>
#include <vector>

#include <stb_image.h>

#include "color_format.hpp"

namespace image {

std::unique_ptr<stbi_uc[], void(*)(void*)> load(const char* filename, int& width, int& height, int& channels) noexcept;
std::vector<float> to_float(const std::unique_ptr<stbi_uc[], void(*)(void*)>& image, int width, int height, float pow) noexcept;
std::vector<float> resize(const std::vector<float>& image, int inWidth, int inHeight, int outWidth, int outHeight) noexcept;
std::vector<float> resize_and_resolve(const std::vector<float>& image, int inWidth, int inHeight, int outWidth, int outHeight) noexcept;
std::vector<stbi_uc> to_data(const std::vector<float>& image, int width, int height, float pow, const std::vector<color_format::component_type>& format) noexcept;
std::vector<float> flatten(const std::vector<std::array<float, 4>>& image) noexcept;
std::vector<std::size_t> palettize(const std::vector<float>& image, const std::vector<std::array<float, 4>>& palette) noexcept;
std::vector<float> expand(const std::vector<std::size_t>& indices, const std::vector<std::array<float, 4>>& palette) noexcept;
std::vector<std::array<float, 4>> gamma_pow(const std::vector<std::array<float, 4>>& image, float gamma) noexcept;

enum class direction {
    plus_x,
    plus_y,
    minus_x,
    minus_y
};

std::pair<direction, direction> direction_pair(const std::string& str) noexcept;

[[nodiscard]]
constexpr bool is_x_axis(direction d) noexcept {
    return d == direction::plus_x || d == direction::minus_x;
}

[[nodiscard]]
constexpr bool same_axis(direction major, direction minor) noexcept {
    return is_x_axis(major) == is_x_axis(minor);
}

[[nodiscard]]
constexpr bool is_normal(direction major, direction minor) noexcept {
    return major == direction::plus_x && minor == direction::plus_y;
}

std::vector<float> orientate(const std::vector<float>& image, int width, int height, direction major, direction minor) noexcept;
std::vector<std::size_t> orientate(const std::vector<std::size_t>& image, int width, int height, direction major, direction minor) noexcept;

} // namespace image
