#include "global.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/event_objects.h"
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
#include "main.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "safari_zone.h"
#include "script.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"

#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_query.h"

#define ROGUE_TRAINER_COUNT (FLAG_ROGUE_TRAINER_END - FLAG_ROGUE_TRAINER_START + 1)
#define ROGUE_ITEM_COUNT (FLAG_ROGUE_ITEM_END - FLAG_ROGUE_ITEM_START + 1)

// 8 badges, 4 elite, 2 champion
#define BOSS_ROOM_COUNT 14

#ifdef ROGUE_DEBUG
EWRAM_DATA u8 gDebug_CurrentTab = 0;
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

extern const u8 gText_RogueDebug_AdvHeader[];
extern const u8 gText_RogueDebug_AdvCount[];
extern const u8 gText_RogueDebug_X[];
extern const u8 gText_RogueDebug_Y[];
#endif

// Box save data
struct RogueBoxSaveData
{
    u32 encryptionKey;
    struct Pokemon playerParty[PARTY_SIZE];
    struct ItemSlot bagPocket_Items[BAG_ITEMS_COUNT];
    struct ItemSlot bagPocket_KeyItems[BAG_KEYITEMS_COUNT];
    struct ItemSlot bagPocket_PokeBalls[BAG_POKEBALLS_COUNT];
    struct ItemSlot bagPocket_TMHM[BAG_TMHM_COUNT];
    struct ItemSlot bagPocket_Berries[BAG_BERRIES_COUNT];
    
    struct RogueAdvPath advPath;
};

ROGUE_STATIC_ASSERT(sizeof(struct RogueBoxSaveData) <= sizeof(struct BoxPokemon) * LEFTOVER_BOXES_COUNT * IN_BOX_COUNT, RogueBoxSaveData);

struct RogueTrainerTemp
{
    u32 seedToRestore;
    u8 allowedType[2];
    u8 disallowedType[2];
    bool8 allowItemEvos;
    bool8 allowLedgendaries;
    bool8 hasAppliedFallback;
    bool8 hasUsedLeftovers;
    bool8 hasUsedShellbell;
#ifdef ROGUE_EXPANSION
    bool8 hasUsedMegaStone;
    bool8 hasUsedZMove;
#endif
};

struct RouteMonPreview
{
    u16 species;
    u8 monSpriteId;
    bool8 isVisible;
};

struct RogueLocalData
{
    bool8 hasQuickLoadPending;
    struct RogueTrainerTemp trainerTemp;
    struct RouteMonPreview encounterPreview[ARRAY_COUNT(gRogueRun.wildEncounters)];
    
    // We encode all our save data as box data :D
    union
    {
        struct BoxPokemon boxes[LEFTOVER_BOXES_COUNT][IN_BOX_COUNT];
        struct RogueBoxSaveData raw;
    } saveData;
};

EWRAM_DATA struct RogueLocalData gRogueLocal = {};
EWRAM_DATA struct RogueRunData gRogueRun = {};
EWRAM_DATA struct RogueHubData gRogueHubData = {};
EWRAM_DATA struct RogueAdvPath gRogueAdvPath = {};

static bool8 IsBossTrainer(u16 trainerNum);
static bool8 IsMiniBossTrainer(u16 trainerNum);

static u8 CalculateBossLevel(void);
static u8 CalculatePlayerLevel(void);
static u8 CalculateWildLevel(void);
static u8 CalculateTrainerLevel(u16 trainerNum);
static u8 GetRoomTypeDifficulty(void);

static bool8 CanLearnMoveByLvl(u16 species, u16 move, s32 level);
static void ApplyMonPreset(struct Pokemon* mon, u8 level, const struct RogueMonPreset* preset);

static u8 GetCurrentWildEncounterCount(void);

static void RandomiseSafariWildEncounters(void);
static void RandomiseWildEncounters(void);
static void ResetTrainerBattles(void);
static void RandomiseEnabledTrainers(void);
static void RandomiseEnabledItems(void);
static void RandomiseBerryTrees(void);

static void HistoryBufferPush(u16* buffer, u16 capacity, u16 value);
static bool8 HistoryBufferContains(u16* buffer, u16 capacity, u16 value);

u16 RogueRandomRange(u16 range, u8 flag)
{
    // Always use rogue random to avoid seeding issues based on flag
    u16 res = RogueRandom();

    if(FlagGet(FLAG_SET_SEED_ENABLED) && (flag == 0 || FlagGet(flag)))
        return res % range;
    else
        return Random() % range;
}

bool8 RogueRandomChance(u8 chance, u16 seedFlag)
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
    if(Rogue_IsRunActive() && 
        gRogueAdvPath.currentRoomType != ADVPATH_ROOM_BOSS && 
        //gRogueAdvPath.currentRoomType != ADVPATH_ROOM_LEGENDARY &&
        gRogueAdvPath.currentRoomType != ADVPATH_ROOM_MINIBOSS)
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
                if(currentLevel < targetLevel)
                {
                    s16 delta = targetLevel - currentLevel;
                    
                    // Level up immediatly to the targetLevel (As it's the soft cap and moves with each fight)
                    desiredExpPerc = 100 * delta;
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
        u8 difficulty = gRogueRun.currentDifficulty;
        
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY)
        {
            // Want to make legendaries hard to catch than other mons in the area
            difficulty += 2;
        }

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
        u32 statusAilment = 0; // STATUS1_NONE

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

void Rogue_ModifyBattleWinnings(u16 trainerNum, u32* money)
{
    if(Rogue_IsRunActive())
    {
        // Once we've gotten champion we want to give a bit more money 
        u8 difficulty = gRogueRun.currentDifficulty;
        u8 difficultyModifier = GetRoomTypeDifficulty();

        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
        {
            if(IsBossTrainer(trainerNum))
            {
                // Keep default calc
            }
            else
            {
                // EXP trainer
                *money = 0;
            }
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_MINIBOSS)
        {
            u8 difficulty = gRogueRun.currentDifficulty;
            *money = (difficulty + 1) * 1000;
        }
        else if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
        {
            if(difficulty <= 11)
            {
                if(difficultyModifier == 2) // Hard
                    *money = *money / 2;
                else
                    *money = *money / 3;
            }
            else
            {
                // Kinder but not by much ;)
                if(difficultyModifier != 2) // !Hard
                    *money = *money / 2;
            }
        }
        else if(!FlagGet(FLAG_ROGUE_EASY_ITEMS))
        {
            if(difficulty <= 11)
            {
                if(difficultyModifier != 2) // !Hard
                    *money = *money / 2;
            }
        }
    }
}

void Rogue_ModifyBattleWaitTime(u16* waitTime, bool8 awaitingMessage)
{
    u8 difficulty = Rogue_IsRunActive() ? gRogueRun.currentDifficulty : 0;

    if(Rogue_FastBattleAnims())
    {
        *waitTime = awaitingMessage ? 8 : 0;
    }
    else if(difficulty != (BOSS_ROOM_COUNT - 1)) // Go at default speed for final fight
    {
        // Still run faster and default game because it's way too slow :(
        *waitTime = *waitTime / 2;
    }
}

s16 Rogue_ModifyBattleSlideAnim(s16 rate)
{
    u8 difficulty = Rogue_IsRunActive() ? gRogueRun.currentDifficulty : 0;

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

    // Just treat megas as gen 1 as they are controlled by a different mechanism
    if(species >= SPECIES_VENUSAUR_MEGA && species <= SPECIES_GROUDON_PRIMAL)
        return 1;
    
    switch(species)
    {
        case SPECIES_KYUREM_WHITE:
        case SPECIES_KYUREM_BLACK:
            return 5;
        
        case SPECIES_NECROZMA_DUSK_MANE:
        case SPECIES_NECROZMA_DAWN_WINGS:
        case SPECIES_NECROZMA_ULTRA:
            return 7;

        case SPECIES_ZACIAN_CROWNED_SWORD:
        case SPECIES_ZAMAZENTA_CROWNED_SHIELD:
        case SPECIES_ETERNATUS_ETERNAMAX:
        case SPECIES_URSHIFU_RAPID_STRIKE_STYLE:
        case SPECIES_ZARUDE_DADA:
        case SPECIES_CALYREX_ICE_RIDER:
        case SPECIES_CALYREX_SHADOW_RIDER:
            return 8;
    }
#endif
    
    return 0;
}

u8 ItemToGen(u16 item)
{
#ifdef ROGUE_EXPANSION
    if(item >= ITEM_FLAME_PLATE && item <= ITEM_PIXIE_PLATE)
        return 4;

    if(item >= ITEM_DOUSE_DRIVE && item <= ITEM_CHILL_DRIVE)
        return 5;

    if(item >= ITEM_FIRE_MEMORY && item <= ITEM_FAIRY_MEMORY)
        return 7;

    // Mega stones are gonna be gen'd by the mons as we already feature toggle them based on key items
    if(item >= ITEM_VENUSAURITE && item <= ITEM_MEWTWONITE_Y)
        return 1;
    if(item >= ITEM_AMPHAROSITE && item <= ITEM_TYRANITARITE)
        return 2;
    if(item >= ITEM_SCEPTILITE && item <= ITEM_LATIOSITE)
        return 3;
    if(item >= ITEM_LOPUNNITE && item <= ITEM_GALLADITE)
        return 4;
    if(item == ITEM_AUDINITE)
        return 5;
    if(item == ITEM_DIANCITE)
        return 6;

    // Z-crystals are key item feature toggled so leave them as always on except to for mon specific ones
    if(item >= ITEM_NORMALIUM_Z && item <= ITEM_MEWNIUM_Z)
        return 1;
    if(item >= ITEM_DECIDIUM_Z && item <= ITEM_ULTRANECROZIUM_Z)
        return 7;

    switch(item)
    {
        case ITEM_SWEET_HEART:
            return 5;

        case ITEM_PEWTER_CRUNCHIES:
            return 1;
        case ITEM_RAGE_CANDY_BAR:
            return 2;
        case ITEM_LAVA_COOKIE:
            return 3;
        case ITEM_OLD_GATEAU:
            return 4;
        case ITEM_CASTELIACONE:
            return 5;
        case ITEM_LUMIOSE_GALETTE:
            return 6;
        case ITEM_SHALOUR_SABLE:
            return 7;
        case ITEM_BIG_MALASADA:
            return 8;

        case ITEM_SUN_STONE:
        case ITEM_DRAGON_SCALE:
        case ITEM_UPGRADE:
        case ITEM_KINGS_ROCK:
        case ITEM_METAL_COAT:
            return 2;

        case ITEM_SHINY_STONE:
        case ITEM_DUSK_STONE:
        case ITEM_DAWN_STONE:
        case ITEM_ICE_STONE: // needed for gen 4 evos
        case ITEM_PROTECTOR:
        case ITEM_ELECTIRIZER:
        case ITEM_MAGMARIZER:
        case ITEM_DUBIOUS_DISC:
        case ITEM_REAPER_CLOTH:
        case ITEM_OVAL_STONE:
        case ITEM_RAZOR_FANG:
        case ITEM_RAZOR_CLAW:
            return 4;

        case ITEM_PRISM_SCALE:
            return 5;

        case ITEM_WHIPPED_DREAM:
        case ITEM_SACHET:
            return 6;

        case ITEM_SWEET_APPLE:
        case ITEM_TART_APPLE:
        case ITEM_CRACKED_POT:
        case ITEM_CHIPPED_POT:
        case ITEM_GALARICA_CUFF:
        case ITEM_GALARICA_WREATH:
        case ITEM_STRAWBERRY_SWEET:
        case ITEM_LOVE_SWEET:
        case ITEM_BERRY_SWEET:
        case ITEM_CLOVER_SWEET:
        case ITEM_FLOWER_SWEET:
        case ITEM_STAR_SWEET:
        case ITEM_RIBBON_SWEET:
            return 8;

        case ITEM_RUSTED_SWORD:
        case ITEM_RUSTED_SHIELD:
            return 8;

        case ITEM_RED_ORB:
        case ITEM_BLUE_ORB:
            return 3;

        case ITEM_LIGHT_BALL:
        case ITEM_LEEK:
        case ITEM_THICK_CLUB:
            return 1;

        case ITEM_DEEP_SEA_SCALE:
        case ITEM_DEEP_SEA_TOOTH:
        case ITEM_SOUL_DEW:
            return 3;

        case ITEM_ADAMANT_ORB:
        case ITEM_LUSTROUS_ORB:
        case ITEM_GRISEOUS_ORB:
            return 4;

        case ITEM_ROTOM_CATALOG:
        case ITEM_GRACIDEA:
            return 4;

        case ITEM_REVEAL_GLASS:
        case ITEM_DNA_SPLICERS:
            return 5;

        case ITEM_ZYGARDE_CUBE:
        case ITEM_PRISON_BOTTLE:
            return 6;

        case ITEM_N_SOLARIZER:
        case ITEM_N_LUNARIZER:
            return 7;

        case ITEM_REINS_OF_UNITY:
            return 8;
    };

#else
    // Item ranges are different so handle differently for EE and vanilla
    switch(item)
    {       
        case ITEM_SUN_STONE:
        case ITEM_DRAGON_SCALE:
        case ITEM_UP_GRADE:
        case ITEM_KINGS_ROCK:
        case ITEM_METAL_COAT:
            return 2;

        case ITEM_DEEP_SEA_SCALE:
        case ITEM_DEEP_SEA_TOOTH:
        case ITEM_SOUL_DEW:
        case ITEM_RED_ORB:
        case ITEM_BLUE_ORB:
        case ITEM_LAVA_COOKIE:
            return 3;
    };
#endif
    
    // Assume gen 1 if we get here (i.e. always on)
    return 1;
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

#if defined(ROGUE_DEBUG) && defined(ROGUE_DEBUG_PAUSE_PANEL)

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
    u8* strPointer = &gStringVar4[0];
    *strPointer = EOS;

    if(JOY_NEW(R_BUTTON) && gDebug_CurrentTab != 2)
    {
        ++gDebug_CurrentTab;
    }
    else if(JOY_NEW(L_BUTTON) && gDebug_CurrentTab != 0)
    {
        --gDebug_CurrentTab;
    }

    // Main tab
    //
    if(gDebug_CurrentTab == 0)
    {
        u8 difficultyLevel = gRogueRun.currentDifficulty;
        u8 playerLevel = CalculatePlayerLevel();
        u8 wildLevel = CalculateWildLevel();

        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);

        if(FlagGet(FLAG_SET_SEED_ENABLED))
        {
            strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, Rogue_GetSeed());
        }

        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Room, gRogueRun.currentRoomIdx);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Difficulty, difficultyLevel);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_PlayerLvl, playerLevel);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildLvl, wildLevel);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildCount, gDebug_WildOptionCount);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_ItemCount, gDebug_ItemOptionCount);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_TrainerCount, gDebug_TrainerOptionCount);
    }
    // Adventure path tab
    //
    else if(gDebug_CurrentTab == 1)
    {
        strPointer = StringAppend(strPointer, gText_RogueDebug_AdvHeader);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_AdvCount, gRogueAdvPath.currentColumnCount);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_X, gRogueAdvPath.currentNodeX);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Y, gRogueAdvPath.currentNodeY);
    }
    // Misc. debug tab
    //
    else
    {
        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_ItemCount,  VarGet(VAR_REPEL_STEP_COUNT));
    }

    return gStringVar4;
}

void Rogue_CreateMiniMenuExtraGFX(void)
{
}

void Rogue_RemoveMiniMenuExtraGFX(void)
{
}

#else

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return Rogue_IsRunActive();
}

u8* Rogue_GetMiniMenuContent(void)
{
    u8 difficultyLevel = gRogueRun.currentDifficulty;

    ConvertIntToDecimalStringN(gStringVar1, gSaveBlock2Ptr->playTimeHours, STR_CONV_MODE_RIGHT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, gSaveBlock2Ptr->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    StringExpandPlaceholders(gStringVar3, gText_RogueHourMinute);

    ConvertIntToDecimalStringN(gStringVar1, gRogueRun.currentRoomIdx, STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar2, difficultyLevel, STR_CONV_MODE_RIGHT_ALIGN, 2);
    
    StringExpandPlaceholders(gStringVar4, gText_RogueRoomProgress);

    return gStringVar4;
}

void Rogue_CreateMiniMenuExtraGFX(void)
{
#ifdef ROGUE_FEATURE_ENCOUNTER_PREVIEW
    u8 i;

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
    {
        //LoadMonIconPalettes();

        for(i = 0; i < GetCurrentWildEncounterCount(); ++i)
        {
            if(gRogueLocal.encounterPreview[i].isVisible)
            {
                gRogueLocal.encounterPreview[i].species = gRogueRun.wildEncounters[i];

                LoadMonIconPalette(gRogueLocal.encounterPreview[i].species);

                if(i < 3)
                    gRogueLocal.encounterPreview[i].monSpriteId = CreateMonIconNoPersonality(gRogueLocal.encounterPreview[i].species, SpriteCallbackDummy, 14 + i * 32, 72, 0, TRUE);
                else
                    gRogueLocal.encounterPreview[i].monSpriteId = CreateMonIconNoPersonality(gRogueLocal.encounterPreview[i].species, SpriteCallbackDummy, (14 + (i - 3) * 32), 72 + 32, 0, TRUE);
            }
            else
            {
                gRogueLocal.encounterPreview[i].species = SPECIES_NONE;

                LoadMonIconPalette(gRogueLocal.encounterPreview[i].species);

                if(i < 3)
                    gRogueLocal.encounterPreview[i].monSpriteId = CreateMissingMonIcon(SpriteCallbackDummy, 14 + i * 32, 72, 0);
                else
                    gRogueLocal.encounterPreview[i].monSpriteId = CreateMissingMonIcon(SpriteCallbackDummy, (14 + (i - 3) * 32), 72 + 32, 0);
            }

            gSprites[gRogueLocal.encounterPreview[i].monSpriteId].oam.priority = 9;
        }
    }
#endif
}

void Rogue_RemoveMiniMenuExtraGFX(void)
{
#ifdef ROGUE_FEATURE_ENCOUNTER_PREVIEW
    u8 i;

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
    {
        for(i = 0; i < GetCurrentWildEncounterCount(); ++i)
        {
            //if(gRogueLocal.encounterPreview[i].species != SPECIES_NONE)
            FreeMonIconPalette(GetIconSpeciesNoPersonality(gRogueLocal.encounterPreview[i].species));

            if(gRogueLocal.encounterPreview[i].monSpriteId != SPRITE_NONE)
                FreeAndDestroyMonIconSprite(&gSprites[gRogueLocal.encounterPreview[i].monSpriteId]);

            gRogueLocal.encounterPreview[i].monSpriteId = SPRITE_NONE;
        }

        //FreeMonIconPalettes();
    }
#endif
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
    RogueQuery_EvolveSpecies(2, FALSE); // To force gen3+ mons off

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
}

static void LoadHubInventory(void)
{
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
}

extern const u8 Rogue_QuickSaveLoad[];

void Rogue_OnSaveGame(void)
{
    u8 i;

    gSaveBlock1Ptr->rogueBlock.saveData.rngSeed = gRngRogueValue;

    memcpy(&gSaveBlock1Ptr->rogueBlock.saveData.runData, &gRogueRun, sizeof(gRogueRun));
    memcpy(&gSaveBlock1Ptr->rogueBlock.saveData.hubData, &gRogueHubData, sizeof(gRogueHubData));

    memcpy(&gRogueLocal.saveData.raw.advPath, &gRogueAdvPath, sizeof(gRogueAdvPath));

    // Move Hub save data into storage box space
    for(i = 0; i < LEFTOVER_BOXES_COUNT; ++i)
    {
        memcpy(&gPokemonStoragePtr->boxes[TOTAL_BOXES_COUNT + i][0], &gRogueLocal.saveData.boxes[i][0], sizeof(struct BoxPokemon) * IN_BOX_COUNT);
    }
}

void Rogue_OnLoadGame(void)
{
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

    memcpy(&gRogueAdvPath, &gRogueLocal.saveData.raw.advPath, sizeof(gRogueAdvPath));

    if(Rogue_IsRunActive() && !FlagGet(FLAG_ROGUE_DEFEATED_BOSS13))
    {
        gRogueLocal.hasQuickLoadPending = TRUE;
        //ScriptContext1_SetupScript(Rogue_QuickSaveLoad);
    }
}

bool8 Rogue_OnProcessPlayerFieldInput(void)
{
#ifndef ROGUE_DEBUG
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

static u16 GetStartDifficulty(void)
{
    u16 skipToDifficulty = VarGet(VAR_ROGUE_SKIP_TO_DIFFICULTY);

    if(skipToDifficulty != 0)
    {
        return skipToDifficulty;
    }
    
    return 0;
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

    gRogueRun.currentRoomIdx = 0;
    gRogueRun.currentDifficulty = GetStartDifficulty();
    gRogueRun.currentLevelOffset = 5;
    gRogueRun.currentRouteIndex = 0;

    // Will get generated later
    gRogueAdvPath.currentColumnCount = 0;
    gRogueAdvPath.currentNodeX = 0;
    gRogueAdvPath.currentNodeY = 0;
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_NONE;

    memset(&gRogueRun.routeHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueRun.routeHistoryBuffer));
    memset(&gRogueRun.legendaryHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueRun.legendaryHistoryBuffer));
    memset(&gRogueRun.wildEncounterHistoryBuffer[0], 0, sizeof(u16) * ARRAY_COUNT(gRogueRun.wildEncounterHistoryBuffer));
    memset(&gRogueRun.miniBossHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueRun.miniBossHistoryBuffer));
    
    VarSet(VAR_ROGUE_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, 0);
    VarSet(VAR_ROGUE_REWARD_MONEY, 0);
    VarSet(VAR_ROGUE_REWARD_CANDY, 0);
    VarSet(VAR_ROGUE_MAX_PARTY_SIZE, PARTY_SIZE);
    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);

    SaveHubInventory();

    gRogueHubData.money = GetMoney(&gSaveBlock1Ptr->money);
    //gRogueHubData.registeredItem = gSaveBlock1Ptr->registeredItem;

    gRogueHubData.playTimeHours = gSaveBlock2Ptr->playTimeHours;
    gRogueHubData.playTimeMinutes = gSaveBlock2Ptr->playTimeMinutes;
    gRogueHubData.playTimeSeconds = gSaveBlock2Ptr->playTimeSeconds;
    gRogueHubData.playTimeVBlanks = gSaveBlock2Ptr->playTimeVBlanks;

    gSaveBlock2Ptr->playTimeHours = 0;
    gSaveBlock2Ptr->playTimeMinutes = 0;
    gSaveBlock2Ptr->playTimeSeconds = 0;
    gSaveBlock2Ptr->playTimeVBlanks = 0;

    SetMoney(&gSaveBlock1Ptr->money, VarGet(VAR_ROGUE_ADVENTURE_MONEY));

    FlagClear(FLAG_ROGUE_FREE_HEAL_USED);

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
}

static void EndRogueRun(void)
{
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_SET_SEED_ENABLED);
    VarSet(VAR_ROGUE_MAX_PARTY_SIZE, PARTY_SIZE);

    //gRogueRun.currentRoomIdx = 0;
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_NONE;

    // Restore money and give reward here too, as it's a bit easier
    SetMoney(&gSaveBlock1Ptr->money, gRogueHubData.money);
    AddMoney(&gSaveBlock1Ptr->money, VarGet(VAR_ROGUE_REWARD_MONEY));

    //gSaveBlock1Ptr->registeredItem = gRogueHubData.registeredItem;

    
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

u8 Rogue_SelectBossRoom(void)
{
    u8 bossId = 0;
    u8 difficulty = gRogueRun.currentDifficulty;

    // TODO - Support refighting gymleaders by wrapping to room selection again

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

    return bossId;
}

u8 Rogue_SelectLegendaryEncounterRoom(void)
{
    u8 mapCount;
    u8 mapIdx;
    const struct RogueEncounterMap* selectedMap = NULL;

    mapCount = gRogueLegendaryEncounterInfo.mapCount;

    // Avoid repeating same encounter 
    do
    {
        mapIdx = Random() % mapCount;
        selectedMap = &gRogueLegendaryEncounterInfo.mapTable[mapIdx];
    }
    while(HistoryBufferContains(&gRogueRun.legendaryHistoryBuffer[0], ARRAY_COUNT(gRogueRun.legendaryHistoryBuffer), mapIdx) && (mapCount > 6 || !IsGenEnabled(SpeciesToGen(selectedMap->encounterId))) );

    HistoryBufferPush(&gRogueRun.legendaryHistoryBuffer[0], ARRAY_COUNT(gRogueRun.legendaryHistoryBuffer), mapIdx);

    return mapIdx;
}

u8 Rogue_SelectMiniBossEncounterRoom(void)
{
    u8 bossId = 0;

    do
    {
        bossId = RogueRandomRange(gRouteMiniBossEncounters.mapCount, OVERWORLD_FLAG);
    }
    while(HistoryBufferContains(&gRogueRun.miniBossHistoryBuffer[0], ARRAY_COUNT(gRogueRun.miniBossHistoryBuffer), bossId));

    HistoryBufferPush(&gRogueRun.miniBossHistoryBuffer[0], ARRAY_COUNT(gRogueRun.miniBossHistoryBuffer), bossId);

    return bossId;
}

u8 Rogue_SelectWildDenEncounterRoom(void)
{
    u16 queryCount;

    RogueQuery_Clear();

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesExcludeCommon();
    RogueQuery_SpeciesIsNotLegendary();
    RogueQuery_TransformToEggSpecies();

    RogueQuery_EvolveSpecies(CalculatePlayerLevel(), TRUE);

    // Have to use uncollapsed queries as this query is too large otherwise
    queryCount = RogueQuery_UncollapsedSpeciesSize();

    return RogueQuery_AtUncollapsedIndex(Random() % queryCount);
}

u8 Rogue_SelectRouteRoom(void)
{
    u8 mapCount;
    u8 mapIdx;

    // Don't replay recent routes
    do
    {
        mapIdx = RogueRandomRange(ROGUE_ROUTE_COUNT, OVERWORLD_FLAG);
    }
    while(HistoryBufferContains(&gRogueRun.routeHistoryBuffer[0], ARRAY_COUNT(gRogueRun.routeHistoryBuffer), mapIdx));

    HistoryBufferPush(&gRogueRun.routeHistoryBuffer[0], ARRAY_COUNT(gRogueRun.routeHistoryBuffer), mapIdx);

    return mapIdx;
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

void Rogue_OnWarpIntoMap(void)
{
    u8 difficultyLevel;
    gRogueAdvPath.isOverviewActive = FALSE;

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
    else if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_ADVENTURE_PATHS)
    {
        gRogueAdvPath.isOverviewActive = TRUE;
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB && Rogue_IsRunActive())
    {
        EndRogueRun();
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
    else if(warp->mapGroup == MAP_GROUP(ROGUE_HUB_TRANSITION) && warp->mapNum == MAP_NUM(ROGUE_HUB_TRANSITION))
    {
        if(FlagGet(FLAG_ROGUE_RANDOM_TRADE_USED))
        {
            // Enable random trader
            FlagClear(FLAG_ROGUE_RANDOM_TRADE_DISABLED);
        }
        else
        {
            FlagSet(FLAG_ROGUE_RANDOM_TRADE_DISABLED);
        }
    }

    // Reset preview data
    memset(&gRogueLocal.encounterPreview[0], 0, sizeof(gRogueLocal.encounterPreview));

    if(Rogue_IsRunActive() && !RogueAdv_OverrideNextWarp(warp))
    {
        ++gRogueRun.currentRoomIdx;

        VarSet(VAR_ROGUE_REWARD_MONEY, VarGet(VAR_ROGUE_REWARD_MONEY) + 300);

        if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
            VarSet(VAR_ROGUE_REWARD_MONEY, VarGet(VAR_ROGUE_REWARD_MONEY) + 100);

        if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
            VarSet(VAR_ROGUE_REWARD_MONEY, VarGet(VAR_ROGUE_REWARD_MONEY) + 100);

        // We're warping into a valid map
        // We've already set the next room type so adjust the scaling now
        switch(gRogueAdvPath.currentRoomType)
        {
            case ADVPATH_ROOM_RESTSTOP:
            {
                FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);

                if(RogueRandomChance(33, OVERWORLD_FLAG))
                {
                    // Enable random trader
                    FlagClear(FLAG_ROGUE_RANDOM_TRADE_DISABLED);
                }
                else
                {
                    FlagSet(FLAG_ROGUE_RANDOM_TRADE_DISABLED);
                }
                break;
            }

            case ADVPATH_ROOM_ROUTE:
            {
                u8 weatherChance = 5 + 20 * gRogueAdvPath.currentRoomParams.perType.route.difficulty;

                gRogueRun.currentRouteIndex = gRogueAdvPath.currentRoomParams.roomIdx;

                // Legacy feature legendaries were on random routes (Just keep them in as debug shortcut)
                #ifdef ROGUE_DEBUG
                    FlagSet(FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE);
                #else
                    FlagClear(FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE);
                #endif

                RandomiseWildEncounters();
                ResetTrainerBattles();
                RandomiseEnabledTrainers();
                RandomiseEnabledItems();
                RandomiseBerryTrees();

                if(gRogueRun.currentDifficulty != 0 && RogueRandomChance(weatherChance, OVERWORLD_FLAG))
                {
                    FlagSet(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                break;
            }

            case ADVPATH_ROOM_BOSS:
            {
                gRogueRun.currentLevelOffset = 0;

                RandomiseEnabledItems();

                // Weather
                if(gRogueRun.currentDifficulty == 0 || FlagGet(FLAG_ROGUE_EASY_TRAINERS))
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS) || gRogueRun.currentDifficulty > 2)
                {
                    FlagSet(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                break;
            }

            case ADVPATH_ROOM_LEGENDARY:
            {
                ResetSpecialEncounterStates();
                ResetTrainerBattles();
                RandomiseEnabledTrainers();
                VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, gRogueLegendaryEncounterInfo.mapTable[gRogueAdvPath.currentRoomParams.roomIdx].encounterId);
                break;
            }

            case ADVPATH_ROOM_WILD_DEN:
            {
                ResetSpecialEncounterStates();
                VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, gRogueAdvPath.currentRoomParams.roomIdx);
                break;
            }

            case ADVPATH_ROOM_MINIBOSS:
            {
                u16 encounterId = gRouteMiniBossEncounters.mapTable[gRogueAdvPath.currentRoomParams.roomIdx].encounterId;

                RandomiseEnabledItems(); // We only want this for the item content tbf

                if(encounterId == OBJ_EVENT_GFX_BRENDAN_NORMAL)
                {
                    switch(gSaveBlock2Ptr->playerGender)
                    {
                        case(STYLE_EMR_BRENDAN):
                            VarSet(VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_BRENDAN_NORMAL);
                            break;
                        case(STYLE_EMR_MAY):
                            VarSet(VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_MAY_NORMAL);
                            break;

                        case(STYLE_RED):
                            VarSet(VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_RED);
                            break;
                        case(STYLE_LEAF):
                            VarSet(VAR_OBJ_GFX_ID_0, OBJ_EVENT_GFX_LEAF);
                            break;
                    };
                }
                else
                {
                    VarSet(VAR_OBJ_GFX_ID_0, encounterId);
                }

                VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, encounterId);

                // Weather
                if(gRogueRun.currentDifficulty == 0 || FlagGet(FLAG_ROGUE_EASY_TRAINERS))
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS) || gRogueRun.currentDifficulty > 2)
                {
                    FlagSet(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                else
                {
                    FlagClear(FLAG_ROGUE_WEATHER_ACTIVE);
                }
                break;
            }
        };

        
        // Ensure we have all badges by this point
        if(gRogueRun.currentDifficulty >= 8)
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
        VarSet(VAR_ROGUE_DIFFICULTY, gRogueRun.currentDifficulty);
        VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, max(gRogueRun.currentDifficulty, VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY)));
        VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, gRogueRun.currentRoomIdx);
        VarSet(VAR_ROGUE_CURRENT_LEVEL_CAP, CalculateBossLevel());
    }
}

void RemoveMonAtSlot(u8 slot, bool8 keepItems)
{
    if(slot < gPlayerPartyCount)
    {
        if(GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES) != SPECIES_NONE)
        {
            u32 hp = 0;
            SetMonData(&gPlayerParty[slot], MON_DATA_HP, &hp);
            RemoveAnyFaintedMons(keepItems);
        }
    }
}

void RemoveAnyFaintedMons(bool8 keepItems)
{
    bool8 hasValidSpecies;
    u8 read;
    u8 write = 0;

    for(read = 0; read < PARTY_SIZE; ++read)
    {
        hasValidSpecies = GetMonData(&gPlayerParty[read], MON_DATA_SPECIES) != SPECIES_NONE;

        if(hasValidSpecies && GetMonData(&gPlayerParty[read], MON_DATA_HP, NULL) != 0)
        {
            // This mon is alive

            // No need to do anything as this mon is in the correct slot
            if(write != read)
            {
                CopyMon(&gPlayerParty[write], &gPlayerParty[read], sizeof(struct Pokemon));
                ZeroMonData(&gPlayerParty[read]);
            }

            ++write;
        }
        else if(hasValidSpecies)
        {
            if(keepItems)
            {
                // Dead so give back held item
                u16 heldItem = GetMonData(&gPlayerParty[read], MON_DATA_HELD_ITEM);
                if(heldItem != ITEM_NONE)
                    AddBagItem(heldItem, 1);
            }

            ZeroMonData(&gPlayerParty[read]);
        }
    }

    gPlayerPartyCount = CalculatePlayerPartyCount();
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

static bool32 DidPlayerRun(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_RAN:
    case B_OUTCOME_PLAYER_TELEPORTED:
    case B_OUTCOME_MON_FLED:
        return TRUE;
    default:
        return FALSE;
    }
}

void Rogue_Battle_EndTrainerBattle(u16 trainerNum)
{
    if(Rogue_IsRunActive())
    {
        if(IsBossTrainer(trainerNum))
        {
            gRogueRun.currentLevelOffset = 10;
            ++gRogueRun.currentDifficulty;
            
            // Just beat last gym leader
            if(gRogueRun.currentDifficulty == 8)
            {
                VarSet(VAR_ROGUE_REWARD_MONEY, VarGet(VAR_ROGUE_REWARD_MONEY) + 5000);
            }
            // Just beat last elite 4 member
            else if(gRogueRun.currentDifficulty == 12)
            {
                VarSet(VAR_ROGUE_REWARD_MONEY, VarGet(VAR_ROGUE_REWARD_MONEY) + 5000);
            }
            // Just beat champion
            else if(gRogueRun.currentDifficulty > 12)
            {
                VarSet(VAR_ROGUE_REWARD_MONEY, VarGet(VAR_ROGUE_REWARD_MONEY) + 7500);
            }

            // Update reward candy only after boss has been defeated
            if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
            {
                if(FlagGet(FLAG_ROGUE_HARD_ITEMS))
                {
                    // Pretty much max difficulty
                    VarSet(VAR_ROGUE_REWARD_CANDY, (gRogueRun.currentDifficulty - GetStartDifficulty()) + 2);
                }
                else
                {
                    VarSet(VAR_ROGUE_REWARD_CANDY, (gRogueRun.currentDifficulty - GetStartDifficulty()) + 1);
                }
            }
            else
            {
                VarSet(VAR_ROGUE_REWARD_CANDY, (gRogueRun.currentDifficulty - GetStartDifficulty()));
            }
        }

        // Adjust this after the boss reset
        if(gRogueRun.currentLevelOffset)
        {
            // Every trainer battle drops level cap by 4
            if(gRogueRun.currentLevelOffset < 4)
                gRogueRun.currentLevelOffset = 0;
            else
                gRogueRun.currentLevelOffset -= 4;
        }

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
        if(gRogueRun.currentLevelOffset && !DidPlayerRun(gBattleOutcome))
        {
            // Every wild battle drops level cap by 3
            if(gRogueRun.currentLevelOffset < 3)
                gRogueRun.currentLevelOffset = 0;
            else
                gRogueRun.currentLevelOffset -= 3;
        }

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

static bool8 IsMirrorTrainer(u16 trainerNum)
{
    switch(trainerNum)
    {
        case TRAINER_RED:
        case TRAINER_LEAF:
        case TRAINER_BRENDAN_PLACEHOLDER:
        case TRAINER_MAY_PLACEHOLDER:
            return TRUE;
    };

    return FALSE;
}

static bool8 IsMiniBossTrainer(u16 trainerNum)
{
    switch(trainerNum)
    {
        case TRAINER_MAXIE_MAGMA_HIDEOUT:
        case TRAINER_ARCHIE:

        case TRAINER_WALLY_VR_1:

        case TRAINER_RED:
        case TRAINER_LEAF:
        case TRAINER_BRENDAN_PLACEHOLDER:
        case TRAINER_MAY_PLACEHOLDER:
            return TRUE;
    };

    return FALSE;
}

static bool8 IsAnyBossTrainer(u16 trainerNum)
{
    return IsBossTrainer(trainerNum) || IsMiniBossTrainer(trainerNum);
}

static bool8 UseCompetitiveMoveset(u16 trainerNum, u8 monIdx, u8 totalMonCount)
{
    bool8 preferCompetitive = FALSE;
    bool8 result = FALSE;
    u8 difficultyLevel = gRogueRun.currentDifficulty;
    u8 difficultyModifier = GetRoomTypeDifficulty();

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY || difficultyModifier == 2) // HARD
    {
        // For regular trainers, Last and first mon can have competitive sets
        preferCompetitive = (monIdx == 0 || monIdx == (totalMonCount - 1));
    }

    if(FlagGet(FLAG_ROGUE_EASY_TRAINERS))
    {
        return FALSE;
    }
    else if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
    {
        if(difficultyLevel == 0) // Last mon has competitive set
            return (preferCompetitive || IsAnyBossTrainer(trainerNum)) && monIdx == (totalMonCount - 1);
        else if(difficultyLevel == 1)
            return (preferCompetitive || IsAnyBossTrainer(trainerNum));
        else
            return TRUE;
    }
    else
    {
        // Start using competitive movesets on 3rd gym
        if(difficultyLevel == 0) // Last mon has competitive set
            return FALSE;
        else if(difficultyLevel == 1)
            return (preferCompetitive || IsAnyBossTrainer(trainerNum)) && monIdx == (totalMonCount - 1);
        else
            return (preferCompetitive || IsAnyBossTrainer(trainerNum));
    }

    return FALSE;
}

static void SeedRogueTrainer(u16 seed, u16 trainerNum, u16 offset)
{
    gRngRogueValue = seed + trainerNum * 3 + offset * 7;
}

static void ConfigureTrainer(u16 trainerNum, u8* forceType, u8* disabledType, bool8* allowItemEvos, bool8* allowLedgendaries, u8* monsCount)
{
    u8 difficultyLevel = gRogueRun.currentDifficulty;

    if(IsMirrorTrainer(trainerNum))
    {
        *monsCount = gPlayerPartyCount;
        return;
    }

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
            *disabledType = TYPE_FLYING; // Too many normal flyings
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
        
        // Minibosses
        case TRAINER_MAXIE_MAGMA_HIDEOUT:
            forceType[0] = TYPE_FIRE;
            forceType[1] = TYPE_DARK;
            break;

        case TRAINER_ARCHIE:
            forceType[0] = TYPE_WATER;
            forceType[1] = TYPE_DARK;
            break;

        case TRAINER_WALLY_VR_1:
            forceType[0] = TYPE_PSYCHIC;
#ifdef ROGUE_EXPANSION
            forceType[1] = TYPE_FAIRY;
#else
            forceType[1] = TYPE_GRASS;
#endif
            break;
    };

    if(IsAnyBossTrainer(trainerNum)) 
    {
        if(difficultyLevel == 0)
        {
            *monsCount = FlagGet(FLAG_ROGUE_HARD_TRAINERS) ? 4 : 3;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 1)
        {
            *monsCount = FlagGet(FLAG_ROGUE_HARD_TRAINERS) ? 5 : 4;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 2)
        {
            *monsCount = FlagGet(FLAG_ROGUE_HARD_TRAINERS) ? 6 : 4;
            *allowItemEvos = FlagGet(FLAG_ROGUE_HARD_TRAINERS);
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel <= 5)
        {
            *monsCount = FlagGet(FLAG_ROGUE_HARD_TRAINERS) ? 6 : 5;
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
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
        {
            // EXP trainer
            *monsCount = 1;
            *forceType = TYPE_NORMAL;
            *allowItemEvos = FALSE;
            *allowLedgendaries = FALSE;
        }
        else if(difficultyLevel == 0)
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
            *monsCount = 1 + RogueRandomRange(4, FLAG_SET_SEED_TRAINERS);
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
            *monsCount = 3 + RogueRandomRange(2, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
        else
        {
            // Champion
            *monsCount = 3 + RogueRandomRange(4, FLAG_SET_SEED_TRAINERS);
            *allowItemEvos = TRUE;
            *allowLedgendaries = TRUE;
        }
    }

    if(FlagGet(FLAG_ROGUE_DOUBLE_BATTLES)) 
    {
        if(*monsCount < 2 && gPlayerPartyCount >= 2)
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
    RogueQuery_EvolveSpecies(CalculateTrainerLevel(trainerNum), gRogueLocal.trainerTemp.allowItemEvos);

    if(gRogueLocal.trainerTemp.allowedType[0] != TYPE_NONE)
    {
        if(gRogueLocal.trainerTemp.allowedType[1] != TYPE_NONE)
            RogueQuery_SpeciesOfTypes(&gRogueLocal.trainerTemp.allowedType[0], 2); // 2 types
        else
            RogueQuery_SpeciesOfType(gRogueLocal.trainerTemp.allowedType[0]); // 1 type
    }

    // Disable types
    if(gRogueLocal.trainerTemp.disallowedType[0] != TYPE_NONE)
    {
        if(gRogueLocal.trainerTemp.disallowedType[1] != TYPE_NONE)
            RogueQuery_SpeciesNotOfTypes(&gRogueLocal.trainerTemp.disallowedType[0], 2); // 2 types
        else
            RogueQuery_SpeciesNotOfType(gRogueLocal.trainerTemp.disallowedType[0]); // 1 type
    }

    if(gRogueLocal.trainerTemp.allowLedgendaries && trainerNum == TRAINER_MAXIE_MAGMA_HIDEOUT)
    {
        RogueQuery_Include(SPECIES_GROUDON);
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

        case TYPE_PSYCHIC:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_GHOST;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_NONE;
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
            gRogueLocal
            .trainerTemp.allowedType[1] = TYPE_NONE;
            break;

        case TYPE_NORMAL:
            hasFallback = TRUE;
            gRogueLocal.trainerTemp.allowedType[0] = TYPE_FIGHTING;
            gRogueLocal.trainerTemp.allowedType[1] = TYPE_GHOST;
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

        gRogueLocal.trainerTemp.disallowedType[0] = TYPE_NONE;
        gRogueLocal.trainerTemp.disallowedType[1] = TYPE_NONE;

        SeedRogueTrainer(gRngRogueValue, trainerNum, RogueRandom() % 17);
        ConfigureTrainer(trainerNum, &gRogueLocal.trainerTemp.allowedType[0], &gRogueLocal.trainerTemp.disallowedType[0], &gRogueLocal.trainerTemp.allowItemEvos, &gRogueLocal.trainerTemp.allowLedgendaries, monsCount);

        ApplyTrainerQuery(trainerNum);

#ifdef ROGUE_DEBUG
        gDebug_TrainerOptionCount = RogueQuery_BufferSize();
#endif

        *useRogueCreateMon = TRUE;
        return;
    }

    *useRogueCreateMon = FALSE;
}

static void SwapMons(u8 aIdx, u8 bIdx, struct Pokemon *party)
{
    if(aIdx != bIdx)
    {
        struct Pokemon tempMon;
        CopyMon(&tempMon, &party[aIdx], sizeof(struct Pokemon));

        CopyMon(&party[aIdx], &party[bIdx], sizeof(struct Pokemon));
        CopyMon(&party[bIdx], &tempMon, sizeof(struct Pokemon));
    }
}

void Rogue_PostCreateTrainerParty(u16 trainerNum, struct Pokemon *party, u8 monsCount)
{
#ifdef ROGUE_EXPANSION
    u8 writeSlot = monsCount - 1;
    u16 item = GetMonData(&party[0], MON_DATA_HELD_ITEM);

    // Try to move mega/z user to back of party
    // TODO - Identify setup user and move them to front of party
    while(writeSlot != 0 && ((item >= ITEM_VENUSAURITE && item <= ITEM_DIANCITE) || (item >= ITEM_NORMALIUM_Z && item <= ITEM_ULTRANECROZIUM_Z)))
    {
        SwapMons(0, writeSlot, party);

        item = GetMonData(&party[0], MON_DATA_HELD_ITEM);
        --writeSlot;
    }
#endif


#if defined(ROGUE_DEBUG) && defined(ROGUE_DEBUG_STEAL_TEAM)
    {
        u8 i;
        u16 exp = Rogue_ModifyExperienceTables(1, 100);

        for(i = 0; i < PARTY_SIZE; ++i)
        {
            ZeroMonData(&gPlayerParty[i]);
        }

        gPlayerPartyCount = monsCount;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            CopyMon(&gPlayerParty[i], &party[i], sizeof(gPlayerParty[i]));

            SetMonData(&gPlayerParty[i], MON_DATA_EXP, &exp);
            CalculateMonStats(&gPlayerParty[i]);
        }
    }
#endif

    gRngRogueValue = gRogueLocal.trainerTemp.seedToRestore;
}

static u16 NextTrainerSpecies(u16 trainerNum, bool8 isBoss, struct Pokemon *party, u8 monIdx, u8 totalMonCount)
{
    u16 species;
    u16 randIdx;
    u16 queryCount = RogueQuery_BufferSize();
    

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS && !isBoss)
    {
        // EXP trainer
        return SPECIES_CHANSEY;
    }

    if(IsMirrorTrainer(trainerNum))
    {
        return GetMonData(&gPlayerParty[monIdx], MON_DATA_SPECIES);
    }

    if(monIdx >= queryCount)
    {
        // Apply the fallback query (If we have one)
        // This will allow for secondary types if we've exhausted the primary one
        ApplyFallbackTrainerQuery(trainerNum);
    }

    // Prevent duplicates, if possible
    // *Only allow duplicates after we've already seen everything in the query
    do
    {
        randIdx = RogueRandomRange(queryCount, isBoss ? FLAG_SET_SEED_BOSSES : FLAG_SET_SEED_TRAINERS);
        species = RogueQuery_BufferPtr()[randIdx];
    }
    while(PartyContainsSpecies(party, monIdx, species) && monIdx < queryCount);

    return species;
}

extern const struct LevelUpMove *const gLevelUpLearnsets[];

static bool8 CanLearnMoveByLvl(u16 species, u16 move, s32 level)
{
    u16 eggSpecies;
    s32 i;

    for (i = 0; gLevelUpLearnsets[species][i].move != LEVEL_UP_END; i++)
    {
        u16 moveLevel;

        if(move == gLevelUpLearnsets[species][i].move)
        {
            moveLevel = gLevelUpLearnsets[species][i].level;

            if (moveLevel > level)
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

static bool8 SelectNextPreset(u16 species, u16 trainerNum, u8 monIdx, u16 randFlag, struct RogueMonPreset* outPreset)
{
    u8 randOffset;
    u8 i;
    bool8 isPresetValid;
    u8 presetCount = gPresetMonTable[species].presetCount;

    
    if(IsMirrorTrainer(trainerNum))
    {
        // Populate mon preset based on exact same team
        outPreset->heldItem = GetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM);
        outPreset->abilityNum = GetMonData(&gPlayerParty[monIdx], MON_DATA_ABILITY_NUM);

        for(i = 0; i < MAX_MON_MOVES; ++i)
        {
            outPreset->moves[i] = GetMonData(&gPlayerParty[monIdx], MON_DATA_MOVE1 + i);
        }

        return TRUE;
    }
    else if(presetCount != 0)
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

#ifdef ROGUE_EXPANSION
            if(!IsMegaEvolutionEnabled())
            {
                // Special case for primal reversion
                if(currPreset->heldItem == ITEM_RED_ORB || currPreset->heldItem == ITEM_BLUE_ORB)
                {
                    isPresetValid = FALSE;
                }
            }

            if(gRogueLocal.trainerTemp.hasUsedMegaStone || !IsMegaEvolutionEnabled())
            {
                if(currPreset->heldItem >= ITEM_VENUSAURITE && currPreset->heldItem <= ITEM_DIANCITE)
                {
                    isPresetValid = FALSE;
                }
            }

            if(gRogueLocal.trainerTemp.hasUsedZMove || !IsZMovesEnabled())
            {
                if(currPreset->heldItem >= ITEM_NORMALIUM_Z && currPreset->heldItem <= ITEM_ULTRANECROZIUM_Z)
                {
                    isPresetValid = FALSE;
                }
            }
#endif

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

#ifdef ROGUE_EXPANSION
            if(!IsMegaEvolutionEnabled())
            {
                // Special case for primal reversion
                if(currPreset->heldItem == ITEM_RED_ORB || currPreset->heldItem == ITEM_BLUE_ORB)
                {
                    outPreset->heldItem = ITEM_NONE;
                }
            }

            if(gRogueLocal.trainerTemp.hasUsedMegaStone || !IsMegaEvolutionEnabled())
            {
                if(currPreset->heldItem >= ITEM_VENUSAURITE && currPreset->heldItem <= ITEM_DIANCITE)
                {
                    outPreset->heldItem = ITEM_NONE;
                }
            }

            if(gRogueLocal.trainerTemp.hasUsedZMove || !IsZMovesEnabled())
            {
                if(currPreset->heldItem >= ITEM_NORMALIUM_Z && currPreset->heldItem <= ITEM_ULTRANECROZIUM_Z)
                {
                    outPreset->heldItem = ITEM_NONE;
                }
            }
#endif
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
#ifdef ROGUE_EXPANSION
        else if(currPreset->heldItem >= ITEM_VENUSAURITE && currPreset->heldItem <= ITEM_DIANCITE)
        {
            gRogueLocal.trainerTemp.hasUsedMegaStone = TRUE;
        }
        else if(currPreset->heldItem >= ITEM_NORMALIUM_Z && currPreset->heldItem <= ITEM_ULTRANECROZIUM_Z)
        {
            gRogueLocal.trainerTemp.hasUsedZMove = TRUE;
        }
#endif

        return TRUE;
    }

    return FALSE;
}

static void ApplyMonPreset(struct Pokemon* mon, u8 level, const struct RogueMonPreset* preset)
{
    u16 i;
    u16 move;
    u16 heldItem;
    u8 writeMoveIdx;
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
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

    if(preset->abilityNum != ABILITY_NONE)
    {
        // We need to set the ability index
#ifdef ROGUE_EXPANSION
        for(i; i < 3; ++i) // 3 to include hidden ability
#else
        for(i; i < 2; ++i)
#endif
        {
            SetMonData(mon, MON_DATA_ABILITY_NUM, &i);

            if(GetMonAbility(mon) == preset->abilityNum)
            {
                // Ability is set
                break;
            }
        }

        if(i == 4)
        {
            // We failed to set it, so fall back to default
            i = 0;
            SetMonData(mon, MON_DATA_ABILITY_NUM, &i);
        }
    }

    if(preset->heldItem != ITEM_NONE)
    {
        SetMonData(mon, MON_DATA_HELD_ITEM, &preset->heldItem);
    }

    // Teach moves from set that we can learn at this lvl
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = preset->moves[i]; 

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

    move = useMaxHappiness ? MAX_FRIENDSHIP : 0;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &move);
}


void Rogue_CreateTrainerMon(u16 trainerNum, struct Pokemon *party, u8 monIdx, u8 totalMonCount)
{
    u16 species;
    u8 level;
    u8 fixedIV;
    struct RogueMonPreset preset;
    u8 difficultyLevel = gRogueRun.currentDifficulty;
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

    if(FlagGet(FLAG_ROGUE_EASY_TRAINERS))
        fixedIV = 0;
    if(FlagGet(FLAG_ROGUE_HARD_TRAINERS))
        fixedIV = MAX_PER_STAT_IVS;
    else
        fixedIV = isBoss && difficultyLevel >= 3 ? 11 : 0;

    if(!FlagGet(FLAG_ROGUE_HARD_TRAINERS) && difficultyLevel != 0 && (!isBoss || difficultyLevel < 4))
    {
        // Team average is something like -2, -1, -1, 0
        level--;

        if(monIdx == 0)
            level--;

        if(level != 100 && monIdx == totalMonCount - 1)
            level++;
    }

#ifdef ROGUE_DEBUG
    // Just force the level down so it can be one shotted but do the rest of the calcs corectly
    CreateMon(mon, species, 5, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
#else
    CreateMon(mon, species, level, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
#endif

    if(UseCompetitiveMoveset(trainerNum, monIdx, totalMonCount) && SelectNextPreset(species, trainerNum, monIdx, isBoss ? FLAG_SET_SEED_BOSSES : FLAG_SET_SEED_TRAINERS, &preset))
        ApplyMonPreset(mon, level, &preset);
}

static u8 GetCurrentWildEncounterCount()
{
    u16 count = ARRAY_COUNT(gRogueRun.wildEncounters);
    u8 difficultyModifier = GetRoomTypeDifficulty();

    if(difficultyModifier == 2) // Hard route
    {
        // Less encounters on hard route
        count = 2;
    }

    return count;
}

void Rogue_CreateWildMon(u8 area, u16* species, u8* level)
{
    // Note: Don't seed individual encounters
    if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
        u8 maxlevel = CalculateWildLevel();
        u8 levelVariation = min(6, maxlevel - 1);

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
            u8 difficultyModifier = GetRoomTypeDifficulty();
            u16 count = ARRAY_COUNT(gRogueRun.wildEncounters);
            u16 randIdx;

            if(difficultyModifier == 2) // Hard route
            {
                // Less encounters on hard route
                count = 2;
            }
            
            do
            {
                // Prevent recent duplicates when on a run (Don't use this in safari mode though)
                randIdx = Random() % count; 
                *species = gRogueRun.wildEncounters[randIdx];
            }
            while(!GetSafariZoneFlag() && (difficultyModifier != 2) && HistoryBufferContains(&gRogueRun.wildEncounterHistoryBuffer[0], ARRAY_COUNT(gRogueRun.wildEncounterHistoryBuffer), *species));

            gRogueLocal.encounterPreview[randIdx].isVisible = TRUE;

            HistoryBufferPush(&gRogueRun.wildEncounterHistoryBuffer[0], ARRAY_COUNT(gRogueRun.wildEncounterHistoryBuffer), *species);
            *level = maxlevel - (Random() % levelVariation);
        }
    }
}

void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId)
{
    *level = CalculateWildLevel();
}

void Rogue_ModifyEventMon(struct Pokemon* mon)
{
    u16 presetIndex;
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u8 presetCount = gPresetMonTable[species].presetCount;
    u16 statA = (Random() % 6);
    u16 statB = (statA + 1 + (Random() % 5)) % 6;
    u16 temp = 31;

    if(presetCount != 0)
    {
        presetIndex = Random() % presetCount;
        ApplyMonPreset(mon, GetMonData(mon, MON_DATA_LEVEL), &gPresetMonTable[species].presets[presetIndex]);
    }

    SetMonData(mon, MON_DATA_HP_IV + statA, &temp);
    SetMonData(mon, MON_DATA_HP_IV + statB, &temp);

    temp = 0;
    SetMonData(mon, MON_DATA_HELD_ITEM, &temp);
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

static void ApplyMartCapacity(u16 capacity)
{
    u16 randIdx;
    u32 startSeed = gRngRogueValue;

    while(RogueQuery_BufferSize() > capacity)
    {
        randIdx = RogueRandom() % RogueQuery_BufferSize();
        RogueQuery_PopCollapsedIndex(randIdx);
    }

    gRngRogueValue = startSeed;
}

const u16* Rogue_CreateMartContents(u16 itemCategory, u16* minSalePrice)
{
    u16 difficulty;
    u16 itemCapacity = 0; // MAX is 0
    
    if(Rogue_IsRunActive())
        difficulty = gRogueRun.currentDifficulty;
    else
        difficulty = VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY);

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
        difficulty = 13;

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


            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                // Do nothing
            }
            else if(Rogue_IsRunActive())
            {
                if(difficulty <= 7)
                    itemCapacity = 5 + difficulty * 2;
            }
            else
            {
                RogueQuery_ItemsInPriceRange(10, 1000 + difficulty * 810);
            }

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
                if(difficulty <= 0)
                    itemCapacity = 10;
                else if(difficulty <= 1)
                    itemCapacity = 15;
                else if(difficulty <= 3)
                    itemCapacity = 20;
                else if(difficulty <= 5)
                    itemCapacity = 30;
                else if(difficulty <= 7)
                    itemCapacity = 40;
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 1000;
            else
                *minSalePrice = 1500;

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
                if(difficulty <= 0)
                    itemCapacity = 10;
                else if(difficulty <= 1)
                    itemCapacity = 15;
                else if(difficulty <= 3)
                    itemCapacity = 20;
                else if(difficulty <= 5)
                    itemCapacity = 30;
                else if(difficulty <= 7)
                    itemCapacity = 40;
            }
            else if(difficulty <= 5)
            {
                // Remove contents 
                RogueQuery_ItemsInPriceRange(10, 11);
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 1000;
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
                if(difficulty <= 0)
                    itemCapacity = 10;
                else if(difficulty <= 1)
                    itemCapacity = 15;
                else if(difficulty <= 3)
                    itemCapacity = 20;
                else if(difficulty <= 5)
                    itemCapacity = 30;
                else if(difficulty <= 7)
                    itemCapacity = 40;
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 1500;
            else
                *minSalePrice = 3000;
            break;
    };

    RogueQuery_CollapseItemBuffer();

    if(itemCapacity != 0)
    {
        u16 maxGen = VarGet(VAR_ROGUE_ENABLED_GEN_LIMIT);

        if(maxGen > 3)
        {
            // Increase capacity by a little bit to accomadate for extra items when in higher gens
            itemCapacity += (maxGen - 3) * 2;
        }

        ApplyMartCapacity(itemCapacity);
    }

    if(RogueQuery_BufferSize() == 0)
    {
        // If we don't have anything then just use this (Really unlucky to happen)
        RogueQuery_Include(ITEM_TINY_MUSHROOM);
        RogueQuery_CollapseItemBuffer();
    }

    return RogueQuery_BufferPtr();
}

void Rogue_RandomisePartyMon(void)
{
    u8 monIdx = gSpecialVar_0x8004;

    if(monIdx == 255)
    {
        // Entire team
        u8 i;
        u16 queryCount;
        u16 species;
        u16 heldItem;
        u8 targetlevel = CalculatePlayerLevel();

        // Query for the current route type
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();
        RogueQuery_SpeciesExcludeCommon();
        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesAndKeepPreEvo(targetlevel, TRUE);

        queryCount = RogueQuery_UncollapsedSpeciesSize();

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            targetlevel = GetMonData(&gPlayerParty[i], MON_DATA_LEVEL);
            heldItem = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);

            species = RogueQuery_AtUncollapsedIndex(Random() % queryCount);

            ZeroMonData(&gPlayerParty[i]);
            CreateMon(&gPlayerParty[i], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

            SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &heldItem);
        }
    }
    else
    {
        u16 queryCount;
        u16 species;
        u8 targetlevel = GetMonData(&gPlayerParty[monIdx], MON_DATA_LEVEL);
        u16 heldItem = GetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM);

        // Query for the current route type
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();
        RogueQuery_SpeciesExcludeCommon();
        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesAndKeepPreEvo(targetlevel, TRUE);

        queryCount = RogueQuery_UncollapsedSpeciesSize();
        species = RogueQuery_AtUncollapsedIndex(Random() % queryCount);

        ZeroMonData(&gPlayerParty[monIdx]);
        CreateMon(&gPlayerParty[monIdx], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

        SetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM, &heldItem);
    }
}

void Rogue_AlterMonIVs(void)
{
    const u16 delta = 10;

    u16 statId;
    u16 ivAmount;
    u16 monIdx = gSpecialVar_0x8004;
    u16 statOp = gSpecialVar_0x8005;

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
            {
                ivAmount = GetMonData(&gPlayerParty[i], statId);

                if(statOp == 0)
                {
                    ivAmount += delta;
                    ivAmount = min(31, ivAmount);
                }
                else
                {
                    if(ivAmount < delta)
                        ivAmount = 0;
                    else
                        ivAmount -= delta;
                }

                SetMonData(&gPlayerParty[i], statId, &ivAmount);
                CalculateMonStats(&gPlayerParty[i]);
            }
        }
    }
    else
    {
        // Modify just 1 mon
        for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
        {
            ivAmount = GetMonData(&gPlayerParty[monIdx], statId);

            if(statOp == 0)
            {
                ivAmount += delta;
                ivAmount = min(31, ivAmount);
            }
            else
            {
                if(ivAmount < delta)
                    ivAmount = 0;
                else
                    ivAmount -= delta;
            }

            SetMonData(&gPlayerParty[monIdx], statId, &ivAmount);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
    }
}

void Rogue_ApplyStatusToMon(void)
{
    u16 statusAilment;
    u16 monIdx = gSpecialVar_0x8004;

    switch(gSpecialVar_0x8005)
    {
        case 0:
            statusAilment = STATUS1_POISON;
            break;

        case 1:
            statusAilment = STATUS1_PARALYSIS;
            break;

        case 2:
            statusAilment = STATUS1_SLEEP;
            break;

        case 3:
            statusAilment = STATUS1_FREEZE;
            break;

        case 4:
            statusAilment = STATUS1_BURN;
            break;
    }

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            SetMonData(&gPlayerParty[i], MON_DATA_STATUS, &statusAilment);
        }
    }
    else
    {
        SetMonData(&gPlayerParty[monIdx], MON_DATA_STATUS, &statusAilment);
    }
}

void Rogue_ReducePartySize(void)
{
    u16 monIdx = gSpecialVar_0x8004;

    if(monIdx < gPlayerPartyCount)
    {
        RemoveMonAtSlot(monIdx, TRUE);
    }

    VarSet(VAR_ROGUE_MAX_PARTY_SIZE, VarGet(VAR_ROGUE_MAX_PARTY_SIZE) - 1);
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
    RogueQuery_EvolveSpecies(maxlevel - min(6, maxlevel - 1), FALSE);
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteIndex].wildTypeTable, ARRAY_COUNT(gRogueRouteTable[gRogueRun.currentRouteIndex].wildTypeTable));

    RogueQuery_CollapseSpeciesBuffer();

    {
        u8 i;

#ifdef ROGUE_DEBUG
        gDebug_WildOptionCount = RogueQuery_BufferSize();
#endif

        for(i = 0; i < ARRAY_COUNT(gRogueRun.wildEncounters); ++i)
        {
            gRogueLocal.encounterPreview[i].isVisible = FALSE;
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
    RogueQuery_SpeciesExcludeCommon();

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
    u8 playerLevel = CalculatePlayerLevel();

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

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_WILD_DEN)
    {
        return playerLevel - 5;
    }

    if(playerLevel < 10)
    {
        return 4;
    }
    else
    {
        return playerLevel - 7;
    }
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
    u8 currLevel = gRogueRun.currentDifficulty;
    return CalculateBossLevelForDifficulty(currLevel);
}

static u8 CalculatePlayerLevel(void)
{
    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        // 5 rooms the constant badges
        return min(15 + (gRogueRun.currentRoomIdx - 1) * 20, MAX_LEVEL);
    }

    return CalculateBossLevel() - gRogueRun.currentLevelOffset;
}

static u8 CalculateTrainerLevel(u16 trainerNum)
{
    if(IsBossTrainer(trainerNum))
    {
        return CalculatePlayerLevel();
    }
    else
    {
        u8 prevBossLevel;
        u8 nextBossLevel;
        u8 difficultyLevel = gRogueRun.currentDifficulty;
        u8 difficultyModifier = GetRoomTypeDifficulty();

        if(difficultyLevel == 0)
        {
            prevBossLevel = 5;
            nextBossLevel = CalculatePlayerLevel();
        }
        else
        {
            prevBossLevel = CalculateBossLevelForDifficulty(difficultyModifier - 1);
            nextBossLevel = CalculatePlayerLevel();
        }

        prevBossLevel = min(prevBossLevel, nextBossLevel);

        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
        {
            // Not boss trainer so must be EXP trainer
            return prevBossLevel;
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_MINIBOSS || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY)
        {
            return nextBossLevel - 5;
        }

        if(difficultyModifier == 0) // Easy
        {
            return prevBossLevel;
        }
        else if(difficultyModifier == 2) // Hard
        {
            return nextBossLevel - 5;
        }
        else
        {
            return nextBossLevel - 10;
        }
    }
}

static u8 GetRoomTypeDifficulty(void)
{
    return (gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE ? gRogueAdvPath.currentRoomParams.perType.route.difficulty : 1);
}

static bool8 RogueRandomChanceTrainer()
{
    u8 difficultyLevel = gRogueRun.currentDifficulty;
    u8 difficultyModifier = GetRoomTypeDifficulty();
    s32 chance = 4 * (difficultyLevel + 1);

    if(difficultyModifier == 0) // Easy
        chance = max(5, chance - 20);
    else if(difficultyModifier == 2) // Hard
        chance = max(15, chance - 15); // Trainers are hard so slightly less frequent, but harder
    else
        chance = max(10, chance);

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        return 0;
    }

    return RogueRandomChance(chance, FLAG_SET_SEED_TRAINERS);
}

static bool8 RogueRandomChanceItem()
{
    s32 chance;
    u8 difficultyModifier = GetRoomTypeDifficulty();

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
    {
        // Use to give healing items in gym rooms
        chance = 0;
    }
    else
    {
        u8 difficultyLevel = gRogueRun.currentDifficulty;
        
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

    if(difficultyModifier == 0) // Easy
        chance = max(10, chance - 25);
    else if(difficultyModifier == 2) // Hard
        chance = min(100, chance + 25);

    return RogueRandomChance(chance, FLAG_SET_SEED_ITEMS);
}

static bool8 RogueRandomChanceBerry()
{
    u8 chance;
    u8 difficultyLevel = gRogueRun.currentDifficulty;

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

    return RogueRandomChance(chance, FLAG_SET_SEED_ITEMS);
}

static void RandomiseEnabledTrainers(void)
{
    u16 i;
    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY)
    {
        u16 randTrainer = RogueRandomRange(6, FLAG_SET_SEED_TRAINERS);

        // Only enable 1 trainer for legendary room
        for(i = 0; i < ROGUE_TRAINER_COUNT; ++i)
        {
            if(i == randTrainer)
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
    else
    {
        for(i = 0; i < ROGUE_TRAINER_COUNT; ++i)
        {
            if(RogueRandomChanceTrainer())
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
}

static void RandomiseItemContent(u8 difficultyLevel)
{
    u16 queryCount;
    u8 difficultyModifier = GetRoomTypeDifficulty();
    u8 dropRarity = gRogueRouteTable[gRogueRun.currentRouteIndex].dropRarity;

    // Give us 1 room of basic items
    if(gRogueRun.currentRoomIdx > 1 && FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        dropRarity += 10;
    }

    if(difficultyModifier == 0) // Easy
    {
        if(dropRarity != 0)
            --dropRarity;
    }
    else if(difficultyModifier == 2) // Hard
    {
        if(dropRarity != 0)
            ++dropRarity;
    }

    // Queue up random items
    RogueQuery_Clear();

    RogueQuery_ItemsIsValid();
    RogueQuery_ItemsNotInPocket(POCKET_KEY_ITEMS);
    RogueQuery_ItemsNotInPocket(POCKET_BERRIES);

    RogueQuery_ItemsExcludeCommon();

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_MINIBOSS)
    {
        RogueQuery_ItemsInPriceRange(1000 + 500 * difficultyLevel, 2000 + 1600 * difficultyLevel);
#ifdef ROGUE_EXPANSION
        RogueQuery_ItemsExcludeRange(ITEM_TINY_MUSHROOM, ITEM_STRANGE_SOUVENIR);
#else
        RogueQuery_ItemsExcludeRange(ITEM_TINY_MUSHROOM, ITEM_HEART_SCALE);
#endif
        RogueQuery_Include(ITEM_MASTER_BALL);
    }
    else
    {
        RogueQuery_ItemsInPriceRange(50 + 100 * (difficultyLevel + dropRarity), 300 + 800 * (difficultyLevel + dropRarity));

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
            }
        }
    }

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
    {
        RogueQuery_ItemsMedicine();
    }

    RogueQuery_CollapseItemBuffer();
    queryCount = RogueQuery_BufferSize();

#ifdef ROGUE_DEBUG
    gDebug_ItemOptionCount = queryCount;
#endif

    
    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_MINIBOSS)
    {
        // Only need to set 2, but try to make them unique
        VarSet(VAR_ROGUE_ITEM0, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
        do
        {
            VarSet(VAR_ROGUE_ITEM1, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
        }
        while(queryCount >= 2 && VarGet(VAR_ROGUE_ITEM0) == VarGet(VAR_ROGUE_ITEM1));
    }
    else
    {
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
}

static void RandomiseEnabledItems(void)
{
    s32 i;
    u8 difficultyLevel = gRogueRun.currentDifficulty;

    for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
    {
        if(RogueRandomChanceItem())
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
        if(RogueRandomChanceBerry())
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