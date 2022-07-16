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
#include "item.h"
#include "load_save.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"

#include "rogue_controller.h"
#include "rogue_query.h"


#define ROGUE_TRAINER_COUNT (FLAG_ROGUE_TRAINER_END - FLAG_ROGUE_TRAINER_START + 1)
#define ROGUE_ITEM_COUNT (FLAG_ROGUE_ITEM_END - FLAG_ROGUE_ITEM_START + 1)

// Gym leaders
#define ROOM_IDX_BOSS0      3
#define ROOM_IDX_BOSS1      6
#define ROOM_IDX_BOSS2      10
#define ROOM_IDX_BOSS3      14
#define ROOM_IDX_BOSS4      18
#define ROOM_IDX_BOSS5      22
#define ROOM_IDX_BOSS6      26
#define ROOM_IDX_BOSS7      30

// Elite 4
#define ROOM_IDX_BOSS8      35
#define ROOM_IDX_BOSS9      40
#define ROOM_IDX_BOSS10     45
#define ROOM_IDX_BOSS11     50

// Champions
#define ROOM_IDX_BOSS12     55
#define ROOM_IDX_BOSS13     60

#define ROOM_BOSS_GYM_START             ROOM_IDX_BOSS0
#define ROOM_BOSS_GYM_END               ROOM_IDX_BOSS7
#define ROOM_BOSS_ELITEFOUR_START       ROOM_IDX_BOSS8
#define ROOM_BOSS_ELITEFOUR_END         ROOM_IDX_BOSS11
#define ROOM_BOSS_CHAMPION_START        ROOM_IDX_BOSS12
#define ROOM_BOSS_CHAMPION_END          ROOM_IDX_BOSS13


static u8 GetDifficultyLevel(u16 roomIdx);

static u8 CalculatePlayerLevel(void);
static u8 CalculateWildLevel(void);
static u8 CalculateTrainerLevel(u16 trainerNum);

static void RandomiseWildEncounters(void);
static void ResetTrainerBattles(void);
static void RandomiseEnabledTrainers(void);
static void RandomiseEnabledItems(void);
static void RandomiseBerryTrees(void);

EWRAM_DATA struct RogueRunData gRogueRun = {};
EWRAM_DATA struct RogueHubData gRogueSaveData = {};

bool8 Rogue_IsRunActive(void)
{
    return FlagGet(FLAG_ROGUE_RUN_ACTIVE);
}

bool8 Rogue_ForceExpAll(void)
{
    return TRUE;
}

void Rogue_ModifyExpGained(struct Pokemon *mon, s32* expGain)
{
    if(Rogue_IsRunActive())
    {
        u8 targetLevel = CalculatePlayerLevel();
        u8 currentLevel = GetMonData(mon, MON_DATA_LEVEL);

        if(currentLevel != 100)
        {
            // Could base of gBaseStats[species].growthRate?
            u32 currLvlExp = gExperienceTables[GROWTH_FAST][currentLevel];
            u32 nextLvlExp = gExperienceTables[GROWTH_FAST][currentLevel + 1];
            u32 lvlExp = (nextLvlExp - currLvlExp);

            if(currentLevel < targetLevel)
            {
                u8 delta = targetLevel - currentLevel;
                s32 desiredExpGain = 0;
                if(delta <= 3)
                {
                    // Give 25% of level always
                    desiredExpGain = (nextLvlExp * 25) / 100;
                }
                else
                {
                    // Give faster levels to catch up
                    delta = (delta - 3);
                    desiredExpGain = (nextLvlExp * 75 * delta) / 100;
                }

                *expGain = max(*expGain, desiredExpGain);
            }
            else
            {
                // No EXP once at target level
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

void Rogue_ModifyCatchRate(u8* catchRate, u8* ballMultiplier)
{
    // TODO -
    //*ballMultiplier = 50;
    *ballMultiplier = 255; // Masterball equiv

    if(*catchRate < 50)
        *catchRate = 50;
}

#ifdef ROGUE_DEBUG
const u8 gText_RogueDebug_Header[] = _("ROGUE DEBUG");
const u8 gText_RogueDebug_Room[] = _("\nRoom: ");
const u8 gText_RogueDebug_Difficulty[] = _("\nDifficulty: ");
const u8 gText_RogueDebug_PlayerLvl[] = _("\nPlayer lvl: ");
const u8 gText_RogueDebug_WildLvl[] = _("\nWild lvl: ");

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return TRUE;
}

static u8* AppendNumberField(u8* strPointer, const u8* field, u32 num)
{
    ConvertIntToDecimalStringN(gStringVar1, num, STR_CONV_MODE_RIGHT_ALIGN, 2);

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
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_Room, gRogueRun.currentRoomIdx);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_Difficulty, difficultyLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_PlayerLvl, playerLevel);
    strPointer = AppendNumberField(strPointer, gText_RogueDebug_WildLvl, wildLevel);

    return gStringVar4;
}
#else

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return Rogue_IsRunActive();
}

u8* Rogue_GetMiniMenuContent(void)
{
    ConvertIntToDecimalStringN(gStringVar1, gRogueRun.currentRoomIdx, STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar2, gPlayerPartyCount, STR_CONV_MODE_RIGHT_ALIGN, 2);
    StringExpandPlaceholders(gStringVar4, gText_RogueRoomProgress);
    return gStringVar4;
}
#endif

void Rogue_OnNewGame(void)
{
    struct Pokemon starterMon;

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);

    FlagSet(FLAG_SYS_B_DASH);
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKEMON_GET);
    EnableNationalPokedex();

    SetLastHealLocationWarp(HEAL_LOCATION_ROGUE_HUB);

#ifdef ROGUE_DEBUG
    //AddBagItem(ITEM_RARE_CANDY, 99);
    //AddBagItem(ITEM_RARE_CANDY, 99);
    //AddBagItem(ITEM_RARE_CANDY, 99);
    SetMoney(&gSaveBlock1Ptr->money, 60000);

    CreateMon(&starterMon, SPECIES_RAYQUAZA, 100, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    GiveMonToPlayer(&starterMon);
#else
    SetMoney(&gSaveBlock1Ptr->money, 100);

    // TEMP - Should do this by script
    CreateMon(&starterMon, SPECIES_RATTATA, 5, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    GiveMonToPlayer(&starterMon);
#endif
}

void Rogue_SetDefaultOptions(void)
{
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
    gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SET;
    //gSaveBlock2Ptr->optionsSound = OPTIONS_SOUND_MONO;
    //gSaveBlock2Ptr->optionsBattleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
    //gSaveBlock2Ptr->optionsBattleSceneOff = FALSE;
    //gSaveBlock2Ptr->regionMapZoom = FALSE;
}

void Rogue_OnLoadMap(void)
{
    // TODO - Remove this

    // Seems to be working? Need to track against flag here though, as this gets called for started maps
    //FlagSet(FLAG_SYS_POKEMON_GET);
    
        //*((s32*)((void*)0)) = 123;

    //s32 i;
    //struct WarpEvent *warpEvent = gMapHeader.events->warps;
    //u8 warpCount = gMapHeader.events->warpCount;
    //
    //for (i = 0; i < warpCount; i++, warpEvent++)
    //{
    //    if(i == 0)
    //    {
    //        // Skip first warp as that should always be left as the entry warp
    //        continue;
    //    }
//
    //    // Should be Prof lab
    //    warpEvent->warpId = 0;
    //    warpEvent->mapNum = 4;
    //    warpEvent->mapGroup = 0;
    //}

    // TODO - Do something
}

static void BeginRogueRun(void)
{
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);

    ClearBerryTrees();

    gRogueRun.currentRoomIdx = 0;
    
    // (1 / N) chance of appearing
    //gRogueRun.trainerSpawnChance = 15;
    //gRogueRun.itemSpawnChance = 75;
    //gRogueRun.berrySpawnChance = 100;

    // Store current states
    SavePlayerParty();
    LoadPlayerBag(); // Bag funcs named in opposite

    gRogueSaveData.money = GetMoney(&gSaveBlock1Ptr->money);
    gRogueSaveData.registeredItem = gSaveBlock1Ptr->registeredItem;

    FlagClear(FLAG_ROGUE_DEFEATED_ROXANNE);
    FlagClear(FLAG_ROGUE_DEFEATED_BRAWLY);
    FlagClear(FLAG_ROGUE_DEFEATED_WATTSON);
    FlagClear(FLAG_ROGUE_DEFEATED_FLANNERY);
    FlagClear(FLAG_ROGUE_DEFEATED_NORMAN);
    FlagClear(FLAG_ROGUE_DEFEATED_WINONA);
    FlagClear(FLAG_ROGUE_DEFEATED_LIZA);
    FlagClear(FLAG_ROGUE_DEFEATED_JUAN);
    
    FlagClear(FLAG_ROGUE_DEFEATED_SIDNEY);
    FlagClear(FLAG_ROGUE_DEFEATED_PHOEBE);
    FlagClear(FLAG_ROGUE_DEFEATED_GLACIA);
    FlagClear(FLAG_ROGUE_DEFEATED_DRAKE);

    FlagClear(FLAG_ROGUE_DEFEATED_WALLACE);
    FlagClear(FLAG_ROGUE_DEFEATED_STEVEN);
}

static void EndRogueRun(void)
{
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    //gRogueRun.currentRoomIdx = 0;

    // Restore current states
    LoadPlayerParty();
    SavePlayerBag(); // Bag funcs named in opposite

    SetMoney(&gSaveBlock1Ptr->money, gRogueSaveData.money);
    gSaveBlock1Ptr->registeredItem = gRogueSaveData.registeredItem;
}

static bool8 IsBossRoom(u16 roomIdx)
{
    switch(roomIdx)
    {
        case ROOM_IDX_BOSS0:
        case ROOM_IDX_BOSS1:
        case ROOM_IDX_BOSS2:
        case ROOM_IDX_BOSS3:
        case ROOM_IDX_BOSS4:
        case ROOM_IDX_BOSS5:
        case ROOM_IDX_BOSS6:
        case ROOM_IDX_BOSS7:
        case ROOM_IDX_BOSS8:
        case ROOM_IDX_BOSS9:
        case ROOM_IDX_BOSS10:
        case ROOM_IDX_BOSS11:
        case ROOM_IDX_BOSS12:
        case ROOM_IDX_BOSS13:
            return TRUE;
    };

    return FALSE;
}

static u8 GetDifficultyLevel(u16 roomIdx)
{
    if(roomIdx <= ROOM_IDX_BOSS0)
        return 0;
    if(roomIdx <= ROOM_IDX_BOSS1)
        return 1;
    if(roomIdx <= ROOM_IDX_BOSS2)
        return 2;
    if(roomIdx <= ROOM_IDX_BOSS3)
        return 3;
    if(roomIdx <= ROOM_IDX_BOSS4)
        return 4;
    if(roomIdx <= ROOM_IDX_BOSS5)
        return 5;
    if(roomIdx <= ROOM_IDX_BOSS6)
        return 6;
    if(roomIdx <= ROOM_IDX_BOSS7)
        return 7;

    if(roomIdx <= ROOM_IDX_BOSS8)
        return 8;
    if(roomIdx <= ROOM_IDX_BOSS9)
        return 9;
    if(roomIdx <= ROOM_IDX_BOSS10)
        return 10;
    if(roomIdx <= ROOM_IDX_BOSS11)
        return 11;

    if(roomIdx <= ROOM_IDX_BOSS12)
        return 12;
    return 13;
}

static void SelectBossRoom(u16 nextRoomIdx, struct WarpData *warp)
{
    u8 bossId = 0;

    do
    {
        //if(nextRoomIdx <= ROOM_BOSS_GYM_START)
        //{
        //    // Smaller pool of allowed bosses
        //    bossId = Random() % 4;
        //}
        
        bossId = Random() % 8;
    }
    while(FlagGet(FLAG_ROGUE_DEFEATED_ROXANNE + bossId));

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
    };
}

void Rogue_OnWarpIntoMap(void)
{
    if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB_TRANSITION && !Rogue_IsRunActive())
    {
        BeginRogueRun();
    }
    else if(gMapHeader.mapLayoutId == LAYOUT_ROGUE_HUB && Rogue_IsRunActive())
    {
        EndRogueRun();
    }
    else if(Rogue_IsRunActive())
    {
        ++gRogueRun.currentRoomIdx;
        
        if(IsBossRoom(gRogueRun.currentRoomIdx))
        {
            ResetTrainerBattles();
            RandomiseEnabledItems();
        }
        else
        {
            gRogueRun.currentRouteType = ROGUE_ROUTE_FIELD;

            RandomiseWildEncounters();
            ResetTrainerBattles();
            RandomiseEnabledTrainers();
            RandomiseEnabledItems();
            RandomiseBerryTrees();
        }
    }
}


void Rogue_OnSetWarpData(struct WarpData *warp)
{
    // Configure random warp
    //if(warp->mapGroup == MAP_GROUP(ROGUE_HUB_TRANSITION) && warp->mapNum == MAP_NUM(ROGUE_HUB_TRANSITION) && warp->warpId == 1)

    if(Rogue_IsRunActive())
    {
        u16 nextRoomIdx = gRogueRun.currentRoomIdx + 1;

        if(IsBossRoom(nextRoomIdx))
        {
            SelectBossRoom(nextRoomIdx, warp);
        }
        else
        {
            // Normal room
            if((nextRoomIdx % 2) == 0)
            {
                warp->mapGroup = MAP_GROUP(ROGUE_ROUTE_FIELD0);
                warp->mapNum = MAP_NUM(ROGUE_ROUTE_FIELD0);
            }
            else
            {
                warp->mapGroup = MAP_GROUP(ROGUE_ROUTE_FIELD1);
                warp->mapNum = MAP_NUM(ROGUE_ROUTE_FIELD1);
            }
        }

        warp->warpId = 0;
        warp->x = -1;
        warp->y = -1;
    }
}

void RemoveAnyFaintedMons(void)
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
            // Dead so give back held item
            u16 heldItem = GetMonData(&gPlayerParty[read], MON_DATA_HELD_ITEM);
            if(heldItem != ITEM_NONE)
                AddBagItem(heldItem, 1);
        }
    }

    gPlayerPartyCount = alivePartyCount;

    for(read = gPlayerPartyCount; read < PARTY_SIZE; ++read)
        ZeroMonData(&gPlayerParty[read]);
}

void Rogue_Battle_StartTrainerBattle(void)
{
    // TODO - Check if double battle mode is active
    //if(gNoOfApproachingTrainers != 2 && gPlayerPartyCount >= 2)
    //{
    //    // Force double?
    //    gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
    //}
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
            RemoveAnyFaintedMons();
        }
    }
}

void Rogue_Battle_EndWildBattle(void)
{
    if(Rogue_IsRunActive())
    {
        if (IsPlayerDefeated(gBattleOutcome) != TRUE)
        {
            RemoveAnyFaintedMons();
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

static bool8 UseCompetitiveMoveset(u16 trainerNum)
{
    // Start using competitive movesets on 3rd gym
    if(IsBossTrainer(trainerNum) && GetDifficultyLevel(gRogueRun.currentRoomIdx) >= 2)
    {
        return TRUE;
    }

    return FALSE;
}

static void ConfigureTrainer(u16 trainerNum, u8* forceType, bool8* allowItemEvos, u8* monsCount)
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
    };

    if(IsBossTrainer(trainerNum))
    {
        if(difficultyLevel == 0)
        {
            *monsCount = 3;
            *allowItemEvos = FALSE;
        }
        else if(difficultyLevel == 1)
        {
            *monsCount = 4;
            *allowItemEvos = FALSE;
        }
        else if(difficultyLevel == 2)
        {
            *monsCount = 4;
            *allowItemEvos = FALSE;
        }
        else if(difficultyLevel <= 5)
        {
            *monsCount = 5;
            *allowItemEvos = FALSE;
        }
        else
        {
            *monsCount = 6;
            *allowItemEvos = TRUE;
        }
    }
    else
    {
        if(difficultyLevel == 0)
        {
            *monsCount = 1 + Random() % 2;
            *forceType = TYPE_NORMAL;
            *allowItemEvos = FALSE;
        }
        else if(difficultyLevel == 1)
        {
            *monsCount = 1 + Random() % 3;
            *forceType = TYPE_NORMAL;
            *allowItemEvos = FALSE;
        }
        else if(difficultyLevel == 2)
        {
            *monsCount = 2 + Random() % 2;
            *allowItemEvos = FALSE;
        }
        else if(difficultyLevel <= 7)
        {
            *monsCount = 3 + Random() % 1;
            *allowItemEvos = FALSE;
        }
        else if(difficultyLevel <= 11)
        {
            // Elite 4
            *monsCount = 3 + Random() % 1;
            *allowItemEvos = TRUE;
        }
        else
        {
            // Champion
            *monsCount = 4 + ((Random() % 5) == 0 ? 2 : 0);
            *allowItemEvos = TRUE;
        }
    }
}

bool8 Rogue_OverrideTrainerItems(u16* items)
{
    if(Rogue_IsRunActive())
    {
        //for (i = 0; i < MAX_TRAINER_ITEMS; i++)
        //{
        //    items[i] = ITEM_NONE;
        //}

        return TRUE;
    }

    return FALSE;
}

void Rogue_PreCreateTrainerParty(u16 trainerNum, bool8* useRogueCreateMon, u8* monsCount)
{
    if(Rogue_IsRunActive())
    {
        u8 allowedType = TYPE_NONE;
        bool8 allowItemEvos = FALSE;
        ConfigureTrainer(trainerNum, &allowedType, &allowItemEvos, monsCount);

        // Query for the current trainer team
        RogueQuery_Clear();

        RogueQuery_SpeciesIsValid();
        RogueQuery_SpeciesIsNotLegendary();

        if(allowedType != TYPE_NONE)
            RogueQuery_SpeciesOfType(allowedType);

        RogueQuery_TransformToEggSpecies();

        // Evolve the species to just below the wild encounter level
        RogueQuery_EvolveSpeciesToLevel(CalculateTrainerLevel(trainerNum));
        
        if(GetDifficultyLevel(gRogueRun.currentRoomIdx) >= 5)
            RogueQuery_EvolveSpeciesByItem();

        if(allowedType != TYPE_NONE)
            RogueQuery_SpeciesOfType(allowedType);

        RogueQuery_CollapseBuffer();

        *useRogueCreateMon = TRUE;
        return;
    }

    *useRogueCreateMon = FALSE;
}

void Rogue_CreateTrainerMon(u16 trainerNum, u8 monIdx, u8 totalMonCount, struct Pokemon *mon)
{
    u16 species;
    u8 level;
    u8 fixedIV;

    u16 queryCount = RogueQuery_BufferSize();
    u16 randIdx = Random() % queryCount;

    species = RogueQuery_BufferPtr()[randIdx];
    level = CalculateTrainerLevel(trainerNum) - 1;
    fixedIV = IsBossTrainer(trainerNum) ? MAX_PER_STAT_IVS : 0;

    if(monIdx == 0)
        level--;

    if(monIdx == totalMonCount - 1)
        level++;

    CreateMon(mon, species, level, fixedIV, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

    if(UseCompetitiveMoveset(trainerNum) && gPresetMonTable[species].presetCount != 0)
    {
        s32 i;
        u8 presetIdx = Random() % gPresetMonTable[species].presetCount;
        const struct RogueMonPreset* preset = &gPresetMonTable[species].presets[presetIdx];

        if(preset->abilityNum != ABILITY_NONE)
        {
            SetMonData(mon, MON_DATA_ABILITY_NUM, &preset->abilityNum);
        }

        if(preset->heldItem != ITEM_NONE)
        {
            SetMonData(mon, MON_DATA_HELD_ITEM, &preset->heldItem);
        }

        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            if(preset->moves[i] != MOVE_NONE)
            {
                SetMonData(mon, MON_DATA_MOVE1 + i, &preset->moves[i]);
                SetMonData(mon, MON_DATA_PP1 + i, &gBattleMoves[preset->moves[i]].pp);
            }
        }
    }
}

void Rogue_CreateWildMon(u8 area, u16* species, u8* level)
{
    if(Rogue_IsRunActive())
    {
        u8 maxlevel = CalculateWildLevel();
        u8 levelVariation = min(6,maxlevel - 1);
        const u16 count = ARRAY_COUNT(gRogueRun.wildEncounters);
        u16 randIdx = Random() % count;

        *species = gRogueRun.wildEncounters[randIdx];
        *level = maxlevel - (Random() % levelVariation);
    }
}

static void RandomiseWildEncounters(void)
{
    u8 maxlevel = CalculateWildLevel();

    // Query for the current route type
    RogueQuery_Clear();

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesIsNotLegendary();
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTable, gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTableCount);
    RogueQuery_TransformToEggSpecies();

    // Evolve the species to just below the wild encounter level
    RogueQuery_EvolveSpeciesToLevel(maxlevel - min(6, maxlevel - 1));
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTable, gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTableCount);

    RogueQuery_CollapseBuffer();

    {
        u8 i;
        u16 randIdx;
        u16 queryCount = RogueQuery_BufferSize();

        for(i = 0; i < ARRAY_COUNT(gRogueRun.wildEncounters); ++i)
        {
            randIdx = Random() % queryCount;
            gRogueRun.wildEncounters[i] = RogueQuery_BufferPtr()[randIdx];
        }
    }
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

static u8 CalculateWildLevel(void)
{
    return CalculatePlayerLevel() - 7;
}

static u8 CalculateBossLevel(u8 difficulty)
{
    // Gym leaders lvs 10 -> 80
    if(difficulty <= 7)
    {
        return 10 + 10 * difficulty;
    }
    else
    {
        // Both champions are lvl 100
        difficulty -= 7;
        return 80 + 4 * difficulty;
    }
}

static u8 CalculatePlayerLevel(void)
{
    u8 prevLevel;
    u8 currLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);

    if(currLevel == 0)
    {
        // Cannot do blending
        return CalculateBossLevel(currLevel);
    }
    else
    {
        prevLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx - 1);
    }

    if(currLevel == prevLevel)
    {
        return CalculateBossLevel(currLevel);
    }
    else
    {
        // We've just transitioned so use midpoint
        return (CalculateBossLevel(prevLevel) + CalculateBossLevel(currLevel)) / 2;
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
            return 2 + gRogueRun.currentRoomIdx;
        }
        else
        {
            // Trainers will lag behind to make grinding easier
            return CalculateBossLevel(difficultyLevel - 1);
        }
    }
}

static bool8 RandomChance(u8 chance)
{
    if(chance == 0)
        return FALSE;
    else if(chance >= 100)
        return TRUE;

    return ((Random() % 100) + 1) <= chance;
}

static bool8 RandomChanceTrainer()
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    u8 chance = 10 + 5 * difficultyLevel;

    return RandomChance(chance);
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
        chance = 75 - 5 * difficultyLevel;
    }

    return RandomChance(chance);
}

static bool8 RandomChanceBerry()
{
    u8 difficultyLevel = GetDifficultyLevel(gRogueRun.currentRoomIdx);
    u8 chance = 95 - 7 * difficultyLevel;

    return RandomChance(chance);
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

static void RandomiseEnabledItems(void)
{
    s32 i;
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

    // TODO - Randomise items
    VarSet(VAR_ROGUE_ITEM0, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM1, ITEM_BERRY_JUICE);
    VarSet(VAR_ROGUE_ITEM2, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM3, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM4, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM5, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM6, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM7, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM8, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM9, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM10, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM11, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM12, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM13, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM14, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM15, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM16, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM17, ITEM_POKE_BALL);
    VarSet(VAR_ROGUE_ITEM18, ITEM_POKE_BALL);
}

// Only take up to Iappa berry, as past that is just misc non-battle related berries
#define BERRY_COUNT (ITEM_IAPAPA_BERRY - FIRST_BERRY_INDEX + 1)

static void RandomiseBerryTrees(void)
{
    s32 i;

    for (i = 0; i < BERRY_TREES_COUNT; i++)
    {
        if(RandomChanceBerry())
        {
            u8 berryItem = FIRST_BERRY_INDEX + (Random() % BERRY_COUNT);
            u8 berry = ItemIdToBerryType(berryItem);
            PlantBerryTree(i, berry, BERRY_STAGE_BERRIES, FALSE);
        }
        else
        {
            RemoveBerryTree(i);
        }
    }
}