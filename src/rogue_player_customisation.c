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

// This is just helpful as can input the UI accurate colours
#define RGB_10_CHANNEL(v) (u8)(((u32)v * (u32)31) / (u32)10)
#define RGB_10(r, g, b) RGB(RGB_10_CHANNEL(r), RGB_10_CHANNEL(g), RGB_10_CHANNEL(b))

struct PlayerOutfit
{
    const struct ObjectEventGraphicsInfo* objectEventGfx[PLAYER_AVATAR_STATE_COUNT];
    const u16* objectEventBasePal;
    const u16* objectEventLayerPal;
    const u32* trainerFrontBasePal;
    const u32* trainerFrontLayerPal;
    const u32* trainerBackBasePal;
    const u32* trainerBackLayerPal;
    u16 layerMaskColours[PLAYER_OUTFIT_STYLE_COUNT];
    u16 trainerFrontPic;
    u16 trainerBackPic;
    u8 bagVariant;
    u8 hasSpritingAnims : 1;
};

struct KnownColour
{
    u16 colour;
    const u8 name[8];
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
static u16 ModifyColourLayer(const struct PlayerOutfit* outfit, u8 layer, u16 layerColour, u16 inputColour);

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
            [PLAYER_OUTFIT_STYLE_APPEARANCE]       = RGB_255(255, 0, 0),
            [PLAYER_OUTFIT_STYLE_PRIMARY]   = RGB_255(0, 255, 0),
            [PLAYER_OUTFIT_STYLE_SECONDARY] = RGB_255(0, 0, 255),
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

static const struct KnownColour sKnownColours_Appearance[] = 
{
    {
        .name = _("Custom"),
        .colour = RGB_255(0, 0, 0),
    },
    {
        .name = _("A"),
        .colour = RGB_255(255, 222, 205),
    },
    {
        .name = _("B"),
        .colour = RGB_255(212, 170, 120),
    },
    {
        .name = _("C"),
        .colour = RGB_255(190, 146, 116),
    },
    {
        .name = _("D"),
        .colour = RGB_255(111, 59, 47),
    }
};

static const struct KnownColour sKnownColours_Clothes[] = 
{
    {
        .name = _("Default"),
        .colour = RGB_255(0, 0, 0),
    },
    {
        .name = _("Custom"),
        .colour = RGB_255(0, 0, 0),
    },

    {
        .name = _("Black"),
        .colour = RGB_10(2, 2, 2),
    },
    {
        .name = _("White"),
        .colour = RGB_10(10, 10, 10),
    },
    {
        .name = _("Grey"),
        .colour = RGB_10(6, 6, 6),
    },

    {
        .name = _("Red"),
        .colour = RGB_10(10, 4, 4),
    },
    {
        .name = _("Green"),
        .colour = RGB_10(3, 10, 3),
    },
    {
        .name = _("Blue"),
        .colour = RGB_10(4, 5, 10),
    },

    
    {
        .name = _("Pink"),
        .colour = RGB_10(10, 8, 8),
    },
    {
        .name = _("Brown"),
        .colour = RGB_10(6, 4, 2),
    },
    {
        .name = _("Purple"),
        .colour = RGB_10(8, 0, 7),
    },
    {
        .name = _("Yellow"),
        .colour = RGB_10(10, 9, 0),
    },
    {
        .name = _("Orange"),
        .colour = RGB_10(10, 6, 0),
    },
};

static const struct PlayerOutfit* GetCurrentOutfit()
{
    // TODO - Should probably have a missing no outfit?
    return &sPlayerOutfits[min(RoguePlayer_GetOutfitId(), PLAYER_OUTFIT_COUNT - 1)];
}

static const struct KnownColour* GetKnownColourArray(u8 layer)
{
    if(layer == PLAYER_OUTFIT_STYLE_APPEARANCE)
        return sKnownColours_Appearance;
    else
        return sKnownColours_Clothes;
}

static u16 GetKnownColourArrayCount(u8 layer)
{
    if(layer == PLAYER_OUTFIT_STYLE_APPEARANCE)
        return ARRAY_COUNT(sKnownColours_Appearance);
    else
        return ARRAY_COUNT(sKnownColours_Clothes);
}

STATIC_ASSERT(PLAYER_OUTFIT_STYLE_COUNT < ARRAY_COUNT(gSaveBlock2Ptr->playerStyles), playerStyleCount);

void RoguePlayer_SetNewGameOutfit()
{
    RoguePlayer_SetOutfitId(0);
    memset(gSaveBlock2Ptr->playerStyles, 0, sizeof(gSaveBlock2Ptr->playerStyles));

    RoguePlayer_SetOutfitStyle(PLAYER_OUTFIT_STYLE_APPEARANCE, sKnownColours_Appearance[1].colour); // TODO - Randomise this

    RoguePlayer_SetOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY, RGB_10(4, 5, 10));
    RoguePlayer_SetOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY, RGB_10(10, 10, 10));
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

static u16* GetOutfitStylePtr(u8 styleId)
{
    // Each ID gets 2 bytes
    return (u16*)&gSaveBlock2Ptr->playerStyles[styleId * 2];
}


void RoguePlayer_SetOutfitStyle(u8 styleId, u16 value)
{
    u16* ptr = GetOutfitStylePtr(styleId);

    // Wrap on ranges
    //switch(styleId)
    //{
    //    case PLAYER_OUTFIT_STYLE_APPEARANCE:
    //        value = WrapRange(value, 0, ARRAY_COUNT(sKnownColours_Appearance) - 1);
    //        break;
    //}

    *ptr = value;
}

u16 RoguePlayer_GetOutfitStyle(u8 styleId)
{
    return *GetOutfitStylePtr(styleId);
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
        u16 layerWhitePoint[PLAYER_OUTFIT_STYLE_COUNT];
        u16* writeBuffer = (u16*)&gDecompressionBuffer[0];

        // Calculate the brightest colour for each layer to act as the white point
        {
            for(l = 0; l < PLAYER_OUTFIT_STYLE_COUNT; ++l)
            {
                layerMask = outfit->layerMaskColours[l];
                layerWhitePoint[l] = RGB(0, 0, 0);

                // Check if this layer is supported for this outfit
                if(layerMask != RGB(0, 0, 0))
                {
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
                                layerWhitePoint[l] = baseCol;
                            }
                        }
                    }
                }

                DebugPrintf("[ModifyOutfitPalette] layer whitepoint %d: RGB(%d, %d, %d)", l, GET_R(layerWhitePoint[l]), GET_G(layerWhitePoint[l]), GET_B(layerWhitePoint[l]));
            }
        }

        // Calculate each colour in the palette
        for(i = 0; i < 16; ++i)
        {
            baseCol = basePal[i];
            layerCol = layerPal[i];

            for(l = 0; l < PLAYER_OUTFIT_STYLE_COUNT; ++l)
            {
                layerMask = outfit->layerMaskColours[l];

                if(layerCol == layerMask)
                {
                    baseCol = ModifyColourLayer(outfit, l, layerWhitePoint[l], baseCol);
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

#define COLOR_TRANSFORM_MULTIPLY_CHANNEL(value, whitePoint, target) min(31, ((((u16)value) * (u16)target) / (u16)whitePoint))

static u16 ModifyColourLayer(const struct PlayerOutfit* outfit, u8 layer, u16 layerWhitePoint, u16 inputColour)
{
    u8 r, g, b;
    u16 targetColour = RoguePlayer_GetOutfitStyle(layer);

    r = GET_R(inputColour);
    g = GET_G(inputColour);
    b = GET_B(inputColour);

    r = COLOR_TRANSFORM_MULTIPLY_CHANNEL(r, GET_R(layerWhitePoint), GET_R(targetColour));
    g = COLOR_TRANSFORM_MULTIPLY_CHANNEL(g, GET_G(layerWhitePoint), GET_G(targetColour));
    b = COLOR_TRANSFORM_MULTIPLY_CHANNEL(b, GET_B(layerWhitePoint), GET_B(targetColour));

    return RGB(r, g, b);
}

#undef COLOR_TRANSFORM_MULTIPLY_CHANNEL