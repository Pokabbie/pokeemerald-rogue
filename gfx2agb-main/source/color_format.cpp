#include "color_format.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <numeric>

#include <fmt/format.h>

using gathered_components_type = std::pair<std::vector<char>, std::size_t>;

static gathered_components_type gather_components(std::string_view::iterator& begin, std::string_view::const_iterator end) noexcept;
static void print_duplicate(const std::vector<color_format::component_type>& components, auto index) noexcept;

std::vector<color_format::component_type> color_format::parse(std::string_view sv) noexcept {
    std::vector<component_type> result;
    result.reserve(4);
    std::size_t totalSize{};

    auto it = sv.cbegin();
    while (it != sv.cend()) {
        const auto [components, size] = gather_components(it, sv.cend());

        for (auto c : components) {
            result.emplace_back(c, size, totalSize);
            totalSize += size;
        }
    }

    const auto dup = std::adjacent_find(result.cbegin(), result.cend(), [](const auto& lhs, const auto& rhs){
        return lhs.channel == rhs.channel;
    });
    if (dup != result.cend()) {
        print_duplicate(result, std::distance(result.cbegin(), dup));
        return {};
    }

    // Flip to big-endian
    for (auto& c : result) {
        c.shift = totalSize - c.size - c.shift;
    }
    return result;
}

static gathered_components_type gather_components(std::string_view::iterator& begin, std::string_view::const_iterator end) noexcept {
    static constexpr auto valid_components = std::string_view("abgrABGR");

    std::vector<char> components;
    components.reserve(4);
    std::size_t size{};

    while (begin != end) {
        auto [ptr, ec] = std::from_chars(std::to_address(begin), std::to_address(end), size);
        if (ec == std::errc()) {
            std::advance(begin, std::distance(std::to_address(begin), ptr));
            break;
        } else if (valid_components.find(*begin) != std::string::npos) {
            components.emplace_back(*begin);
        }
        ++begin;
    }
    return std::make_pair(components, size);
}

static void print_duplicate(const std::vector<color_format::component_type>& components, auto index) noexcept {
    const auto componentString = std::accumulate(components.cbegin(), components.cend(), std::string(), [](auto acc, const auto& c) {
        acc += c.channel;
        return acc;
    });

    fmt::print(stderr, "Encountered duplicate channel {}>{}<{}\n",
        componentString.substr(0, index),
        componentString.substr(index, 1),
        componentString.substr(index + 1)
    );
}

std::array<color_format::color_channel_type, 4> color_format::to_rgba_channels(const std::vector<color_format::component_type>& format) noexcept {
    auto channels = std::array<color_channel_type, 4>{};
    for (const auto& c : format) {
        if (c.channel == 'r') channels[0].low = c;
        if (c.channel == 'R') channels[0].high = c;
        if (c.channel == 'g') channels[1].low = c;
        if (c.channel == 'G') channels[1].high = c;
        if (c.channel == 'b') channels[2].low = c;
        if (c.channel == 'B') channels[2].high = c;
        if (c.channel == 'a') channels[3].low = c;
        if (c.channel == 'A') channels[3].high = c;
    }
    return channels;
}
