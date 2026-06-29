#pragma once

#include <algorithm>
#include <cmath>
#include <string_view>
#include <vector>

namespace color_format {

struct component_type {
    [[nodiscard]]
    constexpr auto mask() const noexcept {
        return (1u << size) - 1u;
    }

    [[nodiscard]]
    constexpr explicit operator bool() const noexcept {
        return bool(size);
    }

    char channel;
    std::size_t size;
    std::size_t shift;
};

std::vector<component_type> parse(std::string_view sv) noexcept;

struct color_channel_type {
    [[nodiscard]]
    constexpr auto size() const noexcept {
        return low.size + high.size;
    }

    [[nodiscard]]
    constexpr auto mask() const noexcept {
        return (1 << size()) - 1;
    }

    [[nodiscard]]
    auto convert(const float x) const noexcept {
        const auto m = mask();
        return std::clamp(
            int(std::round(x * float(m))),
            0,
            m
        );
    }

    [[nodiscard]]
    auto pow(const float x, const float y) const noexcept {
        return convert(std::pow(x, y));
    }

    [[nodiscard]]
    auto to_int(const std::size_t bits) const noexcept {
        const auto l = (bits >> low.shift) & low.mask();
        const auto h = (bits >> high.shift) & high.mask();
        return l | (h << low.size);
    }

    color_format::component_type low{};
    color_format::component_type high{};
};

std::array<color_format::color_channel_type, 4> to_rgba_channels(const std::vector<color_format::component_type>& format) noexcept;

} // namespace color_format
