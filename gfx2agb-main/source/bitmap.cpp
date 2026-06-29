#include "bitmap.hpp"

#include <fstream>

#include <ctopt.hpp>
#include <fmt/format.h>
#include <stb_image_write.h>

#include "color_format.hpp"
#include "image_io.hpp"
#include "logging.hpp"
#include "options.hpp"
#include "palette.hpp"
#include "util.hpp"

namespace {

    template <std::floating_point T>
    constexpr auto pc_display_sRGB = static_cast<T>(2.2);

    constexpr auto png_components = 4;

    const auto png_pixel_format = color_format::parse("ABGR8");

}

int bitmap(ctopt::args::const_iterator begin, ctopt::args::const_iterator end) {
    using namespace options;

    const auto args = get_opts_bitmap(std::move(begin), std::move(end));
    if (!args) {
        fmt::print(stderr, "{}\n", args.error_str());
        fmt::print("{}", get_opts_bitmap.help_str());
        return 1;
    }

    const auto mode = args.get<int>("mode");
    if (mode < 3 || mode > 5) {
        fmt::print(stderr, "{} is not a bitmap mode (expected 3, 4, 5)", mode);
        return 1;
    }

    const auto colorFormat = color_format::parse(args.get<std::string>("format"));
    if (colorFormat.empty()) {
        fmt::print(stderr, "Could not parse color format {}", args.get<std::string>("format"));
        return 1;
    }

    const auto [major, minor] = image::direction_pair(args.get<std::string>("direction"));
    if (image::same_axis(major, minor)) {
        fmt::print(stderr, "Invalid direction {}", args.get<const char*>("direction"));
        return 1;
    }

    int inWidth, inHeight, components;
    vlog::print("Reading image {}", [&](){return fmt::make_format_args(args.get<const char*>("in-image"));});
    const auto image = image::load(args.get<const char*>("in-image"), inWidth, inHeight, components);
    if (!image) {
        fmt::print(stderr, "Could not read image {}", args.get<std::string>("in-image"));
        return 1;
    }

    // Mutable as --direction may change these
    auto [outWidth, outHeight] = util::parse_width_height(inWidth, inHeight,
        args.get<std::optional<std::string>>("width").value_or(mode == 5 ? "160" : "240"),
        args.get<std::optional<std::string>>("height").value_or(mode == 5 ? "120" : "160")
    );

    const auto [inGamma, outGamma] = [&]() {
        const auto gamma = args.get<std::pair<float, float>>("gamma");
        if (std::get<1>(gamma) == 0.0f) {
            return std::make_pair(pc_display_sRGB<float>, std::get<0>(gamma));
        }
        return gamma;
    }();

    vlog::print("Converting to linear with gamma {}", [&](){return fmt::make_format_args(inGamma);});
    auto imageLinear = image::to_float(image, inWidth, inHeight, inGamma);

    if (args.get<bool>("anti-alias")) { // Apply sub-pixel anti-aliasing
        vlog::print("Resizing to {}x{} with sub-pixel anti-aliasing", [&](){return fmt::make_format_args(outWidth, outHeight);});
        imageLinear = image::resize_and_resolve(imageLinear, inWidth, inHeight, outWidth, outHeight);
    } else if (inWidth != outWidth || inHeight != outHeight) {
        vlog::print("Resizing to {}x{}", [&](){return fmt::make_format_args(outWidth, outHeight);});
        imageLinear = image::resize(imageLinear, inWidth, inHeight, outWidth, outHeight);
    }

    const auto* outputPng = args.get<const char*>("out-png");
    const auto* outputData = args.get<const char*>("out-data");
    const auto* inPalette = args.get<const char*>("in-palette");

    const auto load_palette = [&]() {
        int inPalWidth, inPalHeight, palComponents;
        const auto pal = image::load(inPalette, inPalWidth, inPalHeight, palComponents);

        if (pal) {
            vlog::print("Extracting palette from {} with gamma {}", [&](){return fmt::make_format_args(inPalette, inGamma);});
            return palette::extract(
                colorFormat,
                image::to_float(std::move(pal), inPalWidth, inPalHeight, inGamma),
                inPalWidth,
                inPalHeight
            );
        }

        std::string name;
        int columns;
        const auto palette = palette::gpl_load(inPalette, name, columns);
        if (palette.empty()) { // Retry as binary
            vlog::print("Loading {} as binary palette in format {}", [&](){return fmt::make_format_args(inPalette, args.get<std::string>("format"));});
            return palette::binary_load(inPalette, colorFormat);
        }

        vlog::print("Loaded GPL palette from {} (Name: {} Columns: {})", [&](){return fmt::make_format_args(inPalette, name, columns);});
        return image::gamma_pow(palette, inGamma);
    };

    if (mode == 4) {
        const auto* outputPaletteGpl = args.get<const char*>("out-palette-gpl");
        const auto* outputPalettePng = args.get<const char*>("out-palette-png");
        const auto* outputPaletteData = args.get<const char*>("out-palette-data");

        if (!outputPng && !outputData && !outputPaletteGpl && !outputPalettePng && !outputPaletteData) {
            fmt::print(stderr, "No outputs");
            fmt::print("{}", get_opts_bitmap.help_str());
            return 1;
        }

        const auto bpp = args.get<std::size_t>("bpp");
        if (!util::is_pow2_or_mul8(bpp)) {
            fmt::print(stderr, "bpp ({}) must be a power of 2 or a multiple of 8", bpp);
            return 1;
        }

        const auto palette = [&]() {
            if (inPalette) {
                return load_palette();
            }

            const auto colors = [&]() {
                const auto colors = args.get<int>("colors");
                if (colors) {
                    return colors;
                }
                return 1 << bpp;
            }();

            vlog::print("Reducing to {} colors ({} bits per pixel)", [&](){return fmt::make_format_args(colors, bpp);});
            return palette::quantize(
                palette::extract(colorFormat, imageLinear, outWidth, outHeight),
                colors
            );
        }();

        vlog::print("Applying palette ({} colors)", [&](){return fmt::make_format_args(palette.size());});
        auto palettedImage = image::palettize(imageLinear, palette);

        if (!image::is_normal(major, minor)) {
            vlog::print("Applying orientation {}", [&](){return fmt::make_format_args(args.get<std::string>("direction"));});
            palettedImage = image::orientate(palettedImage, outWidth, outHeight, major, minor);
            if (image::is_x_axis(minor)) {
                std::swap(outWidth, outHeight);
            }
        }

        if (outputPaletteGpl) {
            vlog::print("Writing {}", [&](){return fmt::make_format_args(outputPaletteGpl);});
            const auto data = palette::to_gpl(palette, 1.0f / outGamma);

            auto ofs = std::ofstream(outputPaletteGpl, std::ios::binary);
            if (!ofs.is_open()) {
                fmt::print(stderr, "Could not write file {}", outputPaletteGpl);
                return 1;
            }

            ofs << data;
            ofs.close();
        }

        if (outputPng) {
            vlog::print("Writing {}", [&](){return fmt::make_format_args(outputPng);});
            const auto imagePng = image::to_data(image::expand(palettedImage, palette), outWidth, outHeight, 1.0f / outGamma, png_pixel_format);

            const bool written = stbi_write_png(outputPng, outWidth, outHeight, png_components, imagePng.data(), outWidth * png_components);
            if (!written) {
                fmt::print(stderr, "Could not write file {}", outputPng);
                return 1;
            }
        }

        if (outputPalettePng) {
            vlog::print("Writing {}", [&](){return fmt::make_format_args(outputPalettePng);});
            const auto palWidth = static_cast<int>(std::sqrt(palette.size()));
            const auto palHeight = static_cast<int>((palette.size() + (palWidth - 1)) / palWidth);

            auto flat = image::flatten(palette);
            flat.resize(palWidth * palHeight * 4);
            const auto imagePng = image::to_data(flat, palWidth, palHeight, 1.0f / outGamma, png_pixel_format);

            const bool written = stbi_write_png(outputPalettePng, palWidth, palHeight, png_components, imagePng.data(), palWidth * png_components);
            if (!written) {
                fmt::print(stderr, "Could not write file {}", outputPalettePng);
                return 1;
            }
        }

        if (outputData) {
            vlog::print("Writing {}", [&](){return fmt::make_format_args(outputData);});
            const auto data = util::repack_data(palettedImage, bpp);

            auto ofs = std::ofstream(outputData, std::ios::binary);
            if (!ofs.is_open()) {
                fmt::print(stderr, "Could not write file {}", outputData);
                return 1;
            }

            ofs.write(
                data.data(),
                static_cast<std::streamsize>(data.size())
            );
            ofs.close();
        }

        if (outputPaletteData) {
            vlog::print("Writing {}", [&](){return fmt::make_format_args(outputPaletteData);});
            const auto data = image::to_data(image::flatten(palette), static_cast<int>(palette.size()), 1, 1.0f / outGamma, colorFormat);

            auto ofs = std::ofstream(outputPaletteData, std::ios::binary);
            if (!ofs.is_open()) {
                fmt::print(stderr, "Could not write file {}", outputPaletteData);
                return 1;
            }

            ofs.write(
                reinterpret_cast<const char*>(data.data()),
                static_cast<std::streamsize>(data.size())
            );
            ofs.close();
        }

        return 0;
    }

    // Mode 3/5 bitmap
    if (!outputPng && !outputData) {
        fmt::print(stderr, "No outputs");
        fmt::print("{}", get_opts_bitmap.help_str());
        return 1;
    }

    const auto colors = args.get<int>("colors");

    if (inPalette) { // Apply palette
        auto palette = load_palette();
        if (colors) { // And reduce colors
            vlog::print("Reducing to {} colors", [&](){return fmt::make_format_args(colors);});
            palette = palette::quantize(palette, colors);
        }

        vlog::print("Applying palette ({} colors)", [&](){return fmt::make_format_args(palette.size());});
        imageLinear = image::expand(
            image::palettize(imageLinear, palette),
            palette
        );
    } else if (colors) { // Reduce colors
        vlog::print("Reducing to {} colors", [&](){return fmt::make_format_args(colors);});
        const auto palette = palette::quantize(
            palette::extract(colorFormat, imageLinear, outWidth, outHeight),
            colors
        );

        vlog::print("Applying palette ({} colors)", [&](){return fmt::make_format_args(palette.size());});
        imageLinear = image::expand(
            image::palettize(imageLinear, palette),
            palette
        );
    }

    if (!image::is_normal(major, minor)) {
        vlog::print("Applying orientation {}", [&](){return fmt::make_format_args(args.get<std::string>("direction"));});
        imageLinear = image::orientate(imageLinear, outWidth, outHeight, major, minor);
        if (image::is_x_axis(minor)) {
            std::swap(outWidth, outHeight);
        }
    }

    if (outputPng) {
        vlog::print("Writing {}", [&](){return fmt::make_format_args(outputPng);});
        const auto imagePng = image::to_data(imageLinear, outWidth, outHeight, 1.0f / outGamma, png_pixel_format);

        const bool written = stbi_write_png(outputPng, outWidth, outHeight, png_components, imagePng.data(), outWidth * png_components);
        if (!written) {
            fmt::print(stderr, "Could not write file {}", outputPng);
            return 1;
        }
    }

    if (outputData) {
        vlog::print("Writing {}", [&](){return fmt::make_format_args(outputData);});
        const auto data = image::to_data(imageLinear, outWidth, outHeight, 1.0f / outGamma, colorFormat);

        auto ofs = std::ofstream(outputData, std::ios::binary);
        if (!ofs.is_open()) {
            fmt::print(stderr, "Could not write file {}", outputData);
            return 1;
        }

        ofs.write(
            reinterpret_cast<const char*>(data.data()),
            static_cast<std::streamsize>(data.size())
        );
        ofs.close();
    }

    return 0;
}
