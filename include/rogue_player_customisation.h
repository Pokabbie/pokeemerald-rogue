#ifndef ROGUE_PLAYER_CUSTOMISATION__H
#define ROGUE_PLAYER_CUSTOMISATION__H

#define RGB_MAX_UI_VALUE 10
#define MULTI_DIVIDE_ROUND(value, multiply, divide) (((value * multiply) / divide) + ((value * multiply) % divide >= divide/ 2 ? 1 : 0))

#define RGB_CONVERT_TO_UI_RANGE(value) min(31, MULTI_DIVIDE_ROUND((u16)value, (u16)RGB_MAX_UI_VALUE, (u16)31))
#define RGB_CONVERT_FROM_UI_RANGE(value) min(31, MULTI_DIVIDE_ROUND((u16)value, (u16)31, (u16)RGB_MAX_UI_VALUE))

enum 
{
    PLAYER_OUTFIT_STYLE_APPEARANCE,
    PLAYER_OUTFIT_STYLE_PRIMARY,
    PLAYER_OUTFIT_STYLE_SECONDARY,
    PLAYER_OUTFIT_STYLE_COUNT,
};

enum
{
    PLAYER_COLOUR_MODE_HUE,
    PLAYER_COLOUR_MODE_GREYSCALE,
    PLAYER_COLOUR_MODE_COUNT,
};

enum 
{
    OUTFIT_UNLOCK_NONE,
    OUTFIT_UNLOCK_PLACEHOLDER, // the outfit is just a placeholder value and can't be used currently

    OUTFIT_UNLOCK_TEAM_ROCKET,
    OUTFIT_UNLOCK_TEAM_AQUA,
    OUTFIT_UNLOCK_TEAM_MAGMA,
    OUTFIT_UNLOCK_TEAM_GALACTIC,
    OUTFIT_UNLOCK_TEAM_PLASMA, // unused
    OUTFIT_UNLOCK_TEAM_NEO_PLASMA, // unused
    OUTFIT_UNLOCK_TEAM_FLARE, // unused

    OUTFIT_UNLOCK_EASTER_EGG_POKABBIE,
    OUTFIT_UNLOCK_EASTER_EGG_KATE,
    OUTFIT_UNLOCK_EASTER_EGG_ERMA,
    OUTFIT_UNLOCK_EASTER_EGG_RAVEN,
    OUTFIT_UNLOCK_EASTER_EGG_TAILS,

    OUTFIT_UNLOCK_EASTER_EGG_ZEFA,
    OUTFIT_UNLOCK_EASTER_EGG_LIGHTNINGSTRIKE7,
    OUTFIT_UNLOCK_EASTER_EGG_NACHOLORD,
    OUTFIT_UNLOCK_EASTER_EGG_LATERMANNER,
    OUTFIT_UNLOCK_EASTER_EGG_GENERICDOLPHIN,

    OUTFIT_UNLOCK_COUNT,

    OUTFIT_UNLOCK_EASTER_EGG_FIRST = OUTFIT_UNLOCK_EASTER_EGG_POKABBIE,
    OUTFIT_UNLOCK_EASTER_EGG_LAST = OUTFIT_UNLOCK_EASTER_EGG_GENERICDOLPHIN,
};

void RoguePlayer_SetNewGameOutfit();
void RoguePlayer_RandomiseOutfit(bool8 includeOutfitId);

void RoguePlayer_SetOutfitId(u16 outfit);
u16 RoguePlayer_GetOutfitId();
u32 RoguePlayer_GetOutfitTrainerFlags();

bool8 RoguePlayer_HasUnlockedOutfitId(u16 outfit);
void RoguePlayer_EnsureUnlockedOutfitId(u16 outfit);
void RoguePlayer_ActivateOutfitUnlock(u16 outfitUnlock);
bool8 RoguePlayer_HandleEasterEggOutfitUnlocks();

u16 RoguePlayer_GetOutfitCount();
const u8* RoguePlayer_GetOutfitName();

void RoguePlayer_SetOutfitStyle(u8 styleId, u16 value);
u16 RoguePlayer_GetOutfitStyle(u8 styleId);
bool8 RoguePlayer_SupportsOutfitStyle(u8 styleId);

const u8* RoguePlayer_GetOutfitStyleName(u8 styleId);
void RoguePlayer_IncrementOutfitStyleByName(u8 styleId, s8 delta);

bool8 RoguePlayer_HasSpritingAnim();

u16 RoguePlayer_GetTrainerFrontPic();
u16 RoguePlayer_GetTrainerBackPic();
const u16* RoguePlayer_GetTrainerFrontPalette();
const u16* RoguePlayer_GetTrainerBackPalette();

const struct ObjectEventGraphicsInfo* RoguePlayer_GetObjectEventGraphicsInfo(u8 state);
const u16* RoguePlayer_GetOverworldPalette();

const struct ObjectEventGraphicsInfo* RogueNetPlayer_GetObjectEventGraphicsInfo(u8 state);
const u16* RogueNetPlayer_GetOverworldPalette();

u8 RoguePlayer_GetTextVariantId();
u8 RoguePlayer_GetBagGfxVariant();

#endif