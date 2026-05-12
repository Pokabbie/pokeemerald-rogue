#ifndef ROGUE_COLOUR_UTILS__H
#define ROGUE_COLOUR_UTILS__H

#define PALETTE_MODIFY_LAYER_COUNT 3

#define RANGE_TRANSFROM(v, from, to) ((v * from) / to)
#define ACCURACY_TRANSFROM(v, range) ((v / range) / range)
#define RANGE_255_TO_31(v) RANGE_TRANSFROM(v, 31, 255)
#define RANGE_31_TO_255(v) RANGE_TRANSFROM(v, 255, 31)
#define RGB_RANGE_255_TO_31(r, g, b) RGB(RANGE_255_TO_31(r), RANGE_255_TO_31(g), RANGE_255_TO_31(b))

extern u16 const gDefaultPaletteLayerMasks[PALETTE_MODIFY_LAYER_COUNT];

// Each component in range 0-255
struct HSV
{
    u8 h;
    u8 s;
    u8 v;
};

u16 HSVToRGB(struct HSV hsv);
struct HSV RGBtoHSV(u16 rgb);

void Rogue_GenerateLayerPaletteByHue(u16 const* inputPal, u16* outputLayers);
void Rogue_ModifyPaletteByLayersMultiply(u16 const* basePal, u16 const* layerPal, u16* writeBuffer, u16 const* layerMasks, u16 const* chosenColours);
void Rogue_ModifyPaletteByLayersHueShift(u16 const* basePal, u16 const* layerPal, u16* writeBuffer, u16 const* layerMasks, u16 const* chosenColours);

#endif