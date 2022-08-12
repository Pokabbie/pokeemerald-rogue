#include "global.h"
#include "constants/abilities.h"
#include "constants/heal_locations.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/rogue.h"
#include "data.h"

#include "battle.h"
#include "battle_setup.h"
#include "berry.h"
#include "event_data.h"
#include "graphics.h"
#include "item.h"
#include "load_save.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "safari_zone.h"
#include "script.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"

#include "rogue_controller.h"
#include "rogue_query.h"

#define ROGUE_TRAINER_COUNT (FLAG_ROGUE_TRAINER_END - FLAG_ROGUE_TRAINER_START + 1)
#define ROGUE_ITEM_COUNT (FLAG_ROGUE_ITEM_END - FLAG_ROGUE_ITEM_START + 1)

// 8 badges, 4 elite, 2 champion
#define BOSS_ROOM_COUNT 14

#define OVERWORLD_FLAG 0

#ifdef ROGUE_DEBUG
EWRAM_DATA u8 gDebug_WildOptionCount = 0;
EWRAM_DATA u8 gDebug_ItemOptionCount = 0;
EWRAM_DATA u8 gDebug_TrainerOptionCount = 0;

extern const u8 gText_RogueDebug_Header[];
extern const u8 gText_RogueDebug_Room[];
extern const u8 gText_RogueDebug_BossRoom[];
extern const u8 gText_RogueDebug_Difficulty[];
extern const u8 gText_RogueDebug_PlayerLvl[];
extern const u8 gText_RogueDebug_WildLvl[];
extern const u8 gText_RogueDebug_WildCount[];
extern const u8 gText_RogueDebug_ItemCount[];
extern const u8 gText_RogueDebug_TrainerCount[];
extern const u8 gText_RogueDebug_Seed[];
#endif

// Box save data
#ifdef ROGUE_SUPPORT_QUICK_SAVE
struct RogueBoxSaveData
{
    u32 encryptionKey;
    struct Pokemon playerParty[PARTY_SIZE];
    struct ItemSlot bagPocket_Items[BAG_ITEMS_COUNT];
    struct ItemSlot bagPocket_KeyItems[BAG_KEYITEMS_COUNT];
    struct ItemSlot bagPocket_PokeBalls[BAG_POKEBALLS_COUNT];
    struct ItemSlot bagPocket_TMHM[BAG_TMHM_COUNT];
    struct ItemSlot bagPocket_Berries[BAG_BERRIES_COUNT];
};

ROGUE_STATIC_ASSERT(sizeof(struct RogueBoxSaveData) <= sizeof(struct BoxPokemon) * LEFTOVER_BOXES_COUNT * IN_BOX_COUNT, RogueBoxSaveData);

#endif

struct RogueTrainerTemp
{
    u32 seedToRestore;
    u8 allowedType[2];
    bool8 allowItemEvos;
    bool8 allowLedgendaries;
    bool8 hasAppliedFallback;
    bool8 hasUsedLeftovers;
    bool8 hasUsedShellbell;
};

struct RogueLocalData
{
    bool8 hasQuickLoadPending;
    struct RogueTrainerTemp trainerTemp;
    
#ifdef ROGUE_SUPPORT_QUICK_SAVE
    // We encode all our save data as box data :D
    union
    {
        struct BoxPokemon boxes[LEFTOVER_BOXES_COUNT][IN_BOX_COUNT];
        struct RogueBoxSaveData raw;
    } saveData;
#endif
};

EWRAM_DATA struct RogueLocalData gRogueLocal = {};
EWRAM_DATA struct RogueRunData gRogueRun = {};
EWRAM_DATA struct RogueHubData gRogueHubData = {};

static u8 GetDifficultyLevel(u16 roomIdx);
static u16 GetBossRoomForDifficulty(u16 difficulty);
static bool8 IsBossRoom(u16 roomIdx);
static bool8 IsSpecialEncounterRoom(void);

static u8 CalculateBossLevel(void);
static u8 CalculatePlayerLevel(void);
static u8 CalculateWildLevel(void);
static u8 CalculateTrainerLevel(u16 trainerNum);

static void RandomiseSafariWildEncounters(void);
static void RandomiseWildEncounters(void);
static void ResetTrainerBattles(void);
static void RandomiseEnabledTrainers(void);
static void RandomiseEnabledItems(void);
static void RandomiseBerryTrees(void);

static void HistoryBufferPush(u16* buffer, u16 capacity, u16 value);
static bool8 HistoryBufferContains(u16* buffer, u16 capacity, u16 value);

static u16 RogueRandomRange(u16 range, u8 flag)
{
    // Always use rogue random to avoid seeding issues based on flag
    u16 res = RogueRandom();

    if(FlagGet(FLAG_SET_SEED_ENABLED) && (flag == 0 || FlagGet(flag)))
        return res % range;
    else
        return Random() % range;
}

static bool8 RandomChance(u8 chance, u16 seedFlag)
{
    if(chance == 0)
        return FALSE;
    else if(chance >= 100)
        return TRUE;

    return (RogueRandomRange(100, seedFlag) + 1) <= chance;
}

static u16 Rogue_GetSeed(void)
{
    u32 word0 = gSaveBlock1Ptr->dewfordTrends[0].words[0];
    u32 word1 = gSaveBlock1Ptr->dewfordTrends[0].words[1];
    u32 offset = 3;

    //if(Rogue_IsRunActive())
    //{
    //    offset = gRogueRun.currentRoomIdx * 3;
    //}

    return (u16)(word0 + word1 * offset);
}

bool8 Rogue_IsRunActive(void)
{
    return FlagGet(FLAG_ROGUE_RUN_ACTIVE);
}

bool8 Rogue_ForceExpAll(void)
{
    return FlagGet(FLAG_ROGUE_EXP_ALL);
}

bool8 Rogue_FastBattleAnims(void)
{
    if(Rogue_IsRunActive() && !IsBossRoom(gRogueRun.currentRoomIdx) && !IsSpecialEncounterRoom())
    {
        return TRUE;
    }

    return FALSE;
}

void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    
    if(Rogue_IsRunActive() && species != SPECIES_NONE)
    {
        u8 targetLevel = CalculatePlayerLevel();
        u8 maxLevel = CalculateBossLevel();
        u8 currentLevel = GetMonData(mon, MON_DATA_LEVEL);

        if(currentLevel != MAX_LEVEL)
        {
            u8 growthRate = gBaseStats[species].growthRate; // Was using GROWTH_FAST?
            u32 currLvlExp;
            u32 nextLvlExp;
            u32 lvlExp;

            s32 desiredExpPerc = 0;

            if(currentLevel < maxLevel)
            {
                if(currentLevel >= targetLevel)
                {
                    desiredExpPerc = 51;

                    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
                    {
                        desiredExpPerc = 100;
                    }
                }
                else
                {
                    s16 delta = targetLevel - currentLevel;
                    
                    if(delta < 10)
                    {
                        // Give up to 5 levels at once
                        desiredExpPerc = 100 * min(5, delta);
                    }
                    else
                    {
                        // Give up to 10 levels at once
                        desiredExpPerc = 100 * min(10, delta);
                    }

                    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
                    {
                        // Give up to 30 levels at once
                        desiredExpPerc = 100 * min(30, delta);
                    }
                }
            }
            else
            {
                if(FlagGet(FLAG_ROGUE_CAN_OVERLVL))
                {
                    desiredExpPerc = 34;
                }
                else
                {
                    // No EXP once at target level
                    desiredExpPerc = 0;
                }
            }

            if(desiredExpPerc != 0)
            {
                // Pretty sure expGain get's casted to a u16 at some point so there's a limit to how much exp we can give
                s16 actualExpGain = 0;
                s16 lastExpGain = 0;

                //desiredExpPerc = 100;

                // Give levels
                while(desiredExpPerc > 0)
                {
                    currLvlExp = Rogue_ModifyExperienceTables(growthRate, currentLevel);
                    nextLvlExp = Rogue_ModifyExperienceTables(growthRate, currentLevel + 1);
                    lvlExp = (nextLvlExp - currLvlExp);

                    if(desiredExpPerc >= 100)
                    {
                        actualExpGain += lvlExp;
                        desiredExpPerc -= 100;

                        ++currentLevel;
                    }
                    else
                    {
                        s32 actualDelta = (lvlExp * desiredExpPerc) / 100;

                        // Give leftovers
                        actualExpGain += actualDelta;
                        desiredExpPerc = 0;
                    }

                    if(actualExpGain < lastExpGain)
                    {
                        // We must have overflowed :O
                        actualExpGain = 32767; // Max of a s16
                        break;
                    }

                    lastExpGain = actualExpGain;
                }

                *expGain = actualExpGain;
            }
            else
            {
                *expGain = 0;
            }
        }
    }
    else
    {
        // No EXP outside of runs
        *expGain = 0;
    }
}

void Rogue_ModifyEVGain(int* multiplier)
{
    if(FlagGet(FLAG_ROGUE_EV_GAIN_ENABLED))
    {
        *multiplier = 4;
    }
    else
    {
        *multiplier = 0;
    }
}

void Rogue_ModifyCatchRate(u16* catchRate, u16* ballMultiplier)
{ 
    if(Rogue_IsRunActive())
    {
#ifdef ROGUE_DEBUG
        *ballMultiplier = 12345; // Masterball equiv
#else
        u8 difficulty = GetDifficultyLevel(gRogueRun.currentRoomIdx);

        if(difficulty <= 1) // First 2 badges
        {
            *ballMultiplier = *ballMultiplier * 8;
        }
        else if(difficulty <= 2)
        {
            *ballMultiplier = *ballMultiplier * 4;
        }
        else if(difficulty <= 3)
        {
            *ballMultiplier = *ballMultiplier * 3;
        }
        else if(difficulty <= 4)
        {
            *ballMultiplier = *ballMultiplier * 2;
        }
        else if(difficulty <= 7)
        {
            // Minimum of 2x multiplier whilst doing gyms?
            *ballMultiplier = *ballMultiplier * 2;
        }
        else
        {
            // Elite 4 back to normal catch rates
        }
#endif

        // Equiv to Snorlax
        if(*catchRate < 25)
            *catchRate = 25;
    }
    else if(GetSafariZoneFlag())
    {
        *ballMultiplier = 12345; // Masterball equiv
    }
}

void Rogue_ModifyCaughtMon(struct Pokemon *mon)
{
    if(Rogue_IsRunActive())
    {
        u16 hp = GetMonData(mon, MON_DATA_HP);
        u16 maxHp = GetMonData(mon, MON_DATA_MAX_HP);
        u16 statusAilment = 0; // AILMENT_NONE

        hp = max(maxHp / 2, hp);

        // Heal up to 1/2 health and remove status effect
        SetMonData(mon, MON_DATA_HP, &hp);
        SetMonData(mon, MON_DATA_STATUS, &statusAilment);
    }
}

#define PLAYER_STYLE(prefix, x, y) if(gSaveBlock2Ptr->playerStyle0 == x && gSaveBlock2Ptr->playerStyle1 == y) return prefix ## _ ## x ## _ ## y

const u8* Rogue_ModifyPallete8(const u8* input)
{
    // Unused?

    //if(input == &gTrainerBackPic_Brendan[0])
    //{
    //    return gTrainerBackPic_RubySapphireBrendan;
    //}

    return input;
}

const u16* Rogue_ModifyPallete16(const u16* input)
{
    u8 skinStyle = gSaveBlock2Ptr->playerStyle0;

    if(input == &gObjectEventPal_Brendan_0_0[0])
    {
        PLAYER_STYLE(gObjectEventPal_Brendan, 0, 0);
        PLAYER_STYLE(gObjectEventPal_Brendan, 1, 0);
        PLAYER_STYLE(gObjectEventPal_Brendan, 2, 0);
        PLAYER_STYLE(gObjectEventPal_Brendan, 3, 0);
        PLAYER_STYLE(gObjectEventPal_Brendan, 0, 1);
        PLAYER_STYLE(gObjectEventPal_Brendan, 1, 1);
        PLAYER_STYLE(gObjectEventPal_Brendan, 2, 1);
        PLAYER_STYLE(gObjectEventPal_Brendan, 3, 1);
        PLAYER_STYLE(gObjectEventPal_Brendan, 0, 2);
        PLAYER_STYLE(gObjectEventPal_Brendan, 1, 2);
        PLAYER_STYLE(gObjectEventPal_Brendan, 2, 2);
        PLAYER_STYLE(gObjectEventPal_Brendan, 3, 2);
        PLAYER_STYLE(gObjectEventPal_Brendan, 0, 3);
        PLAYER_STYLE(gObjectEventPal_Brendan, 1, 3);
        PLAYER_STYLE(gObjectEventPal_Brendan, 2, 3);
        PLAYER_STYLE(gObjectEventPal_Brendan, 3, 3);
    }

    if(input == &gObjectEventPal_May_0_0[0])
    {
        PLAYER_STYLE(gObjectEventPal_May, 0, 0);
        PLAYER_STYLE(gObjectEventPal_May, 1, 0);
        PLAYER_STYLE(gObjectEventPal_May, 2, 0);
        PLAYER_STYLE(gObjectEventPal_May, 3, 0);
        PLAYER_STYLE(gObjectEventPal_May, 0, 1);
        PLAYER_STYLE(gObjectEventPal_May, 1, 1);
        PLAYER_STYLE(gObjectEventPal_May, 2, 1);
        PLAYER_STYLE(gObjectEventPal_May, 3, 1);
        PLAYER_STYLE(gObjectEventPal_May, 0, 2);
        PLAYER_STYLE(gObjectEventPal_May, 1, 2);
        PLAYER_STYLE(gObjectEventPal_May, 2, 2);
        PLAYER_STYLE(gObjectEventPal_May, 3, 2);
        PLAYER_STYLE(gObjectEventPal_May, 0, 3);
        PLAYER_STYLE(gObjectEventPal_May, 1, 3);
        PLAYER_STYLE(gObjectEventPal_May, 2, 3);
        PLAYER_STYLE(gObjectEventPal_May, 3, 3);
    }

    // Shared palettes between red and leaf
    if(input == &gObjectEventPal_Red_0_0[0])
    {
        PLAYER_STYLE(gObjectEventPal_Red, 0, 0);
        PLAYER_STYLE(gObjectEventPal_Red, 1, 0);
        PLAYER_STYLE(gObjectEventPal_Red, 2, 0);
        PLAYER_STYLE(gObjectEventPal_Red, 3, 0);
        PLAYER_STYLE(gObjectEventPal_Red, 0, 1);
        PLAYER_STYLE(gObjectEventPal_Red, 1, 1);
        PLAYER_STYLE(gObjectEventPal_Red, 2, 1);
        PLAYER_STYLE(gObjectEventPal_Red, 3, 1);
        PLAYER_STYLE(gObjectEventPal_Red, 0, 2);
        PLAYER_STYLE(gObjectEventPal_Red, 1, 2);
        PLAYER_STYLE(gObjectEventPal_Red, 2, 2);
        PLAYER_STYLE(gObjectEventPal_Red, 3, 2);
        PLAYER_STYLE(gObjectEventPal_Red, 0, 3);
        PLAYER_STYLE(gObjectEventPal_Red, 1, 3);
        PLAYER_STYLE(gObjectEventPal_Red, 2, 3);
        PLAYER_STYLE(gObjectEventPal_Red, 3, 3);
    }

    return input;
}

const u32* Rogue_ModifyPallete32(const u32* input)
{
    u8 skinStyle = gSaveBlock2Ptr->playerStyle0;

    if(input == &gTrainerPalette_Brendan_0_0[0])
    {
        PLAYER_STYLE(gTrainerPalette_Brendan, 0, 0);
        PLAYER_STYLE(gTrainerPalette_Brendan, 1, 0);
        PLAYER_STYLE(gTrainerPalette_Brendan, 2, 0);
        PLAYER_STYLE(gTrainerPalette_Brendan, 3, 0);
        PLAYER_STYLE(gTrainerPalette_Brendan, 0, 1);
        PLAYER_STYLE(gTrainerPalette_Brendan, 1, 1);
        PLAYER_STYLE(gTrainerPalette_Brendan, 2, 1);
        PLAYER_STYLE(gTrainerPalette_Brendan, 3, 1);
        PLAYER_STYLE(gTrainerPalette_Brendan, 0, 2);
        PLAYER_STYLE(gTrainerPalette_Brendan, 1, 2);
        PLAYER_STYLE(gTrainerPalette_Brendan, 2, 2);
        PLAYER_STYLE(gTrainerPalette_Brendan, 3, 2);
        PLAYER_STYLE(gTrainerPalette_Brendan, 0, 3);
        PLAYER_STYLE(gTrainerPalette_Brendan, 1, 3);
        PLAYER_STYLE(gTrainerPalette_Brendan, 2, 3);
        PLAYER_STYLE(gTrainerPalette_Brendan, 3, 3);
    }

    // Must swap for compressed version
    //if(input == &gTrainerFrontPic_Brendan[0])
    //{
    //    return gTrainerFrontPic_RubySapphireBrendan;
    //}


    if(input == &gTrainerPalette_May_0_0[0])
    {
        PLAYER_STYLE(gTrainerPalette_May, 0, 0);
        PLAYER_STYLE(gTrainerPalette_May, 1, 0);
        PLAYER_STYLE(gTrainerPalette_May, 2, 0);
        PLAYER_STYLE(gTrainerPalette_May, 3, 0);
        PLAYER_STYLE(gTrainerPalette_May, 0, 1);
        PLAYER_STYLE(gTrainerPalette_May, 1, 1);
        PLAYER_STYLE(gTrainerPalette_May, 2, 1);
        PLAYER_STYLE(gTrainerPalette_May, 3, 1);
        PLAYER_STYLE(gTrainerPalette_May, 0, 2);
        PLAYER_STYLE(gTrainerPalette_May, 1, 2);
        PLAYER_STYLE(gTrainerPalette_May, 2, 2);
        PLAYER_STYLE(gTrainerPalette_May, 3, 2);
        PLAYER_STYLE(gTrainerPalette_May, 0, 3);
        PLAYER_STYLE(gTrainerPalette_May, 1, 3);
        PLAYER_STYLE(gTrainerPalette_May, 2, 3);
        PLAYER_STYLE(gTrainerPalette_May, 3, 3);
    }

    if(input == &gTrainerPalette_Red_Front_0_0[0])
    {
        PLAYER_STYLE(gTrainerPalette_Red_Front, 0, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 1, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 2, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 3, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 0, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 1, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 2, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 3, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 0, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 1, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 2, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 3, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 0, 3);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 1, 3);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 2, 3);
        PLAYER_STYLE(gTrainerPalette_Red_Front, 3, 3);
    }

    // Palette is shared with red
    //if(input == &gTrainerPalette_Leaf[0])
    //{
    //}

    if(input == &gTrainerPalette_Red_Back_0_0[0])
    {
        PLAYER_STYLE(gTrainerPalette_Red_Back, 0, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 1, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 2, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 3, 0);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 0, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 1, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 2, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 3, 1);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 0, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 1, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 2, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 3, 2);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 0, 3);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 1, 3);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 2, 3);
        PLAYER_STYLE(gTrainerPalette_Red_Back, 3, 3);
    }

    // Must swap for compressed version
    //if(input == &gTrainerFrontPic_May[0])
    //{
    //    return gTrainerFrontPic_RubySapphireMay;
    //}

    return input;
}

#undef PLAYER_STYLE

void Rogue_ModifyBattleWinnings(u32* money)
{
    if(Rogue_IsRunActive())
    {
        // Once we've gotten champion we want to give a bit more money 
        u8 difficulty = GetDifficultyLevel(gRogueRun.currentRoomIdx);

        if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
        {
            if(difficulty <= 11)
            {
                *money = *money / 3;
            }
            else
            {
                // Kinder but not by much ;)
                *money = *money / 2;
            }
        }
        else if(!FlagGet(FLAG_ROGUE_EASY_ITEMS))
        {
            if(difficulty <= 11)
            {
                *money = *money / 2;
            }
        }
    }
}

void Rogue_ModifyBattleWaitTime(u16* waitTime)
{
    u8 difficulty = Rogue_IsRunActive() ? GetDifficultyLevel(gRogueRun.currentRoomIdx) : 0;

    if(Rogue_FastBattleAnims())
    {
        // 0 bugs out status sometimes I think?
        *waitTime = 1;//*waitTime / 8;
    }
    else if(difficulty != (BOSS_ROOM_COUNT - 1)) // Go at default speed for final fight
    {
        // Still run faster and default game because it's way too slow :(
        *waitTime = *waitTime / 2;
    }
}

s16 Rogue_ModifyBattleSlideAnim(s16 rate)
{
    u8 difficulty = Rogue_IsRunActive() ? GetDifficultyLevel(gRogueRun.currentRoomIdx) : 0;

    if(Rogue_FastBattleAnims())
    {
        if(rate < 0)
            return rate * 2 - 1;
        else
            return rate * 2 + 1;
    }
    //else if(difficulty == (BOSS_ROOM_COUNT - 1))
    //{
    //    // Go at default speed for final fight
    //    return rate * 2;
    //}

    // Still run faster and default game because it's way too slow :(
    return rate;
}

u8 SpeciesToGen(u16 species)
{
    if(species >= SPECIES_BULBASAUR && species <= SPECIES_MEW)
        return 1;
    if(species >= SPECIES_CHIKORITA && species <= SPECIES_CELEBI)
        return 2;
    if(species >= SPECIES_TREECKO && species <= SPECIES_DEOXYS)
        return 3;
#ifdef ROGUE_EXPANSION
    if(species >= SPECIES_TURTWIG && species <= SPECIES_ARCEUS)
        return 4;
    if(species >= SPECIES_VICTINI && species <= SPECIES_GENESECT)
        return 5;
    if(species >= SPECIES_CHESPIN && species <= SPECIES_VOLCANION)
        return 6;
    if(species >= SPECIES_ROWLET && species <= SPECIES_MELMETAL)
        return 7;
    if(species >= SPECIES_GROOKEY && species <= SPECIES_CALYREX)
        return 8;

    if(species >= SPECIES_RATTATA_ALOLAN && species <= SPECIES_MAROWAK_ALOLAN)
        return 7;
    if(species >= SPECIES_MEOWTH_GALARIAN && species <= SPECIES_STUNFISK_GALARIAN)
        return 8;

    if(species >= SPECIES_BURMY_SANDY_CLOAK && species <= SPECIES_ARCEUS_FAIRY)
        return 4;
#endif
    
    return 0;
}

bool8 IsGenEnabled(u8 gen)
{
#ifdef ROGUE_EXPANSION
    if(gen >= 1 && gen <= 8)
#else
    if(gen >= 1 && gen <= 3)
#endif
    {
        u16 maxGen = VarGet(VAR_ROGUE_ENABLED_GEN_LIMIT);

        if(maxGen == 0)
        {
            // Fallback for broken var
            return TRUE;
        }

        return gen <= maxGen;
    }

    return FALSE;
}

bool8 IsMegaEvolutionEnabled(void)
{
#ifdef ROGUE_EXPANSION
    return CheckBagHasItem(ITEM_MEGA_RING, 1);
#else
    return FALSE;
#endif
}

bool8 IsZMovesEnabled(void)
{
#ifdef ROGUE_EXPANSION
    return CheckBagHasItem(ITEM_Z_POWER_RING, 1);
#else
    return FALSE;
#endif
}

bool8 IsDynamaxEnabled(void)
{
#ifdef ROGUE_EXPANSION
    return CheckBagHasItem(ITEM_DYNAMAX_BAND, 1);
#else
    return FALSE;
#endif
}

#ifdef ROGUE_DEBUG

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return TRUE;
}

static u8* AppendNumberField(u8* strPointer, const u8* field, u32 num)
{
    u8 pow = 2;

    if(num >= 1000)
    {
        pow = 9;
    }
    else if(num >= 100)
    {
        pow = 3;
    }

    ConvertUIntToDecimalStringN(gStringVar1, num, STR_CONV_MODE_RIGHT_ALIGN, pow);

    strPointer = StringAppend(strPointer, field);
    return StringAppend(strPointer, gStringVar1);
}

u8* Rogue_GetMiniMenuContent(void)
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    u8 playerLevel = CalculatePlayerLevel();
    u8 wildLevel = CalculateWildLevel();

    u8* strPointer = &gStringVar4[0];
    *strPointer = EOS;

    strPointer = StringAppend(strPointer, gText_RogueDebug_Header);

    if(FlagGet(FLAG_SET_SEED_ENABLED))
    {
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, Rogue_GetSeed());
    }

    strPointer = AppendNumberField(strPointer, gText_RogueDebug_Room, gRogueRun.currentRoomIdx);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_BossRoom, GetBossRoomForDifficulty(difficultyLevel));
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_Difficulty, difficultyLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_PlayerLvl, playerLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildLvl, wildLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildCount, gDebug_WildOptionCount);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_ItemCount, gDebug_ItemOptionCount);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_TrainerCount, gDebug_TrainerOptionCount);

    return gStringVar4;
}
#else

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return Rogue_IsRunActive();
}

u8* Rogue_GetMiniMenuContent(void)
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    ConvertIntToDecimalStringN(gStringVar1, gSaveBlock2Ptr->playTimeHours, STR_CONV_MODE_RIGHT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, gSaveBlock2Ptr->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringExpandPlaceholders(gStringVar3, gText_RogueHourMinute);

    ConvertIntToDecimalStringN(gStringVar1, gRogueRun.currentRoomIdx, STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar2, difficultyLevel, STR_CONV_MODE_RIGHT_ALIGN, 2);
    
    StringExpandPlaceholders(gStringVar4, gText_RogueRoomProgress);

    return gStringVar4;
}
#endif

static void SelectStartMons(void)
{
    u8 i, j;
    bool8 isValid;
    u16 randIdx;
    u16 queryCount;
    u16 species;

    // Maybe consider compile time caching this query, as it's pretty slow :(
    RogueQuery_Clear();
    RogueQuery_Exclude(SPECIES_SUNKERN);
    RogueQuery_Exclude(SPECIES_SUNFLORA);

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesExcludeCommon();
    RogueQuery_SpeciesIsNotLegendary();
    RogueQuery_TransformToEggSpecies();
    RogueQuery_SpeciesWithAtLeastEvolutionStages(1);

    // Have to use uncollapsed queries as this query is too large otherwise
    queryCount = RogueQuery_UncollapsedSpeciesSize();

    for(i = 0; i < 3;)
    {
        isValid = TRUE;
        randIdx = Random() % queryCount;
        species = RogueQuery_AtUncollapsedIndex(randIdx);

        // Check other starter is not already this
        for(j = 0; j < i; ++j)
        {
            if(VarGet(VAR_ROGUE_STARTER0 + j) == species)
            {
                isValid = FALSE;
                break;
            }
        }

        if(isValid)
        {
            VarSet(VAR_ROGUE_STARTER0 + i, species);
            ++i;
        }
    }

#ifdef ROGUE_DEBUG
    VarSet(VAR_ROGUE_STARTER0, SPECIES_EEVEE);
    VarSet(VAR_ROGUE_STARTER1, SPECIES_CASTFORM);
#endif
}

void Rogue_OnNewGame(void)
{
    SetMoney(&gSaveBlock1Ptr->money, 0);

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE);

#ifdef ROGUE_EXPANSION
    FlagSet(FLAG_ROGUE_EXPANSION_ACTIVE);
#else
    FlagClear(FLAG_ROGUE_EXPANSION_ACTIVE);
#endif

    // Seed settings
    FlagClear(FLAG_SET_SEED_ENABLED);
    FlagSet(FLAG_SET_SEED_ITEMS);
    FlagSet(FLAG_SET_SEED_TRAINERS);
    FlagSet(FLAG_SET_SEED_BOSSES);
    FlagSet(FLAG_SET_SEED_WILDMONS);
    
    // Run settings
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagSet(FLAG_ROGUE_EXP_ALL);
    FlagSet(FLAG_ROGUE_EV_GAIN_ENABLED);
    FlagClear(FLAG_ROGUE_DOUBLE_BATTLES);
    FlagClear(FLAG_ROGUE_CAN_OVERLVL);
    FlagClear(FLAG_ROGUE_EASY_TRAINERS);
    FlagClear(FLAG_ROGUE_HARD_TRAINERS);
    FlagClear(FLAG_ROGUE_EASY_ITEMS);
    FlagClear(FLAG_ROGUE_HARD_ITEMS);
    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);

    VarSet(VAR_ROGUE_ENABLED_GEN_LIMIT, 3);
    VarSet(VAR_ROGUE_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, 0);
    VarSet(VAR_ROGUE_REWARD_MONEY, 0);
    VarSet(VAR_ROGUE_REWARD_CANDY, 0);
    VarSet(VAR_ROGUE_ADVENTURE_MONEY, 0);

    FlagSet(FLAG_SYS_B_DASH);
    EnableNationalPokedex();

    SetLastHealLocationWarp(HEAL_LOCATION_ROGUE_HUB);

    SelectStartMons();

#ifdef ROGUE_DEBUG
    SetMoney(&gSaveBlock1Ptr->money, 999999);

    AddBagItem(ITEM_RARE_CANDY, 99);
    VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, 13);

    //AddBagItem(ITEM_RARE_CANDY, 99);
    //AddBagItem(ITEM_RARE_CANDY, 99);
    //SetMoney(&gSaveBlock1Ptr->money, 60000);
//
    //struct Pokemon starterMon;
    //CreateMon(&starterMon, SPECIES_RAYQUAZA, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_GROUDON, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_KYOGRE, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_DEOXYS, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_LUGIA, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
//
    //CreateMon(&starterMon, SPECIES_HO_OH, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
#endif
}

void Rogue_GameClear(void)
{
    SetContinueGameWarpToHealLocation(HEAL_LOCATION_ROGUE_HUB);
}

void Rogue_SetDefaultOptions(void)
{
#ifdef ROGUE_DEBUG
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
#else
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_SLOW;
#endif
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
    //gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    //gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    //gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    //gSaveBlock2Ptr->regionMapZoom = FALSE;
}

static void CopyFromPocket(u8 pocket, struct ItemSlot* dst)
{
    u16 i;
    u16 count = gBagPockets[pocket].capacity;

    // Use getters to avoid encryption
    for (i = 0; i < BAG_ITEMS_COUNT; i++)
    {
        dst[i].itemId = gBagPockets[pocket].itemSlots[i].itemId;
        dst[i].quantity = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);
    }
}

static void CopyToPocket(u8 pocket, struct ItemSlot* src)
{
    u16 i;
    u16 count = gBagPockets[pocket].capacity;

    for (i = 0; i < BAG_ITEMS_COUNT; i++)
    {
        gBagPockets[pocket].itemSlots[i].itemId = src[i].itemId;
        SetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity, src[i].quantity);
    }
}

static void SaveHubInventory(void)
{
#ifdef ROGUE_SUPPORT_QUICK_SAVE
    u8 i;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        CopyMon(&gRogueLocal.saveData.raw.playerParty[i], &gPlayerParty[i], sizeof(gPlayerParty[i]));
    }
    for(; i < PARTY_SIZE; ++i)
    {
        ZeroMonData(&gRogueLocal.saveData.raw.playerParty[i]);
    }
    
    gRogueLocal.saveData.raw.encryptionKey = gSaveBlock2Ptr->encryptionKey;
    CopyFromPocket(ITEMS_POCKET, &gRogueLocal.saveData.raw.bagPocket_Items[0]);
    CopyFromPocket(KEYITEMS_POCKET, &gRogueLocal.saveData.raw.bagPocket_KeyItems[0]);
    CopyFromPocket(BALLS_POCKET, &gRogueLocal.saveData.raw.bagPocket_PokeBalls[0]);
    CopyFromPocket(TMHM_POCKET, &gRogueLocal.saveData.raw.bagPocket_TMHM[0]);
    CopyFromPocket(BERRIES_POCKET, &gRogueLocal.saveData.raw.bagPocket_Berries[0]);

#else
    // Store current states (Use normal save data)
    SavePlayerParty();
    LoadPlayerBag(); // Bag funcs named in opposite
#endif
}

static void LoadHubInventory(void)
{
#ifdef ROGUE_SUPPORT_QUICK_SAVE
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        CopyMon(&gPlayerParty[i], &gRogueLocal.saveData.raw.playerParty[i], sizeof(gPlayerParty[i]));
    }

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) == SPECIES_NONE)
            break;
    }
    gPlayerPartyCount = i;

    CopyToPocket(ITEMS_POCKET, &gRogueLocal.saveData.raw.bagPocket_Items[0]);
    CopyToPocket(KEYITEMS_POCKET, &gRogueLocal.saveData.raw.bagPocket_KeyItems[0]);
    CopyToPocket(BALLS_POCKET, &gRogueLocal.saveData.raw.bagPocket_PokeBalls[0]);
    CopyToPocket(TMHM_POCKET, &gRogueLocal.saveData.raw.bagPocket_TMHM[0]);
    CopyToPocket(BERRIES_POCKET, &gRogueLocal.saveData.raw.bagPocket_Berries[0]);

    //ApplyNewEncryptionKeyToBagItems(gRogueLocal.saveData.raw.encryptionKey);

#else
    // Restore current states
    LoadPlayerParty();
    SavePlayerBag(); // Bag funcs named in opposite
#endif
}

extern const u8 Rogue_QuickSaveLoad[];

void Rogue_OnSaveGame(void)
{
#ifdef ROGUE_SUPPORT_QUICK_SAVE
    u8 i;

    gSaveBlock1Ptr->rogueBlock.saveData.rngSeed = gRngRogueValue;

    memcpy(&gSaveBlock1Ptr->rogueBlock.saveData.runData, &gRogueRun, sizeof(gRogueRun));
    memcpy(&gSaveBlock1Ptr->rogueBlock.saveData.hubData, &gRogueHubData, sizeof(gRogueHubData));

    // Move Hub save data into storage box space
    for(i = 0; i < LEFTOVER_BOXES_COUNT; ++i)
    {
        memcpy(&gPokemonStoragePtr->boxes[TOTAL_BOXES_COUNT + i][0], &gRogueLocal.saveData.boxes[i][0], sizeof(struct BoxPokemon) * IN_BOX_COUNT);
    }

#endif
}

void Rogue_OnLoadGame(void)
{
#ifdef ROGUE_SUPPORT_QUICK_SAVE
    u8 i;
    memset(&gRogueLocal, 0, sizeof(gRogueLocal));

    gRngRogueValue = gSaveBlock1Ptr->rogueBlock.saveData.rngSeed;

    memcpy(&gRogueRun, &gSaveBlock1Ptr->rogueBlock.saveData.runData, sizeof(gRogueRun));
    memcpy(&gRogueHubData, &gSaveBlock1Ptr->rogueBlock.saveData.hubData, sizeof(gRogueHubData));

    // Move Hub save data out of storage box space
    for(i = 0; i < LEFTOVER_BOXES_COUNT; ++i)
    {
        memcpy(&gRogueLocal.saveData.boxes[i][0], &gPokemonStoragePtr->boxes[TOTAL_BOXES_COUNT + i][0], sizeof(struct BoxPokemon) * IN_BOX_COUNT);
    }

    if(Rogue_IsRunActive() && !FlagGet(FLAG_ROGUE_DEFEATED_BOSS13))
    {
        gRogueLocal.hasQuickLoadPending = TRUE;
        //ScriptContext1_SetupScript(Rogue_QuickSaveLoad);
    }
#endif
}

bool8 Rogue_OnProcessPlayerFieldInput(void)
{
#ifdef ROGUE_SUPPORT_QUICK_SAVE
    if(gRogueLocal.hasQuickLoadPending)
    {
        gRogueLocal.hasQuickLoadPending = FALSE;
        ScriptContext1_SetupScript(Rogue_QuickSaveLoad);
        return TRUE;
    }

#endif
    return FALSE;
}

void Rogue_OnLoadMap(void)
{
    if(GetSafariZoneFlag())
    {
        RandomiseSafariWildEncounters();
    }
}

static u16 GetBossRoomForDifficulty(u16 difficulty)
{
    u16 gymSpacing = 3;
    u16 eliteFourSpacing = 3;
    u16 championSpacing = 4;

    u16 roomIndex = 0;

#ifdef ROGUE_DEBUG
    //gymSpacing = 1;
    //eliteFourSpacing = 1;
    //championSpacing = 1;
#endif

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        roomIndex = 5;
        gymSpacing = 1;
        eliteFourSpacing = 1;
        championSpacing = 1;
    }

    // 0-7 gym leaders
    {
        roomIndex += gymSpacing * (min(difficulty, 7) + 1);
    }

    // 8 - 11 elite four
    if(difficulty >= 8)
    {
        roomIndex += eliteFourSpacing * (min(difficulty, 11) - 8 + 1);
    }

    // 12 - 13 champion
    if(difficulty >= 12)
    {
        roomIndex += championSpacing * (min(difficulty, 13) - 12 + 1);
    }

    return roomIndex;
}

static u16 GetStartDifficulty(void)
{
    u16 skipToDifficulty = VarGet(VAR_ROGUE_SKIP_TO_DIFFICULTY);

    if(skipToDifficulty != 0)
    {
        return skipToDifficulty;
    }
    
    return 0;
}

static u16 GetStartRoomIdx(void)
{
    u16 skipToDifficulty = VarGet(VAR_ROGUE_SKIP_TO_DIFFICULTY);

    if(skipToDifficulty != 0)
    {
        return GetBossRoomForDifficulty(skipToDifficulty - 1);
    }
    
    return 0;
}

static u16 GetStartRestStopRoomIdx(void)
{
    u16 skipToDifficulty = VarGet(VAR_ROGUE_SKIP_TO_DIFFICULTY);

    if(skipToDifficulty != 0)
    {
        return GetBossRoomForDifficulty(skipToDifficulty);
    }
    
    return GetBossRoomForDifficulty(0);
}

static void BeginRogueRun(void)
{
    memset(&gRogueLocal, 0, sizeof(gRogueLocal));

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    if(FlagGet(FLAG_SET_SEED_ENABLED))
    {
        gRngRogueValue = Rogue_GetSeed();
    }

    ClearBerryTrees();

    gRogueRun.currentRoomIdx = GetStartRoomIdx();
    gRogueRun.specialEncounterCounter = 0;
    gRogueRun.nextRestStopRoomIdx = GetStartRestStopRoomIdx();
    gRogueRun.currentRouteIndex = 0;

    memset(&gRogueRun.routeHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueRun.routeHistoryBuffer));
    memset(&gRogueRun.wildEncounterHistoryBuffer[0], 0, sizeof(u16) * ARRAY_COUNT(gRogueRun.routeHistoryBuffer));
    
    VarSet(VAR_ROGUE_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, 0);
    VarSet(VAR_ROGUE_REWARD_MONEY, 0);
    VarSet(VAR_ROGUE_REWARD_CANDY, 0);
    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
    
    SaveHubInventory();

    gRogueHubData.money = GetMoney(&gSaveBlock1Ptr->money);
    gRogueHubData.registeredItem = gSaveBlock1Ptr->registeredItem;

    gRogueHubData.playTimeHours = gSaveBlock2Ptr->playTimeHours;
    gRogueHubData.playTimeMinutes = gSaveBlock2Ptr->playTimeMinutes;
    gRogueHubData.playTimeSeconds = gSaveBlock2Ptr->playTimeSeconds;
    gRogueHubData.playTimeVBlanks = gSaveBlock2Ptr->playTimeVBlanks;

    gSaveBlock2Ptr->playTimeHours = 0;
    gSaveBlock2Ptr->playTimeMinutes = 0;
    gSaveBlock2Ptr->playTimeSeconds = 0;
    gSaveBlock2Ptr->playTimeVBlanks = 0;

    SetMoney(&gSaveBlock1Ptr->money, VarGet(VAR_ROGUE_ADVENTURE_MONEY));

    FlagClear(FLAG_BADGE01_GET);
    FlagClear(FLAG_BADGE02_GET);
    FlagClear(FLAG_BADGE03_GET);
    FlagClear(FLAG_BADGE04_GET);
    FlagClear(FLAG_BADGE05_GET);
    FlagClear(FLAG_BADGE06_GET);
    FlagClear(FLAG_BADGE07_GET);
    FlagClear(FLAG_BADGE08_GET);

    FlagClear(FLAG_ROGUE_DEFEATED_BOSS00);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS01);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS02);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS03);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS04);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS05);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS06);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS07);
    
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS08);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS09);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS10);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS11);

    FlagClear(FLAG_ROGUE_DEFEATED_BOSS12);
    FlagClear(FLAG_ROGUE_DEFEATED_BOSS13);

#ifdef ROGUE_DEBUG
    // TEMP - Testing only
    //gRogueRun.currentRoomIdx = GetBossRoomForDifficulty(13) - 1;
    //gRogueRun.nextRestStopRoomIdx = GetBossRoomForDifficulty(13);

    //gRogueRun.currentRouteIndex = 7;
#endif
}

static void EndRogueRun(void)
{
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_SET_SEED_ENABLED);
    //gRogueRun.currentRoomIdx = 0;

    // Restore money and give reward here too, as it's a bit easier
    SetMoney(&gSaveBlock1Ptr->money, gRogueHubData.money);
    AddMoney(&gSaveBlock1Ptr->money, VarGet(VAR_ROGUE_REWARD_MONEY));

    gSaveBlock1Ptr->registeredItem = gRogueHubData.registeredItem;

    
    //gRogueHubData.playTimeHours += gSaveBlock2Ptr->playTimeHours;
    //gRogueHubData.playTimeMinutes += gSaveBlock2Ptr->playTimeMinutes;
    //gRogueHubData.playTimeSeconds += gSaveBlock2Ptr->playTimeSeconds;
    //gRogueHubData.playTimeVBlanks += gSaveBlock2Ptr->playTimeVBlanks;

    gSaveBlock2Ptr->playTimeHours += gRogueHubData.playTimeHours;
    gSaveBlock2Ptr->playTimeMinutes += gRogueHubData.playTimeMinutes;
    gSaveBlock2Ptr->playTimeSeconds += gRogueHubData.playTimeSeconds + (gRogueHubData.playTimeVBlanks / 60);
    gSaveBlock2Ptr->playTimeVBlanks += gRogueHubData.playTimeVBlanks;

    if(gSaveBlock2Ptr->playTimeVBlanks > 60)
    {
        gSaveBlock2Ptr->playTimeSeconds += gSaveBlock2Ptr->playTimeVBlanks / 60;
        gSaveBlock2Ptr->playTimeVBlanks = gSaveBlock2Ptr->playTimeVBlanks % 60;
    }

    if(gSaveBlock2Ptr->playTimeSeconds > 60)
    {
        gSaveBlock2Ptr->playTimeMinutes += gSaveBlock2Ptr->playTimeSeconds / 60;
        gSaveBlock2Ptr->playTimeSeconds = gSaveBlock2Ptr->playTimeSeconds % 60;
    }

    if(gSaveBlock2Ptr->playTimeMinutes > 60)
    {
        gSaveBlock2Ptr->playTimeHours += gSaveBlock2Ptr->playTimeMinutes / 60;
        gSaveBlock2Ptr->playTimeMinutes = gSaveBlock2Ptr->playTimeMinutes % 60;
    }

    LoadHubInventory();
}

static bool8 IsBossRoom(u16 roomIdx)
{
    u8 i;
    for(i = 0; i < BOSS_ROOM_COUNT; ++i)
    {
        if(GetBossRoomForDifficulty(i) == roomIdx)
            return TRUE;
    }

    return FALSE;
}

static bool8 IsSpecialEncounterRoomWarp(struct WarpData *warp)
{
    // Just an arbitrary number we're gonna use to indicate special encounter rooms
    return warp->warpId == 55;
}

static u8 GetDifficultyLevel(u16 roomIdx)
{
    u8 i;
    for(i = 0; i < BOSS_ROOM_COUNT; ++i)
    {
        if(roomIdx <= GetBossRoomForDifficulty(i))
            return i;
    }

    return BOSS_ROOM_COUNT - 1;
}

static void SelectBossRoom(u16 nextRoomIdx, struct WarpData *warp)
{
    u8 bossId = 0;
    u8 difficulty = GetDifficultyLevel(nextRoomIdx);

    do
    {
        // Gym leaders 0-7
        if(difficulty <= 7)
        {
            bossId = RogueRandomRange(8, OVERWORLD_FLAG);
        }
        // Elite Four 8-11
        else if(difficulty <= 11)
        {
            bossId = 8 + RogueRandomRange(4, OVERWORLD_FLAG);
        }
        // Champion 1
        else if(difficulty == 12) 
        {
            bossId = 12;
            break;
        }
        // Champion 2
        else
        {
            bossId = 13;
            break;
        }
    }
    while(FlagGet(FLAG_ROGUE_DEFEATED_BOSS00 + bossId));

    switch(bossId)
    {
        case 0:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_0);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_0);
            break;

        case 1:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_1);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_1);
            break;

        case 2:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_2);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_2);
            break;

        case 3:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_3);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_3);
            break;

        case 4:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_4);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_4);
            break;

        case 5:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_5);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_5);
            break;

        case 6:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_6);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_6);
            break;

        case 7:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_7);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_7);
            break;


        case 8:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_8);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_8);
            break;

        case 9:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_9);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_9);
            break;

        case 10:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_10);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_10);
            break;

        case 11:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_11);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_11);
            break;


        case 12:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_12);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_12);
            break;

        case 13:
            warp->mapGroup = MAP_GROUP(ROGUE_BOSS_13);
            warp->mapNum = MAP_NUM(ROGUE_BOSS_13);
            break;
    };
}

static void SelectRouteRoom(u16 nextRoomIdx, struct WarpData *warp)
{
    u8 mapCount;
    u8 mapIdx;
    const struct RogueRouteMap* selectedMap = NULL;

    // Don't reply recent routes
    do
    {
        gRogueRun.currentRouteIndex = RogueRandomRange(ROGUE_ROUTE_COUNT, OVERWORLD_FLAG);
    }
    while(HistoryBufferContains(&gRogueRun.routeHistoryBuffer[0], ARRAY_COUNT(gRogueRun.routeHistoryBuffer), gRogueRun.currentRouteIndex));

    selectedMap = &gRogueRouteTable[gRogueRun.currentRouteIndex].map;
    HistoryBufferPush(&gRogueRun.routeHistoryBuffer[0], ARRAY_COUNT(gRogueRun.routeHistoryBuffer), gRogueRun.currentRouteIndex);

    warp->mapGroup = selectedMap->group;
    warp->mapNum = selectedMap->num;
}

static void ResetSpecialEncounterStates(void)
{
    // Special states
    // Rayquaza
    VarSet(VAR_SKY_PILLAR_STATE, 2); // Keep in clean layout, but act as is R is has left for G/K cutscene
    VarSet(VAR_SKY_PILLAR_RAQUAZA_CRY_DONE, 1); // Hide cutscene R
    FlagClear(FLAG_DEFEATED_RAYQUAZA);
    FlagClear(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL); // Show battle
    FlagSet(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA); // Hide cutscene R

    // Groudon + Kyogre
    FlagClear(FLAG_DEFEATED_GROUDON);
    FlagClear(FLAG_DEFEATED_KYOGRE);

    // Mew
    FlagClear(FLAG_HIDE_MEW);
    FlagClear(FLAG_DEFEATED_MEW);
    FlagClear(FLAG_CAUGHT_MEW);

    // Deoxys
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);

    // Ho-oh + Lugia
    FlagClear(FLAG_CAUGHT_HO_OH);
    FlagClear(FLAG_CAUGHT_LUGIA);

    // Regis
    FlagClear(FLAG_DEFEATED_REGICE);
    FlagClear(FLAG_DEFEATED_REGISTEEL);
    FlagClear(FLAG_DEFEATED_REGIROCK);

    // Latis
    //FlagClear(FLAG_DEFEATED_LATIAS_OR_LATIOS);
    //FlagClear(FLAG_CAUGHT_LATIAS_OR_LATIOS);

    // Reset ledgendary encounters
    //FlagSet(FLAG_HIDE_SOUTHERN_ISLAND_EON_STONE);
}

static bool8 PartyContainsSpecies(struct Pokemon *party, u8 partyCount, u16 species)
{
    u8 i;
    u16 s;
    for(i = 0; i < partyCount; ++i)
    {
        s = GetMonData(&party[i], MON_DATA_SPECIES);

        if(s == species)
            return TRUE;
    }

    return FALSE;
}

static void SelectSpecialEncounterRoom(u16 nextRoomIdx, struct WarpData *warp)
{
    u8 mapCount;
    u8 mapIdx;
    u8 swapRouteChance = 60;
    const struct RogueEncounterMap* selectedMap = NULL;
    
    ResetSpecialEncounterStates();

    mapCount = gRogueSpecialEncounterInfo.mapCount;

    // Avoid repeating same encounter (Base of current party)
    do
    {
        // Special encounters are NOT seeded
        mapIdx = Random() % mapCount;
        selectedMap = &gRogueSpecialEncounterInfo.mapTable[mapIdx];
    }
    while(mapCount > 6 && (!IsGenEnabled(SpeciesToGen(selectedMap->encounterSpecies)) || PartyContainsSpecies(&gPlayerParty[0], gPlayerPartyCount, selectedMap->encounterSpecies)));

    warp->mapGroup = selectedMap->group;
    warp->mapNum = selectedMap->num;
}

static bool8 IsSpecialEncounterRoom(void)
{
    u8 i;
    u8 mapCount = gRogueSpecialEncounterInfo.mapCount;

    for(i = 0; i < mapCount; ++i)
    {
        if(gRogueSpecialEncounterInfo.mapTable[i].layout == gMapHeader.mapLayoutId)
            return TRUE;
    }

    return FALSE;
}

static u8 CalcSpecialEncounterChance(u8 difficultyLevel)
{
#ifdef ROGUE_DEBUG
    return 100;
#else
    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        return 33;
    }

    if(difficultyLevel == 0)
    {
        return 0;
    }
    else if(difficultyLevel < 3)
    {
        // Once we have a badge there is small chance
        return 2;
    }
    else
    {
        if(gRogueRun.specialEncounterCounter <= 1)
        {
            return 0;
        }
        else if(gRogueRun.specialEncounterCounter >= 5)
        {
            return 33;
        }
        else if(gRogueRun.specialEncounterCounter >= 15)
        {
            // Very unlucky :(
            return 80;
        }
        else
        {
            return 8;
        }
    }
#endif
}

void Rogue_OnWarpIntoMap(void)
{
    u8 difficultyLevel;

    if(IsMegaEvolutionEnabled() || IsZMovesEnabled() || IsDynamaxEnabled())
    {
        FlagClear(FLAG_ROGUE_RARE_ITEM_MART_DISABLED);
    }
    else
    {
        FlagSet(FLAG_ROGUE_RARE_ITEM_MART_DISABLED);
    }

    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB_TRANSITION)
    {
        if(!Rogue_IsRunActive())
        {
            BeginRogueRun();
        }
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB && Rogue_IsRunActive())
    {
        EndRogueRun();
    }
    else if(Rogue_IsRunActive())
    {
        if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_ENCOUNTER_REST_STOP)
        {
            RandomiseEnabledTrainers();
            FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
        }
        else if(IsSpecialEncounterRoom())
        {
            gRogueRun.specialEncounterCounter = 0;
            FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
        }
        else
        {
            ++gRogueRun.currentRoomIdx;
            ++gRogueRun.specialEncounterCounter;
            difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

            if(RandomChance(CalcSpecialEncounterChance(difficultyLevel), OVERWORLD_FLAG))
                FlagSet(FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE);
            else
                FlagClear(FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE);

            // Ensure we have all badges by this point
            if(difficultyLevel >= 8)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagSet(FLAG_BADGE07_GET);
                FlagSet(FLAG_BADGE08_GET);
            }
            
            // Update VARs
            VarSet(VAR_ROGUE_DIFFICULTY, difficultyLevel);
            VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, max(difficultyLevel, VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY)));
            VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, gRogueRun.currentRoomIdx);

            if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
            {
                if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
                {
                    // Pretty much max difficulty
                    VarSet(VAR_ROGUE_REWARD_MONEY, (gRogueRun.currentRoomIdx - GetStartRoomIdx()) * 500);
                    VarSet(VAR_ROGUE_REWARD_CANDY, (difficultyLevel - GetStartDifficulty()) + 1);
                }
                else
                {
                    VarSet(VAR_ROGUE_REWARD_MONEY, (gRogueRun.currentRoomIdx - GetStartRoomIdx()) * 400);
                    VarSet(VAR_ROGUE_REWARD_CANDY, (difficultyLevel - GetStartDifficulty()) + 1);
                }
            }
            else
            {
                VarSet(VAR_ROGUE_REWARD_MONEY, (gRogueRun.currentRoomIdx - GetStartRoomIdx()) * 300);
                VarSet(VAR_ROGUE_REWARD_CANDY, (difficultyLevel - GetStartDifficulty()));
            }
        
            if(IsBossRoom(gRogueRun.currentRoomIdx))
            {
                ResetTrainerBattles();
                RandomiseEnabledItems();

                // Weather
                if(difficultyLevel == 0 || FlagGet(FLAG_ROGUE_EASY_TRAINERS))
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS) || difficultyLevel > 2)
                {
                    FlagSet(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
            }
            else
            {
                RandomiseWildEncounters();
                ResetTrainerBattles();
                RandomiseEnabledTrainers();
                RandomiseEnabledItems();
                RandomiseBerryTrees();

                if(difficultyLevel != 0 && RandomChance(20, OVERWORLD_FLAG))
                {
                    FlagSet(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
            }
        }
    }
    else if(GetSafariZoneFlag())
    {
        RandomiseSafariWildEncounters();
    }
}


void Rogue_OnSetWarpData(struct WarpData *warp)
{
    if(warp->mapGroup == MAP_GROUP(ROGUE_HUB) && warp->mapNum == MAP_NUM(ROGUE_HUB))
    {
        // Warping back to hub must be intentional
        return;
    }

    if(Rogue_IsRunActive())
    {
        u16 nextRoomIdx = gRogueRun.currentRoomIdx + 1;

        if(IsSpecialEncounterRoomWarp(warp))
        {
            SelectSpecialEncounterRoom(nextRoomIdx, warp);
        }
        else if(!IsBossRoom(gRogueRun.currentRoomIdx) && nextRoomIdx >= gRogueRun.nextRestStopRoomIdx) // If we just came from a boss room give us an extra room of space
        {
            // We're about to hit a rest stop so force it here
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_REST_STOP);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_REST_STOP);

            // Will encounter the next rest stop in 4-6 rooms
            gRogueRun.nextRestStopRoomIdx = nextRoomIdx + 4 + RogueRandomRange(3, OVERWORLD_FLAG);
            
            // We only get 1 rest stop at the begining
            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                gRogueRun.nextRestStopRoomIdx = 255;
            }
        }
        else if(IsBossRoom(nextRoomIdx))
        {
            SelectBossRoom(nextRoomIdx, warp);
        }
        else
        {
            SelectRouteRoom(nextRoomIdx, warp);
        }

        warp->warpId = 0;
        warp->x = -1;
        warp->y = -1;
    }
}

void RemoveAnyFaintedMons(bool8 keepItems)
{
    u8 read;
    u8 write = 0;
    u8 alivePartyCount = 0;

    for(read = 0; read < gPlayerPartyCount; ++read)
    {
        if(GetMonData(&gPlayerParty[read], MON_DATA_HP) != 0)
        {
            if(write != read)
                CopyMon(&gPlayerParty[write], &gPlayerParty[read], sizeof(gPlayerParty[read]));

            ++write;
            ++alivePartyCount;
        }
        else
        {
            if(keepItems)
            {
                // Dead so give back held item
                u16 heldItem = GetMonData(&gPlayerParty[read], MON_DATA_HELD_ITEM);
                if(heldItem != ITEM_NONE)
                    AddBagItem(heldItem, 1);
            }
        }
    }

    gPlayerPartyCount = alivePartyCount;

    for(read = gPlayerPartyCount; read < PARTY_SIZE; ++read)
        ZeroMonData(&gPlayerParty[read]);
}

void Rogue_Battle_StartTrainerBattle(void)
{
    if(FlagGet(FLAG_ROGUE_DOUBLE_BATTLES)) //NoOfApproachingTrainers != 2 
    {
        // No need to check opponent party as we force it to 2 below
        if(gPlayerPartyCount >= 2) // gEnemyPartyCount >= 2
        {
             // Force double?
            gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
        }
    }
}

static bool32 IsPlayerDefeated(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_LOST:
    case B_OUTCOME_DREW:
        return TRUE;
    case B_OUTCOME_WON:
    case B_OUTCOME_RAN:
    case B_OUTCOME_PLAYER_TELEPORTED:
    case B_OUTCOME_MON_FLED:
    case B_OUTCOME_CAUGHT:
        return FALSE;
    default:
        return FALSE;
    }
}

void Rogue_Battle_EndTrainerBattle(void)
{
    if(Rogue_IsRunActive())
    {
        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            RemoveAnyFaintedMons(FALSE);
        }
    }
}

void Rogue_Battle_EndWildBattle(void)
{
    if(Rogue_IsRunActive())
    {
        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            RemoveAnyFaintedMons(FALSE);
        }
    }
}

static bool8 IsBossTrainer(u16 trainerNum)
{
    switch(trainerNum)
    {
        case TRAINER_ROXANNE_1:
        case TRAINER_BRAWLY_1:
        case TRAINER_WATTSON_1:
        case TRAINER_FLANNERY_1:
        case TRAINER_NORMAN_1:
        case TRAINER_WINONA_1:
        case TRAINER_TATE_AND_LIZA_1:
        case TRAINER_JUAN_1:

        case TRAINER_SIDNEY:
        case TRAINER_PHOEBE:
        case TRAINER_GLACIA:
        case TRAINER_DRAKE:

        case TRAINER_WALLACE:
        case TRAINER_STEVEN:
            return TRUE;
    };

    return FALSE;
}

static bool8 UseCompetitiveMoveset(u16 trainerNum, u8 monIdx, u8 totalMonCount)
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    if(FlagGet(FLAG_ROGUE_EASY_TRAINERS))
    {
        return FALSE;
    }
    else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
    {
        if(difficultyLevel == 0) // Last mon has competitive set
            return IsBossTrainer(trainerNum) && monIdx == (totalMonCount - 1);
        else if(difficultyLevel == 1)
            return IsBossTrainer(trainerNum);
        else
            return TRUE;
    }
    else
    {
        // Start using competitive movesets on 3rd gym
        if(IsBossTrainer(trainerNum))
        {
            if(difficultyLevel == 0)
                return FALSE;
            else if(difficultyLevel == 1) // Last mon has competitive set
                return monIdx == (totalMonCount - 1);
            else
                return TRUE;
        }

        return FALSE;
    }
}

static void SeedRogueTrainer(u16 seed, u16 trainerNum, u16 offset)
{
    gRngRogueValue = seed + trainerNum * 3 + offset * 7;
}

static void ConfigureTrainer(u16 trainerNum, u8* forceType, bool8* allowItemEvos, bool8* allowLedgendaries, u8* monsCount)
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    switch(trainerNum)
    {
        case TRAINER_ROXANNE_1:
            *forceType = TYPE_ROCK;
            break;
        case TRAINER_BRAWLY_1:
            *forceType = TYPE_FIGHTING;
            break;
        case TRAINER_WATTSON_1:
            *forceType = TYPE_ELECTRIC;
            break;
        case TRAINER_FLANNERY_1:
            *forceType = TYPE_FIRE;
            break;
        case TRAINER_NORMAN_1:
            *forceType = TYPE_NORMAL;
            break;
        case TRAINER_WINONA_1:
            *forceType = TYPE_FLYING;
            break;
        case TRAINER_TATE_AND_LIZA_1:
            *forceType = TYPE_PSYCHIC;
            break;
        case TRAINER_JUAN_1:
            *forceType = TYPE_WATER;
            break;

        case TRAINER_SIDNEY:
            *forceType = TYPE_DARK;
            break;
        case TRAINER_PHOEBE:
            *forceType = TYPE_GHOST;
            break;
        case TRAINER_GLACIA:
            *forceType = TYPE_ICE;
            break;
        case TRAINER_DRAKE:
            *forceType = TYPE_DRAGON;
            break;

        case TRAINER_WALLACE:
            *forceType = TYPE_WATER;
            break;
        case TRAINER_STEVEN:
            *forceType = TYPE_STEEL;
            break;

        case TRAINER_ROGUE_AQUA_F:
        case TRAINER_ROGUE_AQUA_M:
            forceType[0] = TYPE_DARK;

            if(difficultyLevel > 0)
                forceType[1] = TYPE_WATER;
            break;

        case TRAINER_ROGUE_MAGMA_F:
        case TRAINER_ROGUE_MAGMA_M:
            forceType[0] = TYPE_DARK;

            if(difficultyLevel > 0)
                forceType[1] = TYPE_FIRE;
            break;
    };

    if(IsBossTrainer(trainerNum))
    {
        if(difficultyLevel == 0)
        {
            *monsCount = 3;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 1)
        {
            *monsCount = 4;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 2)
        {
            *monsCount = 4;
            *allowItemEvos = FlagGet(FLAG_ROGUE_HARD_TRAINERS);
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 5)
        {
            *monsCount = 5;
            *allowItemEvos = TRUE;
            *allowLedgendaries = FlagGet(FLAG_ROGUE_HARD_TRAINERS);
        }
        else if(difficultyLevel <= 6) 
        {
            *monsCount = 6;
            *allowItemEvos = TRUE;
            *allowLedgendaries = FlagGet(FLAG_ROGUE_HARD_TRAINERS);
        }
        else // From last gym leader we can see ledgendaries
        {
            *monsCount = 6;
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
    }
    else
    {
        if(difficultyLevel == 0)
        {
            *monsCount = 1 + RogueRandomRange(2, FLAG_SET_SEED_TRAINERS);
            *forceType = TYPE_NORMAL;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 1)
        {
            *monsCount = 1 + RogueRandomRange(3, FLAG_SET_SEED_TRAINERS);
            *forceType = TYPE_NORMAL;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 2)
        {
            *monsCount = 2 + RogueRandomRange(2, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 7)
        {
            *monsCount = 2 + RogueRandomRange(3, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 11)
        {
            // Elite 4
            *monsCount = 3 + RogueRandomRange(3, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
        else
        {
            // Champion
            *monsCount = 4 + (RogueRandomRange(5, FLAG_SET_SEED_TRAINERS) == 0 ? 1 : 0);
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
    }

    if(FlagGet(FLAG_ROGUE_DOUBLE_BATTLES)) 
    {
        if(*monsCount < 2)
        {
            *monsCount = 2;
        }
    }

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        *monsCount = 6;
        *allowItemEvos = TRUE;
        *allowLedgendaries = TRUE;
    }
}

bool8 Rogue_OverrideTrainerItems(u16* items)
{
    if(Rogue_IsRunActive())
    {
        u8 i;

        for (i = 0; i < MAX_TRAINER_ITEMS; i++)
        {
            items[i] = ITEM_NONE;
        }

        return TRUE;
    }

    return FALSE;
}

static void ApplyTrainerQuery(u16 trainerNum)
{
    // Query for the current trainer team
    RogueQuery_Clear();

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesExcludeCommon();

    if(!gRogueLocal.trainerTemp.allowLedgendaries)
        RogueQuery_SpeciesIsNotLegendary();

    if(gRogueLocal.trainerTemp.allowedType[0] != TYPE_NONE)
    {
        if(gRogueLocal.trainerTemp.allowedType[1] != TYPE_NONE)
            RogueQuery_SpeciesOfTypes(&gRogueLocal.trainerTemp.allowedType[0], 2); // 2 types
        else
            RogueQuery_SpeciesOfType(gRogueLocal.trainerTemp.allowedType[0]); // 1 type
    }

    RogueQuery_TransformToEggSpecies();

    // Evolve the species to just below the wild encounter level
    RogueQuery_EvolveSpeciesToLevel(CalculateTrainerLevel(trainerNum));
    
    if(gRogueLocal.trainerTemp.allowItemEvos)
        RogueQuery_EvolveSpeciesByItem();

    if(gRogueLocal.trainerTemp.allowedType[0] != TYPE_NONE)
    {
        if(gRogueLocal.trainerTemp.allowedType[1] != TYPE_NONE)
            RogueQuery_SpeciesOfTypes(&gRogueLocal.trainerTemp.allowedType[0], 2); // 2 types
        else
            RogueQuery_SpeciesOfType(gRogueLocal.trainerTemp.allowedType[0]); // 1 type
    }

    RogueQuery_CollapseSpeciesBuffer();
}

static void ApplyFallbackTrainerQuery(u16 trainerNum)
{
    bool8 hasFallback = FALSE;

    switch(gRogueLocal.trainerTemp.allowedType[0])
    {
        case TYPE_DARK:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_FIGHTING;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_PSYCHIC;
            break;

        case TYPE_STEEL:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_GROUND;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_DRAGON;
            break;

        case TYPE_FIGHTING:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_ROCK;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_NONE;
            break;

        case TYPE_GHOST:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_POISON;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_NONE;
            break;

        case TYPE_DRAGON:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_FIRE;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_WATER;
            break;

        case TYPE_FIRE:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_GROUND;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_NONE;
            break;

        case TYPE_FLYING:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_NORMAL;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_NONE;
            break;

        case TYPE_ICE:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_WATER;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_NONE;
            break;
    }

    if(hasFallback && !gRogueLocal.trainerTemp.hasAppliedFallback)
    {
        gRogueLocal.trainerTemp.hasAppliedFallback = TRUE;
        ApplyTrainerQuery(trainerNum);
    }
}

void Rogue_PreCreateTrainerParty(u16 trainerNum, bool8* useRogueCreateMon, u8* monsCount)
{
    if(Rogue_IsRunActive())
    {
        // Reset trainer temp
        memset(&gRogueLocal.trainerTemp, 0, sizeof(gRogueLocal.trainerTemp));
        gRogueLocal.trainerTemp.seedToRestore = gRngRogueValue;

        gRogueLocal.trainerTemp.allowedType[0] = TYPE_NONE;
        gRogueLocal.trainerTemp.allowedType[1] = TYPE_NONE;

        SeedRogueTrainer(gRngRogueValue, trainerNum, RogueRandom() % 17);
        ConfigureTrainer(trainerNum, &gRogueLocal.trainerTemp.allowedType[0], &gRogueLocal.trainerTemp.allowItemEvos, &gRogueLocal.trainerTemp.allowLedgendaries, monsCount);

        ApplyTrainerQuery(trainerNum);

#ifdef ROGUE_DEBUG
        gDebug_TrainerOptionCount = RogueQuery_BufferSize();
#endif

        *useRogueCreateMon = TRUE;
        return;
    }

    *useRogueCreateMon = FALSE;
}

void Rogue_PostCreateTrainerParty(u16 trainerNum, struct Pokemon *party, u8 monsCount)
{
    //struct Pokemon tempMon;
    //CopyMon(&tempMon, party[0]);

    // TODO - Re-order team hear for best compat (e.g. mega evo later)

    gRngRogueValue = gRogueLocal.trainerTemp.seedToRestore;
}

static u16 NextTrainerSpecies(u16 trainerNum, bool8 isBoss, struct Pokemon *party, u8 monIdx, u8 totalMonCount)
{
    u16 species;
    u16 randIdx;
    u16 queryCount = RogueQuery_BufferSize();
    
    if(monIdx >= queryCount)
    {
        // Apply the fallback query (If we have one)
        // This will allow for secondary types if we've exhausted the primary one
        ApplyFallbackTrainerQuery(trainerNum);
    }

    // Prevent duplicates, if possible (Only for bosses)
    // *Only allow duplicates after we've already seen everything in the query
    do
    {
        randIdx = RogueRandomRange(queryCount, isBoss ? FLAG_SET_SEED_BOSSES : FLAG_SET_SEED_TRAINERS);
        species = RogueQuery_BufferPtr()[randIdx];
    }
    while(isBoss && PartyContainsSpecies(party, monIdx, species) && monIdx < queryCount);

    return species;
}

#ifdef ROGUE_EXPANSION
extern const struct LevelUpMove *const gLevelUpLearnsets[];
#else
extern const u16 *const gLevelUpLearnsets[];
#endif

bool8 CanLearnMoveByLvl(u16 species, u16 move, s32 level)
{
    u16 eggSpecies;
    s32 i;

#ifdef ROGUE_EXPANSION
    for (i = 0; gLevelUpLearnsets[species][i].move != LEVEL_UP_END; i++)
    {
        u16 moveLevel;

        if(move == gLevelUpLearnsets[species][i].move)
        {
            moveLevel = gLevelUpLearnsets[species][i].level;
#else
    for (i = 0; gLevelUpLearnsets[species][i] != LEVEL_UP_END; i++)
    {
        u16 moveLevel;

        if(move == (gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_ID))
        {
            moveLevel = (gLevelUpLearnsets[species][i] & LEVEL_UP_MOVE_LV);
#endif

            if (moveLevel > (level << 9))
                return FALSE;

            return TRUE;
        }
    }

    eggSpecies = Rogue_GetEggSpecies(species);

    if(eggSpecies == species)
    {
        // Must be taught by some other means
        return TRUE;
    }
    else
    {
        // Check if we would've learnt this before evolving (not 100% as can skip middle exclusive learn moves)
        return CanLearnMoveByLvl(eggSpecies, move, level);
    }
}

static bool8 SelectNextPreset(u16 species, u16 randFlag, struct RogueMonPreset* outPreset)
{
    u8 randOffset;
    u8 i;
    bool8 isPresetValid;
    u8 presetCount = gPresetMonTable[species].presetCount;

    if(presetCount != 0)
    {
        const struct RogueMonPreset* currPreset;
        randOffset = RogueRandomRange(presetCount, randFlag);

        // Work from random offset and attempt to find the best preset which slots into this team
        // If none is found, we will use the last option and adjust below
        for(i = 0; i < presetCount; ++i)
        {
            currPreset = &gPresetMonTable[species].presets[((randOffset + i) % presetCount)];
            isPresetValid = TRUE;

            if(currPreset->heldItem == ITEM_LEFTOVERS && gRogueLocal.trainerTemp.hasUsedLeftovers)
            {
                isPresetValid = FALSE;
            }

            if(currPreset->heldItem == ITEM_SHELL_BELL && gRogueLocal.trainerTemp.hasUsedShellbell)
            {
                isPresetValid = FALSE;
            }

            if(isPresetValid)
            {
                break;
            }
        }

        memcpy(outPreset, currPreset, sizeof(struct RogueMonPreset));

        // Swap out limited count items, if they already exist
        if(!isPresetValid)
        {
            if(outPreset->heldItem == ITEM_LEFTOVERS && gRogueLocal.trainerTemp.hasUsedLeftovers)
            {
                // Swap left overs to shell bell
                outPreset->heldItem = ITEM_SHELL_BELL;
            }

            if(outPreset->heldItem == ITEM_SHELL_BELL && gRogueLocal.trainerTemp.hasUsedShellbell)
            {
                // Swap shell bell to NONE (i.e. berry)
                outPreset->heldItem = ITEM_NONE;
            }
        }

        if(outPreset->heldItem == ITEM_NONE)
        {
            // Swap empty item to a berry either lum or sitrus
            outPreset->heldItem = RogueRandomRange(2, randFlag) == 0 ? ITEM_LUM_BERRY : ITEM_SITRUS_BERRY;
        }
        else if(outPreset->heldItem == ITEM_LEFTOVERS)
        {
            gRogueLocal.trainerTemp.hasUsedLeftovers = TRUE;
        }
        else if(outPreset->heldItem == ITEM_SHELL_BELL)
        {
            gRogueLocal.trainerTemp.hasUsedShellbell = TRUE;
        }

        return TRUE;
    }

    return FALSE;
}


void Rogue_CreateTrainerMon(u16 trainerNum, struct Pokemon *party, u8 monIdx, u8 totalMonCount)
{
    u16 species;
    u8 level;
    u8 fixedIV;
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    bool8 isBoss = IsBossTrainer(trainerNum);
    struct Pokemon *mon = &party[monIdx];

    // For wallace will will always have at least 1 ledgendary in last slot
    if(trainerNum == TRAINER_WALLACE)
    {
        if(monIdx == 5)
        {
            // Requery with just legendaries
            RogueQuery_Clear();
            RogueQuery_SpeciesIsValid();
            RogueQuery_SpeciesExcludeCommon();
            RogueQuery_SpeciesIsLegendary();
            RogueQuery_SpeciesOfType(TYPE_WATER);
            RogueQuery_CollapseSpeciesBuffer();
        }
    }
    // For steven will will always have at least 2 ledgendarys, both in last 2 slots
    else if(trainerNum == TRAINER_STEVEN)
    {
        if(monIdx == 4)
        {
            // Requery with just legendaries
            RogueQuery_Clear();
            RogueQuery_SpeciesIsValid();
            RogueQuery_SpeciesExcludeCommon();
            RogueQuery_SpeciesIsLegendary();
            RogueQuery_SpeciesOfType(TYPE_PSYCHIC);
            RogueQuery_CollapseSpeciesBuffer();
        }
    }

    species = NextTrainerSpecies(trainerNum, isBoss, party, monIdx, totalMonCount);
    level = CalculateTrainerLevel(trainerNum);

#ifdef ROGUE_DEBUG
    level = 5;
#endif

    if(FlagGet(FLAG_ROGUE_EASY_TRAINERS))
        fixedIV = 0;
    if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
        fixedIV = MAX_PER_STAT_IVS;
    else
        fixedIV = isBoss ? 11 : 0;

    if(!FlagGet(FLAG_ROGUE_HARD_TRAINERS) && (!isBoss || difficultyLevel < 4))
    {
        // Team average is something like -2, -1, -1, 0
        level--;

        if(monIdx == 0)
            level--;

        if(level != 100 && monIdx == totalMonCount - 1)
            level++;
    }

    CreateMon(mon, species, level, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    if(UseCompetitiveMoveset(trainerNum, monIdx, totalMonCount))
    {
        u8 i;
        u16 move;
        u16 heldItem;
        u8 writeMoveIdx;
        struct RogueMonPreset preset;
        bool8 useMaxHappiness = TRUE;

        // We want to start writing the move from the first free slot and loop back around
        for (writeMoveIdx = 0; writeMoveIdx < MAX_MON_MOVES; writeMoveIdx++)
        {
            move = GetMonData(mon, MON_DATA_MOVE1 + i);
            if(move == MOVE_NONE)
                break;
        }

        // Loop incase we already have 4 moves
        writeMoveIdx = writeMoveIdx % MAX_MON_MOVES;

        if(SelectNextPreset(species, isBoss ? FLAG_SET_SEED_BOSSES : FLAG_SET_SEED_TRAINERS, &preset))
        {
            if(preset.abilityNum != ABILITY_NONE)
            {
                SetMonData(mon, MON_DATA_ABILITY_NUM, &preset.abilityNum);
            }

            if(preset.heldItem != ITEM_NONE)
            {
                SetMonData(mon, MON_DATA_HELD_ITEM, &preset.heldItem);
            }

            // Teach moves from set that we can learn at this lvl
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                move = preset.moves[i]; 

                if(move != MOVE_NONE && CanLearnMoveByLvl(species, move, level))
                {
                    if(move == MOVE_FRUSTRATION)
                        useMaxHappiness = FALSE;

                    SetMonData(mon, MON_DATA_MOVE1 + writeMoveIdx, &move);
                    SetMonData(mon, MON_DATA_PP1 + writeMoveIdx, &gBattleMoves[move].pp);

                    // Loop back round
                    writeMoveIdx = (writeMoveIdx + 1) % MAX_MON_MOVES;
                }
            }
        }

        move = useMaxHappiness ? MAX_FRIENDSHIP : 0;
        SetMonData(mon, MON_DATA_FRIENDSHIP, &move);
    }
}

void Rogue_CreateWildMon(u8 area, u16* species, u8* level)
{
    // Note: Don't seed individual encounters
    if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
        u8 maxlevel = CalculateWildLevel();
        u8 levelVariation = min(6,maxlevel - 1);

        if(GetSafariZoneFlag())
        {
            levelVariation = min(3, maxlevel - 1);
        }

        if(area == 1) //WILD_AREA_WATER)
        {
            const u16 count = ARRAY_COUNT(gRogueRun.fishingEncounters);
            u16 randIdx = Random() % count; 

            *species = gRogueRun.fishingEncounters[randIdx];
            *level = maxlevel - (Random() % levelVariation);
        }
        else
        {
            const u16 count = ARRAY_COUNT(gRogueRun.wildEncounters);
            u16 randIdx;
            
            do
            {
                // Prevent recent duplicates when on a run (Don't use this in safari mode though)
                randIdx = Random() % count; 
                *species = gRogueRun.wildEncounters[randIdx];
            }
            while(!GetSafariZoneFlag() && HistoryBufferContains(&gRogueRun.wildEncounterHistoryBuffer[0], ARRAY_COUNT(gRogueRun.wildEncounterHistoryBuffer), *species));

            HistoryBufferPush(&gRogueRun.wildEncounterHistoryBuffer[0], ARRAY_COUNT(gRogueRun.wildEncounterHistoryBuffer), *species);
            *level = maxlevel - (Random() % levelVariation);
        }
    }
}

void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId)
{
    *level = CalculateWildLevel();
} 

static bool8 ApplyRandomMartChanceCallback(u16 itemId, u16 chance)
{
    // Always use rogue random so this is seeded correctly
    u16 res = 1 + (RogueRandom() % 100);

    return res <= chance;
}

static void ApplyRandomMartChanceQuery(u16 chance)
{
    u32 startSeed = gRngRogueValue;

    if(chance >= 100)
        return;

    RogueQuery_CustomItems(ApplyRandomMartChanceCallback, chance);

    gRngRogueValue = startSeed;
}

const u16* Rogue_CreateMartContents(u16 itemCategory, u16* minSalePrice)
{
    u16 difficulty;
    
    if(Rogue_IsRunActive())
        difficulty = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    else
        difficulty = VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY);

    RogueQuery_Clear();
    RogueQuery_ItemsIsValid();
    RogueQuery_ItemsExcludeCommon();

    RogueQuery_ItemsNotInPocket(POCKET_KEY_ITEMS);
    RogueQuery_ItemsNotInPocket(POCKET_BERRIES);

#ifdef ROGUE_EXPANSION
    RogueQuery_ItemsExcludeRange(ITEM_SEA_INCENSE, ITEM_PURE_INCENSE);
    RogueQuery_ItemsExcludeRange(ITEM_FLAME_PLATE, ITEM_FAIRY_MEMORY);
#endif

    switch(itemCategory)
    {
        case ROGUE_SHOP_MEDICINE:
            RogueQuery_ItemsMedicine();

            RogueQuery_ItemsInPriceRange(10, 300 + difficulty * 400);

            if(difficulty < 4)
            {
                RogueQuery_Exclude(ITEM_FULL_HEAL);
            }

            RogueQuery_Exclude(ITEM_FRESH_WATER);
            RogueQuery_Exclude(ITEM_SODA_POP);
            RogueQuery_Exclude(ITEM_LEMONADE);
            RogueQuery_Exclude(ITEM_MOOMOO_MILK);
            RogueQuery_Exclude(ITEM_ENERGY_POWDER);
            RogueQuery_Exclude(ITEM_ENERGY_ROOT);
            RogueQuery_Exclude(ITEM_HEAL_POWDER);
            RogueQuery_Exclude(ITEM_LAVA_COOKIE);
            RogueQuery_Exclude(ITEM_BERRY_JUICE);
#ifdef ROGUE_EXPANSION
            RogueQuery_ItemsExcludeRange(ITEM_PEWTER_CRUNCHIES, ITEM_BIG_MALASADA);
#endif
            break;

        case ROGUE_SHOP_BALLS:
            RogueQuery_ItemsInPocket(POCKET_POKE_BALLS);

            if(difficulty <= 0)
            {
                RogueQuery_ItemsInPriceRange(10, 200);
            }
            else if(difficulty <= 1)
            {
                RogueQuery_ItemsInPriceRange(10, 600);
            }
            else if(difficulty <= 2)
            {
                RogueQuery_ItemsInPriceRange(10, 1000);
            }
            else if(difficulty >= 11)
            {
                RogueQuery_ItemsInPriceRange(10, 60000);
            }
            else //if(difficulty <= 3)
            {
                RogueQuery_ItemsInPriceRange(10, 2000);
            }

            RogueQuery_Exclude(ITEM_PREMIER_BALL);
            break;

        case ROGUE_SHOP_TMS:
            RogueQuery_ItemsInPocket(POCKET_TM_HM);
            RogueQuery_ItemsExcludeRange(ITEM_HM01, ITEM_HM08);

            RogueQuery_ItemsInPriceRange(10, 1000 + difficulty * 810);
            break;

        case ROGUE_SHOP_BATTLE_ENHANCERS:
            RogueQuery_ItemsBattleEnchancer();
            RogueQuery_ItemsNotRareHeldItem();
            RogueQuery_ItemsInPriceRange(10, 60000);
            
            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                // Do nothing
            }
            else if(Rogue_IsRunActive())
            {
            #ifdef ROGUE_EXPANSION
                if(difficulty <= 0)
                    ApplyRandomMartChanceQuery(5);
                else if(difficulty <= 3)
                    ApplyRandomMartChanceQuery(10);
                else if(difficulty <= 5)
                    ApplyRandomMartChanceQuery(25);
                else if(difficulty <= 7)
                    ApplyRandomMartChanceQuery(50);
                else
                    ApplyRandomMartChanceQuery(100);
            #else
                if(difficulty <= 0)
                    ApplyRandomMartChanceQuery(10);
                else if(difficulty <= 3)
                    ApplyRandomMartChanceQuery(20);
                else if(difficulty <= 7)
                    ApplyRandomMartChanceQuery(50);
                else
                    ApplyRandomMartChanceQuery(100);
            #endif
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 500;
            else
                *minSalePrice = 1000;

            break;

        case ROGUE_SHOP_HELD_ITEMS:
            RogueQuery_ItemsHeldItem();
            RogueQuery_ItemsNotRareHeldItem();
            RogueQuery_ItemsInPriceRange(10, 60000);

            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                // Do nothing
            }
            else if(Rogue_IsRunActive())
            {
            #ifdef ROGUE_EXPANSION
                if(difficulty <= 0)
                    ApplyRandomMartChanceQuery(5);
                else if(difficulty <= 3)
                    ApplyRandomMartChanceQuery(10);
                else if(difficulty <= 5)
                    ApplyRandomMartChanceQuery(25);
                else if(difficulty <= 7)
                    ApplyRandomMartChanceQuery(50);
                else
                    ApplyRandomMartChanceQuery(100);
            #else
                if(difficulty <= 0)
                    ApplyRandomMartChanceQuery(10);
                else if(difficulty <= 3)
                    ApplyRandomMartChanceQuery(20);
                else if(difficulty <= 7)
                    ApplyRandomMartChanceQuery(50);
                else
                    ApplyRandomMartChanceQuery(100);
            #endif
            }
            else if(difficulty <= 5)
            {
                // Remove contents 
                RogueQuery_ItemsInPriceRange(10, 11);
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 500;
            else
                *minSalePrice = 2000;
            break;

        case ROGUE_SHOP_RARE_HELD_ITEMS:
            RogueQuery_ItemsRareHeldItem();
            RogueQuery_ItemsInPriceRange(10, 60000);

            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                // Do nothing
            }
            else if(Rogue_IsRunActive())
            {
            #ifdef ROGUE_EXPANSION
                if(difficulty <= 0)
                    ApplyRandomMartChanceQuery(5);
                else if(difficulty <= 3)
                    ApplyRandomMartChanceQuery(10);
                else if(difficulty <= 5)
                    ApplyRandomMartChanceQuery(25);
                else if(difficulty <= 7)
                    ApplyRandomMartChanceQuery(50);
                else
                    ApplyRandomMartChanceQuery(100);
            #else
                if(difficulty <= 0)
                    ApplyRandomMartChanceQuery(10);
                else if(difficulty <= 3)
                    ApplyRandomMartChanceQuery(20);
                else if(difficulty <= 7)
                    ApplyRandomMartChanceQuery(50);
                else
                    ApplyRandomMartChanceQuery(100);
            #endif
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 1500;
            else
                *minSalePrice = 3000;
            break;
    };

    RogueQuery_CollapseItemBuffer();

    if(RogueQuery_BufferSize() == 0)
    {
        // If we don't have anything then just use this (Really unlucky to happen)
        RogueQuery_Include(ITEM_TINY_MUSHROOM);
        RogueQuery_CollapseItemBuffer();
    }

    return RogueQuery_BufferPtr();
}

static bool8 ContainsSpecies(u16 *party, u8 partyCount, u16 species)
{
    u8 i;
    u16 s;
    for(i = 0; i < partyCount; ++i)
    {
        s = party[i];

        if(s == species)
            return TRUE;
    }

    return FALSE;
}

static u16 NextWildSpecies(u16 * party, u8 monIdx)
{
    u16 species;
    u16 randIdx;
    u16 queryCount = RogueQuery_BufferSize();
    
    // Prevent duplicates, if possible
    do
    {
        randIdx = RogueRandomRange(queryCount, FLAG_SET_SEED_WILDMONS);
        species = RogueQuery_BufferPtr()[randIdx];
    }
    while(ContainsSpecies(party, monIdx, species) && monIdx < queryCount);

    return species;
}

static void RandomiseWildEncounters(void)
{
    u8 maxlevel = CalculateWildLevel();

    // Query for the current route type
    RogueQuery_Clear();

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesExcludeCommon();
    RogueQuery_SpeciesIsNotLegendary();
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteIndex].wildTypeTable, ARRAY_COUNT(gRogueRouteTable[gRogueRun.currentRouteIndex].wildTypeTable));
    RogueQuery_TransformToEggSpecies();

    // Evolve the species to just below the wild encounter level
    RogueQuery_EvolveSpeciesToLevel(maxlevel - min(6, maxlevel - 1));
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteIndex].wildTypeTable, ARRAY_COUNT(gRogueRouteTable[gRogueRun.currentRouteIndex].wildTypeTable));

    RogueQuery_CollapseSpeciesBuffer();

    {
        u8 i;

#ifdef ROGUE_DEBUG
        gDebug_WildOptionCount = RogueQuery_BufferSize();
#endif

        for(i = 0; i < ARRAY_COUNT(gRogueRun.wildEncounters); ++i)
        {
            gRogueRun.wildEncounters[i] = NextWildSpecies(&gRogueRun.wildEncounters[0], i);
        }
    }

    gRogueRun.fishingEncounters[0] = SPECIES_MAGIKARP;
    gRogueRun.fishingEncounters[1] = SPECIES_FEEBAS;
}

static void RogueQuery_SafariTypeForMap()
{
    if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_SOUTH)
    {
        u8 types[] =
        {
            TYPE_NORMAL, TYPE_FIGHTING
#ifdef ROGUE_EXPANSION
            ,TYPE_FAIRY
#endif
        };
        RogueQuery_SpeciesOfTypes(&types[0], ARRAY_COUNT(types));
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_SOUTHWEST)
    {
        u8 types[] =
        {
            TYPE_GRASS, TYPE_POISON, TYPE_DARK
        };
        RogueQuery_SpeciesOfTypes(&types[0], ARRAY_COUNT(types));
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTHWEST)
    {
        u8 types[] =
        {
            TYPE_DRAGON, TYPE_STEEL, TYPE_PSYCHIC
        };
        RogueQuery_SpeciesOfTypes(&types[0], ARRAY_COUNT(types));
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTH)
    {
        u8 types[] =
        {
            TYPE_FLYING, TYPE_GHOST, TYPE_FIRE
        };
        RogueQuery_SpeciesOfTypes(&types[0], ARRAY_COUNT(types));
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTHEAST)
    {
        u8 types[] =
        {
            TYPE_ROCK, TYPE_GROUND, TYPE_ELECTRIC
        };
        RogueQuery_SpeciesOfTypes(&types[0], ARRAY_COUNT(types));
    }
    else // SAFARI_ZONE_SOUTHEAST
    {
        u8 types[] =
        {
            TYPE_WATER, TYPE_BUG, TYPE_ICE
        };
        RogueQuery_SpeciesOfTypes(&types[0], ARRAY_COUNT(types));
    }
}

static void RandomiseSafariWildEncounters(void)
{
    u8 maxlevel = CalculateWildLevel();

    // Query for the current zone
    RogueQuery_Clear();
    RogueQuery_SpeciesIsValid();

    if(VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY) < 11)
    {
        // Once we've defeated the elite 4, we're going to allow legendaries for fun!
        RogueQuery_SpeciesIsNotLegendary();
    }

    RogueQuery_SpeciesInPokedex();

    RogueQuery_SafariTypeForMap();
    RogueQuery_TransformToEggSpecies();
    RogueQuery_SafariTypeForMap();

    RogueQuery_CollapseSpeciesBuffer();

    {
        u8 i;
        u16 randIdx;
        u16 queryCount = RogueQuery_BufferSize();

#ifdef ROGUE_DEBUG
        gDebug_WildOptionCount = queryCount;
#endif

        if(queryCount == 0)
        {
            for(i = 0; i < ARRAY_COUNT(gRogueRun.wildEncounters); ++i)
            {
                // Just encounter self, as we don't have a great fallback?
                gRogueRun.wildEncounters[i] = Rogue_GetEggSpecies(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES));
            }
        }
        else
        {
            for(i = 0; i < ARRAY_COUNT(gRogueRun.wildEncounters); ++i)
            {
                gRogueRun.wildEncounters[i] = NextWildSpecies(&gRogueRun.wildEncounters[0], i);
            }
        }
    }

    gRogueRun.fishingEncounters[0] = SPECIES_MAGIKARP;
    gRogueRun.fishingEncounters[1] = SPECIES_FEEBAS;
}

static void ResetTrainerBattles(void)
{
    // Reset trainer encounters
    s16 i;
    for(i = 0; i < TRAINERS_COUNT; ++i)
    {
        ClearTrainerFlag(i);
    }
}

u8 GetLeadMonLevel(void);

static u8 CalculateWildLevel(void)
{
    if(GetSafariZoneFlag())
    {
        if((Random() % 6) == 0)
        {
            // Occasionally throw in starter level mons
            return 7;
        }
        else
        {
            return GetLeadMonLevel();
        }
    }

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        // 5 rooms the constant badges
        return min(5 + (gRogueRun.currentRoomIdx - 1) * 10, MAX_LEVEL);
    }

    return CalculatePlayerLevel() - 7;
}

static u8 CalculateBossLevelForDifficulty(u8 difficulty)
{
    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        return MAX_LEVEL;
    }

    // Gym leaders lvs 15 -> 85
    if(difficulty <= 7)
    {
        return min(100, 15 + 10 * difficulty);
    }
    else
    {
        // Both champions are lvl 100
        difficulty -= 7; // (1 - 6)
        return min(100, 85 + 3 * difficulty);
    }
}

static u8 CalculateBossLevel()
{
    u8 currLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    return CalculateBossLevelForDifficulty(currLevel);
}

static u8 CalculatePlayerLevel(void)
{
    u8 prevLevel;
    u8 currLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        // 5 rooms the constant badges
        return min(15 + (gRogueRun.currentRoomIdx - 1) * 20, MAX_LEVEL);
    }

    if(currLevel == 0)
    {
        if(gRogueRun.currentRoomIdx <= 1)
        {
            // Just hard guessing this one
            return 10;
        }
        else
        {
            // Cannot do blending
            return CalculateBossLevelForDifficulty(currLevel);
        }
    }
    else
    {
        prevLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx - 1);
    }

    if(currLevel == prevLevel)
    {
        return CalculateBossLevelForDifficulty(currLevel);
    }
    else
    {
        // We've just transitioned so use midpoint
        return (CalculateBossLevelForDifficulty(prevLevel) + CalculateBossLevelForDifficulty(currLevel)) / 2;
    }
}

static u8 CalculateTrainerLevel(u16 trainerNum)
{
    if(IsBossTrainer(trainerNum))
    {
        return CalculatePlayerLevel();
    }
    else
    {
        u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

        if(difficultyLevel == 0)
        {
            return CalculatePlayerLevel() - 7;
        }
        else
        {
            if(difficultyLevel >= 12)
            {
                // Before champion gap is super small
                return CalculatePlayerLevel() - 5;
            }
            else
            {
                // Trainers will lag behind to make grinding easier
                return CalculatePlayerLevel() - 10;
            }
        }
    }
}

static bool8 RandomChanceTrainer()
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    u8 chance = max(10, 5 * difficultyLevel);

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        return 0;
    }

    return RandomChance(chance, FLAG_SET_SEED_TRAINERS);
}

static bool8 RandomChanceItem()
{
    u8 chance;

    if(IsBossRoom(gRogueRun.currentRoomIdx))
    {
        chance = 75;
    }
    else
    {
        u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
        
        if(FlagGet(FLAG_ROGUE_EASY_ITEMS))
        {
            chance = max(15, 75 - min(50, 4 * difficultyLevel));
        }
        else if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
        {
            chance = max(5, 30 - min(28, 2 * difficultyLevel));
        }
        else
        {
            chance = max(10, 55 - min(50, 4 * difficultyLevel));
        }
    }

    return RandomChance(chance, FLAG_SET_SEED_ITEMS);
}

static bool8 RandomChanceBerry()
{
    u8 chance;
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    if(FlagGet(FLAG_ROGUE_EASY_ITEMS))
    {
        chance = max(15, 95 - min(85, 7 * difficultyLevel));
    }
    else if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
    {
        chance = max(5, 50 - min(48, 5 * difficultyLevel));
    }
    else
    {
        chance = max(10, 70 - min(65, 5 * difficultyLevel));
    }

    return RandomChance(chance, FLAG_SET_SEED_ITEMS);
}

static void RandomiseEnabledTrainers(void)
{
    s32 i;
    for(i = 0; i < ROGUE_TRAINER_COUNT; ++i)
    {
        if(RandomChanceTrainer())
        {
            // Clear flag to show
            FlagClear(FLAG_ROGUE_TRAINER_START + i);
        }
        else
        {
            // Set flag to hide
            FlagSet(FLAG_ROGUE_TRAINER_START + i);
        }
    }
}

static void RandomiseItemContent(u8 difficultyLevel)
{
    u16 queryCount;
    u8 dropRarity = gRogueRouteTable[gRogueRun.currentRouteIndex].dropRarity;

    // Give us 1 room of basic items
    if(gRogueRun.currentRoomIdx > 1 && FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        dropRarity += 10;
    }

    // Queue up random items
    RogueQuery_Clear();

    RogueQuery_ItemsInPriceRange(50 + 100 * (difficultyLevel + dropRarity), 300 + 800 * (difficultyLevel + dropRarity));

    RogueQuery_ItemsIsValid();
    RogueQuery_ItemsNotInPocket(POCKET_KEY_ITEMS);
    RogueQuery_ItemsNotInPocket(POCKET_BERRIES);

    RogueQuery_ItemsExcludeCommon();

    if(IsBossRoom(gRogueRun.currentRoomIdx))
    {
        RogueQuery_ItemsMedicine();
    }

    if(!FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        if(difficultyLevel <= 1)
        {
            RogueQuery_ItemsNotInPocket(POCKET_TM_HM);
        }

        if(difficultyLevel <= 3)
        {
            RogueQuery_ItemsNotHeldItem();
            RogueQuery_ItemsNotRareHeldItem();

            //More item tweaks (removal of dynamax items)
        }
    }

    RogueQuery_CollapseItemBuffer();
    queryCount = RogueQuery_BufferSize();

#ifdef ROGUE_DEBUG
    gDebug_ItemOptionCount = queryCount;
#endif

    // These VARs aren't sequential
    VarSet(VAR_ROGUE_ITEM0, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM1, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM2, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM3, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM4, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM5, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM6, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM7, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM8, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM9, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM10, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM11, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM12, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM13, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM14, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM15, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM16, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM17, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
    VarSet(VAR_ROGUE_ITEM18, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
}

static void RandomiseEnabledItems(void)
{
    s32 i;
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
    {
        if(RandomChanceItem())
        {
            // Clear flag to show
            FlagClear(FLAG_ROGUE_ITEM_START + i);
        }
        else
        {
            // Set flag to hide
            FlagSet(FLAG_ROGUE_ITEM_START + i);
        }
    }

    RandomiseItemContent(difficultyLevel);
}

#define FIRST_USELESS_BERRY_INDEX ITEM_RAZZ_BERRY
#define LAST_USELESS_BERRY_INDEX  ITEM_BELUE_BERRY

// Ignore enigma berry as it's useless in gen 3
#define BERRY_COUNT (LAST_BERRY_INDEX - FIRST_BERRY_INDEX)

static void RandomiseBerryTrees(void)
{
    s32 i;

    for (i = 0; i < BERRY_TREES_COUNT; i++)
    {
        if(RandomChanceBerry())
        {
            u16 berryItem;
            u16 berry;

            do
            {
                berryItem = FIRST_BERRY_INDEX + RogueRandomRange(BERRY_COUNT, FLAG_SET_SEED_ITEMS);
            }
            while(berryItem >= FIRST_USELESS_BERRY_INDEX && berryItem <= LAST_USELESS_BERRY_INDEX);

            berry = ItemIdToBerryType(berryItem);
            PlantBerryTree(i, berry, BERRY_STAGE_BERRIES, FALSE);
        }
        else
        {
            RemoveBerryTree(i);
        }
    }
}

static void HistoryBufferPush(u16* buffer, u16 capacity, u16 value)
{
    u16 i;
    for(i = 1; i < capacity; ++i)
    {
        buffer[i] = buffer[i - 1];
    }

    buffer[0] = value;
}

static bool8 HistoryBufferContains(u16* buffer, u16 capacity, u16 value)
{
    u16 i;
    for(i = 0; i < capacity; ++i)
    {
        if(buffer[i] == value)
            return TRUE;
    }

    return FALSE;
}