#include "image_io.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <iterator>
#include <numeric>
#include <ranges>

#include "stb_image_resize.h"
#include "util.hpp"

namespace {

constexpr auto rgba_channels = 4;

auto bitlen_to_byte_size(auto x) noexcept {
    return (x + 7) / 8;
}

}

std::unique_ptr<stbi_uc[], void(*)(void*)> image::load(const char* filename, int& width, int& height, int& channels) noexcept {
    auto* img = stbi_load(filename, &width, &height, &channels, rgba_channels);
    return {img, img ? stbi_image_free : [](void*){}};
}

std::vector<float> image::to_float(const std::unique_ptr<stbi_uc[], void(*)(void*)>& image, int width, int height, float pow) noexcept {
    const auto imageStride = width * rgba_channels * sizeof(stbi_uc);

    auto result = std::vector<float>{};
    result.reserve(width * height * rgba_channels);

    for (int yy = 0; yy < height; ++yy) {
        for (int xx = 0; xx < imageStride; xx += 4) {
            result.emplace_back(util::pow_clamp(image[yy * imageStride + xx + 0] / 255.0, pow));
            result.emplace_back(util::pow_clamp(image[yy * imageStride + xx + 1] / 255.0, pow));
            result.emplace_back(util::pow_clamp(image[yy * imageStride + xx + 2] / 255.0, pow));
            result.emplace_back(util::pow_clamp(image[yy * imageStride + xx + 3] / 255.0, 1.0));
        }
    }

    return result;
}

std::vector<float> image::resize(const std::vector<float>& image, int inWidth, int inHeight, int outWidth, int outHeight) noexcept {
    auto result = std::vector<float>{};
    result.resize(outWidth * outHeight * rgba_channels);

    stbir_resize_float(
        image.data(), inWidth, inHeight, static_cast<int>(inWidth * rgba_channels * sizeof(float)),
        result.data(), outWidth, outHeight, static_cast<int>(outWidth * rgba_channels * sizeof(float)),
        rgba_channels
    );

    return result;
}

std::vector<float> image::resize_and_resolve(const std::vector<float>& image, int inWidth, int inHeight, int outWidth, int outHeight) noexcept {
    const auto aliasedWidth = outWidth * 3;

    auto resized = resize(image, inWidth, inHeight, aliasedWidth, outHeight);

    const auto read_pixel = [&resized, aliasedWidth](int x, int y) {
        const auto stride = aliasedWidth * rgba_channels;
        x = std::min(x, aliasedWidth - 1); // Clamp X

        return std::array<float, rgba_channels>{
            resized[y * stride + (x * rgba_channels) + 0],
            resized[y * stride + (x * rgba_channels) + 1],
            resized[y * stride + (x * rgba_channels) + 2],
            resized[y * stride + (x * rgba_channels) + 3]
        };
    };

    auto result = std::vector<float>{};
    result.reserve(outWidth * outHeight * rgba_channels);

    for (int yy = 0; yy < outHeight; ++yy) {
        for (int xx = 0; xx < outWidth; ++xx) {
            const auto left = read_pixel(xx * 3 + 0, yy);
            const auto center = read_pixel(xx * 3 + 1, yy);
            const auto right = read_pixel(xx * 3 + 2, yy);

            result.emplace_back((right[0] + center[0]) / 2.0f);
            result.emplace_back((left[1] + center[1] + right[1]) / 3.0f);
            result.emplace_back((center[2] + left[2]) / 2.0f);
            result.emplace_back((left[3] + center[3] + right[3]) / 3.0f);
        }
    }

    return result;
}

std::vector<stbi_uc> image::to_data(const std::vector<float>& image, int width, int height, float pow, const std::vector<color_format::component_type>& format) noexcept {
    const auto channels = color_format::to_rgba_channels(format);

    const auto bytesPerPixel = bitlen_to_byte_size(std::accumulate(std::cbegin(channels), std::cend(channels), std::size_t{0}, [](auto acc, const auto& c) {
        acc += c.size();
        return acc;
    }));

    static constexpr auto shift_bits = [](auto& pixel, auto bits, const auto& channel) {
        if (channel.low) {
            pixel |= std::size_t(bits & channel.low.mask()) << channel.low.shift;
        }
        if (channel.high) {
            pixel |= std::size_t((bits >> channel.low.size) & channel.high.mask()) << channel.high.shift;
        }
    };

    auto result = std::vector<stbi_uc>{};
    result.resize(width * height * bytesPerPixel);

    for (int yy = 0; yy < height; ++yy) {
        for (int xx = 0; xx < width; ++xx) {
            auto pixel = std::size_t{};

            for (int ii = 0; ii < 3; ++ii) {
                const auto bits = channels[ii].pow(image[yy * (width * rgba_channels) + (xx * rgba_channels) + ii], pow);
                shift_bits(pixel, bits, channels[ii]);
            }

            const auto alpha = channels[3].convert(image[yy * (width * rgba_channels) + (xx * rgba_channels) + 3]);
            shift_bits(pixel, alpha, channels[3]);

            auto* dest = result.data() + (yy * (width * bytesPerPixel) + (xx * bytesPerPixel));
            std::memcpy(dest, &pixel, bytesPerPixel);
        }
    }

    return result;
}

std::vector<float> image::flatten(const std::vector<std::array<float, 4>>& image) noexcept {
    auto result = std::vector<float>{};
    result.reserve(image.size() * 4);

    [[maybe_unused]]
    const auto end = std::accumulate(std::cbegin(image), std::cend(image), std::back_inserter(result), [](auto out, auto c){
        return std::copy_n(std::cbegin(c), 4, out);
    });

    return result;
}

using color_type = std::array<float, 4>;

std::vector<std::size_t> image::palettize(const std::vector<float>& image, const std::vector<color_type>& palette) noexcept {
    const auto nearest_point_index = [&](const auto& p) {
        const auto compare_distance = [&](const auto& p1, const auto& p2) {
            const auto distance = [](const auto& p1, const auto& p2) {
                return std::sqrt(std::pow(p1[0] - p2[0], 2) +
                                 std::pow(p1[1] - p2[1], 2) +
                                 std::pow(p1[2] - p2[2], 2)); // Don't compare alpha
            };
            return distance(p1, p) < distance(p2, p);
        };
        auto min_point_it = std::ranges::min_element(palette, compare_distance);
        return std::distance(std::begin(palette), min_point_it);
    };

    auto result = std::vector<std::size_t>{};
    result.reserve(image.size() / 4);

    for (int ii = 0; ii < image.size(); ii += 4) {
        result.emplace_back(nearest_point_index(color_type{
                image[ii + 0],
                image[ii + 1],
                image[ii + 2],
                image[ii + 3]
        }));
    }

    return result;
}

std::vector<float> image::expand(const std::vector<std::size_t>& indices, const std::vector<std::array<float, 4>>& palette) noexcept {
    auto result = std::vector<float>{};
    result.reserve(indices.size() * 4);

    for (auto idx : indices) {
        if (idx >= palette.size()) {
            continue;
        }

        const auto color = palette[idx];
        result.emplace_back(color[0]);
        result.emplace_back(color[1]);
        result.emplace_back(color[2]);
        result.emplace_back(color[3]);
    }

    return result;
}

std::vector<color_type> image::gamma_pow(const std::vector<std::array<float, 4>>& image, float gamma) noexcept {
    auto result = std::vector<std::array<float, 4>>{};
    result.reserve(image.size());

    for (auto& v : image) {
        result.push_back(std::array<float, 4>{
            util::pow_clamp(v[0], gamma),
            util::pow_clamp(v[1], gamma),
            util::pow_clamp(v[2], gamma),
            v[3]
        });
    }

    return result;
}

static auto directional_for(int low, int high, int dir, auto lambda) noexcept {
    if (dir < 0) {
        for (int ii = high - 1; ii >= low; --ii) {
            lambda(ii);
        }
    } else {
        for (int ii = low; ii < high; ++ii) {
            lambda(ii);
        }
    }
}

std::pair<image::direction, image::direction> image::direction_pair(const std::string& str) noexcept {
    if (str.length() != 4) {
        return {direction::plus_x, direction::plus_x};
    }

    auto v = std::vector<direction>{};
    v.reserve(2);

    for (int idx = 0; idx < 4; idx += 2) {
        if (str[idx + 0] == '+') {
            if (str[idx + 1] == 'x') {
                v.emplace_back(direction::plus_x);
            } else if (str[idx + 1] == 'y') {
                v.emplace_back(direction::plus_y);
            }
        } else if (str[idx + 0] == '-') {
            if (str[idx + 1] == 'x') {
                v.emplace_back(direction::minus_x);
            } else if (str[idx + 1] == 'y') {
                v.emplace_back(direction::minus_y);
            }
        }
    }

    if (v.size() != 2) {
        return {direction::plus_x, direction::plus_x};
    }

    return {v[0], v[1]};
}

std::vector<float> image::orientate(const std::vector<float>& image, int width, int height, image::direction major, image::direction minor) noexcept {
    auto result = std::vector<float>{};
    result.reserve(image.size());

    const int majorStride = -1 + (2 * (major == direction::plus_x || major == direction::plus_y));
    const int minorStride = -1 + (2 * (minor == direction::plus_x || minor == direction::plus_y));

    if (major == direction::plus_x || major == direction::minus_x) {
        directional_for(0, height, minorStride, [&](auto yy) {
            directional_for(0, width, majorStride, [&](auto xx) {
                result.emplace_back(image[yy * width + xx]);
            });
        });
    } else {
        directional_for(0, width, majorStride, [&](auto xx) {
            directional_for(0, height, minorStride, [&](auto yy) {
                result.emplace_back(image[yy * width + xx]);
            });
        });
    }

    return result;
}

std::vector<std::size_t> image::orientate(const std::vector<std::size_t>& image, int width, int height, image::direction major, image::direction minor) noexcept {
    auto result = std::vector<std::size_t>{};
    result.reserve(image.size());

    const int majorStride = -1 + (2 * (major == direction::plus_x || major == direction::plus_y));
    const int minorStride = -1 + (2 * (minor == direction::plus_x || minor == direction::plus_y));

    if (major == direction::plus_x || major == direction::minus_x) {
        directional_for(0, height, minorStride, [&](auto yy) {
            directional_for(0, width, majorStride, [&](auto xx) {
                result.emplace_back(image[yy * width + xx]);
            });
        });
    } else {
        directional_for(0, width, majorStride, [&](auto xx) {
            directional_for(0, height, minorStride, [&](auto yy) {
                result.emplace_back(image[yy * width + xx]);
            });
        });
    }

    return result;
}
