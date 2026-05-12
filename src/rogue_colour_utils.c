#include "global.h"
#include "constants/rgb.h"

#include "rogue_colour_utils.h"

u16 const gDefaultPaletteLayerMasks[PALETTE_MODIFY_LAYER_COUNT] =
{
    RGB(31, 0, 0),
    RGB(0, 31, 0),
    RGB(0, 0, 31),
};

u16 HSVToRGB(struct HSV hsv)
{
    u8 r, g, b;
    u8 region, remainder, p, q, t;
    
    if (hsv.s == 0)
    {
        r = hsv.v;
        g = hsv.v;
        b = hsv.v;
        return RGB_RANGE_255_TO_31(r, g, b);
    }
    
    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6; 
    
    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;
    
    switch (region)
    {
        case 0:
            r = hsv.v; g = t; b = p;
            break;
        case 1:
            r = q; g = hsv.v; b = p;
            break;
        case 2:
            r = p; g = hsv.v; b = t;
            break;
        case 3:
            r = p; g = q; b = hsv.v;
            break;
        case 4:
            r = t; g = p; b = hsv.v;
            break;
        default:
            r = hsv.v; g = p; b = q;
            break;
    }
    
    return RGB_RANGE_255_TO_31(r, g, b);
}

struct HSV RGBtoHSV(u16 rgb)
{
    struct HSV hsv;
    u8 r, g, b, rgbMin, rgbMax;

    r = RANGE_31_TO_255(GET_R(rgb));
    g = RANGE_31_TO_255(GET_G(rgb));
    b = RANGE_31_TO_255(GET_B(rgb));

    rgbMin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbMax = r > g ? (r > b ? r : b) : (g > b ? g : b);
    
    hsv.v = rgbMax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * (s64)(rgbMax - rgbMin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbMax == r)
        hsv.h = 0 + 43 * (g - b) / (rgbMax - rgbMin);
    else if (rgbMax == g)
        hsv.h = 85 + 43 * (b - r) / (rgbMax - rgbMin);
    else
        hsv.h = 171 + 43 * (r - g) / (rgbMax - rgbMin);

    return hsv;
}

static bool8 IsColourExemptFromHueTint(struct HSV hsv )
{
    return hsv.s <= 0 || hsv.v <= 20;
}

static u8 GetHueDistance(u8 a, u8 b)
{
    return a < b ? b - a : a - b;
}

static u8 GetAverageTintHue(u16 const* inputPal)
{
    u8 i;
    u32 total = 0;
    u32 count = 0;

    for(i = 0; i < 16; ++i)
    {
        u16 rgb = inputPal[i];
        struct HSV hsv = RGBtoHSV(rgb);

        if(!IsColourExemptFromHueTint(hsv))
        {
            total += hsv.h;
            ++count;
        }
    }

    return SAFE_DIV(total, count);
}

static u8 GetFurthestTintHue(u16 const* inputPal, u8 targetHue)
{
    u8 i;
    u8 hue = 0;
    u8 maxDist = 0;

    for(i = 0; i < 16; ++i)
    {
        u16 rgb = inputPal[i];
        struct HSV hsv = RGBtoHSV(rgb);

        if(!IsColourExemptFromHueTint(hsv))
        {
            u8 currDist = GetHueDistance(targetHue, hsv.h);
            if(currDist > maxDist)
            {
                hue = hsv.h;
                maxDist = currDist;
            }
        }
    }

    return hue;
}

static u8 GetNearestTintHue(u16 const* inputPal, u8 targetHue)
{
    u8 i;
    u8 hue = 0;
    u8 maxDist = 0;

    for(i = 0; i < 16; ++i)
    {
        u16 rgb = inputPal[i];
        struct HSV hsv = RGBtoHSV(rgb);

        if(!IsColourExemptFromHueTint(hsv))
        {
            u8 currDist = GetHueDistance(targetHue, hsv.h);
            if(currDist > maxDist)
            {
                hue = hsv.h;
                maxDist = currDist;
            }
        }
    }

    return hue;
}

static u8 ModifyHue(s32 currHue, s32 averageHue, s32 targetHue)
{
    s32 delta = averageHue - currHue;
    s32 outHue = targetHue + delta;

    if(outHue < 0)
    {
        outHue += 255;
    }

    return outHue % 255;
}

static bool8 InsertionSortPlaceBefore(u8 elemA, u8 elemB, u8 targetHue)
{
    u8 distA = GetHueDistance(elemA, targetHue);
    u8 distB = GetHueDistance(elemB, targetHue);
    return distA < distB;
}

static void InsertionSort(u8 elem, u8* buffer, u8 currBufferCount, u8 targetHue)
{
    if(currBufferCount == 0)
    {
        // Insert remaining item at the end
        buffer[currBufferCount] = elem;
    }
    else if(currBufferCount == 1)
    {
        if(InsertionSortPlaceBefore(elem, buffer[0], targetHue))
        {
            buffer[currBufferCount] = buffer[0];
            buffer[0] = elem;
        }
        else
        {
            buffer[currBufferCount] = elem;
        }
    }
    else
    {
        u16 index = 0;
        u16 minIndex = 0;
        u16 maxIndex = currBufferCount - 1;

        // Insert sort, find the index to insert at
        while(minIndex != maxIndex)
        {
            AGB_ASSERT(minIndex < maxIndex);

            index = (maxIndex + minIndex) / 2;

            if(InsertionSortPlaceBefore(elem, buffer[index], targetHue))
            {
                if(maxIndex == index)
                    --maxIndex;
                else
                    maxIndex = index;
            }
            else
            {
                if(minIndex == index)
                    ++minIndex;
                else
                    minIndex = index;
            }
        }

        AGB_ASSERT(minIndex == maxIndex);

        // Special case to sort the end of the list
        if(minIndex == currBufferCount - 1)
        {
            if(InsertionSortPlaceBefore(elem, buffer[currBufferCount - 1], targetHue))
            {
                buffer[currBufferCount] = buffer[currBufferCount - 1];
                buffer[currBufferCount - 1] = elem;
            }
            else
            {
                buffer[currBufferCount] = elem;
            }
        }
        else
        {

            // Shift everything up
            for(index = currBufferCount; TRUE; --index)
            {
                buffer[index] = buffer[index - 1];

                if(index == minIndex + 1)
                    break;
            }

            buffer[minIndex] = elem;
        }
    }
}

static void CalculateNearFarHues(u16 const* inputPal, u8 avgHue, u8* nearHue, u8* farHue)
{
    u8 i;
    u8 sortedHues[16];
    u8 count = 0;

    for(i = 0; i < 16; ++i)
    {
        u16 rgb = inputPal[i];
        struct HSV hsv = RGBtoHSV(rgb);

        if(!IsColourExemptFromHueTint(hsv))
        {
            InsertionSort(hsv.h, sortedHues, count++, avgHue);
        }
    }

    if(count < 4)
    {
        *nearHue = sortedHues[0];
        *farHue = sortedHues[count - 1];
    }
    else
    {
        *nearHue = sortedHues[0];
        *farHue = sortedHues[count - 1];
    }
}

void Rogue_GenerateLayerPaletteByHue(u16 const* inputPal, u16* outputLayers)
{
    u8 i;
    u8 nearestHue, furthestHue;
    u8 avgHue = GetAverageTintHue(inputPal);
    //u8 nearestHue = GetNearestTintHue(inputPal, avgHue);
    //u8 furthestHue = GetFurthestTintHue(inputPal, nearestHue);

    CalculateNearFarHues(inputPal, avgHue, &nearestHue, &furthestHue);

    for(i = 0; i < 16; ++i) // assume 16 palette slots (We currently don't have any use cases outside of this anyway)
    {
        u16 rgb = inputPal[i];
        struct HSV hsv = RGBtoHSV(rgb);

        if(IsColourExemptFromHueTint(hsv))
        {
            // Don't modify
            outputLayers[i] = RGB_BLACK;
        }
        else
        {
            u8 nearDist = GetHueDistance(hsv.h, nearestHue);
            u8 farDist = GetHueDistance(hsv.h, furthestHue);
            bool8 isPrimaryPal = nearDist < farDist;

            outputLayers[i] = isPrimaryPal ? RGB_RED : RGB_GREEN;
        }
    }

}

static u16 GreyScaleColour(u16 colour)
{
    u8 brightness = max(GET_R(colour), max(GET_G(colour), GET_B(colour)));
    return RGB(brightness, brightness, brightness);
}

static u16 CalculateWhitePointFor(u16 layerMask, const u16* basePal, const u16* layerPal)
{
    u16 layerWhitePoint = RGB(0, 0, 0);

    // Check if this layer is supported for this outfit
    if(layerMask != RGB(0, 0, 0))
    {
        u8 i;
        u16 baseCol;
        u16 layerCol;
        u16 currBrightness;
        u16 maxBrightness;

        // Calculate the average base colour in this layer
        maxBrightness = 0;

        for(i = 0; i < 16; ++i)
        {
            baseCol = basePal[i];
            layerCol = layerPal[i];

            if(layerCol == layerMask)
            {
                currBrightness = max(GET_R(baseCol), max(GET_G(baseCol), GET_B(baseCol)));

                if(maxBrightness == 0 || currBrightness > maxBrightness)
                {
                    maxBrightness = currBrightness;
                    layerWhitePoint = baseCol;
                }
            }
        }
    }

    return layerWhitePoint;
}

static bool8 ShouldModifyColourLayer(u16 chosenColour)
{
    // If alpha, just use input colour
    if((chosenColour & RGB_ALPHA) != 0)
        return FALSE;

    return TRUE;
}

#define COLOR_TRANSFORM_MULTIPLY_CHANNEL(value, whitePoint, target) min(31, ((((u32)value) * (u32)target) / (u32)whitePoint))

static u16 ModifyColourLayerMultiply(u16 chosenColour, u16 layerWhitePoint, u16 inputColour)
{
    u8 r, g, b;

    inputColour = GreyScaleColour(inputColour);
    r = GET_R(inputColour);
    g = GET_G(inputColour);
    b = GET_B(inputColour);

    r = COLOR_TRANSFORM_MULTIPLY_CHANNEL(r, GET_R(layerWhitePoint), GET_R(chosenColour));
    g = COLOR_TRANSFORM_MULTIPLY_CHANNEL(g, GET_G(layerWhitePoint), GET_G(chosenColour));
    b = COLOR_TRANSFORM_MULTIPLY_CHANNEL(b, GET_B(layerWhitePoint), GET_B(chosenColour));

    return RGB(r, g, b);
}

static u16 ModifyColourLayerHueShift(u16 chosenColour, u16 layerWhitePoint, u16 inputColour)
{
    struct HSV inputHSV = RGBtoHSV(inputColour);
    struct HSV chosenHSV = RGBtoHSV(chosenColour);
    struct HSV whitePointHSV = RGBtoHSV(layerWhitePoint);
    s32 s = inputHSV.s;
    s32 sDelta = (s32)chosenHSV.s - (s32)whitePointHSV.s;
    
    inputHSV.h -= whitePointHSV.h;
    inputHSV.h += chosenHSV.h;

    s += sDelta;
    inputHSV.s = min(255, max(0, s));

    return HSVToRGB(inputHSV);
}

#undef COLOR_TRANSFORM_MULTIPLY_CHANNEL

void Rogue_ModifyPaletteByLayersMultiply(u16 const* basePal, u16 const* layerPal, u16* writeBuffer, u16 const* layerMasks, u16 const* chosenColours)
{
    // Apply the dynamic changes using the layer pal
    u8 i, l;
    u16 baseCol, layerCol, layerMask;
    u16 layerWhitePoint[PALETTE_MODIFY_LAYER_COUNT];

    // Calculate the brightest colour for each layer to act as the white point
    // Do this in greyscale
    {
        for(l = 0; l < PALETTE_MODIFY_LAYER_COUNT; ++l)
        {
            layerWhitePoint[l] = GreyScaleColour(CalculateWhitePointFor(layerMasks[l], basePal, layerPal));
        }
    }

    // Calculate each colour in the palette
    for(i = 0; i < 16; ++i)
    {
        baseCol = basePal[i];
        layerCol = layerPal[i];

        for(l = 0; l < PALETTE_MODIFY_LAYER_COUNT; ++l)
        {
            layerMask = layerMasks[l];

            if(layerCol == layerMask && ShouldModifyColourLayer(chosenColours[l]) == TRUE)
            {
                // Expect the whitepoint to already be in greyscale
                baseCol = ModifyColourLayerMultiply(chosenColours[l], layerWhitePoint[l], baseCol);
                break;
            }
        }

        writeBuffer[i] = baseCol;
        //writeBuffer[i] = layerCol; // debug view
    }
}

void Rogue_ModifyPaletteByLayersHueShift(u16 const* basePal, u16 const* layerPal, u16* writeBuffer, u16 const* layerMasks, u16 const* chosenColours)
{
    // Apply the dynamic changes using the layer pal
    u8 i, l;
    u16 baseCol, layerCol, layerMask;
    u16 layerWhitePoint[PALETTE_MODIFY_LAYER_COUNT];

    // Calculate the brightest colour for each layer to act as the white point
    // Do this in greyscale
    {
        for(l = 0; l < PALETTE_MODIFY_LAYER_COUNT; ++l)
        {
            layerWhitePoint[l] = CalculateWhitePointFor(layerMasks[l], basePal, layerPal);
        }
    }

    // Calculate each colour in the palette
    for(i = 0; i < 16; ++i)
    {
        baseCol = basePal[i];
        layerCol = layerPal[i];

        for(l = 0; l < PALETTE_MODIFY_LAYER_COUNT; ++l)
        {
            layerMask = layerMasks[l];

            if(layerCol == layerMask && ShouldModifyColourLayer(chosenColours[l]) == TRUE)
            {
                // Expect the whitepoint to already be in greyscale
                baseCol = ModifyColourLayerHueShift(chosenColours[l], layerWhitePoint[l], baseCol);
                break;
            }
        }

        writeBuffer[i] = baseCol;
        //writeBuffer[i] = layerCol; // debug view
    }
}