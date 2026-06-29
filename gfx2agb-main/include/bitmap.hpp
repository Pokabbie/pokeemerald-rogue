#pragma once

#include <string_view>

#include <ctopt.hpp>

int bitmap(ctopt::args::const_iterator begin, ctopt::args::const_iterator end);

namespace {

    inline constexpr std::string_view help_formats = R"(Channels:
  b Lower blue channel
  B Upper blue channel
  g Lower green channel
  G Upper green channel
  r Lower red channel
  R Upper red channel
  a Lower alpha channel
  A Upper alpha channel

Minimum bits-per-channel:
  1

Maximum bits-per-channel:
  16

Example formats:
  g1BGR5    [default]
  BGRA8
  BGR8
  BGR5
  A8R8G8B8
  R8
)";

}
