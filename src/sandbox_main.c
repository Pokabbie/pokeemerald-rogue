#include "global.h"
#include "constants/battle.h"
#include "constants/battle_ai.h"
#include "constants/map_types.h"
#include "constants/trainers.h"

#include "battle.h"
#include "battle_setup.h"
#include "data.h"
#include "event_data.h"
#include "graphics.h"
#include "palette.h"

#include "sandbox_main.h"

//extern const u16 gTilesetPalettes_General[][16];
extern const u16 gTilesetPalettes_General02_Spring[];
extern const u16 gTilesetPalettes_General02_Summer[];
extern const u16 gTilesetPalettes_General02_Autumn[];
extern const u16 gTilesetPalettes_General02_Winter[];

static const u16 sLevelcapTrainers[] = 
{
    TRAINER_ROXANNE_1,
    TRAINER_BRAWLY_1,
    TRAINER_WATTSON_1,
    TRAINER_FLANNERY_1,

    TRAINER_NORMAN_1,
    TRAINER_WINONA_1,
    TRAINER_TATE_AND_LIZA_1,
    TRAINER_JUAN_1,

    TRAINER_DRAKE,
    TRAINER_STEVEN_CHAMPION,
};

static bool8 IsKeyTrainerBattle()
{
    u8 trainerClass = gTrainers[gTrainerBattleOpponent_A].trainerClass;

    switch (trainerClass)
    {
    case TRAINER_CLASS_AQUA_ADMIN:
    case TRAINER_CLASS_AQUA_LEADER:
    case TRAINER_CLASS_ELITE_FOUR:
    case TRAINER_CLASS_LEADER:
    case TRAINER_CLASS_CHAMPION:
    case TRAINER_CLASS_KANTO_CHAMPION:
    case TRAINER_CLASS_MAGMA_ADMIN:
    case TRAINER_CLASS_MAGMA_LEADER:
    case TRAINER_CLASS_RIVAL:
    case TRAINER_CLASS_COMMUNITY_CHALLENGER:
    case TRAINER_CLASS_PAST_ATTEMPT:

    case TRAINER_CLASS_SALON_MAIDEN:
    case TRAINER_CLASS_DOME_ACE:
    case TRAINER_CLASS_PALACE_MAVEN:
    case TRAINER_CLASS_ARENA_TYCOON:
    case TRAINER_CLASS_FACTORY_HEAD:
    case TRAINER_CLASS_PIKE_QUEEN:
    case TRAINER_CLASS_PYRAMID_KING:
        return TRUE;
    }

    return FALSE;
}

bool8 Sandbox_UseFastBattleAnims()
{
    // Force slow anims for significant battles
    if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && IsKeyTrainerBattle())
        return FALSE;

    // Force slow anims for legendaries
    if((gBattleTypeFlags & BATTLE_TYPE_LEGENDARY) != 0)
        return FALSE;

    return TRUE;
}

u16 Sandbox_ModifyBattleWaitTime(u16 waitTime, bool8 awaitingMessage)
{
    if(Sandbox_UseFastBattleAnims())
    {
        return awaitingMessage ? 8 : 0;
    }
    else
    {
        if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && IsKeyTrainerBattle())
        {
            u8 trainerClass = gTrainers[gTrainerBattleOpponent_A].trainerClass;

            // Champ fight at engine default
            if(trainerClass == TRAINER_CLASS_CHAMPION || trainerClass == TRAINER_CLASS_KANTO_CHAMPION)
                return waitTime;

            // Still run faster and default game because it's way too slow :(
            return waitTime / 2;
        }
        else
            // Go faster, but not quite gym leader slow
            return waitTime / 4;
    }
}

s16 Sandbox_ModifyBattleSlideAnim(s16 speed)
{
    if(Sandbox_UseFastBattleAnims())
    {
        if(speed < 0)
            return speed * 2 - 1;
        else
            return speed * 2 + 1;
    }

    return speed;
}

#define PLAYER_STYLE(prefix, x, y) if(style1 == x && style2 == y) return prefix ## _ ## x ## _ ## y

const void* Sandbox_ModifyLoadPalette(const void *src)
{
    const u8 gender = gSaveBlock2Ptr->playerGender;
    const u8 style1 = gSaveBlock2Ptr->playerStyle[0];
    const u8 style2 = gSaveBlock2Ptr->playerStyle[1];

    // ObjectEvent palette
    if(gender == 0 && src == gObjectEventPal_Brendan)
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gObjectEventPal_Brendan, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }
    if(gender == 1 && src == gObjectEventPal_May)
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gObjectEventPal_May, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }

    // Trainer
    // front/back pics
    if(gender == 0 && (src == gTrainerPalette_Brendan || src == gTrainerBackPic_Brendan))
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gTrainerPalette_Brendan, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }
    if(gender == 1 && (src == gTrainerPalette_May || src == gTrainerBackPic_May))
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gTrainerPalette_May, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }

    return src;
}

const void* Sandbox_ModifyLoadCompressedPalette(const void *src)
{
    return Sandbox_ModifyLoadPalette(src);
}

u32 Sandbox_GetTrainerAIFlags(u16 trainerNum)
{
    u32 flags = gTrainers[trainerNum].aiFlags;


#ifdef POKEMON_EXPANSION
    // Every trainer is smart
    flags |= AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SETUP_FIRST_TURN | AI_FLAG_HP_AWARE;

    // EX only flags
    flags |= AI_FLAG_WILL_SUICIDE | AI_FLAG_SMART_SWITCHING;

    if(gTrainers[trainerNum].doubleBattle)
    {
        flags |= AI_FLAG_HELP_PARTNER;
    }
#else
    // Every trainer is smart
    flags |= AI_SCRIPT_CHECK_BAD_MOVE | AI_SCRIPT_TRY_TO_FAINT | AI_SCRIPT_CHECK_VIABILITY | AI_SCRIPT_SETUP_FIRST_TURN | AI_SCRIPT_HP_AWARE | AI_SCRIPT_TRY_SUNNY_DAY_START;
#endif

    return flags;
}

static u8 CalculateTrainerLevel(u16 trainerId)
{
    u8 i;
    u8 levelCap = 0;

    for(i = 0; i < gTrainers[trainerId].partySize; ++i)
    {
        switch (gTrainers[trainerId].partyFlags)
        {
        case 0:
            {
                const struct TrainerMonNoItemDefaultMoves *party = gTrainers[trainerId].party.NoItemDefaultMoves;
                levelCap = max(levelCap, party[i].lvl);
            }
            break;
        case F_TRAINER_PARTY_CUSTOM_MOVESET:
            {
                const struct TrainerMonNoItemCustomMoves *party = gTrainers[trainerId].party.NoItemCustomMoves;
                levelCap = max(levelCap, party[i].lvl);
            }
            break;
        case F_TRAINER_PARTY_HELD_ITEM:
            {
                const struct TrainerMonItemDefaultMoves *party = gTrainers[trainerId].party.ItemDefaultMoves;
                levelCap = max(levelCap, party[i].lvl);
            }
            break;
        case F_TRAINER_PARTY_CUSTOM_MOVESET | F_TRAINER_PARTY_HELD_ITEM:
            {
                const struct TrainerMonItemCustomMoves *party = gTrainers[trainerId].party.ItemCustomMoves;
                levelCap = max(levelCap, party[i].lvl);
            }
            break;
        case F_TRAINER_PARTY_CUSTOM_MOVESET | F_TRAINER_PARTY_HELD_ITEM | F_TRAINER_PARTY_ABILITY:
            {
                const struct TrainerMonItemCustomMovesAbility *party = gTrainers[trainerId].party.ItemCustomMovesAbility;
                levelCap = max(levelCap, party[i].lvl);
            }
            break;
        }
    }

    return levelCap;
}

u8 Sandbox_GetCurrentLevelCap()
{
    u8 i;
    u16 trainerId;

    for(i = 0; i < ARRAY_COUNT(sLevelcapTrainers); ++i)
    {
        trainerId = sLevelcapTrainers[i];

        if(!HasTrainerBeenFought(trainerId))
        {
            return CalculateTrainerLevel(trainerId);
        }
    }

    return 100;
}

static bool8 ShouldApplySeasonTintForCurrentMap()
{
    return gMapHeader.mapType != MAP_TYPE_INDOOR;
}

// Will only apply override palette if the input matches
static void TintPalette_CompareOverride(u16 *palette, u16 count, const u16* comparePalette, const u16* overridePalette)
{
    u16 i, j;
    u16 colour;

    for (i = 0; i < count; i++)
    {
        colour = *palette;

        for(j = 0; j < 16; ++j)
        {
            if(comparePalette[j] == colour)
            {
                colour = overridePalette[j];
                break;
            }
        }

        *palette++ = colour;
    }
}

static u8 CalcCurrentSeason()
{
    u8 badgeCount = 0;
    u32 i;

    for (i = FLAG_BADGE01_GET; i < FLAG_BADGE01_GET + NUM_BADGES; i++)
    {
        if (FlagGet(i))
            badgeCount++;
    }

    return (badgeCount + gSaveBlock2Ptr->playerTrainerId[0]) % SEASON_COUNT;
}

static void TintPalette_Season(u16 *palette, u16 count)
{
    switch (CalcCurrentSeason())
    {
    case SEASON_SPRING:
        break;
    case SEASON_SUMMER:
        TintPalette_CompareOverride(palette, count, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Summer);
        break;
    case SEASON_AUTUMN:
        TintPalette_CompareOverride(palette, count, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Autumn);
        break;
    case SEASON_WINTER:
        TintPalette_CompareOverride(palette, count, gTilesetPalettes_General02_Spring, gTilesetPalettes_General02_Winter);
        break;
    }
}

void Sandbox_ModifyOverworldPalette(u16 offset, u16 count)
{
    if(ShouldApplySeasonTintForCurrentMap)
    {
        TintPalette_Season(&gPlttBufferUnfaded[offset], count * 16);
        CpuCopy16(&gPlttBufferUnfaded[offset], &gPlttBufferFaded[offset], count * 16);
    }
}

void Sandbox_HasTrainerBeenFought()
{
    u16 trainerId = VarGet(VAR_0x8004);
    VarSet(VAR_RESULT, HasTrainerBeenFought(trainerId));
}