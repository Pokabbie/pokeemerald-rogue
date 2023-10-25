#include "global.h"
#include "constants/event_objects.h"
#include "constants/rgb.h"
#include "constants/trainers.h"

#include "event_object_movement.h"
#include "decompress.h"
#include "global.fieldmap.h"
#include "graphics.h"
#include "item_menu_icons.h"
#include "random.h"

#include "rogue_player_customisation.h"

// Make sure we round to UI range here
#define RGB_255_CHANNEL(v) RGB_CONVERT_FROM_UI_RANGE(RGB_CONVERT_TO_UI_RANGE(min(31, (u8)(((u32)v * (u32)31) / (u32)255))))
#define RGB_255(r, g, b) RGB(RGB_255_CHANNEL(r), RGB_255_CHANNEL(g), RGB_255_CHANNEL(b))

// This is just helpful as can input the UI accurate colours
#define RGB_UI(r, g, b) RGB(RGB_CONVERT_FROM_UI_RANGE(r), RGB_CONVERT_FROM_UI_RANGE(g), RGB_CONVERT_FROM_UI_RANGE(b))

struct PlayerOutfit
{
    const struct ObjectEventGraphicsInfo* objectEventGfx[PLAYER_AVATAR_STATE_COUNT];
    const u16* objectEventBasePal;
    const u16* objectEventLayerPal;
    const u32* trainerFrontBasePal;
    const u32* trainerFrontLayerPal;
    const u32* trainerBackBasePal;
    const u32* trainerBackLayerPal;
    const u8 name[12];
    bool8 supportedLayers[PLAYER_OUTFIT_STYLE_COUNT];
    u16 trainerFrontPic;
    u16 trainerBackPic;
    u8 bagVariant;
    u8 hasSpritingAnims : 1;
};

struct KnownColour
{
    u16 colour;
    const u8 name[12];
    bool8 isCustomColour : 1;
};

enum 
{
    PLAYER_OUTFIT_RED,
    PLAYER_OUTFIT_LEAF,
    PLAYER_OUTFIT_ETHAN,
    PLAYER_OUTFIT_LYRA,
    PLAYER_OUTFIT_BRENDAN,
    PLAYER_OUTFIT_MAY,
    PLAYER_OUTFIT_LUCAS,
    PLAYER_OUTFIT_DAWN,
    PLAYER_OUTFIT_HILBERT,
    PLAYER_OUTFIT_HILDA,
    PLAYER_OUTFIT_NATE,
    PLAYER_OUTFIT_ROSA,
    PLAYER_OUTFIT_CALEM,
    PLAYER_OUTFIT_SERENA,

    // Secret unlocks
    PLAYER_OUTFIT_POKABBIE,
    PLAYER_OUTFIT_KATE,

    PLAYER_OUTFIT_COUNT,
};

static u16 CalculateWhitePointFor(const struct PlayerOutfit* outfit, u8 layer, const u16* basePal, const u16* layerPal);
static const u16* ModifyOutfitPalette(const struct PlayerOutfit* outfit, const u16* basePal, const u16* layerPal);
static const u16* ModifyOutfitCompressedPalette(const struct PlayerOutfit* outfit, const u32* basePalSrc, const u32* layerPalSrc);
static u16 ModifyColourLayer(const struct PlayerOutfit* outfit, u8 layer, u16 layerColour, u16 inputColour);

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerBrendanNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerBrendanRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerMayNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerMayRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerRedNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerRedRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLeafNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLeafRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerEthanNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerEthanRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLyraNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_PlayerLyraRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_LucasNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_LucasRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_DawnNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_DawnRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_HilbertNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_HilbertRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_HildaNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_HildaRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_NateNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_NateRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_RosaNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_RosaRiding;

extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_CalemNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_CalemRiding;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_SerenaNormal;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_SerenaRiding;


extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Misc_Aroma_Girl;
extern const struct ObjectEventGraphicsInfo gObjectEventGraphicsInfo_Glitch_Kate;

// We should always load something to as other object animation/effects rely on this palette slot
#define DEFAULT_PAL_TO_LOAD gObjectEventPal_PlayerBrendanBase
#define DEFAULT_OVERWORLD_PAL DEFAULT_PAL_TO_LOAD

static const struct PlayerOutfit sPlayerOutfits[PLAYER_OUTFIT_COUNT] =
{
    [PLAYER_OUTFIT_BRENDAN] =
    {
        .name = _("Brendan"),
        .trainerFrontPic = TRAINER_PIC_BRENDAN,
        .trainerBackPic = TRAINER_BACK_PIC_BRENDAN,
        .bagVariant = BAG_GFX_VARIANT_BRENDAN,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerBrendanNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerBrendanRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerBrendanBase,
        .objectEventLayerPal = gObjectEventPal_PlayerBrendanLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerBrendanBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerBrendanLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerBrendanBase,
        .trainerBackLayerPal= gTrainerPalette_PlayerBrendanLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    [PLAYER_OUTFIT_MAY] =
    {
        .name = _("May"),
        .trainerFrontPic = TRAINER_PIC_MAY,
        .trainerBackPic = TRAINER_BACK_PIC_MAY,
        .bagVariant = BAG_GFX_VARIANT_MAY,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerMayNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerMayRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerMayBase,
        .objectEventLayerPal = gObjectEventPal_PlayerMayLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerMayBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerMayLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerMayBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerMayLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },

    [PLAYER_OUTFIT_RED] =
    {
        .name = _("Red"),
        .trainerFrontPic = TRAINER_PIC_RED,
        .trainerBackPic = TRAINER_BACK_PIC_RED,
        .bagVariant = BAG_GFX_VARIANT_RED,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerRedNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerRedRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerRedLeafBase,
        .objectEventLayerPal = gObjectEventPal_PlayerRedLeafLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerRedLeafFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerRedLeafFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerRedLeafBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerRedLeafBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    [PLAYER_OUTFIT_LEAF] =
    {
        .name = _("Leaf"),
        .trainerFrontPic = TRAINER_PIC_LEAF,
        .trainerBackPic = TRAINER_BACK_PIC_LEAF,
        .bagVariant = BAG_GFX_VARIANT_LEAF,
        .hasSpritingAnims = TRUE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerLeafNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerLeafRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerRedLeafBase,
        .objectEventLayerPal = gObjectEventPal_PlayerRedLeafLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerRedLeafFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerRedLeafFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerRedLeafBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerRedLeafBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    
    [PLAYER_OUTFIT_ETHAN] =
    {
        .name = _("Ethan"),
        .trainerFrontPic = TRAINER_PIC_ETHAN,
        .trainerBackPic = TRAINER_BACK_PIC_ETHAN,
        .bagVariant = BAG_GFX_VARIANT_BRENDAN_SILVER,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerEthanNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerEthanRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerEthanBase,
        .objectEventLayerPal = gObjectEventPal_PlayerEthanLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerEthanFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerEthanFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerEthanBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerEthanBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    [PLAYER_OUTFIT_LYRA] =
    {
        .name = _("Lyra"),
        .trainerFrontPic = TRAINER_PIC_LYRA,
        .trainerBackPic = TRAINER_BACK_PIC_LYRA,
        .bagVariant = BAG_GFX_VARIANT_LEAF_SILVER,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_PlayerLyraNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_PlayerLyraRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerLyraBase,
        .objectEventLayerPal = gObjectEventPal_PlayerLyraLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerLyraFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerLyraFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerLyraBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerLyraBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },

    [PLAYER_OUTFIT_LUCAS] =
    {
        .name = _("Lucas"),
        .trainerFrontPic = TRAINER_PIC_LUCAS,
        .trainerBackPic = TRAINER_BACK_PIC_LUCAS,
        .bagVariant = BAG_GFX_VARIANT_RED_SILVER,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_LucasNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_LucasRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerLucasBase,
        .objectEventLayerPal = gObjectEventPal_PlayerLucasLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerLucasFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerLucasFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerLucasBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerLucasBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = FALSE,
        }
    },
    [PLAYER_OUTFIT_DAWN] =
    {
        .name = _("Dawn"),
        .trainerFrontPic = TRAINER_PIC_DAWN,
        .trainerBackPic = TRAINER_BACK_PIC_DAWN,
        .bagVariant = BAG_GFX_VARIANT_LEAF,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_DawnNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_DawnRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerDawnBase,
        .objectEventLayerPal = gObjectEventPal_PlayerDawnLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerDawnFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerDawnFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerDawnBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerDawnBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },

    [PLAYER_OUTFIT_HILBERT] =
    {
        .name = _("Hilbert"),
        .trainerFrontPic = TRAINER_PIC_PLACEHOLDER_NPC,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_LEAF_BLACK,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_HilbertNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_HilbertRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerHilbertBase,
        .objectEventLayerPal = gObjectEventPal_PlayerHilbertLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerDawnFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerDawnFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerDawnBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerDawnBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    [PLAYER_OUTFIT_HILDA] =
    {
        .name = _("Hilda"),
        .trainerFrontPic = TRAINER_PIC_PLACEHOLDER_NPC,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_LEAF_PINK,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_HildaNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_HildaRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerHildaBase,
        .objectEventLayerPal = gObjectEventPal_PlayerHildaLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerDawnFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerDawnFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerDawnBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerDawnBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    
    [PLAYER_OUTFIT_NATE] =
    {
        .name = _("Nate"),
        .trainerFrontPic = TRAINER_PIC_PLACEHOLDER_NPC,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_BRENDAN_SILVER,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_NateNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_NateRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerNateBase,
        .objectEventLayerPal = gObjectEventPal_PlayerNateLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerDawnFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerDawnFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerDawnBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerDawnBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    [PLAYER_OUTFIT_ROSA] =
    {
        .name = _("Rosa"),
        .trainerFrontPic = TRAINER_PIC_PLACEHOLDER_NPC,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_LEAF_PINK,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_RosaNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_RosaRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerRosaBase,
        .objectEventLayerPal = gObjectEventPal_PlayerRosaLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerDawnFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerDawnFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerDawnBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerDawnBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    
    [PLAYER_OUTFIT_CALEM] =
    {
        .name = _("Calem"),
        .trainerFrontPic = TRAINER_PIC_PLACEHOLDER_NPC,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_LEAF_BLACK,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_CalemNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_CalemRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerCalemBase,
        .objectEventLayerPal = gObjectEventPal_PlayerCalemLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerDawnFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerDawnFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerDawnBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerDawnBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = TRUE,
        }
    },
    [PLAYER_OUTFIT_SERENA] =
    {
        .name = _("Serena"),
        .trainerFrontPic = TRAINER_PIC_PLACEHOLDER_NPC,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_LEAF_PINK,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_SerenaNormal,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_SerenaRiding,
        },
        .objectEventBasePal = gObjectEventPal_PlayerSerenaBase,
        .objectEventLayerPal = gObjectEventPal_PlayerSerenaLayers,
        .trainerFrontBasePal = gTrainerPalette_PlayerDawnFrontBase,
        .trainerFrontLayerPal = gTrainerPalette_PlayerDawnFrontLayers,
        .trainerBackBasePal = gTrainerPalette_PlayerDawnBackBase,
        .trainerBackLayerPal = gTrainerPalette_PlayerDawnBackLayers,
        .supportedLayers = 
        {
            [PLAYER_OUTFIT_STYLE_APPEARANCE] = TRUE,
            [PLAYER_OUTFIT_STYLE_PRIMARY] = TRUE,
            [PLAYER_OUTFIT_STYLE_SECONDARY] = FALSE,
        }
    },

    [PLAYER_OUTFIT_POKABBIE] =
    {
        .name = _("Pokabbie"),
        .trainerFrontPic = TRAINER_PIC_POKABBIE,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_LEAF_PINK,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_Misc_Aroma_Girl,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_Misc_Aroma_Girl,
        },
        .objectEventBasePal = DEFAULT_OVERWORLD_PAL,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = NULL,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = NULL,
        .trainerBackLayerPal = NULL,
    },
    [PLAYER_OUTFIT_KATE] =
    {
        .name = _("Kate"),
        .trainerFrontPic = TRAINER_PIC_GLITCH_KATE,
        .trainerBackPic = TRAINER_BACK_PIC_NONE,
        .bagVariant = BAG_GFX_VARIANT_LEAF_BLACK,
        .hasSpritingAnims = FALSE,
        .objectEventGfx = 
        {
            [PLAYER_AVATAR_STATE_NORMAL]            = &gObjectEventGraphicsInfo_Glitch_Kate,
            [PLAYER_AVATAR_STATE_RIDE_GRABBING]     = &gObjectEventGraphicsInfo_Glitch_Kate,
        },
        .objectEventBasePal = DEFAULT_OVERWORLD_PAL,
        .objectEventLayerPal = NULL,
        .trainerFrontBasePal = NULL,
        .trainerFrontLayerPal = NULL,
        .trainerBackBasePal = NULL,
        .trainerBackLayerPal = NULL,
    }
};

static const u16 sLayerMaskColours[PLAYER_OUTFIT_STYLE_COUNT] =
{
    [PLAYER_OUTFIT_STYLE_APPEARANCE]    = RGB_255(255, 0, 0),
    [PLAYER_OUTFIT_STYLE_PRIMARY]       = RGB_255(0, 255, 0),
    [PLAYER_OUTFIT_STYLE_SECONDARY]     = RGB_255(0, 0, 255),
};

static const struct KnownColour sKnownColours_Appearance[] = 
{
    {
        .name = _("Custom"),
        .colour = RGB_255(0, 0, 0),
        .isCustomColour = TRUE,
    },

    {
        .name = _("A"),
        .colour = RGB_255(254, 227, 212),
    },
    {
        .name = _("B"),
        .colour = RGB_255(255, 219, 172),
    },
    {
        .name = _("C"),
        .colour = RGB_255(241, 194, 125),
    },
    {
        .name = _("D"),
        .colour = RGB_255(224, 172, 105),
    },
    {
        .name = _("E"),
        .colour = RGB_255(198, 134, 66),
    },
    {
        .name = _("F"),
        .colour = RGB_255(141, 85, 36),
    },
    {
        .name = _("G"),
        .colour = RGB_255(94, 55, 46),
    },
};

static const struct KnownColour sKnownColours_Clothes[] = 
{
    {
        .name = _("Custom"),
        .colour = RGB_255(0, 0, 0),
        .isCustomColour = TRUE,
    },

    {
        .name = _("Black"),
        .colour = RGB_UI(3, 3, 3),
    },
    {
        .name = _("White"),
        .colour = RGB_UI(10, 10, 10),
    },
    {
        .name = _("Grey"),
        .colour = RGB_UI(6, 6, 6),
    },

    {
        .name = _("Red"),
        .colour = RGB_UI(10, 4, 4),
    },
    {
        .name = _("Green"),
        .colour = RGB_UI(3, 10, 3),
    },
    {
        .name = _("Blue"),
        .colour = RGB_UI(4, 5, 10),
    },

    
    {
        .name = _("Pink"),
        .colour = RGB_UI(10, 6, 8),
    },
    {
        .name = _("Brown"),
        .colour = RGB_UI(5, 3, 2),
    },
    {
        .name = _("Purple"),
        .colour = RGB_UI(8, 0, 8),
    },
    {
        .name = _("Yellow"),
        .colour = RGB_UI(10, 9, 0),
    },
    {
        .name = _("Orange"),
        .colour = RGB_UI(10, 6, 0),
    },
    {
        .name = _("Gold"),
        .colour = RGB_UI(9, 7, 0),
    },
};

STATIC_ASSERT(PLAYER_OUTFIT_STYLE_COUNT < ARRAY_COUNT(gSaveBlock2Ptr->playerStyles), playerStyleCount);

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

static u8 GetKnownColourIndex(const struct PlayerOutfit* outfit, u8 layer, u16 colour)
{
    u8 i, customColourIdx;
    const struct KnownColour* knownColours = GetKnownColourArray(layer);
    u16 knownColoursCount = GetKnownColourArrayCount(layer);

    customColourIdx = 0;

    // Try to find the index before falling back to custom
    for(i = 0; i < knownColoursCount; ++i)
    {
        if(knownColours[i].isCustomColour != 0)
        {
            // Track this, but ignore it (It will be the fallback)
            customColourIdx = i;
        }
        else if(knownColours[i].colour == colour)
        {
            return i;
        }
    }

    return customColourIdx;
}

static void RandomiseOutfitStyle(u8 styleId)
{
    u8 idx;
    const struct KnownColour* knownColours = GetKnownColourArray(styleId);
    u16 knownColourCount = GetKnownColourArrayCount(styleId);

    do
    {
        idx = Random() % knownColourCount;
    } 
    while (knownColours[idx].isCustomColour);

    RoguePlayer_SetOutfitStyle(styleId, knownColours[idx].colour);
}

void RoguePlayer_SetNewGameOutfit()
{
    memset(gSaveBlock2Ptr->playerStyles, 0, sizeof(gSaveBlock2Ptr->playerStyles));

    // Default to blue and white with random appearance
    RoguePlayer_RandomiseOutfit(TRUE);
    RoguePlayer_SetOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY, RGB_UI(4, 5, 10));
    RoguePlayer_SetOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY, RGB_UI(10, 10, 10));
}

void RoguePlayer_RandomiseOutfit(bool8 includeOutfitId)
{
    if(includeOutfitId)
    {
        // TODO - Limit this, if we have outfit unlocks
        RoguePlayer_SetOutfitId(Random() % PLAYER_OUTFIT_COUNT);
    }

    RandomiseOutfitStyle(PLAYER_OUTFIT_STYLE_APPEARANCE);
    RandomiseOutfitStyle(PLAYER_OUTFIT_STYLE_PRIMARY);
    RandomiseOutfitStyle(PLAYER_OUTFIT_STYLE_SECONDARY);
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

const u8* RoguePlayer_GetOutfitName()
{
    return GetCurrentOutfit()->name;
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

bool8 RoguePlayer_SupportsOutfitStyle(u8 styleId)
{
    return GetCurrentOutfit()->supportedLayers[styleId] != FALSE;
}

const u8* RoguePlayer_GetOutfitStyleName(u8 styleId)
{
    const struct KnownColour* knownColours = GetKnownColourArray(styleId);
    u8 idx = GetKnownColourIndex(GetCurrentOutfit(), styleId, RoguePlayer_GetOutfitStyle(styleId));
    return knownColours[idx].name;
}

void RoguePlayer_IncrementOutfitStyleByName(u8 styleId, s8 delta)
{
    const struct KnownColour* knownColours = GetKnownColourArray(styleId);
    u16 knownColourCount = GetKnownColourArrayCount(styleId);
    s8 idx = GetKnownColourIndex(GetCurrentOutfit(), styleId, RoguePlayer_GetOutfitStyle(styleId));

    while(TRUE)
    {
        idx = WrapRange(idx + delta, 0, knownColourCount - 1);

        // Update the style colour
        if(knownColours[idx].isCustomColour)
        {
            // ignore this
        }
        else
        {
            RoguePlayer_SetOutfitStyle(styleId, knownColours[idx].colour);
            break;
        }
    }
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

static u16 GreyScaleColour(u16 colour)
{
    u8 brightness = max(GET_R(colour), max(GET_G(colour), GET_B(colour)));
    return RGB(brightness, brightness, brightness);
}

static u16 CalculateWhitePointFor(const struct PlayerOutfit* outfit, u8 layer, const u16* basePal, const u16* layerPal)
{
    u16 layerMask = sLayerMaskColours[layer];
    u16 layerWhitePoint = RGB(0, 0, 0);

    // Check if this layer is supported for this outfit
    if(layerMask != RGB(0, 0, 0) && outfit->supportedLayers[layer] == TRUE)
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
        // Do this in greyscale
        {
            for(i = 0; i < PLAYER_OUTFIT_STYLE_COUNT; ++i)
            {
                layerWhitePoint[i] = GreyScaleColour(CalculateWhitePointFor(outfit, i, basePal, layerPal));
            }
        }

        // Calculate each colour in the palette
        for(i = 0; i < 16; ++i)
        {
            baseCol = basePal[i];
            layerCol = layerPal[i];

            for(l = 0; l < PLAYER_OUTFIT_STYLE_COUNT; ++l)
            {
                layerMask = sLayerMaskColours[l];

                if(layerCol == layerMask && outfit->supportedLayers[l] == TRUE)
                {
                    // Expect the whitepoint to already be in greyscale
                    baseCol = ModifyColourLayer(outfit, l, layerWhitePoint[l], GreyScaleColour(baseCol));
                    break;
                }
            }

            writeBuffer[i] = baseCol;
        }

        return writeBuffer;
    }

    // Always return a valid palette to avoid reading out of bounds memory
    return basePal != NULL ? basePal : DEFAULT_PAL_TO_LOAD;
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