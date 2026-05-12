#ifndef ROGUE_COLOUR_UTILS__H
#define ROGUE_COLOUR_UTILS__H

#define PALETTE_MODIFY_LAYER_COUNT 3

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
void Rogue_ModifyPaletteByLayers(u16 const* basePal, u16 const* layerPal, u16* writeBuffer, u16 const* layerMasks, u16 const* chosenColours);

#endif