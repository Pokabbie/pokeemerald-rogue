#pragma once

#include <ctopt.hpp>
#include <fmt/format.h>

namespace options {

    static constexpr auto option_help = ctopt::option('h', "help").help_text("Print help").flag_counter();
    static constexpr auto option_dump_version = ctopt::option("dump-version").help_text("Print version").flag_counter();
    static constexpr auto option_dump_formats = ctopt::option("help-formats").help_text("Print image format help").flag_counter();
    static constexpr auto option_verbose = ctopt::option('v', "verbose").help_text("Verbose logging").flag_counter();

    static constexpr auto get_opts = make_options_no_error(
        option_help,
        option_dump_version,
        option_dump_formats,
        option_verbose
    );

    static constexpr auto get_opts_bitmap = make_options(
        ctopt::option('i', "in-image").meta("filepath").help_text("Input: image").required(),
        ctopt::option('o', "out-data").meta("filepath").help_text("Output: Binary data"),
        ctopt::option('p', "out-palette-data").meta("filepath").help_text("Output: Binary palette data"),
        ctopt::option('m', "mode").meta("integer").help_text("GBA bitmap background mode (3, 4, 5)").default_value("3"),
        ctopt::option('w', "width").meta("integer").help_text("Bitmap width"),
        ctopt::option('h', "height").meta("integer").help_text("Bitmap height"),
        ctopt::option('f', "format").meta("string").help_text("Output color format. Use --help-formats to view color format info.").default_value("g1BGR5"),
        ctopt::option('g', "gamma").meta("string").help_text("Gamma ratio input:output. eg: 2.2:4.0").default_value("2.2:2.2").min(1).max(2).separator(':'),
        ctopt::option('b', "bpp").meta("integer").help_text("Palette index bits per pixel").default_value("8"),
        ctopt::option('c', "colors").meta("integer").help_text("Maximum colors in the palette"),
        ctopt::option('d', "direction").meta("string").help_text("Output stride direction. +x+y describes upper-left row-major. +y-x describes upper-right column-major.").default_value("+x+y"),
        ctopt::option("in-palette").meta("filepath").help_text("Input: palette (image, binary, .gpl)"),
        ctopt::option("out-png").meta("filepath").help_text("Output: PNG image"),
        ctopt::option("out-palette-png").meta("filepath").help_text("Output: Palette as PNG image"),
        ctopt::option("out-palette-gpl").meta("filepath").help_text("Output: Palette as GPL file"),
        ctopt::option("anti-alias").help_text("Apply sub-pixel anti-aliasing").flag_counter()
    );

    static inline const auto help_str = fmt::format(R"({}
Commands:
  bitmap  Convert an image file to a bitmap

bitmap {})",
        get_opts.help_str(), get_opts_bitmap.help_str()
    );
}
