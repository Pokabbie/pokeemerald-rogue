# gfx2agb

Graphics converter for Game Boy Advance

## Dependencies

C++20

## Building

### CMake 3.18+

To build in directory `build`:

```shell
cmake -S . -B build
cmake --build build
```

Install from the built `build/` directory to the `bin/` directory with `cmake --install build`.

## Usage

```shell
gfx2agb [<options>] <command> [<command options>]
```

Where `<command>` is `bitmap`.

```
Options:
  -h --help       Print help
  --dump-version  Print version
  --help-formats  Print image format help
  -v --verbose    Verbose logging

Commands:
  bitmap  Convert an image file to a bitmap

bitmap Options:
  -i --in-image=filepath          Input: image
  -o --out-data=filepath          Output: Binary data
  -p --out-palette-data=filepath  Output: Binary palette data
  -m --mode=integer               GBA bitmap background mode (3, 4, 5) [default: 3]
  -w --width=integer              Bitmap width
  -h --height=integer             Bitmap height
  -f --format=string              Output color format. Use --help-formats to view color format info. [default: g1BGR5]
  -g --gamma=string               Gamma ratio input:output. eg: 2.2:4.0 [default: 2.2:2.2]
  -b --bpp=integer                Palette index bits per pixel [default: 8]
  -c --colors=integer             Maximum colors in the palette
  -d --direction=string           Output stride direction. +x+y describes upper-left row-major. +y-x describes upper-right column-major. [default: +x+y]
  --in-palette=filepath           Input: palette (image, binary, .gpl)
  --out-png=filepath              Output: PNG image
  --out-palette-png=filepath      Output: Palette as PNG image
  --out-palette-gpl=filepath      Output: Palette as GPL file
  --anti-alias                    Apply sub-pixel anti-aliasing
```

## Examples

### Resize & convert to Mode 3 bitmap

Scales `my picture.jpg` to 240x160, and output a binary suitable for displaying with Mode 3 graphics.

```shell
gfx2agb bitmap -m3 -i "my picture.jpg" -o picture.bin
```

If we wanted a very smooth result we can use the `--anti-alias` switch to apply sub-pixel anti-aliasing.

### Resize & convert to Mode 4 bitmap with palette

Scales `my picture.jpg` to 240x160, and output an up-to 256 color palette binary with a corresponding image binary suitable for displaying with Mode 4 graphics.

```shell
gfx2agb bitmap -m4 -i "my picture.jpg" -p picture.pal -o picture.bin
```

### Apply AGB001 gamma to an image

Converts `my picture.jpg` to `picture.png`, maintains the input width & height, and increases the gamma to 4.0 (roughly matching the AGB001 display).

```shell
gfx2agb bitmap -m3 -i "my picture.jpg" --out-png picture.png --width=iw --height=ih --gamma=4.0
```

This type of image to PNG conversion isn't a recommended use case for gfx2agb, but it is still supported.

### Convert to a 64x64 texture

Produces a 64x64 texture that has been oriented to be ideal for texture-mapping with a ray-caster engine.

```shell
gfx2agb bitmap -m4 -i "my texture.png" -o texture.bin --width=64 --height=64 --in-palette="my palette.gpl" --direction=+y+x
```

This resizes `my palette.gpl` to 64x64, applies the palette `my palette.gpl`, and outputs the binary `texture.bin` column-first (`+y+x`).
