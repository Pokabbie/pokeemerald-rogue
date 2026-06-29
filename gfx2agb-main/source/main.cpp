#include <ctopt.hpp>
#include <fmt/format.h>

#include "bitmap.hpp"
#include "logging.hpp"
#include "options.hpp"

int main(int argc, char* argv[]) {
    using namespace options;

    const auto args = get_opts(argc, argv);
    if (!args) {
        fmt::print(stderr, "{}\n", args.error_str());
        fmt::print("{}", help_str);
        return 1;
    }

    if (args.get<bool>("help")) {
        fmt::print("{}", help_str);
        return 0;
    }

    if (args.get<bool>("dump-version")) {
        fmt::print("{}.{}.{}", GFX2AGB_VERSION_MAJOR, GFX2AGB_VERSION_MINOR, GFX2AGB_VERSION_PATCH);
        return 0;
    }

    if (args.get<bool>("help-formats")) {
        fmt::print("{}", help_formats);
        return 0;
    }

    if (args.get<bool>("verbose")) {
        vlog::verbose = true;
    }

    if (args.cbegin() != args.cend()) {
        // Check commands
        if (*args.cbegin() == "bitmap") {
            return bitmap(++args.cbegin(), args.cend());
        }
    }

    fmt::print(stderr, "No command given\n");
    fmt::print("{}", help_str);
    return 1;
}

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_resize.h>
#include <stb_image_write.h>
