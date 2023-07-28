#include "global.h"
#include "constants/event_objects.h"
#include "constants/rgb.h"
#include "constants/trainers.h"

#include "event_object_movement.h"
#include "decompress.h"
#include "global.fieldmap.h"
#include "graphics.h"
#include "item_menu_icons.h"

#include "rogue_player_customisation.h"

#define RGB_255_CHANNEL(v) (u8)(((u32)v * (u32)31) / (u32)255)
#define RGB_255(r, g, b) RGB(RGB_255_CHANNEL(r), RGB_255_CHANNEL(g), RGB_255_CHANNEL(b))

#define MAX_COLOUR_HUE_RANGE 10
#define MAX_COLOUR_SAT_RANGE 5
#define MAX_COLOUR_LUM_RANGE 5

struct PlayerOutfit
{
    const struct ObjectEventGraphicsInfo* objectEventGfx[PLAYER_AVATAR_STATE_COUNT];
    const u16* objectEventBasePal;
    const u16* objectEventLayerPal;
    const u32* trainerFrontBasePal;
    const u32* trainerFrontLayerPal;
    const u32* trainerBackBasePal;
    const u32* trainerBackLayerPal;
    u16 layerMaskColours[PLAYER_LAYER_COUNT];
    u16 trainerFrontPic;
    u16 trainerBackPic;
    u8 bagVariant;
    u8 hasSpritingAnims : 1;
};

enum 
{
    PLAYER_OUTFIT_BRENDAN,
    PLAYER_OUTFIT_MAY,
    PLAYER_OUTFIT_RED,
    PLAYER_OUTFIT_LEAF,
    PLAYER_OUTFIT_ETHAN,
    PLAYER_OUTFIT_LYRA,
    
    PLAYER_OUTFIT_TEST,

    PLAYER_OUTFIT_COUNT,
};

static const u16* ModifyOutfitPalette(const struct PlayerOutfit* outfit, const u16* basePal, const u16* layerPal);
static const u16* ModifyOutfitCompressedPalette(const struct PlayerOutfit* outfit, const u32* basePalSrc, const u32* layerPalSrc);
static u16 ModifyColourLayer(const struct PlayerOutfit* outfit, u16 colour, u8 layer);

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerBrendanNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerBrendanFieldMove;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerBrendanRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerMayNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerMayFieldMove;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerMayRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerRedNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerRedFieldMove;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerRedRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLeafNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLeafFieldMove;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLeafRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerEthanNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerEthanFieldMove;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerEthanRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLyraNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLyraFieldMove;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLyraRiding;

static const u16 sAppearanceColourProfiles[] = 
{
    RGB_255(255, 222, 205), // zero point
    RGB_255(212, 170, 120),
    RGB_255(190, 146, 116),
    RGB_255(111, 59, 47),

    //RGB_255(0, 0, 255),
    //RGB_255(0, 255, 0),
    //RGB_255(255, 0, 0),
};

static const struct PlayerOutfit sPlayerOutfits[PLAYER_OUTFIT_COUNT] =
{
    [PLAYER_OUTFIT_BRENDAN] =
    {
        .trainerFrontPic = TRAINER_PIC_BRENDAN,
        .trainerBackPic = TRAINER_BACK_PIC_BRENDAN,
        .bagVariant = BAG_GFX_VARIANT_BRENDAN,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerBrendanNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerBrendanRiding,
            [PLAYER_AVATAR_STATE_FIELD_MOVE]        = &gObjectEventGraphicsInfo_PlayerBrendanFieldMove, // <- todo remove this
        },
        .objectEventBasePal = gObjectEventPal_PlayerBrendanBase,
        .objectEventLayerPal = gObjectEventPal_PlayerBrendanLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerBrendanBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerBrendanLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerBrendanBase,
        .trainerBackLayerPal= gTrainerPalette_PlayerBrendanLayers,
        .layerMaskColours = 
        {
            [PLAYER_LAYER_APPEARANCE]       = RGB_255(255, 0, 0),
            [PLAYER_LAYER_PRIMARY_COLOUR]   = RGB_255(0, 255, 0),
            [PLAYER_LAYER_SECONDARY_COLOUR] = RGB_255(0, 0, 255),
        }
    },
    [PLAYER_OUTFIT_MAY] =
    {
        .trainerFrontPic = TRAINER_PIC_MAY,
        .trainerBackPic = TRAINER_BACK_PIC_MAY,
        .bagVariant = BAG_GFX_VARIANT_MAY,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerMayNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerMayRiding,
            [PLAYER_AVATAR_STATE_FIELD_MOVE]        = &gObjectEventGraphicsInfo_PlayerMayFieldMove, // <- todo remove this
        },
        .objectEventBasePal = gObjectEventPal_May_0_0,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = gTrainerPalette_May_0_0,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = gTrainerPalette_May_0_0,
        .trainerBackLayerPal = NULL,
    },

    [PLAYER_OUTFIT_RED] =
    {
        .trainerFrontPic = TRAINER_PIC_RED,
        .trainerBackPic = TRAINER_BACK_PIC_MAY,
        .bagVariant = BAG_GFX_VARIANT_RED,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerRedNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerRedRiding,
            [PLAYER_AVATAR_STATE_FIELD_MOVE]        = &gObjectEventGraphicsInfo_PlayerRedFieldMove, // <- todo remove this
        },
        .objectEventBasePal = gObjectEventPal_Red_0_0,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = gTrainerPalette_Red_Front_0_0,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = gTrainerPalette_Red_Back_0_0,
        .trainerBackLayerPal = NULL,
    },
    [PLAYER_OUTFIT_LEAF] =
    {
        .trainerFrontPic = TRAINER_PIC_LEAF,
        .trainerBackPic = TRAINER_BACK_PIC_LEAF,
        .bagVariant = BAG_GFX_VARIANT_LEAF,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerLeafNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerLeafRiding,
            [PLAYER_AVATAR_STATE_FIELD_MOVE]        = &gObjectEventGraphicsInfo_PlayerLeafFieldMove, // <- todo remove this
        },
        .objectEventBasePal = gObjectEventPal_Red_0_0,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = gTrainerPalette_Red_Front_0_0,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = gTrainerPalette_Red_Back_0_0,
        .trainerBackLayerPal = NULL,
    },
    
    [PLAYER_OUTFIT_ETHAN] =
    {
        .trainerFrontPic = TRAINER_PIC_ETHAN,
        .trainerBackPic = TRAINER_BACK_PIC_ETHAN,
        .bagVariant = BAG_GFX_VARIANT_ETHAN,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerEthanNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerEthanRiding,
            [PLAYER_AVATAR_STATE_FIELD_MOVE]        = &gObjectEventGraphicsInfo_PlayerEthanFieldMove, // <- todo remove this
        },
        .objectEventBasePal = gObjectEventPal_Ethan_0_0,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = gTrainerPalette_Ethan_Front_0_0,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = gTrainerPalette_Ethan_Back_0_0,
        .trainerBackLayerPal = NULL,
    },
    [PLAYER_OUTFIT_LYRA] =
    {
        .trainerFrontPic = TRAINER_PIC_LYRA,
        .trainerBackPic = TRAINER_BACK_PIC_LYRA,
        .bagVariant = BAG_GFX_VARIANT_LYRA,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerLyraNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerLyraRiding,
            [PLAYER_AVATAR_STATE_FIELD_MOVE]        = &gObjectEventGraphicsInfo_PlayerLyraFieldMove, // <- todo remove this
        },
        .objectEventBasePal = gObjectEventPal_Lyra_0_0,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = gTrainerPalette_Lyra_Front_0_0,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = gTrainerPalette_Lyra_Back_0_0,
        .trainerBackLayerPal = NULL,
    },
    [PLAYER_OUTFIT_TEST] =
    {
        .trainerFrontPic = TRAINER_PIC_MAGMA_GRUNT_F,
        .trainerBackPic = TRAINER_BACK_PIC_WALLY,
        .bagVariant = BAG_GFX_VARIANT_LYRA,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerLyraNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerLyraRiding,
            [PLAYER_AVATAR_STATE_FIELD_MOVE]        = &gObjectEventGraphicsInfo_PlayerLyraFieldMove, // <- todo remove this
        },
        .objectEventBasePal = gObjectEventPal_Red_0_0,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = gTrainerPalette_Red_Front_0_0,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = gTrainerPalette_Red_Back_0_0,
        .trainerBackLayerPal = NULL,
    }
};

static const struct PlayerOutfit* GetCurrentOutfit()
{
    // TODO - Should probably have a missing no outfit?
    return &sPlayerOutfits[min(RoguePlayer_GetOutfitId(), PLAYER_OUTFIT_COUNT - 1)];
}

STATIC_ASSERT(PLAYER_OUTFIT_STYLE_COUNT < ARRAY_COUNT(gSaveBlock2Ptr->playerStyles), playerStyleCount);

void RoguePlayer_SetNewGameOutfit()
{
    RoguePlayer_SetOutfitId(0);
    memset(gSaveBlock2Ptr->playerStyles, 0, sizeof(gSaveBlock2Ptr->playerStyles));

    gSaveBlock2Ptr->playerStyles[PLAYER_OUTFIT_STYLE_APPEARANCE] = 0; // TODO - Randomise this
    // Default is blue/white!
    gSaveBlock2Ptr->playerStyles[PLAYER_OUTFIT_STYLE_PRIMARY_HUE] = 3;
    gSaveBlock2Ptr->playerStyles[PLAYER_OUTFIT_STYLE_PRIMARY_SAT] = 1;
    gSaveBlock2Ptr->playerStyles[PLAYER_OUTFIT_STYLE_PRIMARY_LUM] = 0;

    gSaveBlock2Ptr->playerStyles[PLAYER_OUTFIT_STYLE_SECONDARY_HUE] = 0;
    gSaveBlock2Ptr->playerStyles[PLAYER_OUTFIT_STYLE_SECONDARY_SAT] = -5;
    gSaveBlock2Ptr->playerStyles[PLAYER_OUTFIT_STYLE_SECONDARY_LUM] = 1;
}

void RoguePlayer_SetOutfitId(u16 outfit)
{
    gSaveBlock2Ptr->playerGender = outfit;
}

u16 RoguePlayer_GetOutfitId()
{
    return gSaveBlock2Ptr->playerGender;
}

u16 RoguePlayer_GetOutfitCount()
{
    return PLAYER_OUTFIT_COUNT;
}

static s8 WrapRange(s8 value, s8 minVal, s8 maxVal)
{
    s8 range = maxVal - minVal;

    while(value < minVal)
        value += range;

    while(value > maxVal)
        value -= range;

    return value;
}

static s8 ClampRange(s8 value, s8 minVal, s8 maxVal)
{
    return min(maxVal, max(minVal, value));
}

void RoguePlayer_SetOutfitStyle(u8 styleId, s8 value)
{
    // Wrap on ranges
    switch(styleId)
    {
        case PLAYER_OUTFIT_STYLE_APPEARANCE:
            value = WrapRange(value, 0, ARRAY_COUNT(sAppearanceColourProfiles) - 1);
            break;

        case PLAYER_OUTFIT_STYLE_PRIMARY_HUE:
        case PLAYER_OUTFIT_STYLE_SECONDARY_HUE:
            value = WrapRange(value, 0, MAX_COLOUR_HUE_RANGE);
            break;

        case PLAYER_OUTFIT_STYLE_PRIMARY_SAT:
        case PLAYER_OUTFIT_STYLE_SECONDARY_SAT:
            value = ClampRange(value, -MAX_COLOUR_SAT_RANGE, MAX_COLOUR_SAT_RANGE);
            break;

        case PLAYER_OUTFIT_STYLE_PRIMARY_LUM:
        case PLAYER_OUTFIT_STYLE_SECONDARY_LUM:
            value = ClampRange(value, -MAX_COLOUR_LUM_RANGE, MAX_COLOUR_LUM_RANGE);
            break;
    }

    gSaveBlock2Ptr->playerStyles[styleId] = value;
}

s8 RoguePlayer_GetOutfitStyle(u8 styleId)
{
    return gSaveBlock2Ptr->playerStyles[styleId];
}

s8 RoguePlayer_GetOutfitStylePerc(u8 styleId)
{
    s16 value = RoguePlayer_GetOutfitStyle(styleId);

    // Wrap on ranges
    switch(styleId)
    {
        case PLAYER_OUTFIT_STYLE_APPEARANCE:
            value = (value * 100) / (ARRAY_COUNT(sAppearanceColourProfiles) - 1);
            break;

        case PLAYER_OUTFIT_STYLE_PRIMARY_HUE:
        case PLAYER_OUTFIT_STYLE_SECONDARY_HUE:
            value = (value * 100) / MAX_COLOUR_HUE_RANGE;
            break;

        case PLAYER_OUTFIT_STYLE_PRIMARY_SAT:
        case PLAYER_OUTFIT_STYLE_SECONDARY_SAT:
            value = (value * 100) / MAX_COLOUR_SAT_RANGE;
            break;

        case PLAYER_OUTFIT_STYLE_PRIMARY_LUM:
        case PLAYER_OUTFIT_STYLE_SECONDARY_LUM:
            value = (value * 100) / MAX_COLOUR_LUM_RANGE;
            break;
    }

    return (s8)value;
}

bool8 RoguePlayer_HasSpritingAnim()
{
    return GetCurrentOutfit()->hasSpritingAnims != 0;
}

u16 RoguePlayer_GetTrainerFrontPic()
{
    return GetCurrentOutfit()->trainerFrontPic;
}

u16 RoguePlayer_GetTrainerBackPic()
{
    return GetCurrentOutfit()->trainerBackPic;
}

const u16* RoguePlayer_GetTrainerFrontPalette()
{
    const struct PlayerOutfit* outfit = GetCurrentOutfit();
    const u32* basePalSrc = outfit->trainerFrontBasePal;
    const u32* layerPalSrc = outfit->trainerFrontLayerPal;

    return ModifyOutfitCompressedPalette(outfit, basePalSrc, layerPalSrc);
}

const u16* RoguePlayer_GetTrainerBackPalette()
{
    const struct PlayerOutfit* outfit = GetCurrentOutfit();
    const u32* basePalSrc = outfit->trainerBackBasePal;
    const u32* layerPalSrc = outfit->trainerBackLayerPal;

    return ModifyOutfitCompressedPalette(outfit, basePalSrc, layerPalSrc);
}

const struct ObjectEventGraphicsInfo* RoguePlayer_GetObjectEventGraphicsInfo(u8 state)
{
    return GetCurrentOutfit()->objectEventGfx[state];
}

const u16* RoguePlayer_GetOverworldPalette()
{
    const struct PlayerOutfit* outfit = GetCurrentOutfit();
    const u16* basePal = outfit->objectEventBasePal;
    const u16* layerPal = outfit->objectEventLayerPal;

    return ModifyOutfitPalette(outfit, outfit->objectEventBasePal, outfit->objectEventLayerPal);
}

u8 RoguePlayer_GetTextVariantId()
{
    return gSaveBlock2Ptr->playerTrainerId[0];
}

u8 RoguePlayer_GetBagGfxVariant()
{
    return GetCurrentOutfit()->bagVariant;
}

static const u16* ModifyOutfitPalette(const struct PlayerOutfit* outfit, const u16* basePal, const u16* layerPal)
{
    if(layerPal != NULL)
    {
        // Apply the dynamic changes using the layer pal
        u8 i, l;
        u16 baseCol, layerCol, layerMask;
        u16* writeBuffer = (u16*)&gDecompressionBuffer[0];

        for(i = 0; i < 16; ++i)
        {
            baseCol = basePal[i];
            layerCol = layerPal[i];

            for(l = 0; l < PLAYER_LAYER_COUNT; ++l)
            {
                layerMask = outfit->layerMaskColours[l];

                if(layerCol == layerMask && layerMask != RGB(0, 0, 0) && layerMask != RGB(31, 31, 31))
                {
                    baseCol = ModifyColourLayer(outfit, baseCol, l); 
                    break;
                }
            }

            writeBuffer[i] = baseCol;
        }

        return writeBuffer;
    }

    return basePal;
}

static const u16* ModifyOutfitCompressedPalette(const struct PlayerOutfit* outfit, const u32* basePalSrc, const u32* layerPalSrc)
{
    // Decompress into different area of decompression buffer
    u16* tempBuffer = (u16*)&gDecompressionBuffer[0];
    u16* basePal = &tempBuffer[16];
    u16* layerPal = &tempBuffer[32];

    LZ77UnCompWram(basePalSrc, basePal);

    if(layerPalSrc != NULL)
        LZ77UnCompWram(layerPalSrc, layerPal);
    else
        layerPal = NULL;

    return ModifyOutfitPalette(outfit, basePal, layerPal);
}

#define COLOR_TRANSFORM_MULTIPLY_CHANNEL(value, from, to) min(31, ((((u16)value) * (u16)to) / (u16)from))

u16 rgb2hsl(u16 col);
u16 hsl2rgb(u16 col);

static u16 ModifyColourLayer_Appearance(const struct PlayerOutfit* outfit, u16 colour)
{
    u8 r, g, b;
    u8 appearance = RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_APPEARANCE);

    r = GET_R(colour);
    g = GET_G(colour);
    b = GET_B(colour);

    r = COLOR_TRANSFORM_MULTIPLY_CHANNEL(r, GET_R(sAppearanceColourProfiles[0]), GET_R(sAppearanceColourProfiles[appearance]));
    g = COLOR_TRANSFORM_MULTIPLY_CHANNEL(g, GET_G(sAppearanceColourProfiles[0]), GET_G(sAppearanceColourProfiles[appearance]));
    b = COLOR_TRANSFORM_MULTIPLY_CHANNEL(b, GET_B(sAppearanceColourProfiles[0]), GET_B(sAppearanceColourProfiles[appearance]));

    return RGB(r, g, b);
}

static u16 ModifyColourLayer_Colour(const struct PlayerOutfit* outfit, u16 colour, s16 hShift, s16 sShift, s16 lShift)
{
    s16 h, s, l;

    hShift = (hShift * 32) / MAX_COLOUR_HUE_RANGE;
    sShift = (sShift * 32) / MAX_COLOUR_SAT_RANGE;
    lShift = (lShift * 32) / MAX_COLOUR_LUM_RANGE;

    colour = rgb2hsl(colour);

    h = GET_R(colour);
    s = GET_G(colour);
    l = GET_B(colour);

    h = (h + hShift) % 32;
    s = ClampRange(s + sShift, 0, 31);
    l = ClampRange(l + lShift, 0, 31);

    return hsl2rgb(RGB((u8)h, (u8)s, (u8)l));
}

static u16 ModifyColourLayer_Hue(const struct PlayerOutfit* outfit, u16 colour, u8 shift)
{
    u8 h, s, l;

    shift = ((u16)shift * 32) / MAX_COLOUR_HUE_RANGE;
    colour = rgb2hsl(colour);

    h = GET_R(colour);
    s = GET_G(colour);
    l = GET_B(colour);

    h = (h + shift) % 32;

    return hsl2rgb(RGB(h, s, l));
}

static u16 ModifyColourLayer_GreyScale(const struct PlayerOutfit* outfit, u16 colour, u8 shift)
{
    u8 h, s, l;

    shift = ((u16)shift * 32) / MAX_COLOUR_HUE_RANGE;
    colour = rgb2hsl(colour);

    h = GET_R(colour);
    //s = GET_G(colour);
    //l = GET_B(colour);

    s = 0;
    l = min(31, shift);

    return hsl2rgb(RGB(h, s, l));
}

static u16 ModifyColourLayer(const struct PlayerOutfit* outfit, u16 colour, u8 layer)
{
    switch (layer)
    {
    case PLAYER_LAYER_APPEARANCE:
        colour = ModifyColourLayer_Appearance(outfit, colour);
        break;

    case PLAYER_LAYER_PRIMARY_COLOUR:
            colour = ModifyColourLayer_Colour(
                outfit, colour, 
                RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY_HUE),
                RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY_SAT),
                RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY_LUM)
            );
        break;

    case PLAYER_LAYER_SECONDARY_COLOUR:
            colour = ModifyColourLayer_Colour(
                outfit, colour, 
                RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY_HUE),
                RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY_SAT),
                RoguePlayer_GetOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY_LUM)
            );
        break;
    }

    return colour;
}

#undef COLOR_TRANSFORM_MULTIPLY_255








// HSL <---> RGB conversions
//


#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

struct RGB
{
  float r;
  float g;
  float b;
};

struct HSL
{
  float h;
  float s;
  float l;
};

/*
 * Converts an RGB color value to HSL. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes r, g, and b are contained in the set [0, 255] and
 * returns HSL in the set [0, 1].
 */
u16 rgb2hsl(u16 col)
{
    float r, g, b, h, s, l, max, min;
  
    r = GET_R(col) / 31.0f;
    g = GET_G(col) / 31.0f;
    b = GET_B(col) / 31.0f;
  
    max = MAX(MAX(r,g),b);
    min = MIN(MIN(r,g),b);

    h = s = l = (max + min) / 2;

    if (max == min) 
    {
        h = s = 0; // achromatic
    }
  else 
  {
    float d = max - min;
    s = (l > 0.5) ? d / (2 - max - min) : d / (max + min);
    
    if (max == r) 
    {
      h = (g - b) / d + (g < b ? 6 : 0);
    }
    else if (max == g) 
    {
      h = (b - r) / d + 2;
    }
    else if (max == b) 
    {
      h = (r - g) / d + 4;
    }
    
    h /= 6;
  }

  return RGB(
    min(31, (u8)(h * 32)),
    min(31, (u8)(s * 32)),
    min(31, (u8)(l * 32))
  );
}

////////////////////////////////////////////////////////////////////////

/*
 * Converts an HUE to r, g or b.
 * returns float in the set [0, 1].
 */
float hue2rgb(float p, float q, float t)
{

  if (t < 0) 
    t += 1;
  if (t > 1) 
    t -= 1;
  if (t < 1./6) 
    return p + (q - p) * 6 * t;
  if (t < 1./2) 
    return q;
  if (t < 2./3)   
    return p + (q - p) * (2./3 - t) * 6;
    
  return p;
  
}

////////////////////////////////////////////////////////////////////////

/*
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns RGB in the set [0, 255].
 */
u16 hsl2rgb(u16 col)
{
    float r, g, b, h, s, l, max, min;

    h = GET_R(col) / 31.0f;
    s = GET_G(col) / 31.0f;
    l = GET_B(col) / 31.0f;
  
    if(0 == s) 
    {
        r = g = b = l; // achromatic
    }
    else
    {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        r = hue2rgb(p, q, h + 1./3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1./3);
    }

  return RGB(
    min(31, (u8)(r * 32)),
    min(31, (u8)(g * 32)),
    min(31, (u8)(b * 32))
  );
}