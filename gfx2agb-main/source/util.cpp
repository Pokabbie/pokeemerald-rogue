#include "util.hpp"

#include <array>

#include <exprtk.hpp>

std::pair<int, int> util::parse_width_height(int inWidth, int inHeight, const std::string& widthExpr, const std::string& heightExpr) noexcept {
    auto symbol_table = exprtk::symbol_table<double>{};
    symbol_table.add_constant("iw", inWidth);
    symbol_table.add_constant("ih", inHeight);
    symbol_table.add_constants();

    auto expr = exprtk::expression<double>{};
    expr.register_symbol_table(symbol_table);

    auto parser = exprtk::parser<double>{};

    parser.compile(widthExpr, expr);
    const auto outWidth = static_cast<int>(std::round(expr.value()));

    parser.compile(heightExpr, expr);
    const auto outHeight = static_cast<int>(std::round(expr.value()));

    return {outWidth, outHeight};
}

std::vector<char> util::repack_data(const std::vector<std::size_t>& data, std::size_t bpp) noexcept {
    const auto mask = (bpp << 1) - 1;

    auto size = bpp * data.size();
    size = (size + 7) / 8;

    auto result = std::vector<char>{};
    result.reserve(size);

    auto buflen = std::make_signed_t<std::size_t>{};
    auto buffer = std::size_t{};

    const auto copy_to_result = [&]() {
        auto bytes = std::array<char, sizeof(buffer)>{};

        const auto copy = (buflen + 7) / 8;
        std::memcpy(&bytes, &buffer, copy);
        buffer >>= buflen;

        int ii = 0;
        while (buflen >= 8 && ii < copy) {
            result.emplace_back(bytes[ii++]);
            buflen -= 8;
        }
    };

    for (auto v : data) {
        const auto d = v & mask;
        buffer |= d << buflen;
        buflen += static_cast<decltype(buflen)>(bpp);

        if (buflen >= 8) {
            copy_to_result();
        }
    }
    if (buflen) {
        copy_to_result();
    }

    return result;
}
