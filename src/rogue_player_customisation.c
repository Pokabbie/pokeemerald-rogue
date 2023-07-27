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
        u8 i;
        u16 baseCol, layerCol;
        u16* writeBuffer = (u16*)&gDecompressionBuffer[0];

        for(i = 0; i < 16; ++i)
        {
            baseCol = basePal[i];
            layerCol = layerPal[i];

            if(layerCol == RGB_255(255, 0, 0))
                baseCol = layerCol;

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