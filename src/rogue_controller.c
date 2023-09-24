#include "global.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/event_objects.h"
#include "constants/heal_locations.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/rogue.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/trainer_types.h"
#include "constants/weather.h"
#include "data.h"
#include "gba/isagbprint.h"

#include "battle.h"
#include "battle_setup.h"
#include "berry.h"
#include "event_data.h"
#include "graphics.h"
#include "item.h"
#include "load_save.h"
#include "malloc.h"
#include "main.h"
#include "money.h"
#include "m4a.h"
#include "overworld.h"
#include "party_menu.h"
#include "palette.h"
#include "play_time.h"
#include "player_pc.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "rtc.h"
#include "safari_zone.h"
#include "script.h"
#include "siirtc.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"

#include "rogue.h"
#include "rogue_assistant.h"
#include "rogue_automation.h"
#include "rogue_adventurepaths.h"
#include "rogue_campaign.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_followmon.h"
#include "rogue_hub.h"
#include "rogue_player_customisation.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_ridemon.h"
#include "rogue_safari.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_timeofday.h"
#include "rogue_trainers.h"


#define ROGUE_TRAINER_COUNT (FLAG_ROGUE_TRAINER_END - FLAG_ROGUE_TRAINER_START + 1)
#define ROGUE_ITEM_COUNT (FLAG_ROGUE_ITEM_END - FLAG_ROGUE_ITEM_START + 1)

#ifdef ROGUE_DEBUG
EWRAM_DATA u8 gDebug_CurrentTab = 0;
EWRAM_DATA u8 gDebug_WildOptionCount = 0;
EWRAM_DATA u8 gDebug_ItemOptionCount = 0;
EWRAM_DATA u8 gDebug_TrainerOptionCount = 0;

extern const u8 gText_RogueDebug_Header[];
extern const u8 gText_RogueDebug_Save[];
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

extern const u8 gText_TrainerName_Default[];

#define LAB_MON_COUNT 3

struct RouteMonPreview
{
    u8 monSpriteId;
};

// Temp data only ever stored in RAM
struct RogueLocalData
{
    struct RouteMonPreview encounterPreview[WILD_ENCOUNTER_GRASS_CAPACITY];
    u16 wildEncounterHistoryBuffer[3];
    bool8 runningToggleActive : 1;
    bool8 hasQuickLoadPending : 1;
    bool8 hasValidQuickSave : 1;
    bool8 hasSaveWarningPending : 1;
    bool8 hasVersionUpdateMsgPending : 1;
};

struct RogueLabEncounterData
{
    struct Pokemon party[LAB_MON_COUNT];
};

typedef u16 hot_track_dat;

struct RogueHotTracking
{
    hot_track_dat initSeed;
    hot_track_dat rollingSeed;
    hot_track_dat triggerCount;
    hot_track_dat triggerMin;
    hot_track_dat triggerMax;
    hot_track_dat triggerAccumulation;
};

static struct RogueHotTracking gRogueHotTracking;

EWRAM_DATA struct RogueRunData gRogueRun = {};

// Temporary data, that isn't remembered
EWRAM_DATA struct RogueLocalData gRogueLocal = {};
EWRAM_DATA struct RogueAdvPath gRogueAdvPath = {};

// TODO - Fix this and make sure it is tracked in gRogueRun and saved correctly!!! 
EWRAM_DATA struct RogueLabEncounterData gRogueLabEncounterData = {};


static void ResetHotTracking();

static u8 CalculateWildLevel(u8 variation);

static bool8 CanLearnMoveByLvl(u16 species, u16 move, s32 level);

static u8 GetCurrentWildEncounterCount(void);
static u16 GetWildGrassEncounter(u8 index);
static u16 GetWildWaterEncounter(u8 index);
static u16 GetWildEncounterIndexFor(u16 species);

static void SwapMons(u8 aIdx, u8 bIdx, struct Pokemon *party);
static void SwapMonItems(u8 aIdx, u8 bIdx, struct Pokemon *party);

static void RandomiseSafariWildEncounters(void);
static void RandomiseWildEncounters(void);
static void RandomiseFishingEncounters(void);
static void ResetTrainerBattles(void);
static void RandomiseEnabledItems(void);
static void RandomiseBerryTrees(void);

static void RandomiseCharmItems(void);

u16 RogueRandomRange(u16 range, u8 flag)
{
    // Always use rogue random to avoid seeding issues based on flag
    u16 res = RogueRandom();

    if(range <= 1)
        return 0;

    return res % range;

    //if(FlagGet(FLAG_SET_SEED_ENABLED) && (flag == 0 || FlagGet(flag)))
    //    return res % range;
    //else
    //    return Random() % range;
}

bool8 RogueRandomChance(u8 chance, u16 seedFlag)
{
    if(chance == 0)
        return FALSE;
    else if(chance >= 100)
        return TRUE;

    return (RogueRandomRange(100, seedFlag) + 1) <= chance;
}

u16 Rogue_GetStartSeed(void)
{
    u32 word0 = gSaveBlock1Ptr->dewfordTrends[0].words[0];
    u32 word1 = gSaveBlock1Ptr->dewfordTrends[0].words[1];
    u32 offset = 3;

    //if(Rogue_IsRunActive())
    //{
    //    offset = gRogueRun.enteredRoomCounter * 3;
    //}

    return (u16)(word0 + word1 * offset);
}

u16 Rogue_GetShinyOdds(void)
{
    return 512;
}

static u16 GetEncounterChainShinyOdds(u8 count)
{
    u16 baseOdds = Rogue_GetShinyOdds();

    // By the time we reach 32 encounters, we want to be at max odds
    // Don't start increasing shiny rate until we pass 8 encounters
    if(count <= 8)
    {
        return baseOdds;
    }
    else
    {
        u16 range = 32 - 8;
        count = min(count - 8, range);

        return max(32, baseOdds - ((baseOdds * count) / range));
    }
}

bool8 Rogue_IsRunActive(void)
{
    return FlagGet(FLAG_ROGUE_RUN_ACTIVE);
}

bool8 Rogue_InWildSafari(void)
{
    return FlagGet(FLAG_ROGUE_WILD_SAFARI);
}

bool8 Rogue_ForceExpAll(void)
{
    return Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EXP_ALL);
}

bool8 Rogue_FastBattleAnims(void)
{
    if(GetSafariZoneFlag())
    {
        return TRUE;
    }

    return !Rogue_UseKeyBattleAnims();
}

bool8 Rogue_UseKeyBattleAnims(void)
{
    if(Rogue_IsRunActive())
    {
        // Force slow anims for bosses
        if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && Rogue_IsKeyTrainer(gTrainerBattleOpponent_A))
            return TRUE;

        // Force slow anims for legendaries
        if((gBattleTypeFlags & BATTLE_TYPE_LEGENDARY) != 0)
            return TRUE;
    }

    return FALSE;
}

bool8 Rogue_GetBattleAnimsEnabled(void)
{
    return !(Rogue_UseKeyBattleAnims() ? gSaveBlock2Ptr->optionsBossBattleSceneOff : gSaveBlock2Ptr->optionsDefaultBattleSceneOff);
}

extern const struct Song gSongTable[];

u8 Rogue_ModifySoundVolume(struct MusicPlayerInfo *mplayInfo, u8 volume, u16 soundType)
{
    // 10 is eqv of 100%
    u8 audioLevel = 10;

    switch (soundType)
    {
    case ROGUE_SOUND_TYPE_CRY:
        // Don't modify this?
        break;
    
    default:
        if(mplayInfo == &gMPlayInfo_BGM)
        {
            audioLevel = gSaveBlock2Ptr->optionsSoundChannelBGM;
        }
        else 
        {
            if(
                mplayInfo->songHeader == gSongTable[SE_SELECT].header ||
                mplayInfo->songHeader == gSongTable[SE_DEX_SCROLL].header ||
                mplayInfo->songHeader == gSongTable[SE_PIN].header
            )
            {
                // Just UI sound effects
                audioLevel = gSaveBlock2Ptr->optionsSoundChannelSE;
            }
            else if(gMain.inBattle)
            {
                // Assume all sounds are battle effects
                audioLevel = gSaveBlock2Ptr->optionsSoundChannelBattleSE;
            }
        }
        break;
    }

    if(audioLevel != 10)
    {
        return (volume * audioLevel) / 10;
    }

    return volume;
}


void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    
    if(Rogue_IsRunActive() && species != SPECIES_NONE)
    {
        u8 targetLevel = Rogue_CalculatePlayerMonLvl();
        u8 maxLevel = Rogue_CalculateBossMonLvl();
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
                if(Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_OVER_LVL))
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
    // We don't give out EVs normally instead we reward at end of trainer battles based on nature
    *multiplier = 0;
}

void Rogue_ModifyCatchRate(u16 species, u16* catchRate, u16* ballMultiplier)
{ 
    if(GetSafariZoneFlag() || Rogue_InWildSafari() || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INSTANT_CAPTURE))
    {
        *ballMultiplier = 12345; // Masterball equiv
    }
    else if(Rogue_IsRunActive())
    {
        u16 startMultiplier = *ballMultiplier;
        u8 difficulty = gRogueRun.currentDifficulty;
        u8 wildEncounterIndex = GetWildEncounterIndexFor(species);
        u8 speciesCatchCount = 0;

        if(wildEncounterIndex != WILD_ENCOUNTER_TOTAL_CAPACITY)
            speciesCatchCount = gRogueRun.wildEncounters.catchCounts[wildEncounterIndex];
        
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY)
        {
            // Want to make legendaries hard to catch than other mons in the area
            difficulty += 1;
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

        // Modify the catch rate based on how many times we've caught this mon
        if(speciesCatchCount > 2)
        {
            // Already caught a few, so use the base multiplier
            *ballMultiplier = startMultiplier;
        }
        else if(speciesCatchCount > 4)
        {
            // Now we want to discourage catching more mons
            *ballMultiplier = max(1, startMultiplier / 2);
        }

        // Apply charms
        {
            u32 perc;
            u16 rateInc = GetCharmValue(EFFECT_CATCH_RATE);
            u16 rateDec = GetCurseValue(EFFECT_CATCH_RATE);
            
            if(rateInc > rateDec)
            {
                rateInc = rateInc - rateDec;
                rateDec = 0;
            }
            else
            {
                rateDec = rateDec - rateInc;
                rateInc = 0;
            }

            perc = 100;

            if(rateInc > 0)
            {
                perc = 100 + rateInc;
            }
            else if(rateDec > 0)
            {
                perc = (rateDec > 100 ? 0 : 100 - rateDec);
            }

            if(perc != 100)
                *ballMultiplier = ((u32)*ballMultiplier * perc) / 100;
        }

        // After we've caught a few remove the catch rate buff
        if(speciesCatchCount <= 3)
        {
            // Equiv to Snorlax
            if(*catchRate < 25)
                *catchRate = 25;
        }
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

        // Apply charms to IVs
        {
            u16 i;
            u16 value;

            u16 ivInc = GetCharmValue(EFFECT_WILD_IV_RATE);
            u16 ivDec = GetCurseValue(EFFECT_WILD_IV_RATE);

            if(ivInc > ivDec)
            {
                ivInc = ivInc - ivDec;
                ivDec = 0;
            }
            else
            {
                ivDec = ivDec - ivInc;
                ivInc = 0;
            }

            ivInc = min(ivInc, 31);
            ivDec = min(ivDec, 31);

            for(i = 0; i < 6; ++i)
            {
                value = GetMonData(mon, MON_DATA_HP_IV + i);

                if(ivInc != 0)
                {
                    value = max(ivInc, value);
                }

                if(ivDec != 0)
                {
                    value = min(31 - ivDec, value);
                }

                SetMonData(mon, MON_DATA_HP_IV + i, &value);
            }
        }
    
        // Increment catch counter for in route mons
        {
            u16 species = GetMonData(mon, MON_DATA_SPECIES);
            u8 index = GetWildEncounterIndexFor(species);

            if(index != WILD_ENCOUNTER_TOTAL_CAPACITY && gRogueRun.wildEncounters.catchCounts[index] != 255)
            {
                ++gRogueRun.wildEncounters.catchCounts[index];
            }
        }
    }
}

const void* Rogue_ModifyPaletteLoad(const void* input)
{
    if(input == &gObjectEventPal_PlayerPlaceholder[0])
    {
        return RoguePlayer_GetOverworldPalette();
    }

    return input;
}

bool8 Rogue_ModifyPaletteDecompress(const u32* input, void* buffer)
{
    const u16* overrideBuffer = NULL;

    if(input == gTrainerPalette_PlayerFrontPlaceholder)
    {
        overrideBuffer = RoguePlayer_GetTrainerFrontPalette();
    }

    if(input == gTrainerPalette_PlayerBackPlaceholder)
    {
        overrideBuffer = RoguePlayer_GetTrainerBackPalette();
    }

    if(overrideBuffer != NULL)
    {
        // Copy over from override buffer into actual output (only really needed if buffer isn't gDecompressBuffer)
        if(overrideBuffer != buffer)
        {
            u8 i;
            u16* writeBuffer = (u16*)buffer;

            for(i = 0; i < 16; ++i) // assume 16 palette slots (We currently don't have any use cases outside of this anyway)
                writeBuffer[i] = overrideBuffer[i];
        }

        return TRUE;
    }

    return FALSE;
}

void Rogue_ModifyOverworldPalette(u16 offset, u16 count)
{
    RogueToD_ModifyOverworldPalette(offset, count);
}

void Rogue_ModifyBattlePalette(u16 offset, u16 count)
{
    RogueToD_ModifyBattlePalette(offset, count);
}

void Rogue_ModifyBattleWinnings(u16 trainerNum, u32* money)
{
    if(Rogue_IsRunActive())
    {
        // Once we've gotten champion we want to give a bit more money 
        u8 difficulty = gRogueRun.currentDifficulty;
        u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();

        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_BOSS)
        {
            if(Rogue_IsKeyTrainer(trainerNum))
            {
                u8 difficulty = gRogueRun.currentDifficulty;
                *money = (difficulty + 1) * 500;
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

        // Snap/Floor to multiple of ten
        if(*money > 100)
        {
            *money /= 10;
            *money *= 10;
        }
        else if(*money != 0)
        {
            *money = 100;
        }
    }
}

void Rogue_ModifyBattleWaitTime(u16* waitTime, bool8 awaitingMessage)
{
    u8 difficulty = Rogue_IsRunActive() ? gRogueRun.currentDifficulty : 0;

    // We won't modify absolute wait flags
    if(*waitTime & B_WAIT_TIME_ABSOLUTE)
    {
        *waitTime &= ~B_WAIT_TIME_ABSOLUTE;
        return;
    }

    if(Rogue_FastBattleAnims())
    {
        *waitTime = awaitingMessage ? 8 : 0;
    }
    else if(difficulty < (ROGUE_MAX_BOSS_COUNT - 1)) // Go at default speed for final fight
    {
        if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && Rogue_IsAnyBossTrainer(gTrainerBattleOpponent_A))
            // Still run faster and default game because it's way too slow :(
            *waitTime = *waitTime / 2;
        else
            // Go faster, but not quite gym leader slow
            *waitTime = *waitTime / 4;
    }

    if(!Rogue_GetBattleAnimsEnabled())
    {
        // If we don't have anims on wait message for at least a little bit
        *waitTime = max(4, *waitTime);
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
    //else if(difficulty == (ROGUE_MAX_BOSS_COUNT - 1))
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
    // Hisui is classes as gen8
    if(species >= SPECIES_WYRDEER && species <= SPECIES_ENAMORUS)
        return 8;

    if(species >= SPECIES_RATTATA_ALOLAN && species <= SPECIES_MAROWAK_ALOLAN)
        return 7;
    if(species >= SPECIES_MEOWTH_GALARIAN && species <= SPECIES_STUNFISK_GALARIAN)
        return 8;

    // Hisui is classes as gen8
    if(species >= SPECIES_GROWLITHE_HISUIAN && species <= SPECIES_DECIDUEYE_HISUIAN)
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
        
        //case SPECIES_ZYGARDE_COMPLETE:
        //    return 6;

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

    // Alternate forms
    switch(species)
    {
        case SPECIES_MEOWSTIC_FEMALE:
            return 7;

        case SPECIES_INDEEDEE_FEMALE:
            return 8;
    }

    if(species >= SPECIES_LYCANROC_MIDNIGHT && species <= SPECIES_LYCANROC_DUSK)
        return 7;

    if(species >= SPECIES_TOXTRICITY_LOW_KEY && species <= SPECIES_ALCREMIE_RAINBOW_SWIRL)
        return 8;
#endif
    
    return 0;
}

u8 ItemToGen(u16 item)
{
    if(!Rogue_IsRunActive() && FlagGet(FLAG_ROGUE_MET_POKABBIE))
    {
        // We want all items to appear in the hub, so long as we've unlocked the expanded mode
        return 1;
    }

#ifdef ROGUE_EXPANSION
    if(item >= ITEM_FLAME_PLATE && item <= ITEM_PIXIE_PLATE)
        return 4;

    if(item >= ITEM_DOUSE_DRIVE && item <= ITEM_CHILL_DRIVE)
        return 5;

    if(item >= ITEM_FIRE_MEMORY && item <= ITEM_FAIRY_MEMORY)
        return 7;

    if(item >= ITEM_RED_NECTAR && item <= ITEM_PURPLE_NECTAR)
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
        case ITEM_GRISEOUS_CORE: // <- Always have griseous core avaliable, so can transform as we did in og game
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
        
        // Hisui items gen8
        case ITEM_BLACK_AUGURITE:
        case ITEM_PEAT_BLOCK:
        case ITEM_ADAMANT_CRYSTAL:
        case ITEM_LUSTROUS_GLOBE:
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
    if(Rogue_IsRunActive())
        return gRogueRun.megasEnabled; // cached result
    else
        return CheckBagHasItem(ITEM_MEGA_RING, 1);

#else
    return FALSE;
#endif
}

bool8 IsZMovesEnabled(void)
{
#ifdef ROGUE_EXPANSION
    if(Rogue_IsRunActive())
        return gRogueRun.zMovesEnabled; // cached result
    else
        return CheckBagHasItem(ITEM_Z_POWER_RING, 1);
#else
    return FALSE;
#endif
}

bool8 IsDynamaxEnabled(void)
{
#ifdef ROGUE_EXPANSION
    return FALSE;
#else
    return FALSE;
#endif
}

static u32 GetPresetMonFlags(u16 species)
{
    u32 flags;
#ifdef ROGUE_EXPANSION
    u16 species2;;
#endif
    
    flags = gPresetMonTable[species].flags;

#ifdef ROGUE_EXPANSION
    species2 = GET_BASE_SPECIES_ID(species);
    if(species2 != species)
        flags |= gPresetMonTable[species2].flags;
#endif

    return flags;
}

bool8 CheckPresetMonFlags(u16 species, u32 flag)
{
    return (GetPresetMonFlags(species) & flag) != 0;
}

#if defined(ROGUE_DEBUG)

u16 Debug_MiniMenuHeight(void)
{
    u16 height = 10;
    return height;
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

u8* Debug_GetMiniMenuContent(void)
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
        u8 playerLevel = Rogue_CalculatePlayerMonLvl();
        u8 wildLevel = CalculateWildLevel(0);

        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);

        if(FlagGet(FLAG_SET_SEED_ENABLED))
        {
            strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, Rogue_GetStartSeed());
        }

        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Save, RogueSave_GetVersionId());
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Room, gRogueRun.enteredRoomCounter);
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
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_AdvCount, gRogueAdvPath.roomCount);
        //strPointer = AppendNumberField(strPointer, gText_RogueDebug_X, gRogueAdvPath.currentNodeX);
        //strPointer = AppendNumberField(strPointer, gText_RogueDebug_Y, gRogueAdvPath.currentNodeY);
    }
#ifdef ROGUE_FEATURE_AUTOMATION
    // Automation tab
    //
    else
    {
        u16 i;

        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);

        for(i = 0; i < 8; ++i) // Rogue_AutomationBufferSize()
        {
            strPointer = AppendNumberField(strPointer, gText_RogueDebug_X, Rogue_ReadAutomationBuffer(i));
        }
    }
#else
    // Misc. debug tab
    //
    else
    {

        strPointer = StringAppend(strPointer, gText_RogueDebug_Header);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, gSaveBlock1Ptr->dewfordTrends[0].words[0]);
        strPointer = AppendNumberField(strPointer, gText_RogueDebug_Seed, gSaveBlock1Ptr->dewfordTrends[0].words[1]);
    }
#endif

    return gStringVar4;
}

#endif

bool8 Rogue_ShouldShowMiniMenu(void)
{
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return TRUE;

    if(GetSafariZoneFlag())
        return FALSE;

    return TRUE;
}

u16 Rogue_MiniMenuHeight(void)
{
    u16 height = Rogue_IsRunActive() ? 3 : 1;

#if defined(ROGUE_DEBUG)
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return Debug_MiniMenuHeight();
#endif

    if(GetSafariZoneFlag())
    {
        height = 3;
    }
    else
    {
        if(Rogue_IsActiveCampaignScored())
        {
            ++height;
        }
    }

    return height * 2;
}

extern const u8 gText_StatusRoute[];
extern const u8 gText_StatusBadges[];
extern const u8 gText_StatusScore[];
extern const u8 gText_StatusTimer[];
extern const u8 gText_StatusClock[];
extern const u8 gText_StatusSeasonSpring[];
extern const u8 gText_StatusSeasonSummer[];
extern const u8 gText_StatusSeasonAutumn[];
extern const u8 gText_StatusSeasonWinter[];

u8* Rogue_GetMiniMenuContent(void)
{
    u8* strPointer = &gStringVar4[0];

#if defined(ROGUE_DEBUG)
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return Debug_GetMiniMenuContent();
#endif

    *strPointer = EOS;

    // Season stamp
    switch(RogueToD_GetSeason())
    {
        case SEASON_SPRING:
            strPointer = StringAppend(strPointer, gText_StatusSeasonSpring);
            break;
        case SEASON_SUMMER:
            strPointer = StringAppend(strPointer, gText_StatusSeasonSummer);
            break;
        case SEASON_AUTUMN:
            strPointer = StringAppend(strPointer, gText_StatusSeasonAutumn);
            break;
        case SEASON_WINTER:
            strPointer = StringAppend(strPointer, gText_StatusSeasonWinter);
            break;
    }

    // Clock
    ConvertIntToDecimalStringN(gStringVar1, RogueToD_GetHours(), STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar2, RogueToD_GetMinutes(), STR_CONV_MODE_LEADING_ZEROS, 2);

    StringExpandPlaceholders(gStringVar3, gText_StatusClock);
    strPointer = StringAppend(strPointer, gStringVar3);
    
    if(Rogue_IsRunActive())
    {
        // Run time
        ConvertIntToDecimalStringN(gStringVar1, gSaveBlock2Ptr->playTimeHours, STR_CONV_MODE_RIGHT_ALIGN, 3);
        ConvertIntToDecimalStringN(gStringVar2, gSaveBlock2Ptr->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(gStringVar3, gText_StatusTimer);
        strPointer = StringAppend(strPointer, gStringVar3);

        // Badges
        ConvertIntToDecimalStringN(gStringVar1, gRogueRun.currentDifficulty, STR_CONV_MODE_RIGHT_ALIGN, 4);
        StringExpandPlaceholders(gStringVar3, gText_StatusBadges);
        strPointer = StringAppend(strPointer, gStringVar3);
    }

    // Score
    if(Rogue_IsActiveCampaignScored())
    {
        ConvertIntToDecimalStringN(gStringVar1, Rogue_GetCampaignScore(), STR_CONV_MODE_RIGHT_ALIGN, 6);

        StringExpandPlaceholders(gStringVar3, gText_StatusScore);
        strPointer = StringAppend(strPointer, gStringVar3);
    }

    *(strPointer - 1) = EOS; // Remove trailing \n
    return gStringVar4;
}

void Rogue_CreateMiniMenuExtraGFX(void)
{
    u8 i;
    u8 palIndex;
    u8 oamPriority = 0; // Render infront of background
    u16 palBuffer[16];

#if defined(ROGUE_DEBUG)
    // Don't show whilst info panel is visible
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return;
#endif

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE || GetSafariZoneFlag())
    {
        bool8 isVisible;
        u16 yOffset = 24 + Rogue_MiniMenuHeight() * 8;

        //LoadMonIconPalettes();

        for(i = 0; i < GetCurrentWildEncounterCount(); ++i)
        {
            //u8 paletteOffset = i;
            u8 paletteOffset = 0; // No palette offset as we're going to greyscale and share anyway
            u16 targetSpecies = GetWildGrassEncounter(i);

            isVisible = GetSetPokedexSpeciesFlag(targetSpecies, FLAG_GET_SEEN);

            if(isVisible)
            {
                LoadMonIconPaletteCustomOffset(targetSpecies, paletteOffset);

                gRogueLocal.encounterPreview[i].monSpriteId = CreateMonIconCustomPaletteOffset(targetSpecies, SpriteCallbackDummy, (14 + (i % 3) * 32), yOffset + (i / 3) * 32, oamPriority, paletteOffset);
            }
            else
            {
                LoadMonIconPaletteCustomOffset(SPECIES_NONE, paletteOffset);
                gRogueLocal.encounterPreview[i].monSpriteId = CreateMissingMonIcon(SpriteCallbackDummy, (14 + (i % 3) * 32), yOffset + (i / 3) * 32, 0, paletteOffset);
            }

            // Have to grey out icon as I can't figure out why custom palette offsets still seem to be stomping over each other
            // The best guess I have is that the overworld palette is doing some extra behaviour but who knows
            palIndex = IndexOfSpritePaletteTag(gSprites[gRogueLocal.encounterPreview[i].monSpriteId].template->paletteTag);
            CpuCopy16(&gPlttBufferUnfaded[0x100 + palIndex * 16], &palBuffer[0], 32);
            //TintPalette_CustomTone(&palBuffer[0], 16, 510, 510, 510);
            TintPalette_GrayScale2(&palBuffer[0], 16);
            LoadPalette(&palBuffer[0], 0x100 + palIndex * 16, 32);
        }
    }
}

void Rogue_RemoveMiniMenuExtraGFX(void)
{
    u8 i;

#if defined(ROGUE_DEBUG)
    // Don't show whilst info panel is visible
    if(RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INFO_PANEL))
        return;
#endif

    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE || GetSafariZoneFlag())
    {
        bool8 isVisible;

        for(i = 0; i < GetCurrentWildEncounterCount(); ++i)
        {
            u8 paletteOffset = i;
            u16 targetSpecies = GetWildGrassEncounter(i);

            isVisible = GetSetPokedexSpeciesFlag(targetSpecies, FLAG_GET_SEEN);

            if(isVisible)
                FreeMonIconPaletteCustomOffset(GetIconSpeciesNoPersonality(targetSpecies), paletteOffset);
            else
                FreeMonIconPaletteCustomOffset(GetIconSpeciesNoPersonality(SPECIES_NONE), paletteOffset);

            if(gRogueLocal.encounterPreview[i].monSpriteId != SPRITE_NONE)
                FreeAndDestroyMonIconSprite(&gSprites[gRogueLocal.encounterPreview[i].monSpriteId]);

            gRogueLocal.encounterPreview[i].monSpriteId = SPRITE_NONE;
        }

        //FreeMonIconPalettes();
    }
}

struct StarterSelectionData
{
    u16 species[3];
    u8 count;
};

int GetMovePower(u16 move, u8 moveType, u16 defType1, u16 defType2, u16 defAbility, u16 mode);

static u8 SelectStartMons_CalculateWeight(u16 index, u16 species, void* data)
{
    u8 i;
    u16 starterSpecies;
    struct StarterSelectionData* starters = (struct StarterSelectionData*)data;
    //u8 weight = 1;

    for(i = 0; i < starters->count; ++i)
    {
        // Don't dupe starters
        if(starters->species[i] == species)
            return 0;

        starterSpecies = starters->species[i];

        if(gBaseStats[starterSpecies].type1 == gBaseStats[species].type1 || gBaseStats[starterSpecies].type2 == gBaseStats[species].type1)
            return 0;

        if(gBaseStats[species].type1 != gBaseStats[species].type2)
        {
            if(gBaseStats[starterSpecies].type1 == gBaseStats[species].type2 || gBaseStats[starterSpecies].type2 == gBaseStats[species].type2)
                return 0;
        }
    }

    // Only have weight if types don't overlap
    return 1;
}

static void SelectStartMons(bool8 isSeeded)
{
    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);
    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(2, FALSE, FALSE); // to force mons to fit gen settings
    RogueMonQuery_AnyActiveEvos(QUERY_FUNC_INCLUDE, FALSE);

    {
        u8 i;
        struct StarterSelectionData starters;
        starters.count = 0;

        RogueWeightQuery_Begin();

        for(i = 0; i < ARRAY_COUNT(starters.species); ++i)
        {
            if(i == 0)
                RogueWeightQuery_FillWeights(1);
            else
                RogueWeightQuery_CalculateWeights(SelectStartMons_CalculateWeight, &starters);

            starters.species[i] = RogueWeightQuery_SelectRandomFromWeights(isSeeded ? RogueRandom() : Random());
            starters.count = i + 1;
        }

        RogueWeightQuery_End();
        
        VarSet(VAR_ROGUE_STARTER0, starters.species[0]);
        VarSet(VAR_ROGUE_STARTER1, starters.species[1]);
        VarSet(VAR_ROGUE_STARTER2, starters.species[2]);

#ifdef ROGUE_DEBUG
        //VarSet(VAR_ROGUE_STARTER0, SPECIES_EEVEE);
        //VarSet(VAR_ROGUE_STARTER1, SPECIES_CASTFORM);
#endif
    }

    RogueMonQuery_End();
}

static void ClearPokemonHeldItems(void)
{
    struct BoxPokemon* boxMon;
    u16 boxId, boxPosition;
    u16 itemId = ITEM_NONE;

    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            boxMon = GetBoxedMonPtr(boxId, boxPosition);

            if(GetBoxMonData(boxMon, MON_DATA_SPECIES) != SPECIES_NONE)
                SetBoxMonData(boxMon, MON_DATA_HELD_ITEM, &itemId);
        }
    }

    for(boxId = 0; boxId < gPlayerPartyCount; ++boxId)
    {
        if(GetMonData(&gPlayerParty[boxId], MON_DATA_SPECIES) != SPECIES_NONE)
                SetMonData(&gPlayerParty[boxId], MON_DATA_HELD_ITEM, &itemId);
    }
}

void Rogue_ResetConfigHubSettings(void)
{
    // TODO - Replace this??

    // Seed settings
    FlagClear(FLAG_SET_SEED_ENABLED);
    FlagSet(FLAG_SET_SEED_ITEMS);
    FlagSet(FLAG_SET_SEED_TRAINERS);
    FlagSet(FLAG_SET_SEED_BOSSES);
    FlagSet(FLAG_SET_SEED_WILDMONS);
    
    // Basic settings
    FlagClear(FLAG_ROGUE_EASY_ITEMS);
    FlagClear(FLAG_ROGUE_HARD_ITEMS);

    // Expansion Room settings
    VarSet(VAR_ROGUE_ENABLED_GEN_LIMIT, 3);
    VarSet(VAR_ROGUE_REGION_DEX_LIMIT, 0);

    FlagSet(FLAG_ROGUE_HOENN_ROUTES);
    FlagSet(FLAG_ROGUE_HOENN_BOSSES);

    FlagSet(FLAG_ROGUE_KANTO_ROUTES);
    FlagSet(FLAG_ROGUE_JOHTO_ROUTES);

    FlagClear(FLAG_ROGUE_KANTO_BOSSES);
    FlagClear(FLAG_ROGUE_JOHTO_BOSSES);
}

void Rogue_OnNewGame(void)
{
    RogueSave_ClearData();

    RoguePlayer_SetNewGameOutfit();
    StringCopy(gSaveBlock2Ptr->playerName, gText_TrainerName_Default);

    StringCopy(gSaveBlock2Ptr->pokemonHubName, gText_ExpandedPlaceholder_PokemonHub);
    
    SetMoney(&gSaveBlock1Ptr->money, 0);
    memset(&gRogueLocal, 0, sizeof(gRogueLocal));

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_WILD_SAFARI);
    FlagClear(FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE);
    FlagClear(FLAG_ROGUE_LVL_TUTORIAL);

    FlagClear(FLAG_ROGUE_PRE_RELEASE_COMPAT_WARNING);

    FlagSet(FLAG_ROGUE_HOENN_ROUTES);
    FlagSet(FLAG_ROGUE_HOENN_BOSSES);

    FlagSet(FLAG_ROGUE_KANTO_ROUTES);
    FlagSet(FLAG_ROGUE_JOHTO_ROUTES);

    FlagClear(FLAG_ROGUE_KANTO_BOSSES);
    FlagClear(FLAG_ROGUE_JOHTO_BOSSES);

#ifdef ROGUE_EXPANSION
    FlagSet(FLAG_ROGUE_EXPANSION_ACTIVE);
#else
    FlagClear(FLAG_ROGUE_EXPANSION_ACTIVE);
#endif

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    VarSet(VAR_ROGUE_DESIRED_CAMPAIGN, ROGUE_CAMPAIGN_NONE);

    Rogue_ResetSettingsToDefaults();
    Rogue_ResetConfigHubSettings();

    VarSet(VAR_ROGUE_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, 0);
    VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, 0);
    VarSet(VAR_ROGUE_ADVENTURE_MONEY, 0);
    VarSet(VAR_ROGUE_DESIRED_WEATHER, WEATHER_NONE);

    VarSet(VAR_ROGUE_REGION_DEX_LIMIT, 0);
    VarSet(VAR_ROGUE_DESIRED_CAMPAIGN, ROGUE_CAMPAIGN_NONE);

    FlagSet(FLAG_SYS_B_DASH);
    EnableNationalPokedex();

    RogueToD_SetTime(60 * 10);

    SetLastHealLocationWarp(HEAL_LOCATION_ROGUE_HUB);

    ClearBerryTrees();
    SelectStartMons(FALSE);

    ResetQuestStateAfter(0);
    Rogue_ResetCampaignAfter(0);
    RogueHub_ClearProgress();

#ifdef ROGUE_DEBUG
    FlagClear(FLAG_ROGUE_DEBUG_DISABLED);
#else
    FlagSet(FLAG_ROGUE_DEBUG_DISABLED);
#endif

    Rogue_ClearPopupQueue();
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
    //gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    //gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    //gSaveBlock2Ptr->regionMapZoom = FALSE;
}

extern const u8 Rogue_QuickSaveLoad[];
extern const u8 Rogue_QuickSaveVersionWarning[];
extern const u8 Rogue_QuickSaveVersionUpdate[];

void Rogue_NotifySaveVersionUpdated(u16 fromNumber, u16 toNumber)
{
    if(Rogue_IsRunActive())
        gRogueLocal.hasSaveWarningPending = TRUE;
    else
        gRogueLocal.hasVersionUpdateMsgPending = TRUE;

    // TODO - Hook up warnings here??
    //if(IsPreReleaseCompatVersion(gSaveBlock1Ptr->rogueCompatVersion))
    //    FlagSet(FLAG_ROGUE_PRE_RELEASE_COMPAT_WARNING);
}

void Rogue_NotifySaveLoaded(void)
{
    gRogueLocal.hasQuickLoadPending = FALSE;

    if(Rogue_IsRunActive() && !FlagGet(FLAG_ROGUE_RUN_COMPLETED))
    {
        gRogueLocal.hasQuickLoadPending = TRUE;
    }
}

bool8 Rogue_OnProcessPlayerFieldInput(void)
{
    if(gRogueLocal.hasSaveWarningPending)
    {
        gRogueLocal.hasSaveWarningPending = FALSE;
        ScriptContext1_SetupScript(Rogue_QuickSaveVersionWarning);
        return TRUE;
    }
    else if(gRogueLocal.hasVersionUpdateMsgPending)
    {
        gRogueLocal.hasVersionUpdateMsgPending = FALSE;
        ScriptContext1_SetupScript(Rogue_QuickSaveVersionUpdate);
        return TRUE;
    }
    else if(!RogueDebug_GetConfigToggle(DEBUG_TOGGLE_ALLOW_SAVE_SCUM) && gRogueLocal.hasQuickLoadPending)
    {
        gRogueLocal.hasQuickLoadPending = FALSE;

        VarSet(VAR_0x8004, gRogueRun.isQuickSaveValid);
        gRogueRun.isQuickSaveValid = FALSE;

        ScriptContext1_SetupScript(Rogue_QuickSaveLoad);
        return TRUE;
    }
    else if(FollowMon_ProcessMonInteraction() == TRUE)
    {
        return TRUE;
    }

    return FALSE;
}

static hot_track_dat HotTrackingRtcToCounter(struct SiiRtcInfo* rtc)
{
    return RtcGetDayCount(rtc) * 24 * 60 * 60 +
           ConvertBcdToBinary(rtc->hour) * 60 * 60 + 
           ConvertBcdToBinary(rtc->minute) * 60 + 
           ConvertBcdToBinary(rtc->second);
}

static hot_track_dat HotTrackingLocalRtcToCounter(void)
{
    struct SiiRtcInfo localRtc;
    RtcGetRawInfo(&localRtc);

    return HotTrackingRtcToCounter(&localRtc);
}


static void ResetHotTracking()
{
    gRogueHotTracking.initSeed = HotTrackingLocalRtcToCounter();
    gRogueHotTracking.rollingSeed = gRogueHotTracking.initSeed;
    gRogueHotTracking.triggerCount = 0;
    gRogueHotTracking.triggerMin = (hot_track_dat)-1;
    gRogueHotTracking.triggerMax = 0;
    gRogueHotTracking.triggerAccumulation = 0;

    DebugPrintf("HotTracking init:%d roll:%d", gRogueHotTracking.initSeed, gRogueHotTracking.rollingSeed);
}

static void UpdateHotTracking()
{
    hot_track_dat localCounter = HotTrackingLocalRtcToCounter();
    hot_track_dat seedCounter = localCounter - gRogueHotTracking.initSeed;
    hot_track_dat rollingCounter = localCounter - gRogueHotTracking.rollingSeed;

    if(rollingCounter > 1)
    {
        gRogueHotTracking.initSeed = localCounter;
        gRogueHotTracking.rollingSeed = localCounter;
        
        ++gRogueHotTracking.triggerCount;
        gRogueHotTracking.triggerMin = min(gRogueHotTracking.triggerMin, rollingCounter);
        gRogueHotTracking.triggerMax = max(gRogueHotTracking.triggerMax, rollingCounter);
        gRogueHotTracking.triggerAccumulation += rollingCounter;

        DebugPrintf("HotTracking trigger:%d roll:%d trigger:%d", seedCounter, rollingCounter, gRogueHotTracking.triggerCount);
    }
    else
    {
        gRogueHotTracking.rollingSeed = localCounter;
    }
}

void Rogue_MainInit(void)
{
    ResetHotTracking();

    RogueQuery_Init();
    Rogue_RideMonInit();
    Rogue_AssistantInit();

#ifdef ROGUE_FEATURE_AUTOMATION
    Rogue_AutomationInit();
#endif
}

void Rogue_MainCB(void)
{
    //Additional 3rd maincallback which is always called

    if(Rogue_GetActiveCampaign() != ROGUE_CAMPAIGN_NONE)
    {
        UpdateHotTracking();
    }

    Rogue_AssistantMainCB();

#ifdef ROGUE_FEATURE_AUTOMATION
    Rogue_AutomationCallback();
#endif
}

void Rogue_OverworldCB(u16 newKeys, u16 heldKeys, bool8 inputActive)
{
    if(inputActive)
    {
        if(!(gPlayerAvatar.flags & (PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE | PLAYER_AVATAR_FLAG_SURFING | PLAYER_AVATAR_FLAG_UNDERWATER | PLAYER_AVATAR_FLAG_CONTROLLABLE)))
        {
            // Update running toggle
            if(gSaveBlock2Ptr->optionsAutoRunToggle && (newKeys & B_BUTTON) != 0)
            {
                gRogueLocal.runningToggleActive = !gRogueLocal.runningToggleActive;
            }
        }
    }
    
    Rogue_AssistantOverworldCB();
}

bool8 Rogue_IsRunningToggledOn()
{
    return gRogueLocal.runningToggleActive;
}

void Rogue_OnSpawnObjectEvent(struct ObjectEvent *objectEvent)
{
    if(FollowMon_IsMonObject(objectEvent, TRUE))
    {
        FollowMon_OnObjectEventSpawned(objectEvent);
    }
}

void Rogue_OnRemoveObjectEvent(struct ObjectEvent *objectEvent)
{
    if(FollowMon_IsMonObject(objectEvent, TRUE))
    {
        FollowMon_OnObjectEventRemoved(objectEvent);
    }
}

void Rogue_OnMovementType_Player(struct Sprite *sprite)
{
    Rogue_UpdateRideMonSprites();
}

void Rogue_OnResumeMap()
{
    //Rogue_CreateDestroyRideMonSprites();
}

// Called when object events first loaded for a map
void Rogue_InitObjectEventsLocal()
{
    Rogue_CreateDestroyRideMonSprites();
}

// Called when an already loaded map is returned to e.g. from a battle
void Rogue_OnSpawnObjectEventsOnReturnToField(s16 x, s16 y)
{
    Rogue_CreateDestroyRideMonSprites();
}

u16 Rogue_GetHotTrackingData(u16* count, u16* average, u16* min, u16* max)
{
    *count = gRogueHotTracking.triggerCount;
    *average = gRogueHotTracking.triggerAccumulation / gRogueHotTracking.triggerCount;
    *min = gRogueHotTracking.triggerMin;
    *max = gRogueHotTracking.triggerMax;
}


void Rogue_OnLoadMap(void)
{
    if(GetSafariZoneFlag())
    {
        // Reset preview data
        memset(&gRogueLocal.encounterPreview[0], 0, sizeof(gRogueLocal.encounterPreview));
        Rogue_ClearPopupQueue();

        RandomiseSafariWildEncounters();
        //Rogue_PushPopup(POPUP_MSG_SAFARI_ENCOUNTERS, 0);
    }
    else if(!Rogue_IsRunActive())
    {
        // Apply metatiles for the map we're in
        RogueHub_ApplyMapMetatiles();
    }

    SetupFollowParterMonObjectEvent();
}

u16 GetStartDifficulty(void)
{
    u16 skipToDifficulty = 0;

    if(RogueDebug_GetConfigRange(DEBUG_RANGE_START_DIFFICULTY) != 0)
    {
        skipToDifficulty = RogueDebug_GetConfigRange(DEBUG_RANGE_START_DIFFICULTY);
    }

    return skipToDifficulty;
}

static bool8 HasAnyActiveEvos(u16 species)
{
    u8 i;
    struct Evolution evo;

    for(i = 0; i < EVOS_PER_MON; ++i)
    {
        Rogue_ModifyEvolution(species, i, &evo);

        if(evo.targetSpecies != SPECIES_NONE)
        {
#ifdef ROGUE_EXPANSION
            if(evo.method != EVO_MEGA_EVOLUTION &&
                evo.method != EVO_MOVE_MEGA_EVOLUTION &&
                evo.method != EVO_PRIMAL_REVERSION
            )
#endif
                return TRUE;
        }
    }

    return FALSE;
}

static void GiveMonPartnerRibbon(void)
{
    u8 i;
    u16 species;
    bool8 ribbonSet = TRUE;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        if(species != SPECIES_NONE)
        {
            SetMonData(&gPlayerParty[i], MON_DATA_EFFORT_RIBBON, &ribbonSet);

            if(Rogue_GetEvolutionCount(species) != 0 && !HasAnyActiveEvos(species))
                Rogue_PushPopup_UnableToEvolve(i);
        }
    }
}

bool8 Rogue_IsPartnerMonInTeam(void)
{
    u8 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
        {
            if(GetMonData(&gPlayerParty[i], MON_DATA_EFFORT_RIBBON))
                return TRUE;
        }
    }

    return FALSE;
}

static u16 CalculateRewardLvlMonCount()
{
    u8 i;
    u16 validCount = 0;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gPlayerParty[i], MON_DATA_LEVEL) != MAX_LEVEL)
        {
            ++validCount;
        }
    }

    return validCount;
}

u16 Rogue_PostRunRewardLvls()
{
    u16 lvlCount = 2;
    u16 targettedMons = CalculateRewardLvlMonCount();

    if(targettedMons == 0)
    {
        lvlCount = 0;
    }
    else if(targettedMons > 1)
    {
        // Only give 1 lvl per mon
        lvlCount = 1;
    }

    lvlCount *= gRogueRun.currentDifficulty;

    if(lvlCount != 0)
    {
        u8 i, j;
        u32 exp;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            for(j = 0; j < lvlCount; ++j)
            {
                if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE && GetMonData(&gPlayerParty[i], MON_DATA_LEVEL) != MAX_LEVEL)
                {
                    exp = Rogue_ModifyExperienceTables(gBaseStats[GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL)].growthRate, GetMonData(&gPlayerParty[i], MON_DATA_LEVEL, NULL) + 1);
                    SetMonData(&gPlayerParty[i], MON_DATA_EXP, &exp);
                    CalculateMonStats(&gPlayerParty[i]);
                }
            }
        }
    }

    return lvlCount;
}

u16 Rogue_PostRunRewardMoney()
{
    u16 amount = 0;

    if(gRogueRun.enteredRoomCounter > 1)
    {
        u16 i = gRogueRun.enteredRoomCounter - 1;

        switch (Rogue_GetDifficultyRewardLevel())
        {
        case DIFFICULTY_LEVEL_EASY:
            amount = i * 200;
            break;

        case DIFFICULTY_LEVEL_MEDIUM:
            amount = i * 250;
            break;

        case DIFFICULTY_LEVEL_HARD:
            amount = i * 300;
            break;
        
        case DIFFICULTY_LEVEL_BRUTAL:
            amount = i * 350;
            break;
        }
    }

    AddMoney(&gSaveBlock1Ptr->money, amount);
    return amount;
}

static void ResetFaintedLabMonAtSlot(u16 slot)
{
    u16 species;

    struct Pokemon* mon = &gRogueLabEncounterData.party[slot];

    if(slot == VarGet(VAR_STARTER_MON))
    {
        species = SPECIES_SUNKERN;
    }
    else
    {
        species = VarGet(VAR_ROGUE_STARTER0 + slot);
    }

    CreateMonWithNature(mon, species, 7, USE_RANDOM_IVS, Random() % NUM_NATURES);
}

static void InitialiseFaintedLabMons(void)
{
    u16 i;
    for(i = 0; i < LAB_MON_COUNT; ++i)
    {
        ResetFaintedLabMonAtSlot(i);
    }
}

static u16 GetPartyWeakLegendary(void)
{
    u16 i;
    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesLegendary(species))
        {
            return species;
        }
    }

    return SPECIES_NONE;
}

static u16 GetPartyStrongLegendary(void)
{
    u16 i;
    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        
        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesLegendary(species))
        {
            if(CheckPresetMonFlags(species, MON_FLAG_STRONG_WILD))
            {
                return species;
            }
        }
    }

    return SPECIES_NONE;
}

static void BeginRogueRun_ModifyParty(void)
{
    // Always clear out EVs as we shouldn't have them in the HUB anymore
    {
        u16 i;
        u16 temp = 0;
        u32 exp;
        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
            if(species != SPECIES_NONE)
            {
                SetMonData(&gPlayerParty[i], MON_DATA_HP_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_ATK_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_DEF_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_SPEED_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_SPATK_EV, &temp);
                SetMonData(&gPlayerParty[i], MON_DATA_SPDEF_EV, &temp);

                // Force to starter lvl
                exp = Rogue_ModifyExperienceTables(gBaseStats[GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL)].growthRate, STARTER_MON_LEVEL);
                SetMonData(&gPlayerParty[i], MON_DATA_EXP, &exp);

                // Partner's can't reappear in safari
                gPlayerParty[i].rogueExtraData.isSafariIllegal = TRUE;

                CalculateMonStats(&gPlayerParty[i]);
            }
        }
    }
}

static bool8 CanEnterWithItem(u16 itemId, bool8 isBasicBagEnabled)
{
    u8 pocket;
    if(!isBasicBagEnabled)
        return TRUE;

    pocket = GetPocketByItemId(itemId);
    if(pocket == POCKET_KEY_ITEMS || pocket == POCKET_CHARMS)
        return TRUE;

    return FALSE;
}

static void SetupRogueRunBag()
{
    u16 i;
    u16 itemId;
    u16 quantity;
    bool8 isBasicBagEnabled = Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_BAG_WIPE);

    SetMoney(&gSaveBlock1Ptr->money, 0);
    ClearBag();

    // Re-add items
    for(i = 0; i < BAG_ITEM_CAPACITY; ++i)
    {
        itemId = RogueSave_GetHubBagItemIdAt(i);
        quantity = RogueSave_GetHubBagItemQuantityAt(i);
        
        if(itemId != ITEM_NONE && CanEnterWithItem(itemId, isBasicBagEnabled))
        {
            AddBagItem(itemId, quantity);
        }
    }

    // Give basic inventory
    if(isBasicBagEnabled)
    {
        AddBagItem(ITEM_POKE_BALL, 5);
        AddBagItem(ITEM_POTION, 1);
    }

#ifdef ROGUE_DEBUG
    AddBagItem(ITEM_ESCAPE_ROPE, 255);
    AddBagItem(ITEM_RARE_CANDY, 255);
#endif

    RecalcCharmCurseValues();

    // TODO - Rework this??
    //SetMoney(&gSaveBlock1Ptr->money, VarGet(VAR_ROGUE_ADVENTURE_MONEY));
}

static void BeginRogueRun(void)
{
    DebugPrint("BeginRogueRun");
    
    memset(&gRogueLocal, 0, sizeof(gRogueLocal));
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    ResetHotTracking();

#ifdef ROGUE_EXPANSION
    // Cache the results for the run (Must do before ActiveRun flag is set)
    gRogueRun.megasEnabled = IsMegaEvolutionEnabled();
    gRogueRun.zMovesEnabled = IsZMovesEnabled();
    // CheckBagHasItem(ITEM_DYNAMAX_BAND, 1)
#endif

    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    Rogue_PreActivateDesiredCampaign();

    //if(FlagGet(FLAG_SET_SEED_ENABLED))
    //{
    //    gRngRogueValue = Rogue_GetStartSeed();
    //}

    gRogueRun.baseSeed = Random();
    gRogueRun.currentDifficulty = GetStartDifficulty();
    gRogueRun.currentLevelOffset = 5;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    
    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        gRogueRun.currentLevelOffset = 80;
    }

    // Apply some base seed for anything which needs to be randomly setup
    SeedRogueRng(gRogueRun.baseSeed * 23151 + 29867);
    
    memset(&gRogueRun.completedBadges[0], TYPE_NONE, sizeof(gRogueRun.completedBadges));
    Rogue_ChooseBossTrainersForNewAdventure();

    VarSet(VAR_ROGUE_DIFFICULTY, gRogueRun.currentDifficulty);
    VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, 0);
    VarSet(VAR_ROGUE_DESIRED_WEATHER, WEATHER_NONE);

    VarSet(VAR_ROGUE_FLASK_HEALS_USED, 0);
    VarSet(VAR_ROGUE_FLASK_HEALS_MAX, 4);

    RogueSave_SaveHubStates();

    ClearBerryTrees();
    RandomiseFishingEncounters();
    InitialiseFaintedLabMons();
    PlayTimeCounter_Reset();
    PlayTimeCounter_Start();

    BeginRogueRun_ModifyParty();
    SetupRogueRunBag();

    FlagClear(FLAG_ROGUE_FREE_HEAL_USED);
    FlagClear(FLAG_ROGUE_RUN_COMPLETED);

    // Enable randoman trader at start
    if(IsQuestCollected(QUEST_MrRandoman))
    {
        FlagClear(FLAG_ROGUE_RANDOM_TRADE_DISABLED);
    }
    else
    {
        FlagSet(FLAG_ROGUE_RANDOM_TRADE_DISABLED);
    }

    Rogue_PostActivateDesiredCampaign();

    FlagClear(FLAG_ROGUE_TRAINERS_WEAK_LEGENDARIES);
    FlagClear(FLAG_ROGUE_TRAINERS_STRONG_LEGENDARIES);

    {
        u16 weakSpecies = GetPartyWeakLegendary();
        u16 strongSpecies = GetPartyStrongLegendary();

        if(weakSpecies != SPECIES_NONE)
            FlagSet(FLAG_ROGUE_TRAINERS_WEAK_LEGENDARIES);

        if(strongSpecies != SPECIES_NONE)
            FlagSet(FLAG_ROGUE_TRAINERS_STRONG_LEGENDARIES);

        if(strongSpecies != SPECIES_NONE)
            Rogue_PushPopup_StrongPokemonClause(strongSpecies);
        else if(weakSpecies != SPECIES_NONE)
            Rogue_PushPopup_WeakPokemonClause(weakSpecies);

    }


    GiveMonPartnerRibbon();

    QuestNotify_BeginAdventure();
}

static void EndRogueRun(void)
{
    QuestNotify_EndAdventure();

    if(Rogue_IsCampaignActive())
        Rogue_DeactivateActiveCampaign();

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_NONE;


    // We're back from adventure, so any mon we finished or retired with add to the safari
    {
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
            if(species != SPECIES_NONE)
            {
                RogueSafari_PushMon(&gPlayerParty[i]);
            }
        }
    }

    RogueSave_LoadHubStates();

    // Grow berries based on progress in runs
    BerryTreeTimeUpdate(90 * gRogueRun.enteredRoomCounter);
}

static bool8 IsLegendaryEncounterEnabled(u16 legendaryId, bool8 applyLegendaryDifficulty)
{
    u16 species = gRogueLegendaryEncounterInfo.mapTable[legendaryId].encounterId;
    u16 maxGen = VarGet(VAR_ROGUE_ENABLED_GEN_LIMIT);
    bool8 allowStrongSpecies = FALSE;

    if(!RoguePokedex_IsSpeciesEnabled(species))
    {
        return FALSE;
    }

    if(applyLegendaryDifficulty)
    {
        allowStrongSpecies = TRUE;
    }
    else
    {
        switch (Rogue_GetConfigRange(DIFFICULTY_RANGE_LEGENDARY))
        {
        case DIFFICULTY_LEVEL_EASY:
            allowStrongSpecies = TRUE;
            break;

        case DIFFICULTY_LEVEL_MEDIUM:
            allowStrongSpecies = (gRogueRun.currentDifficulty >= 7);
            break;

        case DIFFICULTY_LEVEL_HARD:
            allowStrongSpecies = FALSE;
            break;

        case DIFFICULTY_LEVEL_BRUTAL:
            // Technically this should never happen
            allowStrongSpecies = FALSE;
            break;
        }
    }

    if(!allowStrongSpecies)
    {
        if(CheckPresetMonFlags(species, MON_FLAG_STRONG_WILD))
        {
            // We're not allowed this encounter as it's too strong
            return FALSE;
        }
    }

    if(HistoryBufferContains(&gRogueAdvPath.legendaryHistoryBuffer[0], ARRAY_COUNT(gRogueAdvPath.legendaryHistoryBuffer), legendaryId))
    {
        return FALSE;
    }
  
    return TRUE;
}

static u16 NextLegendaryId(bool8 applyLegendaryDifficulty)
{
    u16 i;
    u16 randIdx;
    u16 enabledLegendariesCount = 0;

    for(i = 0; i < gRogueLegendaryEncounterInfo.mapCount; ++i)
    {
        if(IsLegendaryEncounterEnabled(i, applyLegendaryDifficulty))
            ++enabledLegendariesCount;
    }

    if(enabledLegendariesCount == 0)
    {
        // We've exhausted all enabled legendary options, so we're going to wipe the buffer and try again
        memset(&gRogueAdvPath.legendaryHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueAdvPath.legendaryHistoryBuffer));
        return NextLegendaryId(FALSE);
    }

    randIdx = RogueRandomRange(enabledLegendariesCount, OVERWORLD_FLAG);
    enabledLegendariesCount = 0;

    for(i = 0; i < gRogueLegendaryEncounterInfo.mapCount; ++i)
    {
        if(IsLegendaryEncounterEnabled(i, applyLegendaryDifficulty))
        {
            if(enabledLegendariesCount == randIdx)
                return i;
            else
                ++enabledLegendariesCount;
        }
    }

    return gRogueLegendaryEncounterInfo.mapCount - 1;
}

u8 Rogue_SelectLegendaryEncounterRoom(void)
{    
    u16 legendaryId = NextLegendaryId(TRUE);

    HistoryBufferPush(&gRogueAdvPath.legendaryHistoryBuffer[0], ARRAY_COUNT(gRogueAdvPath.legendaryHistoryBuffer), legendaryId);

    return legendaryId;
}


void Rogue_SelectMiniBossRewardMons()
{
    u16 indexA, indexB;
    u32 startSeed = gRngRogueValue;
    u8 partySize = CalculateEnemyPartyCount();

    if(partySize == 1)
    {
        indexA = 0;
        indexB = 0;
    }
    else if(partySize == 2)
    {
        indexA = 0;
        indexB = 1;
    }
    else
    {
        u8 i;
        u16 species;

        // Select first index
        indexA = RogueRandomRange(partySize, FLAG_SET_SEED_TRAINERS);

        for(i = 0; i < partySize; ++i)
        {
            species = GetMonData(&gEnemyParty[indexA], MON_DATA_SPECIES);

            // Accept first non legendary
            if(!RoguePokedex_IsSpeciesLegendary(species))
                break;
            
            indexA = (indexA + 1) % partySize;
        }

        // Select 2nd index
        indexB = RogueRandomRange(partySize, FLAG_SET_SEED_TRAINERS);

        for(i = 0; i < partySize; ++i)
        {
            species = GetMonData(&gEnemyParty[indexB], MON_DATA_SPECIES);

            // Avoid duplicate index
            if(indexB != indexA)
            {
                // Accept first non legendary
                if(!RoguePokedex_IsSpeciesLegendary(species))
                    break;
            }
            
            indexB = (indexB + 1) % partySize;
        }
    }

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, GetMonData(&gEnemyParty[indexA], MON_DATA_SPECIES));
    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, GetMonData(&gEnemyParty[indexB], MON_DATA_SPECIES));

    gRngRogueValue = startSeed;
}

static u8 RandomMonType(u16 seedFlag)
{
    u8 type;

    do
    {
        type = RogueRandomRange(NUMBER_OF_MON_TYPES, seedFlag);
    }
    while(type == TYPE_MYSTERY);

    return type;
}

u16 Rogue_SelectWildDenEncounterRoom(void)
{
    u16 species;
    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);
    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(Rogue_CalculatePlayerMonLvl(), TRUE, FALSE);

    {
        RogueWeightQuery_Begin();
        RogueWeightQuery_FillWeights(1);

        species = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());

        RogueWeightQuery_End();
    }

    RogueMonQuery_End();

    return species;
}

static bool8 IsRouteEnabled(u16 routeId)
{
    const struct RogueRouteEncounter* route = &gRogueRouteTable.routes[routeId];
    u16 includeFlags = ROUTE_FLAG_NONE;
    u16 excludeFlags = ROUTE_FLAG_NONE;
    
    if(FlagGet(FLAG_ROGUE_HOENN_ROUTES))
        includeFlags |= ROUTE_FLAG_HOENN;

    if(FlagGet(FLAG_ROGUE_KANTO_ROUTES))
        includeFlags |= ROUTE_FLAG_KANTO;

    if(FlagGet(FLAG_ROGUE_JOHTO_ROUTES))
        includeFlags |= ROUTE_FLAG_JOHTO;

    // Use the custom fallback set >:3
    if(includeFlags == ROUTE_FLAG_NONE)
    {
        includeFlags |= ROUTE_FLAG_FALLBACK_REGION;
    }

    if(excludeFlags != ROUTE_FLAG_NONE && (route->mapFlags & excludeFlags) != 0)
    {
        return FALSE;
    }

    if(includeFlags == ROUTE_FLAG_NONE || (route->mapFlags & includeFlags) != 0)
    {
        if(!HistoryBufferContains(&gRogueAdvPath.routeHistoryBuffer[0], ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer), routeId))
        {
            return TRUE;
        }
    }

    return FALSE;
}

static u16 NextRouteId()
{
    u16 i;
    u16 randIdx;
    u16 enabledRoutesCount = 0;

    for(i = 0; i < gRogueRouteTable.routeCount; ++i)
    {
        if(IsRouteEnabled(i))
            ++enabledRoutesCount;
    }

    if(enabledRoutesCount == 0)
    {
        // We've exhausted all enabled route options, so we're going to wipe the buffer and try again
        memset(&gRogueAdvPath.routeHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer));
        return NextRouteId();
    }

    randIdx = RogueRandomRange(enabledRoutesCount, OVERWORLD_FLAG);
    enabledRoutesCount = 0;

    for(i = 0; i < gRogueRouteTable.routeCount; ++i)
    {
        if(IsRouteEnabled(i))
        {
            if(enabledRoutesCount == randIdx)
                return i;
            else
                ++enabledRoutesCount;
        }
    }

    return gRogueRouteTable.routeCount - 1;
}

void Rogue_ResetAdventurePathBuffers()
{
    memset(&gRogueAdvPath.routeHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer));
    memset(&gRogueAdvPath.legendaryHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueAdvPath.legendaryHistoryBuffer));
    memset(&gRogueAdvPath.miniBossHistoryBuffer[0], (u16)-1, sizeof(u16) * ARRAY_COUNT(gRogueAdvPath.miniBossHistoryBuffer));
}

u8 Rogue_SelectRouteRoom(void)
{
    u16 routeId = NextRouteId();

    HistoryBufferPush(&gRogueAdvPath.routeHistoryBuffer[0], ARRAY_COUNT(gRogueAdvPath.routeHistoryBuffer), routeId);

    return routeId;
}

static void ResetSpecialEncounterStates(void)
{
    // Special states
    // Rayquaza
    VarSet(VAR_SKY_PILLAR_STATE, 2); // Keep in clean layout, but act as is R is has left for G/K cutscene
    //VarSet(VAR_SKY_PILLAR_RAQUAZA_CRY_DONE, 1); // Hide cutscene R
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
    FlagClear(FLAG_DEFEATED_HO_OH);
    FlagClear(FLAG_DEFEATED_LUGIA);

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

static bool8 IsRareShopActive()
{
#ifdef ROGUE_EXPANSION        
    return IsMegaEvolutionEnabled() || IsZMovesEnabled();
#else
    return FALSE;
#endif
}

static bool8 IsQuestRewardShopActive()
{
    // Apply shop reward items (Only applicable in hb)
    u16 i, j;
    u16 itemId;
    
    if(Rogue_IsRunActive())
        return FALSE;

    for(i = QUEST_FIRST; i < QUEST_CAPACITY; ++i)
    {
        if(IsQuestCollected(i))
        {
            for(j = 0; j < QUEST_MAX_ITEM_SHOP_REWARD_COUNT; ++j)
            {
                itemId = gRogueQuests[i].unlockedShopRewards[j];
                if(itemId != ITEM_NONE)
                    return TRUE;
            }
        }
    }

    return FALSE;
}

void Rogue_OnWarpIntoMap(void)
{
    u8 difficultyLevel;
    gRogueAdvPath.isOverviewActive = FALSE;

    FlagSet(FLAG_ROGUE_REWARD_ITEM_MART_DISABLED);
    FlagSet(FLAG_ROGUE_RARE_ITEM_MART_DISABLED);

    if(IsRareShopActive())
        FlagClear(FLAG_ROGUE_RARE_ITEM_MART_DISABLED);

    if(IsQuestRewardShopActive())
        FlagClear(FLAG_ROGUE_REWARD_ITEM_MART_DISABLED);


    // Set new safari flag on entering area
    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE)
    {
        FlagSet(FLAG_ROGUE_WILD_SAFARI);
        RogueSafari_ResetSpawns();
    }
    else
        FlagClear(FLAG_ROGUE_WILD_SAFARI);


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
    else if((gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_ADVENTURE_ENTRANCE || gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB_ADVENTURE_ENTERANCE) && Rogue_IsRunActive())
    {
        EndRogueRun();
    }

    if(Rogue_IsRunActive())
    {
        RogueToD_AddMinutes(60);
    }
    else
    {
        RogueToD_AddMinutes(15);
    }
}


void Rogue_OnSetWarpData(struct WarpData *warp)
{
    if(warp->mapGroup == MAP_GROUP(ROGUE_HUB) && warp->mapNum == MAP_NUM(ROGUE_HUB))
    {
        // Warping back to hub must be intentional
        return;
    }
    else if(warp->mapGroup == MAP_GROUP(ROGUE_AREA_ADVENTURE_ENTRANCE) && warp->mapNum == MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE))
    {
        // Warping back to hub must be intentional
        return;
    }
    else if(warp->mapGroup == MAP_GROUP(ROGUE_HUB_ADVENTURE_ENTERANCE) && warp->mapNum == MAP_NUM(ROGUE_HUB_ADVENTURE_ENTERANCE))
    {
        // Warping back to hub must be intentional
        return;
    }
    else if(warp->mapGroup == MAP_GROUP(ROGUE_ADVENTURE_PATHS) && warp->mapNum == MAP_NUM(ROGUE_ADVENTURE_PATHS))
    {
        // Ensure the run has started if we're trying to directly warp into the paths screen
        if(!Rogue_IsRunActive())
        {
            BeginRogueRun();
        }
    }

    // Reset preview data
    memset(&gRogueLocal.encounterPreview[0], 0, sizeof(gRogueLocal.encounterPreview));

    if(Rogue_IsRunActive())
    {
        u8 warpType = RogueAdv_OverrideNextWarp(warp);

        if(warpType == ROGUE_WARP_TO_ADVPATH)
        {
            if(gRogueRun.enteredRoomCounter == 0)
                QuestNotify_OnExitHubTransition();
        }
        else if(warpType == ROGUE_WARP_TO_ROOM)
        {
            ++gRogueRun.enteredRoomCounter;

            VarSet(VAR_ROGUE_DESIRED_WEATHER, WEATHER_NONE);

            // We're warping into a valid map
            // We've already set the next room type so adjust the scaling now
            switch(gRogueAdvPath.currentRoomType)
            {
                case ADVPATH_ROOM_RESTSTOP:
                {
                    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE) || RogueRandomChance(33, OVERWORLD_FLAG))
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
                    RandomiseEnabledItems();
                    RandomiseBerryTrees();

                    if(gRogueRun.currentDifficulty != 0 && RogueRandomChance(weatherChance, OVERWORLD_FLAG))
                    {
                        u8 randIdx = RogueRandomRange(ARRAY_COUNT(gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable), OVERWORLD_FLAG);
                        u16 chosenType = gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable[randIdx];
                        u16 weatherType = gRogueTypeWeatherTable[chosenType];

                        VarSet(VAR_ROGUE_DESIRED_WEATHER, weatherType);
                    }
                    break;
                }

                case ADVPATH_ROOM_BOSS:
                {
                    u16 trainerNum;
                    trainerNum = gRogueAdvPath.currentRoomParams.perType.boss.trainerNum;

                    gRogueRun.currentLevelOffset = 0;
                    RandomiseEnabledItems();

                    VarSet(VAR_OBJ_GFX_ID_0, Rogue_GetTrainerObjectEventGfx(trainerNum));
                    VarSet(VAR_ROGUE_DESIRED_WEATHER, Rogue_GetTrainerWeather(trainerNum));

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, trainerNum);
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, Rogue_GetTrainerTypeAssignment(trainerNum));
                    break;
                }

                case ADVPATH_ROOM_MINIBOSS:
                {
                    u16 trainerNum;
                    trainerNum = gRogueAdvPath.currentRoomParams.perType.miniboss.trainerNum;

                    RandomiseEnabledItems();

                    VarSet(VAR_OBJ_GFX_ID_0, Rogue_GetTrainerObjectEventGfx(trainerNum));
                    VarSet(VAR_ROGUE_DESIRED_WEATHER, Rogue_GetTrainerWeather(trainerNum));

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, trainerNum);
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, Rogue_GetTrainerTypeAssignment(trainerNum));
                    break;
                }

                case ADVPATH_ROOM_LEGENDARY:
                {
                    u16 species = gRogueLegendaryEncounterInfo.mapTable[gRogueAdvPath.currentRoomParams.roomIdx].encounterId;
                    ResetSpecialEncounterStates();
                    ResetTrainerBattles();
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, species);
                    FollowMon_SetGraphics(
                        0, 
                        species,
                        gRogueAdvPath.currentRoomParams.perType.legendary.shinyState
                    );
                    break;
                }

                case ADVPATH_ROOM_WILD_DEN:
                {
                    ResetSpecialEncounterStates();
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, gRogueAdvPath.currentRoomParams.perType.wildDen.species);
                    
                    FollowMon_SetGraphics(
                        0, 
                        gRogueAdvPath.currentRoomParams.perType.wildDen.species, 
                        gRogueAdvPath.currentRoomParams.perType.wildDen.shinyState
                    );
                    break;
                }

                case ADVPATH_ROOM_LAB:
                {
                    RandomiseCharmItems();

                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, GetMonData(&gRogueLabEncounterData.party[0], MON_DATA_SPECIES));
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1, GetMonData(&gRogueLabEncounterData.party[1], MON_DATA_SPECIES));
                    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2, GetMonData(&gRogueLabEncounterData.party[2], MON_DATA_SPECIES));
                    break;
                }

                case ADVPATH_ROOM_DARK_DEAL:
                {
                    RandomiseCharmItems();
                    break;
                }
            };

            // Update VARs
            VarSet(VAR_ROGUE_CURRENT_ROOM_IDX, gRogueRun.enteredRoomCounter);
            VarSet(VAR_ROGUE_CURRENT_LEVEL_CAP, Rogue_CalculateBossMonLvl());
        }
    }

    FollowMon_OnWarp();
    QuestNotify_OnWarp(warp);
}

void Rogue_ModifyMapHeader(struct MapHeader *mapHeader)
{
    // NOTE: This method shouldn't be used
    // For some reason editing the map header and repointing stuff messes with other collections in the header
    // e.g. repointing object events messes with the warps for some reason
}

void Rogue_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp)
{
    RogueHub_ModifyMapWarpEvent(mapHeader, warpId, warp);
}

bool8 Rogue_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection)
{
    if(!RogueHub_AcceptMapConnection(mapHeader, connection))
    {
        return FALSE;
    }

    return TRUE;
}

static bool8 IsHubMapGroup()
{
    return gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_HUB) || gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_AREA_HOME) || gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROGUE_INTERIOR_HOME);
}

static bool8 RogueRandomChanceTrainer();

void Rogue_ModifyObjectEvents(struct MapHeader *mapHeader, bool8 loadingFromSave, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity)
{
    // If we're in run and not trying to exit (gRogueAdvPath.currentRoomType isn't wiped at this point)
    if(Rogue_IsRunActive() && !IsHubMapGroup())
    {
        if(mapHeader->mapLayoutId == LAYOUT_ROGUE_ADVENTURE_PATHS)
        {
            RogueAdv_ModifyObjectEvents(mapHeader, objectEvents, objectEventCount, objectEventCapacity);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE && !loadingFromSave)
        {
            u8 write, read;
            u8 originalCount = *objectEventCount;
            u16 trainerHistory[20];

            u16 trainerNum;
            const struct RogueTrainer* trainer;

            write = 0;
            read = 0;

            for(;read < originalCount; ++read)
            {
                if(write != read)
                {
                    memcpy(&objectEvents[write], &objectEvents[read], sizeof(struct ObjectEventTemplate));
                }

                // Adjust trainers
                //
                if(objectEvents[write].trainerType == TRAINER_TYPE_NORMAL && objectEvents[write].trainerRange_berryTreeId != 0)
                {
                    // Don't increment write, if we're not accepting the trainer
                    if(!FlagGet(FLAG_ROGUE_GAUNTLET_MODE) && RogueRandomChanceTrainer())
                    {
                        trainerNum = Rogue_NextRouteTrainerId(&trainerHistory[0], ARRAY_COUNT(trainerHistory));
                        trainer = Rogue_GetTrainer(trainerNum);

                        if(trainer != NULL)
                        {
                            objectEvents[write].graphicsId = trainer->objectEventGfx;
                            objectEvents[write].flagId = 0;//FLAG_ROGUE_TRAINER0 + ;

                            // Accept this trainer
                            write++;
                        }
                    }
                }
                // Adjust items
                //
                else if(objectEvents[write].flagId >= FLAG_ROGUE_ITEM_START && objectEvents[write].flagId <= FLAG_ROGUE_ITEM_END)
                {
                    // Already decided this earlier
                    if(!FlagGet(objectEvents[write].flagId))
                    {
                        u16 idx = objectEvents[write].flagId - FLAG_ROGUE_ITEM_START;
                        u16 itemId = VarGet(VAR_ROGUE_ITEM_START + idx);

                        // Default to a greyed out pokeball
                        objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_POKE_BALL;

                        if(Rogue_IsEvolutionItem(itemId))
                        {
                            objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_EVO_STONE;
                        }
                        else
                        {
                            switch (ItemId_GetPocket(itemId))
                            {
                            case POCKET_HELD_ITEMS:
                                objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_HOLD_ITEM;
                                break;

                            case POCKET_MEDICINE:
                                objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_MEDICINE;
                                break;

                            case POCKET_POKE_BALLS:
                                objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_BALL;
                                break;

                            case POCKET_TM_HM:
                                objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_SILVER_TM;
                                // todo - hookup OBJ_EVENT_GFX_ITEM_GOLD_TM
                                break;
#ifdef ROGUE_EXPANSION
                            case POCKET_STONES:
                                if(itemId >= ITEM_RED_ORB && itemId <= ITEM_DIANCITE)
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_MEGA_STONE;
                                else if(itemId >= ITEM_NORMALIUM_Z && itemId <= ITEM_ULTRANECROZIUM_Z)
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_Z_CRYSTAL;
                                else
                                    objectEvents[write].graphicsId = OBJ_EVENT_GFX_ITEM_POKE_BALL;
                                break;
#endif
                            }
                        }

                        // Accept this item
                        write++;
                    }
                }
                else
                {
                    // Accept all other types of object
                    write++;
                }
            }

            *objectEventCount = write;
        }
    }
}

static void PushFaintedMonToLab(struct Pokemon* srcMon)
{
    u16 temp;
    struct Pokemon* destMon;
    u16 i = Random() % (LAB_MON_COUNT + 1);
    
    if(i >= LAB_MON_COUNT)
    {
        // Ignore this fainted mon
        return;
    }

    destMon = &gRogueLabEncounterData.party[i];
    CopyMon(destMon, srcMon, sizeof(*destMon));

    temp = max(1, GetMonData(destMon, MON_DATA_MAX_HP) / 2);
    SetMonData(destMon, MON_DATA_HP, &temp);

    // Wipe EVs
    temp = 0;
    SetMonData(destMon, MON_DATA_HP_EV, &temp);
    SetMonData(destMon, MON_DATA_ATK_EV, &temp);
    SetMonData(destMon, MON_DATA_DEF_EV, &temp);
    SetMonData(destMon, MON_DATA_SPEED_EV, &temp);
    SetMonData(destMon, MON_DATA_SPATK_EV, &temp);
    SetMonData(destMon, MON_DATA_SPDEF_EV, &temp);
    CalculateMonStats(destMon);
}

void Rogue_CopyLabEncounterMonNickname(u16 index, u8* dst)
{
    if(index < LAB_MON_COUNT)
    {
        GetMonData(&gRogueLabEncounterData.party[index], MON_DATA_NICKNAME, dst);
        StringGet_Nickname(dst);
    }
}

bool8 Rogue_GiveLabEncounterMon(u16 index)
{
    if(gPlayerPartyCount < PARTY_SIZE && index < LAB_MON_COUNT)
    {
        CopyMon(&gPlayerParty[gPlayerPartyCount], &gRogueLabEncounterData.party[index], sizeof(gPlayerParty[gPlayerPartyCount]));

        // Already in safari from? (Maybe should track index and then wipe here, as we could have higher priority)
        gPlayerParty[gPlayerPartyCount].rogueExtraData.isSafariIllegal = TRUE;

        gPlayerPartyCount = CalculatePlayerPartyCount();
        ResetFaintedLabMonAtSlot(index);
        return TRUE;
    }

    return FALSE;
}

void RemoveMonAtSlot(u8 slot, bool8 keepItems, bool8 shiftUpwardsParty, bool8 canSendToLab)
{
    if(slot < gPlayerPartyCount)
    {
        if(GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES) != SPECIES_NONE)
        {
            u32 hp = 0;
            SetMonData(&gPlayerParty[slot], MON_DATA_HP, &hp);

            if(shiftUpwardsParty)
            {
                RemoveAnyFaintedMons(keepItems, canSendToLab);
            }
            else
            {
                if(keepItems)
                {
                    // Dead so give back held item
                    u16 heldItem = GetMonData(&gPlayerParty[slot], MON_DATA_HELD_ITEM);
                    if(heldItem != ITEM_NONE)
                        AddBagItem(heldItem, 1);
                }

                if(canSendToLab)
                    PushFaintedMonToLab(&gPlayerParty[slot]);

                ZeroMonData(&gPlayerParty[slot]);
            }
        }
    }
}

void RemoveAnyFaintedMons(bool8 keepItems, bool8 canSendToLab)
{
    bool8 hasValidSpecies;
    u8 read;
    u8 write = 0;
    bool8 hasMonFainted = FALSE;

    // If we're finished, we don't want to release any mons, just check if anything has fainted or not
    if(Rogue_IsRunActive() && gRogueRun.currentDifficulty >= ROGUE_MAX_BOSS_COUNT)
    {
        for(read = 0; read < PARTY_SIZE; ++read)
        {
            hasValidSpecies = GetMonData(&gPlayerParty[read], MON_DATA_SPECIES) != SPECIES_NONE;

            if(hasValidSpecies && GetMonData(&gPlayerParty[read], MON_DATA_HP, NULL) != 0)
            {
                // This mon is alive
            }
            else if(hasValidSpecies)
            {
                hasMonFainted = TRUE;
                break;
            }
        }
    }
    else
    {
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
                else
                    hasMonFainted = TRUE;

                RogueSafari_PushMon(&gPlayerParty[read]);

                if(canSendToLab)
                    PushFaintedMonToLab(&gPlayerParty[read]);

                ZeroMonData(&gPlayerParty[read]);
            }
        }
    }

    if(hasMonFainted)
    {
        Rogue_CampaignNotify_OnMonFainted();
        QuestNotify_OnMonFainted();
    }

    gPlayerPartyCount = CalculatePlayerPartyCount();
}

void Rogue_Battle_StartTrainerBattle(void)
{
    bool8 shouldDoubleBattle = FALSE;

    switch(Rogue_GetConfigRange(DIFFICULTY_RANGE_BATTLE_FORMAT))
    {
        case BATTLE_FORMAT_SINGLES:
            shouldDoubleBattle = FALSE;
            break;

        case BATTLE_FORMAT_DOUBLES:
            shouldDoubleBattle = TRUE;
            break;

        case BATTLE_FORMAT_MIXED:
            shouldDoubleBattle = (RogueRandom() % 2);
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
    }

    if(shouldDoubleBattle) //NoOfApproachingTrainers != 2 
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

static bool32 DidPlayerCatch(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_CAUGHT:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 DidCompleteWildChain(u32 battleOutcome)
{
    switch (battleOutcome)
    {
    case B_OUTCOME_WON:
        return TRUE;
    }

    return FALSE;
}

static bool32 DidFailWildChain(u32 battleOutcome, u16 species)
{
    switch (battleOutcome)
    {
    // If you catch anything, you end the chain
    case B_OUTCOME_CAUGHT:
        return TRUE;
    }

    // If we fail a battle against the mon we're chaining, end the chain
    if(GetWildChainSpecies() == species)
    {
        switch (battleOutcome)
        {
        case B_OUTCOME_LOST:
        case B_OUTCOME_DREW:
        case B_OUTCOME_RAN:
        case B_OUTCOME_PLAYER_TELEPORTED:
        case B_OUTCOME_MON_FLED:
        case B_OUTCOME_FORFEITED:
        case B_OUTCOME_MON_TELEPORTED:
            return TRUE;
        }
    }

    return FALSE;
}

const u16 gNatureEvRewardStatTable[NUM_NATURES][NUM_STATS] =
{
                       // Hp  Atk Def Spd Sp.Atk Sp.Def
    [NATURE_HARDY]   = {  6,  2,  2,  4,  4,     4},
    [NATURE_LONELY]  = {  4,  6,  0,  4,  4,     4},
    [NATURE_BRAVE]   = {  4,  6,  4,  0,  4,     4},
    [NATURE_ADAMANT] = {  4,  6,  4,  4,  0,     4},
    [NATURE_NAUGHTY] = {  4,  6,  4,  4,  4,     0},
    [NATURE_BOLD]    = {  4,  0,  6,  4,  4,     4},
    [NATURE_DOCILE]  = {  6,  4,  4,  4,  2,     2},
    [NATURE_RELAXED] = {  4,  4,  6,  0,  4,     4},
    [NATURE_IMPISH]  = {  4,  4,  6,  4,  0,     4},
    [NATURE_LAX]     = {  4,  4,  6,  4,  4,     0},
    [NATURE_TIMID]   = {  4,  0,  4,  6,  4,     4},
    [NATURE_HASTY]   = {  4,  4,  0,  6,  4,     4},
    [NATURE_SERIOUS] = {  6,  2,  4,  4,  2,     4},
    [NATURE_JOLLY]   = {  4,  4,  4,  6,  0,     4},
    [NATURE_NAIVE]   = {  4,  4,  4,  6,  4,     0},
    [NATURE_MODEST]  = {  4,  0,  4,  4,  6,     4},
    [NATURE_MILD]    = {  4,  4,  0,  4,  6,     4},
    [NATURE_QUIET]   = {  4,  4,  4,  0,  6,     4},
    [NATURE_BASHFUL] = {  6,  4,  2,  4,  4,     2},
    [NATURE_RASH]    = {  4,  4,  4,  4,  6,     0},
    [NATURE_CALM]    = {  4,  0,  4,  4,  4,     6},
    [NATURE_GENTLE]  = {  4,  4,  0,  4,  4,     6},
    [NATURE_SASSY]   = {  4,  4,  4,  0,  4,     6},
    [NATURE_CAREFUL] = {  4,  4,  4,  4,  0,     6},
    [NATURE_QUIRKY]  = {  2,  4,  4,  4,  4,     4},
};

// Modified version of MonGainEVs
// Award EVs based on nature
static void MonGainRewardEVs(struct Pokemon *mon)
{
    u8 evs[NUM_STATS];
    u16 evIncrease;
    u16 totalEVs;
    u16 heldItem;
    u8 holdEffect;
    int i, multiplier;
    u8 stat;
    u8 bonus;
    u8 nature;

    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM, 0);
    nature = GetNature(mon);

    if (heldItem == ITEM_ENIGMA_BERRY)
    {
        if (gMain.inBattle)
            holdEffect = gEnigmaBerries[0].holdEffect;
        else
            holdEffect = gSaveBlock1Ptr->enigmaBerry.holdEffect;
    }
    else
    {
        holdEffect = ItemId_GetHoldEffect(heldItem);
    }

    stat = ItemId_GetSecondaryId(heldItem);
    bonus = ItemId_GetHoldEffectParam(heldItem);

    for (i = 0; i < NUM_STATS; i++)
    {
        evs[i] = GetMonData(mon, MON_DATA_HP_EV + i, 0);
        totalEVs += evs[i];
    }

    for (i = 0; i < NUM_STATS; i++)
    {
        if (totalEVs >= MAX_TOTAL_EVS)
            break;

        if (CheckPartyHasHadPokerus(mon, 0))
            multiplier = 2;
        else
            multiplier = 1;
        
        if(multiplier == 0)
            continue;

        evIncrease = gNatureEvRewardStatTable[nature][i];

#ifdef ROGUE_EXPANSION
        if (holdEffect == HOLD_EFFECT_POWER_ITEM && stat == i)
            evIncrease += bonus;
#endif

        if (holdEffect == HOLD_EFFECT_MACHO_BRACE)
            multiplier *= 2;

        evIncrease *= multiplier;

        if (totalEVs + (s16)evIncrease > MAX_TOTAL_EVS)
            evIncrease = ((s16)evIncrease + MAX_TOTAL_EVS) - (totalEVs + evIncrease);

        if (evs[i] + (s16)evIncrease > MAX_PER_STAT_EVS)
        {
            int val1 = (s16)evIncrease + MAX_PER_STAT_EVS;
            int val2 = evs[i] + evIncrease;
            evIncrease = val1 - val2;
        }

        evs[i] += evIncrease;
        totalEVs += evIncrease;
        SetMonData(mon, MON_DATA_HP_EV + i, &evs[i]);
    }
}


void Rogue_Battle_EndTrainerBattle(u16 trainerNum)
{
    if(Rogue_IsRunActive())
    {
        bool8 isBossTrainer = Rogue_IsBossTrainer(trainerNum);

        if(isBossTrainer)
        {
            u8 nextLevel;
            u8 prevLevel = Rogue_CalculateBossMonLvl();

            // Update badge for trainer card
            gRogueRun.completedBadges[gRogueRun.currentDifficulty] = Rogue_GetTrainerTypeAssignment(trainerNum);

            if(gRogueRun.completedBadges[gRogueRun.currentDifficulty] == TYPE_NONE)
                gRogueRun.completedBadges[gRogueRun.currentDifficulty] = TYPE_MYSTERY;

            // Increment difficulty
            ++gRogueRun.currentDifficulty;
            nextLevel = Rogue_CalculateBossMonLvl();

            gRogueRun.currentLevelOffset = nextLevel - prevLevel;

            if(gRogueRun.currentDifficulty >= ROGUE_MAX_BOSS_COUNT)
            {
                FlagSet(FLAG_IS_CHAMPION);
                FlagSet(FLAG_ROGUE_RUN_COMPLETED);
            }

            VarSet(VAR_ROGUE_DIFFICULTY, gRogueRun.currentDifficulty);
            VarSet(VAR_ROGUE_FURTHEST_DIFFICULTY, max(gRogueRun.currentDifficulty, VarGet(VAR_ROGUE_FURTHEST_DIFFICULTY)));
        }

        // Adjust this after the boss reset
        if(gRogueRun.currentLevelOffset)
        {
            u8 levelOffsetDelta = 3;
            
            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                levelOffsetDelta = 5;
            }

            // Every trainer battle drops level cap slightly
            if(gRogueRun.currentLevelOffset < levelOffsetDelta)
                gRogueRun.currentLevelOffset = 0;
            else
                gRogueRun.currentLevelOffset -= levelOffsetDelta;
        }

        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            QuestNotify_OnTrainerBattleEnd(isBossTrainer);
            RemoveAnyFaintedMons(FALSE, TRUE);

            // Reward EVs based on nature
            if(Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_EV_GAIN))
            {
                u16 i;

                for(i = 0; i < gPlayerPartyCount; ++i)
                {
                    if(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE)
                    {
                        MonGainRewardEVs(&gPlayerParty[i]);
                        CalculateMonStats(&gPlayerParty[i]);
                    }
                }
            }
        }
        else
        {
            QuestNotify_OnMonFainted();
        }
    }
}

void Rogue_Battle_EndWildBattle(void)
{
    u16 wildSpecies = GetMonData(&gEnemyParty[0], MON_DATA_SPECIES);

    if(DidCompleteWildChain(gBattleOutcome))
        UpdateWildEncounterChain(wildSpecies);
    else if(DidFailWildChain(gBattleOutcome, wildSpecies))
        UpdateWildEncounterChain(SPECIES_NONE);

    if(Rogue_IsRunActive())
    {
        if(gRogueRun.currentLevelOffset && !DidPlayerRun(gBattleOutcome))
        {
            u8 levelOffsetDelta = 3;
            
            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                levelOffsetDelta = 5;
            }

            // Don't increase the level caps if we only caught the mon
            if(!DidPlayerCatch(gBattleOutcome))
            {
                // Every trainer battle drops level cap slightly
                if(gRogueRun.currentLevelOffset < levelOffsetDelta)
                    gRogueRun.currentLevelOffset = 0;
                else
                    gRogueRun.currentLevelOffset -= levelOffsetDelta;
            }
        }

        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            QuestNotify_OnWildBattleEnd();
            RemoveAnyFaintedMons(FALSE, TRUE);
        }
        else
        {
            QuestNotify_OnMonFainted();
        }
    }
}

void Rogue_Safari_EndWildBattle(void)
{
    if (gBattleOutcome == B_OUTCOME_CAUGHT)
    {
        u8 safariIndex = RogueSafari_GetPendingBattleMonIdx();
        RogueSafari_ClearSafariMonAtIdx(safariIndex);
    }
}

bool8 Rogue_AllowItemUse(u16 itemId)
{
    //if (gMain.inBattle)
    //{
    //    return FALSE;
    //}

    return TRUE;
}

void Rogue_OnItemUse(u16 itemId)
{
    //if (gMain.inBattle)
    //{
    //}
}

void Rogue_PreBattleSetup(void)
{
    if(IsCurseActive(EFFECT_ITEM_SHUFFLE))
    {
        u16 i;
        u8 size = CalculatePlayerPartyCount();

        for(i = 0; i < size; ++i)
            SwapMonItems(i, Random() % size, gPlayerParty);
    }
}

bool8 Rogue_OverrideTrainerItems(u16* items)
{
#ifdef ROGUE_FEATURE_AUTOMATION
    if(TRUE)
#else
    if(Rogue_IsRunActive())
#endif
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

extern const u16* const gRegionalDexSpecies[];
extern u16 gRegionalDexSpeciesCount[];

static void SwapMonItems(u8 aIdx, u8 bIdx, struct Pokemon *party)
{
    if(aIdx != bIdx)
    {
        u16 itemA = GetMonData(&party[aIdx], MON_DATA_HELD_ITEM);
        u16 itemB = GetMonData(&party[bIdx], MON_DATA_HELD_ITEM);

        SetMonData(&party[aIdx], MON_DATA_HELD_ITEM, &itemB);
        SetMonData(&party[bIdx], MON_DATA_HELD_ITEM, &itemA);
    }
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

void Rogue_ApplyMonPreset(struct Pokemon* mon, u8 level, const struct RogueMonPreset* preset)
{
#ifdef ROGUE_EXPANSION
    u16 const abilityCount = 3;
#else
    u16 const abilityCount = 2;
#endif
    u16 i;
    u16 move;
    u16 heldItem;
    u8 writeMoveIdx;
    u16 initialMonMoves[MAX_MON_MOVES];
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    bool8 useMaxHappiness = TRUE;

    // We want to start writing the move from the first free slot and loop back around
    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        initialMonMoves[i] = GetMonData(mon, MON_DATA_MOVE1 + i);

        move = MOVE_NONE;
        SetMonData(mon, MON_DATA_MOVE1 + i, &move);
    }

    // abilityNum is actually the abilityId
    if(preset->abilityNum != ABILITY_NONE)
    {
        // We need to set the ability index
        for(i = 0; i < abilityCount; ++i)
        {
            SetMonData(mon, MON_DATA_ABILITY_NUM, &i);

            if(GetMonAbility(mon) == preset->abilityNum)
            {
                // Ability is set
                break;
            }
        }

        if(i >= abilityCount)
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
    writeMoveIdx = 0;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = preset->moves[i]; 

        if(move != MOVE_NONE && CanLearnMoveByLvl(species, move, level))
        {
            if(move == MOVE_FRUSTRATION)
                useMaxHappiness = FALSE;

            SetMonData(mon, MON_DATA_MOVE1 + writeMoveIdx, &move);
            SetMonData(mon, MON_DATA_PP1 + writeMoveIdx, &gBattleMoves[move].pp);
            ++writeMoveIdx;
        }
    }

    if(preset->allowMissingMoves)
    {
        // Fill the remainer slots with empty moves
        for (i = writeMoveIdx; i < MAX_MON_MOVES; i++)
        {
            move = MOVE_NONE; 
            SetMonData(mon, MON_DATA_MOVE1 + i, &move);
            SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[move].pp);
        }
    }
    else
    {
        // Try to re-teach initial moves to fill out last slots
        for(i = 0; i < MAX_MON_MOVES && writeMoveIdx < MAX_MON_MOVES; ++i)
        {
            move = initialMonMoves[i]; 

            if(move != MOVE_NONE && !MonKnowsMove(mon, move))
            {
                SetMonData(mon, MON_DATA_MOVE1 + writeMoveIdx, &move);
                SetMonData(mon, MON_DATA_PP1 + writeMoveIdx, &gBattleMoves[move].pp);
                ++writeMoveIdx;
            }
        }
    }

    move = useMaxHappiness ? MAX_FRIENDSHIP : 0;
    SetMonData(mon, MON_DATA_FRIENDSHIP, &move);

    if(preset->hiddenPowerType != TYPE_NONE)
    {
        u16 value;
        bool8 ivStatsOdd[6];

        #define oddHP ivStatsOdd[0]
        #define oddAtk ivStatsOdd[1]
        #define oddDef ivStatsOdd[2]
        #define oddSpeed ivStatsOdd[3]
        #define oddSpAtk ivStatsOdd[4]
        #define oddSpDef ivStatsOdd[5]

        oddHP = TRUE;
        oddAtk = TRUE;
        oddDef = TRUE;
        oddSpeed = TRUE;
        oddSpAtk = TRUE;
        oddSpDef = TRUE;

        switch(preset->hiddenPowerType)
        {
            case TYPE_FIGHTING:
                oddDef = FALSE;
                oddSpeed = FALSE;
                oddSpAtk = FALSE;
                oddSpDef = FALSE;
                break;

            case TYPE_FLYING:
                oddSpeed = FALSE;
                oddSpAtk = FALSE;
                oddSpDef = FALSE;
                break;

            case TYPE_POISON:
                oddDef = FALSE;
                oddSpAtk = FALSE;
                oddSpDef = FALSE;
                break;

            case TYPE_GROUND:
                oddSpAtk = FALSE;
                oddSpDef = FALSE;
                break;

            case TYPE_ROCK:
                oddDef = FALSE;
                oddSpeed = FALSE;
                oddSpDef = FALSE;
                break;

            case TYPE_BUG:
                oddSpeed = FALSE;
                oddSpDef = FALSE;
                break;

            case TYPE_GHOST:
                oddAtk = FALSE;
                oddSpDef = FALSE;
                break;

            case TYPE_STEEL:
                oddSpDef = FALSE;
                break;

            case TYPE_FIRE:
                oddAtk = FALSE;
                oddSpeed = FALSE;
                oddSpAtk = FALSE;
                break;

            case TYPE_WATER:
                oddSpeed = FALSE;
                oddSpAtk = FALSE;
                break;

            case TYPE_GRASS:
                oddHP = FALSE;
                oddSpAtk = FALSE;
                break;

            case TYPE_ELECTRIC:
                oddSpAtk = FALSE;
                break;

            case TYPE_PSYCHIC:
                oddHP = FALSE;
                oddSpeed = FALSE;
                break;

            case TYPE_ICE:
                oddSpeed = FALSE;
                break;

            case TYPE_DRAGON:
                oddHP = FALSE;
                break;

            case TYPE_DARK:
                break;
        }

        #undef oddHP
        #undef oddAtk
        #undef oddDef
        #undef oddSpeed
        #undef oddSpAtk
        #undef oddSpDef

        for(i = 0; i < 6; ++i)
        {
            value = GetMonData(mon, MON_DATA_HP_IV + i);

            if((value % 2) == 0) // Current IV is even
            {
                if(ivStatsOdd[i]) // Wants to be odd
                {
                    if(value == 0)
                        value = 1;
                    else
                        --value;
                }
            }
            else // Current IV is odd
            {
                if(!ivStatsOdd[i]) // Wants to be even
                    --value;
            }

            SetMonData(mon, MON_DATA_HP_IV + i, &value);
        }
    }
}

static u8 GetCurrentWildEncounterCount()
{    
    u16 count = 0;
    u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();

    if(GetSafariZoneFlag())
    {
        count = 6;
    }
    else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
    {
        u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
        count = 4;

        if(difficultyModifier == 2) // Hard route
        {
            // Less encounters
            count = 2;
        }
        else if(difficultyModifier == 1) // Avg route
        {
            // Slightly less encounters
            count = 3;
        }

        // Apply charms
        {
            u16 decValue = GetCurseValue(EFFECT_WILD_ENCOUNTER_COUNT);

            count += GetCharmValue(EFFECT_WILD_ENCOUNTER_COUNT);

            if(decValue > count)
                count = 0;
            else
                count -= decValue;
        }

        // Clamp
        count = max(count, 1);
        count = min(count, WILD_ENCOUNTER_GRASS_CAPACITY);

        // Move count down if we have haven't actually managed to spawn in enough unique encounters
        while(count != 0 && GetWildGrassEncounter(count - 1) == SPECIES_NONE)
        {
            count--;
        }
    }

    return count;
}

static u16 GetWildGrassEncounter(u8 index)
{
    AGB_ASSERT(index < WILD_ENCOUNTER_GRASS_CAPACITY);

    if(index < WILD_ENCOUNTER_GRASS_CAPACITY)
    {
        return gRogueRun.wildEncounters.species[index];
    }

    return SPECIES_NONE;
}

static u16 GetWildWaterEncounter(u8 index)
{
    AGB_ASSERT(index < WILD_ENCOUNTER_WATER_CAPACITY);

    if(index < WILD_ENCOUNTER_WATER_CAPACITY)
    {
        return gRogueRun.wildEncounters.species[WILD_ENCOUNTER_GRASS_CAPACITY + index];
    }

    return SPECIES_NONE;
}

static u16 GetWildEncounterIndexFor(u16 species)
{
    u8 i;
    u16 checkSpecies;

#ifdef ROGUE_EXPANSION
    species = GET_BASE_SPECIES_ID(species);
#endif

    for(i = 0; i < WILD_ENCOUNTER_TOTAL_CAPACITY; ++i)
    {
        checkSpecies = gRogueRun.wildEncounters.species[i];

#ifdef ROGUE_EXPANSION
        checkSpecies = GET_BASE_SPECIES_ID(checkSpecies);
#endif

        if(species == checkSpecies)
        {
            return i;
        }
    }

    return WILD_ENCOUNTER_TOTAL_CAPACITY;
}

void Rogue_ModifyWildMonHeldItem(u16* itemId)
{
    if(Rogue_IsRunActive())
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_WILD_DEN)
        {
            *itemId = 0;
            return;
        }

        if(!IsGenEnabled(ItemToGen(*itemId)))
        {
            *itemId = 0;
        }
        else
        {
            // Banned Items
            switch (*itemId)
            {
            case ITEM_SACRED_ASH:
            case ITEM_REVIVAL_HERB:
            case ITEM_REVIVE:
            case ITEM_MAX_REVIVE:
            case ITEM_RARE_CANDY:
            case ITEM_HEART_SCALE:
            case ITEM_LUCKY_EGG:
                *itemId = 0;
                break;
            }
        }

    }
    else if(GetSafariZoneFlag() || Rogue_InWildSafari())
    {
        *itemId = 0;
    }

}

static bool8 IsChainSpeciesValidForSpawning(u8 area, u16 species)
{
    u8 i;

    if(species == SPECIES_NONE)
        return FALSE;

    if(area == 1) //WILD_AREA_WATER)
    {
        for(i = 0; i < WILD_ENCOUNTER_WATER_CAPACITY; ++i)
        {
            if(GetWildWaterEncounter(i) == species)
                return TRUE;
        }
    }
    else
    {
        u16 count = GetCurrentWildEncounterCount();

        for(i = 0; i < count; ++i)
        {
            if(GetWildGrassEncounter(i) == species)
                return TRUE;
        }
    }

    return FALSE;
}

static u16 GetChainSpawnOdds(u8 encounterCount)
{
    return 10 - min(encounterCount, 9);
}

static bool8 ForceChainSpeciesSpawn(u8 area)
{
    if(GetWildChainCount() > 1 && IsChainSpeciesValidForSpawning(area, GetWildChainSpecies()))
    {
        // We're allow to spawn the chain species in for this area
        return ((Random() % GetChainSpawnOdds(GetWildChainCount())) == 0);
    }

    return FALSE;
}

void Rogue_CreateWildMon(u8 area, u16* species, u8* level, bool8* forceShiny)
{
    if(Rogue_InWildSafari())
    {
        *species = SPECIES_ABRA;
        *level = STARTER_MON_LEVEL;
        *forceShiny = FALSE;
        return;
    }

    // Note: Don't seed individual encounters
    else if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
        u16 shinyOdds = Rogue_GetShinyOdds();

        if(GetSafariZoneFlag())
            *level  = CalculateWildLevel(3);
        else
            *level  = CalculateWildLevel(6);

        if(ForceChainSpeciesSpawn(area))
        {
            *species = GetWildChainSpecies();
            shinyOdds = GetEncounterChainShinyOdds(GetWildChainCount());
        }
        else if(area == 1) //WILD_AREA_WATER)
        {
            u16 randIdx = Random() % WILD_ENCOUNTER_WATER_CAPACITY; 

            *species = GetWildWaterEncounter(randIdx);
        }
        else
        {
            u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
            u16 count = GetCurrentWildEncounterCount();
            u16 historyBufferCount = ARRAY_COUNT(gRogueLocal.wildEncounterHistoryBuffer);
            u16 randIdx;
            
            do
            {
                // Prevent recent duplicates when on a run (Don't use this in safari mode though)
                randIdx = Random() % count; 
                *species = GetWildGrassEncounter(randIdx);

                if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_LATERMANNER)
                    break;
            }
            while(!GetSafariZoneFlag() && (count > historyBufferCount) && HistoryBufferContains(&gRogueLocal.wildEncounterHistoryBuffer[0], historyBufferCount, *species));

            HistoryBufferPush(&gRogueLocal.wildEncounterHistoryBuffer[0], historyBufferCount, *species);
        }

        *forceShiny = (Random() % shinyOdds) == 0;
    }
}

u16 Rogue_SelectRandomWildMon(void)
{
    if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
        u16 count = GetCurrentWildEncounterCount();
        return GetWildGrassEncounter(Random() % count);
    }

    return SPECIES_NONE;
}

bool8 Rogue_PreferTraditionalWildMons(void)
{
    if(Rogue_IsRunActive())
    {
        return !Rogue_GetConfigToggle(DIFFICULTY_TOGGLE_OVERWORLD_MONS);
    }

    return FALSE;
}

bool8 Rogue_AreWildMonEnabled(void)
{
    if(Rogue_PreferTraditionalWildMons())
    {
        return FALSE;
    }

    if(Rogue_IsRunActive() || GetSafariZoneFlag())
    {
        return GetCurrentWildEncounterCount() > 0;
    }

    if(Rogue_InWildSafari())
    {
        return TRUE;
    }

    return FALSE;
}

void Rogue_CreateEventMon(u16* species, u8* level, u16* itemId)
{
    if(Rogue_InWildSafari())
    {
        // Thrown away later, so doesn't matter too much
        *level = STARTER_MON_LEVEL;
    }
    else
    {
        *level = CalculateWildLevel(3);
    }
}

void Rogue_ModifyEventMon(struct Pokemon* mon)
{
    if(Rogue_InWildSafari())
    {
        u32 value;
        struct RogueSafariMon* safariMon = RogueSafari_GetPendingBattleMon();

        AGB_ASSERT(safariMon != NULL);
        if(safariMon != NULL)
        {
            u8 text[POKEMON_NAME_LENGTH + 1];
            u16 eggSpecies = Rogue_GetEggSpecies(safariMon->species);

            RogueSafari_CopyFromSafariMon(safariMon, &mon->box);

            // Make baby form
            if(eggSpecies != safariMon->species)
            {
                SetMonData(mon, MON_DATA_SPECIES, &eggSpecies);
                GetMonData(mon, MON_DATA_NICKNAME, text);

                if(StringCompareN(text, gSpeciesNames[safariMon->species], POKEMON_NAME_LENGTH) == 0)
                {
                    // Doesn't have a nickname so update to match species name
                    StringCopy_Nickname(text, gSpeciesNames[eggSpecies]);
                    SetMonData(mon, MON_DATA_NICKNAME, text);
                }
            }

            CalculateMonStats(mon);
        }
    }
    else
    {
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_WILD_DEN)
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
                Rogue_ApplyMonPreset(mon, GetMonData(mon, MON_DATA_LEVEL), &gPresetMonTable[species].presets[presetIndex]);
            }

            // Bump 2 of the IVs to max
            SetMonData(mon, MON_DATA_HP_IV + statA, &temp);
            SetMonData(mon, MON_DATA_HP_IV + statB, &temp);

            // Clear held item
            temp = 0;
            SetMonData(mon, MON_DATA_HELD_ITEM, &temp);
        }
        else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY)
        {
            u8 i;
            u16 moveId;

            // Replace roar with hidden power to avoid pokemon roaring itself out of battle
            for (i = 0; i < MAX_MON_MOVES; i++)
            {
                moveId = GetMonData(mon, MON_DATA_MOVE1 + i);
                if( moveId == MOVE_ROAR || 
                    moveId == MOVE_WHIRLWIND || 
                    moveId == MOVE_EXPLOSION ||
                    moveId == MOVE_SELF_DESTRUCT || 
                    moveId == MOVE_TELEPORT)
                {
                    moveId = MOVE_HIDDEN_POWER;
                    SetMonData(mon, MON_DATA_MOVE1 + i, &moveId);
                    SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[moveId].pp);
                }
            }
        }
    }
}

void Rogue_ModifyScriptMon(struct Pokemon* mon)
{
    if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_MINIBOSS)
    {
        u32 temp;
        u16 species = GetMonData(mon, MON_DATA_SPECIES);
        u16 statA = (Random() % 6);
        u16 statB = (statA + 1 + (Random() % 5)) % 6;

        // Apply the miniboss preset for this mon
        {
            u8 i;
            u8 target;
            u8 partySize = CalculateEnemyPartyCount();

            // Find the matching species
            for(i = 0; i < partySize; ++i)
            {
                if(species == GetMonData(&gEnemyParty[i], MON_DATA_SPECIES))
                    break;
            }

            target = i;

            if(target != partySize)
            {
                struct RogueMonPreset customPreset;
                customPreset.heldItem = GetMonData(&gEnemyParty[target], MON_DATA_HELD_ITEM);
                customPreset.abilityNum = GetMonAbility(&gEnemyParty[target]);

                for(i = 0; i < MAX_MON_MOVES; ++i)
                    customPreset.moves[i] = GetMonData(&gEnemyParty[target], MON_DATA_MOVE1 + i);

                Rogue_ApplyMonPreset(mon, Rogue_CalculatePlayerMonLvl(), &customPreset);
            }
        }

        // Bump 2 of the IVs to max
        temp = 31;
        SetMonData(mon, MON_DATA_HP_IV + statA, &temp);
        SetMonData(mon, MON_DATA_HP_IV + statB, &temp);

        // Clear held item
        temp = 0;
        SetMonData(mon, MON_DATA_HELD_ITEM, &temp);

        // Set to the correct level
        temp = Rogue_ModifyExperienceTables(gBaseStats[species].growthRate, Rogue_CalculatePlayerMonLvl());
        SetMonData(mon, MON_DATA_EXP, &temp);
        CalculateMonStats(mon);

        temp = GetMonData(mon, MON_DATA_LEVEL);
        SetMonData(mon, MON_DATA_MET_LEVEL, &temp);
    }
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
    RogueQuery_ItemsNotInPocket(POCKET_CHARMS);

    if(itemCategory != ROGUE_SHOP_BERRIES)
    {
        RogueQuery_ItemsNotInPocket(POCKET_BERRIES);
    }

    // Just sell PP max rather than be fiddly with price
    RogueQuery_Exclude(ITEM_PP_UP);

#ifdef ROGUE_EXPANSION
    RogueQuery_ItemsExcludeRange(ITEM_SEA_INCENSE, ITEM_PURE_INCENSE);

    // Merchants can't sell plates
    RogueQuery_ItemsExcludeRange(ITEM_FLAME_PLATE, ITEM_FAIRY_MEMORY);

    // Not allowed to buy these items in the hub
    if(!Rogue_IsRunActive())
    {
        RogueQuery_ItemsExcludeRange(ITEM_HEALTH_FEATHER, ITEM_SWIFT_FEATHER);
        RogueQuery_ItemsExcludeRange(ITEM_HP_UP, ITEM_CARBOS);
    }
#else
    // Not allowed to buy these items in the hub
    if(!Rogue_IsRunActive())
    {
        // These items aren't next to each other in vanilla
        RogueQuery_ItemsExcludeRange(ITEM_HP_UP, ITEM_CALCIUM);
        RogueQuery_Exclude(ITEM_ZINC);
    }
#endif

    switch(itemCategory)
    {
        case ROGUE_SHOP_GENERAL:
            RogueQuery_ItemsMedicine();

            RogueQuery_Include(ITEM_REPEL);
            RogueQuery_Include(ITEM_SUPER_REPEL);
            RogueQuery_Include(ITEM_MAX_REPEL);

            RogueQuery_ItemsInPriceRange(10, 300 + difficulty * 400);

            if(difficulty < 4)
            {
                RogueQuery_Exclude(ITEM_FULL_HEAL);
            }
            else if(Rogue_IsRunActive())
            {
                RogueQuery_Include(ITEM_ESCAPE_ROPE);
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
                if(difficulty <= 7)
                    itemCapacity = 10 + 4 * difficulty;
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 1500;
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
                if(difficulty <= 7)
                    itemCapacity = 4 + 4 * difficulty;
            }
            else if(difficulty <= 5)
            {
                // Remove contents 
                RogueQuery_ItemsInPriceRange(10, 11);
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 1500;
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
                if(difficulty <= 7)
                    itemCapacity = 4 + 1 * difficulty;
            }
            else
            {
#ifdef ROGUE_EXPANSION
                if(!IsQuestCollected(QUEST_MegaEvo))
                    RogueQuery_ItemsExcludeRange(ITEM_VENUSAURITE, ITEM_DIANCITE);

                if(!IsQuestCollected(QUEST_ZMove))
                    RogueQuery_ItemsExcludeRange(ITEM_NORMALIUM_Z, ITEM_ULTRANECROZIUM_Z);
#endif
            }

            if(Rogue_IsRunActive())
                *minSalePrice = 1500;
            else
                *minSalePrice = 3000;
            break;

        case ROGUE_SHOP_QUEST_REWARDS:
            RogueQuery_ExcludeAll();

            // Will include below

            *minSalePrice = 2000;
            break;

        case ROGUE_SHOP_BERRIES:
            {
                // Include berries from collected quests
                u16 i, j;
                RogueQuery_ExcludeAll();

                for(i = QUEST_FIRST; i < QUEST_CAPACITY; ++i)
                {
                    if(IsQuestCollected(i))
                    {
                        for(j = 0; j < QUEST_MAX_REWARD_COUNT; ++j)
                        {
                            if(gRogueQuests[i].rewards[j].type == QUEST_REWARD_NONE)
                            {
                                break;
                            }
                            else if(gRogueQuests[i].rewards[j].type == QUEST_REWARD_GIVE_ITEM)
                            {
                                u16 itemId = gRogueQuests[i].rewards[j].params[0];
                                if(itemId >= FIRST_BERRY_INDEX && itemId <= LAST_BERRY_INDEX)
                                {
                                    RogueQuery_Include(itemId);
                                }
                            }
                        }
                    }
                }

                *minSalePrice = 2000;
            }
            break;

            
        case ROGUE_SHOP_CHARMS:
            {
                RogueQuery_ExcludeAll();

                #ifdef ROGUE_DEBUG
                // Normally we only can buy curse items
                RogueQuery_IncludeRange(FIRST_ITEM_CHARM, LAST_ITEM_CHARM);
                #endif

                RogueQuery_IncludeRange(FIRST_ITEM_CURSE, LAST_ITEM_CURSE);
            }
            *minSalePrice = 0;
            break;
    };

    // Apply shop reward items (Only applicable in hb)
    if(!Rogue_IsRunActive())
    {
        u16 i, j;
        u16 itemId;

        for(i = QUEST_FIRST; i < QUEST_CAPACITY; ++i)
        {
            for(j = 0; j < QUEST_MAX_ITEM_SHOP_REWARD_COUNT; ++j)
            {
                itemId = gRogueQuests[i].unlockedShopRewards[j];
                if(itemId == ITEM_NONE)
                    break;

                // Always put reward items in the reward shop (Not allowed in other shops)
                if(itemCategory == ROGUE_SHOP_QUEST_REWARDS && IsQuestCollected(i))
                    RogueQuery_Include(itemId);
                else
                    RogueQuery_Exclude(itemId);
            }
        }
    }

    RogueQuery_CollapseItemBuffer();

    if(itemCapacity != 0)
    {
        //u16 maxGen = VarGet(VAR_ROGUE_ENABLED_GEN_LIMIT);

        //if(maxGen > 3)
        //{
        //    // Increase capacity by a little bit to accomadate for extra items when in higher gens
        //    itemCapacity += (maxGen - 3) * 2;
        //}

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

static void ApplyTutorMoveCapacity(u8* count, u16* moves, u16 capacity)
{
    u16 i;
    u16 randIdx;
    u32 startSeed = gRngRogueValue;

    while(*count > capacity)
    {
        
        if(Rogue_IsRunActive())
            randIdx = RogueRandom() % *count;
        else
            randIdx = 0; // Always take from the front, as that's where the "good moves" are

        --*count;

        for(i = randIdx; i < *count; ++i)
        {
            moves[i] = moves[i + 1];
        }
    }

    gRngRogueValue = startSeed;
}

void Rogue_ModifyTutorMoves(struct Pokemon* mon, u8 tutorType, u8* count, u8* hiddenCount, u16* moves)
{
    if(tutorType != 0) // TEACH_STATE_RELEARN
    {
        u16 difficulty;
        u16 capacity = 0; // MAX is 0
        u8 startCount = *count;
    
        if(Rogue_IsRunActive())
        {
            difficulty = gRogueRun.currentDifficulty;

            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
                difficulty = 13;

            if(difficulty < 8)
                capacity = 3 + difficulty * 1;
        }
        else
        {
            capacity = 5;

            if(IsQuestCollected(QUEST_NoFainting2) && IsQuestCollected(QUEST_NoFainting3))
                capacity = 0;
            else if(IsQuestCollected(QUEST_NoFainting2) || IsQuestCollected(QUEST_NoFainting3))
                capacity += 5;
        }

        if(capacity != 0)
        {
            ApplyTutorMoveCapacity(count, moves, capacity);
        }

        *hiddenCount = startCount - *count;
        
        // Remove moves we already know (We want to do this after capacity so the randomisation is consistent)
        {
            u16 readIdx;
            u16 writeIdx = 0;

            for(readIdx = 0; readIdx < *count; ++readIdx)
            {
                if(!MonKnowsMove(mon, moves[readIdx]))
                {
                    moves[writeIdx++] = moves[readIdx];
                }
            }

            *count = writeIdx;
        }
    }
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

    if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_LATERMANNER)
    {
        return SPECIES_FARFETCHD;
    }

    // Prevent duplicates, if possible
    do
    {
        randIdx = RogueRandomRange(queryCount, FLAG_SET_SEED_WILDMONS);
        species = RogueQuery_BufferPtr()[randIdx];
    }
    while(ContainsSpecies(party, monIdx, species) && monIdx < queryCount);

    return species;
}


static u8 RandomiseWildEncounters_CalculateWeight(u16 index, u16 species, void* data)
{
#ifdef ROGUE_EXPANSION
    switch (species)
    {
    case SPECIES_DEERLING:
    case SPECIES_SAWSBUCK:
        if(RogueToD_GetSeason() != SEASON_SPRING)
            return 0;
        break;

    case SPECIES_DEERLING_SUMMER:
    case SPECIES_SAWSBUCK_SUMMER:
        if(RogueToD_GetSeason() != SEASON_SUMMER)
            return 0;
        break;

    case SPECIES_DEERLING_AUTUMN:
    case SPECIES_SAWSBUCK_AUTUMN:
        if(RogueToD_GetSeason() != SEASON_AUTUMN)
            return 0;
        break;

    case SPECIES_DEERLING_WINTER:
    case SPECIES_SAWSBUCK_WINTER:
        if(RogueToD_GetSeason() != SEASON_WINTER)
            return 0;
        break;

    case SPECIES_LYCANROC:
        if(RogueToD_IsNight() || RogueToD_IsDusk())
            return 0;
        break;

    case SPECIES_LYCANROC_MIDNIGHT:
        if(!RogueToD_IsNight())
            return 0;
        break;

    case SPECIES_LYCANROC_DUSK:
        if(!RogueToD_IsDusk())
            return 0;
        break;

    case SPECIES_ROCKRUFF:
        if(RogueToD_IsDusk())
            return 0;
        break;

    case SPECIES_ROCKRUFF_OWN_TEMPO:
        if(!RogueToD_IsDusk())
            return 0;
        break;

    default:
        break;
    }
#endif


    return 1;
}

static void RandomiseWildEncounters(void)
{
    u8 maxlevel = CalculateWildLevel(0);
    u32 typeFlags;

    typeFlags = Rogue_GetTypeFlagsFromArray(
        &gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable[0], 
        ARRAY_COUNT(gRogueRouteTable.routes[gRogueRun.currentRouteIndex].wildTypeTable)
    );

    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();

    // Prefilter to mons of types we're interested in
    RogueMonQuery_EvosContainType(QUERY_FUNC_INCLUDE, typeFlags);
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(maxlevel - min(6, maxlevel - 1), FALSE, FALSE);

    // Now we've evolved we're only caring about mons of this type
    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, typeFlags);

    {
        u8 i;
        RogueWeightQuery_Begin();

        RogueWeightQuery_CalculateWeights(RandomiseWildEncounters_CalculateWeight, NULL);

        for(i = 0; i < WILD_ENCOUNTER_GRASS_CAPACITY; ++i)
        {
            if(RogueWeightQuery_HasAnyWeights())
                gRogueRun.wildEncounters.species[i] = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(RogueRandom(), 0);
            else
                gRogueRun.wildEncounters.species[i] = SPECIES_NONE;

            gRogueRun.wildEncounters.catchCounts[i] = 0;
        }

        RogueWeightQuery_End();
    }

    RogueMonQuery_End();
}

static u8 RandomiseFishingEncounters_CalculateWeight(u16 index, u16 species, void* data)
{
    // We should prefer simpler water mons here
    return 1;
}

static void RandomiseFishingEncounters(void)
{
    RogueMonQuery_Begin();

    RogueMonQuery_IsSpeciesActive();

    // Prefilter to mons of types we're interested in
    RogueMonQuery_EvosContainType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(TYPE_WATER));
    RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

    RogueMonQuery_TransformIntoEggSpecies();

    // Now we've evolved we're only caring about mons of this type
    RogueMonQuery_IsOfType(QUERY_FUNC_INCLUDE, MON_TYPE_VAL_TO_FLAGS(TYPE_WATER));

    {
        u8 i;
        RogueWeightQuery_Begin();

        RogueWeightQuery_CalculateWeights(RandomiseFishingEncounters_CalculateWeight, NULL);

        for(i = 0; i < WILD_ENCOUNTER_WATER_CAPACITY; ++i)
        {
            if(RogueWeightQuery_HasAnyWeights())
                gRogueRun.wildEncounters.species[WILD_ENCOUNTER_GRASS_CAPACITY + i] = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(RogueRandom(), 0);
            else
                gRogueRun.wildEncounters.species[WILD_ENCOUNTER_GRASS_CAPACITY + i] = SPECIES_NONE;

            gRogueRun.wildEncounters.catchCounts[WILD_ENCOUNTER_GRASS_CAPACITY + i] = 0;
        }

        RogueWeightQuery_End();
    }

    RogueMonQuery_End();
}

void Rogue_SafariTypeForMap(u8* outArray, u8 arraySize)
{
    AGB_ASSERT(arraySize == 3);

    outArray[0] = TYPE_NONE;
    outArray[1] = TYPE_NONE;
    outArray[2] = TYPE_NONE;

    if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_SOUTH)
    {
        outArray[0] = TYPE_NORMAL;
        outArray[1] = TYPE_FIGHTING;
#ifdef ROGUE_EXPANSION
        outArray[2] = TYPE_FAIRY;
#endif
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_SOUTHWEST)
    {
        outArray[0] = TYPE_GRASS;
        outArray[1] = TYPE_POISON;
        outArray[2] = TYPE_DARK;
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTHWEST)
    {
        outArray[0] = TYPE_DRAGON;
        outArray[1] = TYPE_STEEL;
        outArray[2] = TYPE_PSYCHIC;
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTH)
    {
        outArray[0] = TYPE_FLYING;
        outArray[1] = TYPE_GHOST;
        outArray[2] = TYPE_FIRE;
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_SAFARI_ZONE_NORTHEAST)
    {
        outArray[0] = TYPE_ROCK;
        outArray[1] = TYPE_GROUND;
        outArray[2] = TYPE_ELECTRIC;
    }
    else // SAFARI_ZONE_SOUTHEAST
    {
        outArray[0] = TYPE_WATER;
        outArray[1] = TYPE_BUG;
        outArray[2] = TYPE_ICE;
    }
}

static void RandomiseSafariWildEncounters(void)
{
    // No longer supported code path
    AGB_ASSERT(FALSE);
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

static u8 CalculateWildLevel(u8 variation)
{
    u8 wildLevel;
    u8 playerLevel = Rogue_CalculatePlayerMonLvl();

    if(GetSafariZoneFlag())
    {
        if((Random() % 6) == 0)
        {
            // Occasionally throw in starter level mons
            wildLevel = 7;
        }
        else
        {
            wildLevel = GetLeadMonLevel();
        }
    }
    else if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_LEGENDARY || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_WILD_DEN)
    {
        wildLevel = playerLevel - 5;
    }
    else if(playerLevel < 10)
    {
        wildLevel = 4;
    }
    else
    {
        wildLevel = playerLevel - 7;
    }

    variation = min(variation, wildLevel);

    if(variation != 0)
        return wildLevel - (Random() % variation);
    else
        return wildLevel;
}

u8 Rogue_GetEncounterDifficultyModifier()
{
    return (gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE ? gRogueAdvPath.currentRoomParams.perType.route.difficulty : 1);
}

static bool8 RogueRandomChanceTrainer()
{
    u8 difficultyLevel = gRogueRun.currentDifficulty;
    u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
    s32 chance = 4 * (difficultyLevel + 1);

    if(difficultyModifier == 0) // Easy
        chance = max(5, chance - 20);
    else if(difficultyModifier == 2) // Hard
        chance = max(15, chance - 15); // Trainers are hard so slightly less frequent, but harder
    else
        chance = max(10, chance);

    return RogueRandomChance(chance, FLAG_SET_SEED_TRAINERS);
}

static bool8 RogueRandomChanceItem()
{
    s32 chance;
    u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();

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

    if(!FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        if(difficultyModifier == 0) // Easy
            chance = max(10, chance - 25);
        else if(difficultyModifier == 2) // Hard
            chance = min(100, chance + 25);
    }

    return TRUE;//RogueRandomChance(chance, FLAG_SET_SEED_ITEMS);
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

static void RandomiseItemContent(u8 difficultyLevel)
{
    u16 queryCount;
    u8 difficultyModifier = Rogue_GetEncounterDifficultyModifier();
    u8 dropRarity = gRogueRouteTable.routes[gRogueRun.currentRouteIndex].dropRarity;

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        // Give us 1 room of basic items
        if(gRogueRun.enteredRoomCounter > 1)
        {
            dropRarity += 10;
        }
    }
    else
    {
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
    }

    // Queue up random items
    RogueQuery_Clear();

    RogueQuery_ItemsIsValid();
    RogueQuery_ItemsNotInPocket(POCKET_KEY_ITEMS);
    RogueQuery_ItemsNotInPocket(POCKET_CHARMS);
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
        RogueQuery_Include(ITEM_ESCAPE_ROPE);
        RogueQuery_Include(ITEM_RARE_CANDY);
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
        u8 i;
        for(i = 0; i < ROGUE_ITEM_COUNT; ++i)
        {
            VarSet(VAR_ROGUE_ITEM_START + i, RogueQuery_BufferPtr()[RogueRandomRange(queryCount, FLAG_SET_SEED_ITEMS)]);
        }
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

static void RandomiseCharmItems(void)
{
    u16 tempBuffer[5];
    u16 tempBufferCount = 0;

    // Charm Items
    VarSet(VAR_ROGUE_ITEM0, Rogue_NextCharmItem(tempBuffer, tempBufferCount++));
    VarSet(VAR_ROGUE_ITEM1, Rogue_NextCharmItem(tempBuffer, tempBufferCount++));
    VarSet(VAR_ROGUE_ITEM2, Rogue_NextCharmItem(tempBuffer, tempBufferCount++));

    // Curse Items
    VarSet(VAR_ROGUE_ITEM10, Rogue_NextCurseItem(tempBuffer, tempBufferCount++));
    //VarSet(VAR_ROGUE_ITEM11, Rogue_NextCurseItem(tempBuffer, 4));
    //VarSet(VAR_ROGUE_ITEM12, Rogue_NextCurseItem(tempBuffer, 5));
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