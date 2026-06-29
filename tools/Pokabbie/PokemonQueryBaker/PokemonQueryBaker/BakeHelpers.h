#pragma once
#include "gba/gba.h"
#include "constants/global.h"
#include "constants/rogue.h"

#ifdef ROGUE_EXPANSION
#include "config.h"
#include "config/pokemon.h"
#include "constants/cries.h"
#include "constants/pokedex.h"
#include "constants/form_change_types.h"
#include "constants/battle.h"
#include "constants/maps.h"
#include "pokemon_animation.h"
#endif

#include "constants/abilities.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/region_map_sections.h"
#include "constants/map_groups.h"
#include "constants/rogue.h"
#include "constants/rogue_pokedex.h"

#define min(x, y) (x < y ? x : y)
#define max(x, y) (x > y ? x : y)

#define _(str) (str)
#define ARRAY_COUNT(array) (size_t)(sizeof(array) / sizeof((array)[0]))

#define INCBIN_U8(...) { 0 }
#define INCBIN_U16(...) { 0 }
#define INCBIN_U32(...) { 0 }

#ifdef ROGUE_EXPANSION
#define COMPOUND_STRING(str) ""
#define POKEDEX_DESC_STRING(str) ""
#define FORM_SPECIES_END (0xffff)

#define DIV_ROUND_UP(val, roundBy)(((val) / (roundBy)) + (((val) % (roundBy)) ? 1 : 0))

#define MON_COORDS_SIZE(width, height)(DIV_ROUND_UP(width, 8) << 4 | DIV_ROUND_UP(height, 8))
#define GET_MON_COORDS_WIDTH(size)((size >> 4) * 8)
#define GET_MON_COORDS_HEIGHT(size)((size & 0xF) * 8)

#define GET_BASE_SPECIES_ID(speciesId) (gRogueSpeciesInfo[speciesId].formSpeciesIdTable != NULL ? gRogueSpeciesInfo[speciesId].formSpeciesIdTable[0] : speciesId)

struct Fusion
{
    u16 fusionStorageIndex;
    u16 itemId;
    u16 targetSpecies1;
    u16 targetSpecies2;
    u16 fusingIntoMon;
    u16 fusionMove;
    u16 unfuseForgetMove;
};

// From Sprite.h
struct AnimFrameCmd
{
    // If the sprite has an array of images, this is the array index.
    // If the sprite has a sheet, this is the tile offset.
    u32 imageValue : 16;

    u32 duration : 6;
    u32 hFlip : 1;
    u32 vFlip : 1;
};

struct AnimLoopCmd
{
    u32 type : 16;
    u32 count : 6;
};

struct AnimJumpCmd
{
    u32 type : 16;
    u32 target : 6;
};

// The first halfword of this union specifies the type of command.
// If it -2, then it is a jump command. If it is -1, then it is the end of the script.
// Otherwise, it is the imageValue for a frame command.
union AnimCmd
{
    s16 type;
    struct AnimFrameCmd frame;
    struct AnimLoopCmd loop;
    struct AnimJumpCmd jump;
};

#define ANIMCMD_FRAME(...) \
    {.frame = {__VA_ARGS__}}
#define ANIMCMD_LOOP(_count) \
    {.loop = {.type = -3, .count = _count}}
#define ANIMCMD_JUMP(_target) \
    {.jump = {.type = -2, .target = _target}}
#define ANIMCMD_END \
    {.type = -1}
#endif

struct Evolution
{
    u16 method;
    u16 param;
    u16 targetSpecies;
};

#ifdef ROGUE_EXPANSION
struct FormChange
{
    u16 method;
    u16 targetSpecies;
    u16 param1;
    u16 param2;
    u16 param3;
};

struct SpeciesInfo /*0x8C*/
{
    /* 0x00 */ u8 baseHP;
    /* 0x01 */ u8 baseAttack;
    /* 0x02 */ u8 baseDefense;
    /* 0x03 */ u8 baseSpeed;
    /* 0x04 */ u8 baseSpAttack;
    /* 0x05 */ u8 baseSpDefense;
    /* 0x06 */ u8 types[2];
    /* 0x08 */ u8 catchRate;
    /* 0x09 */ u8 forceTeraType;
    /* 0x0A */ u16 expYield; // expYield was changed from u8 to u16 for the new Exp System.
    /* 0x0C */ u16 evYield_HP : 2;
    u16 evYield_Attack : 2;
    u16 evYield_Defense : 2;
    u16 evYield_Speed : 2;
    /* 0x0D */ u16 evYield_SpAttack : 2;
    u16 evYield_SpDefense : 2;
    u16 padding2 : 4;
    /* 0x0E */ u16 itemCommon;
    /* 0x10 */ u16 itemRare;
    /* 0x12 */ u8 genderRatio;
    /* 0x13 */ u8 eggCycles;
    /* 0x14 */ u8 friendship;
    /* 0x15 */ u8 growthRate;
    /* 0x16 */ u8 eggGroups[2];
    /* 0x18 */ u16 abilities[NUM_ABILITY_SLOTS]; // 3 abilities, no longer u8 because we have over 255 abilities now.
    /* 0x1E */ u8 safariZoneFleeRate;
    // Pokédex data
    /* 0x1F */ u8 categoryName[13];
    /* 0x1F */ u8 speciesName[POKEMON_NAME_LENGTH + 1];
    /* 0x2C */ u16 cryId;
    /* 0x2E */ u16 natDexNum;
    /* 0x30 */ u16 height; //in decimeters
    /* 0x32 */ u16 weight; //in hectograms
    /* 0x34 */ u16 pokemonScale;
    /* 0x36 */ u16 pokemonOffset;
    /* 0x38 */ u16 trainerScale;
    /* 0x3A */ u16 trainerOffset;
    /* 0x3C */ const u8* description;
    /* 0x40 */ u8 bodyColor : 7;
    // Graphical Data
    u8 noFlip : 1;
    /* 0x41 */ u8 frontAnimDelay;
    /* 0x42 */ u8 frontAnimId;
    /* 0x43 */ u8 backAnimId;
    /* 0x44 */ const union AnimCmd* const* frontAnimFrames;
    /* 0x48 */ const u32* frontPic;
    /* 0x4C */ const u32* frontPicFemale;
    /* 0x50 */ const u32* backPic;
    /* 0x54 */ const u32* backPicFemale;
    /* 0x58 */ const u32* palette;
    /* 0x5C */ const u32* paletteFemale;
    /* 0x60 */ const u32* shinyPalette;
    /* 0x64 */ const u32* shinyPaletteFemale;
    /* 0x68 */ const u8* iconSprite;
    /* 0x6C */ const u8* iconSpriteFemale;
    /* 0x70 */ const u8* footprint;
    // All Pokémon pics are 64x64, but this data table defines where in this 64x64 frame the sprite's non-transparent pixels actually are.
    /* 0x74 */ u8 frontPicSize; // The dimensions of this drawn pixel area.
    /* 0x74 */ u8 frontPicSizeFemale; // The dimensions of this drawn pixel area.
    /* 0x75 */ u8 frontPicYOffset; // The number of pixels between the drawn pixel area and the bottom edge.
    /* 0x76 */ u8 backPicSize; // The dimensions of this drawn pixel area.
    /* 0x76 */ u8 backPicSizeFemale; // The dimensions of this drawn pixel area.
    /* 0x77 */ u8 backPicYOffset; // The number of pixels between the drawn pixel area and the bottom edge.
    /* 0x78 */ u8 iconPalIndex : 3;
    u8 iconPalIndexFemale : 3;
    u8 stub : 1;
    u8 padding3 : 1;
    /* 0x79 */ u8 enemyMonElevation; // This determines how much higher above the usual position the enemy Pokémon is during battle. Species that float or fly have nonzero values.
    // Flags
    /* 0x7A */ u32 isLegendary : 1;
    u32 isMythical : 1;
    u32 isUltraBeast : 1;
    u32 isParadoxForm : 1;
    u32 isMegaEvolution : 1;
    u32 isPrimalReversion : 1;
    u32 isUltraBurst : 1;
    u32 isGigantamax : 1;
    u32 isTeraForm : 1;
    u32 isAlolanForm : 1;
    u32 isGalarianForm : 1;
    u32 isHisuianForm : 1;
    u32 isPaldeanForm : 1;
    u32 cannotBeTraded : 1;
    u32 allPerfectIVs : 1;
    u32 dexForceRequired : 1; // This species will be taken into account for Pokédex ratings even if they have the "isMythical" flag set.
    u32 padding4 : 16;
    // Move Data
    /* 0x88 */ const struct Evolution* evolutions;
    /* 0x84 */ const u16* formSpeciesIdTable;
    /* 0x84 */ const struct FormChange* formChangeTable;
};
#else
struct BaseStats
{
    u8 baseHP;
    u8 baseAttack;
    u8 baseDefense;
    u8 baseSpeed;
    u8 baseSpAttack;
    u8 baseSpDefense;
    u8 type1;
    u8 type2;
    u8 catchRate;
    u8 expYield;
    u16 evYield_HP : 2;
    u16 evYield_Attack : 2;
    u16 evYield_Defense : 2;
    u16 evYield_Speed : 2;
    u16 evYield_SpAttack : 2;
    u16 evYield_SpDefense : 2;
    u16 itemCommon;
    u16 itemRare;
    u8 genderRatio;
    u8 eggCycles;
    u8 friendship;
    u8 growthRate;
    u8 eggGroup1;
    u8 eggGroup2;
    u8 abilities[2];
    u8 safariZoneFleeRate;
    u8 bodyColor : 7;
    u8 noFlip : 1;
};
#endif

struct RoguePokedexVariant
{
    const u8* displayName;
    const u16* speciesList;
    u16 speciesCount;
    u8 genLimit;
};

struct RoguePokedexRegion
{
    const u8* displayName;
    const u16* variantList;
    u16 variantCount;
};

void memcpy(void* dst, void* src, size_t size);