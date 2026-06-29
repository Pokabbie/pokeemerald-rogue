#pragma once

#include <algorithm>
#include <bit>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace util {

std::pair<int, int> parse_width_height(int inWidth, int inHeight, const std::string& widthExpr, const std::string& heightExpr) noexcept;
std::vector<char> repack_data(const std::vector<std::size_t>& data, std::size_t bpp) noexcept;

[[nodiscard]]
auto pow_clamp(auto x, auto pow) noexcept -> float {
    using x_type = decltype(x);
    return static_cast<float>(std::clamp(std::pow(x, x_type(pow)), x_type{}, x_type{1}));
}

[[nodiscard]]
constexpr bool is_pow2_or_mul8(std::unsigned_integral auto x) noexcept {
    if ((x % 8) == 0) {
        return true;
    }
    return std::has_single_bit(x);
}

} // namespace util
