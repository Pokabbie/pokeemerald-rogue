#include "palette.hpp"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include "util.hpp"

static auto to_bits(const std::array<color_format::color_channel_type, 4>& channels, std::array<float, 4> x) noexcept -> std::size_t;

std::vector<std::array<float, 4>> palette::extract(const std::vector<color_format::component_type>& format, const std::vector<float>& image, int width, int height) noexcept {
    using color = std::array<float, 4>;

    const auto channels = color_format::to_rgba_channels(format);

    const auto color_compare = [&channels](color lhs, color rhs) {
        return to_bits(channels, lhs) < to_bits(channels, rhs);
    };

    auto result = std::set<color, decltype(color_compare)>{color_compare};

    const int stride = width * 4;
    for (int yy = 0; yy < height; ++yy) {
        for (int xx = 0; xx < stride; xx += 4) {
            const auto col = color{
                image[yy * stride + xx + 0],
                image[yy * stride + xx + 1],
                image[yy * stride + xx + 2],
                image[yy * stride + xx + 3]
            };
            result.emplace(col);
        }
    }

    return std::vector<std::array<float, 4>>{std::cbegin(result), std::cend(result)};
}

static auto to_bits(const std::array<color_format::color_channel_type, 4>& channels, std::array<float, 4> x) noexcept -> std::size_t {
    const auto red = std::clamp(static_cast<int>(std::round(x[0] * static_cast<float>(channels[0].mask()))), 0, channels[0].mask());
    const auto green = std::clamp(static_cast<int>(std::round(x[1] * static_cast<float>(channels[1].mask()))), 0, channels[1].mask());
    const auto blue = std::clamp(static_cast<int>(std::round(x[2] * static_cast<float>(channels[2].mask()))), 0, channels[2].mask());
    const auto alpha = std::clamp(static_cast<int>(std::round(x[3] * static_cast<float>(channels[3].mask()))), 0, channels[3].mask());

    return std::size_t(alpha) | (std::size_t(blue) << channels[3].size()) |
        (std::size_t(green) << (channels[2].size() + channels[3].size())) |
        (std::size_t(red) << (channels[1].size() + channels[2].size() + channels[3].size()));
}

using color_type = std::array<float, 4>;
using palette_type = std::vector<color_type>;

std::vector<std::array<float, 4>> palette::quantize(const std::vector<std::array<float, 4>>& palette, int colors) noexcept {
    static constexpr auto square_distance = [](color_type a, color_type b) {
        const auto c = std::array<float, 3>{
            a[0] - b[0],
            a[1] - b[1],
            a[2] - b[2]
        };
        return (c[0] * c[0]) + (c[1] * c[1]) + (c[2] * c[2]);
    };

    const auto maxColors = std::min(palette.size(), std::size_t(colors));

    auto clusterCenters = std::vector<color_type>(maxColors);
    auto rng = std::mt19937{};
    rng.seed(0xF3BCC909);
    auto dist = std::uniform_int_distribution<std::size_t>{0, palette.size() - 1};
    for (auto& center : clusterCenters) {
        center = palette[dist(rng)];
    }

    for (auto iter = std::size_t{}; iter < 100; ++iter) {
        // Assign each data point to the nearest cluster center
        auto clusters = std::vector<palette_type>(maxColors);
        for (const auto& color : palette) {
            auto minDistance = std::numeric_limits<double>::max();
            auto minIndex = std::size_t{};
            for (auto ii = std::size_t{}; ii < maxColors; ++ii) {
                const auto distance = square_distance(color, clusterCenters[ii]);
                if (distance < minDistance) {
                    minDistance = distance;
                    minIndex = ii;
                }
            }
            clusters[minIndex].push_back(color);
        }

        // Update the cluster centers to the mean of the assigned data points
        auto converged = true;
        for (auto ii = std::size_t{}; ii < maxColors; ++ii) {
            if (clusters[ii].empty()) {
                continue;
            }

            auto newCenter = color_type{};

            for (const auto& color : clusters[ii]) {
                newCenter[0] += color[0];
                newCenter[1] += color[1];
                newCenter[2] += color[2];
                newCenter[3] += color[3];
            }

            newCenter[0] /= float(clusters[ii].size());
            newCenter[1] /= float(clusters[ii].size());
            newCenter[2] /= float(clusters[ii].size());
            newCenter[3] /= float(clusters[ii].size());

            if (newCenter != clusterCenters[ii]) {
                converged = false;
                clusterCenters[ii] = newCenter;
            }
        }

        if (converged) {
            break;
        }
    }

    return clusterCenters;
}

static std::map<std::string, std::string> parse_gpl_meta(std::ifstream& file) noexcept;
static std::string trim(const std::string& str) noexcept;
static palette_type parse_gpl_entries(std::ifstream& file, std::vector<std::string>& names) noexcept;

palette_type palette::gpl_load(const char* path, std::string& name, int& columns) noexcept {
    name = "";
    columns = 0;

    auto file = std::ifstream(path);
    if (!file.is_open()) {
        return {};
    }

    auto line = std::string{};
    std::getline(file, line);
    if (line != "GIMP Palette") {
        return {};
    }

    // Parse meta data (Name, Columns)
    auto meta = parse_gpl_meta(file);
    name = meta["Name"];
    const auto& columnsString = meta["Columns"];
    std::from_chars(columnsString.data(), columnsString.data() + columnsString.size(), columns);

    // Parse color entries
    auto names = std::vector<std::string>{};
    auto result = parse_gpl_entries(file, names);

    file.close();
    return result;
}

static std::map<std::string, std::string> parse_gpl_meta(std::ifstream& file) noexcept {
    auto meta = std::map<std::string, std::string>{};

    auto line = std::string{};
    while (std::getline(file, line)) {
        auto colon_pos = line.find(':');

        if (colon_pos != std::string::npos) {
            const auto key = trim(line.substr(0, colon_pos));
            const auto value = trim(line.substr(colon_pos + 1));

            meta[key] = value;
        } else {
            break; // Reached end of meta-data
        }
    }

    return meta;
}

static std::string trim(const std::string& str) noexcept {
    auto first = str.find_first_not_of(" \t");
    auto last = str.find_last_not_of(" \t");

    if (first == std::string::npos) {
        return {};
    }

    return str.substr(first, last - first + 1);
}

static palette_type parse_gpl_entries(std::ifstream& file, std::vector<std::string>& names) noexcept {
    auto result = palette_type{};

    auto line = std::string{};
    while (std::getline(file, line)) {
        auto currentColor = std::vector<int>{};

        using d_type = decltype(currentColor)::difference_type;

        auto ss = std::stringstream(line);

        std::string elem;
        while (ss >> elem) {
            if (elem.starts_with('#')) {
                break;
            }

            int value;
            auto [p, ec] = std::from_chars(elem.data(), elem.data() + elem.size(), value);
            if (ec == std::errc{}) {
                elem = "";
                currentColor.push_back(value);
                if (currentColor.size() == 4) {
                    break; // Reached RGBA limit
                }
            } else {
                break;
            }
        }

        if (currentColor.empty()) {
            continue; // Read garbage line
        }

        if (ss.eof()) {
            names.emplace_back(elem);
        } else {
            auto nameStr = ss.str();
            names.push_back(nameStr.substr(nameStr.find(elem)));
        }

        auto value = std::array<int, 4>{};
        std::copy(std::cbegin(currentColor), std::cbegin(currentColor) + std::min(d_type(currentColor.size()), d_type{4}), std::begin(value));

        if (currentColor.size() < 4) {
            value[3] = 255;
        }

        result.emplace_back(color_type{
            std::clamp(float(value[0]) / 255.0f, 0.0f, 255.0f),
            std::clamp(float(value[1]) / 255.0f, 0.0f, 255.0f),
            std::clamp(float(value[2]) / 255.0f, 0.0f, 255.0f),
            std::clamp(float(value[3]) / 255.0f, 0.0f, 255.0f)
        });
    }

    return result;
}

static std::array<std::size_t, 4> from_bits(const std::array<color_format::color_channel_type, 4>& format, std::size_t bits) noexcept;

palette_type palette::binary_load(const char* path, const std::vector<color_format::component_type>& format) noexcept {
    auto file = std::ifstream(path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }

    const auto channels = color_format::to_rgba_channels(format);
    const auto bpp = std::accumulate(std::cbegin(channels), std::cend(channels), std::size_t{}, [](auto acc, const color_format::color_channel_type& c) {
        acc += c.size();
        return acc;
    });
    const auto bytePerPixel = std::make_signed_t<decltype(bpp)>((bpp + 7) / 8);

    auto result = std::set<std::array<std::size_t, 4>>{};

    auto value = std::array<char, sizeof(std::size_t)>{};
    while (file.read(value.data(), bytePerPixel)) {
        auto bits = std::size_t{};
        std::memcpy(&bits, value.data(), bytePerPixel);

        result.emplace(from_bits(channels, bits));
    }

    file.close();

    auto palette = palette_type{};
    palette.reserve(result.size());

    std::transform(std::cbegin(result), std::cend(result), std::back_inserter(palette), [&channels](const auto& c) {
        return color_type{
            float(c[0]) / float(channels[0].mask()),
            float(c[1]) / float(channels[1].mask()),
            float(c[2]) / float(channels[2].mask()),
            channels[3].size() ? float(c[3]) / float(channels[3].mask()) : 1.0f
        };
    });

    return palette;
}

static std::array<std::size_t, 4> from_bits(const std::array<color_format::color_channel_type, 4>& format, std::size_t bits) noexcept {
    return std::array<std::size_t, 4>{
        format[0].to_int(bits),
        format[1].to_int(bits),
        format[2].to_int(bits),
        format[3].to_int(bits)
    };
}

std::string palette::to_gpl(const palette_type& palette, float pow) noexcept {
    auto result = fmt::format(
        "GIMP Palette\r\nName: gfx2agb ({}.{}.{}) {} colors\r\nColumns: 16\r\n#\r\n",
        GFX2AGB_VERSION_MAJOR, GFX2AGB_VERSION_MINOR, GFX2AGB_VERSION_PATCH,
        palette.size()
    );

    for (const auto& color : palette) {
        const auto red = static_cast<int>(util::pow_clamp(color[0], pow) * 255);
        const auto green = static_cast<int>(util::pow_clamp(color[1], pow) * 255);
        const auto blue = static_cast<int>(util::pow_clamp(color[2], pow) * 255);

        result += fmt::format("{: >3} {: >3} {: >3}\r\n", red, green, blue);
    }

    return result;
}
