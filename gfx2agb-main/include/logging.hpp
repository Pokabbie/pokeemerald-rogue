#pragma once

#include <utility>

#include <fmt/format.h>

namespace vlog {

    inline bool verbose = false;

    void print(std::string_view fmt, auto argsSupplier) noexcept {
        if (verbose) {
            fmt::vprint("- " + std::string(fmt) + "\n", argsSupplier());
        }
    }
}
