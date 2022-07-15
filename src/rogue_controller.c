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
        if(TRUE)//if(gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        {
            u8 targetLevel = gRogueRun.playerMonLevel;
            u8 currentLevel = GetMonData(mon, MON_DATA_LEVEL);

            if(currentLevel < targetLevel)
            {
                u8 delta = targetLevel - currentLevel;
                if(delta > 3)
                {
                    delta = 3;
                }

                *expGain *= delta * 2;
            }
            else
            {
                // No EXP once at target level
                *expGain = 0;
            }
        }
        else
        {
            // No EXP for wild battles
            *expGain = 0;
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
const u8 gText_RogueDebugText00[] = _("ROGUE DEBUG\nROOM ");

bool8 Rogue_ShouldShowMiniMenu(void)
{
    return TRUE;
}

u8* Rogue_GetMiniMenuContent(void)
{
    u8* strPointer = &gStringVar4[0];
    *strPointer = EOS;

    strPointer = StringAppend(strPointer, gText_RogueDebugText00);
    strPointer = StringAppend(strPointer, gText_RogueDebugText00);
    strPointer = StringAppend(strPointer, gText_RogueDebugText00);

    //ConvertIntToDecimalStringN(gStringVar1, gRogueRun.currentRoomIdx, STR_CONV_MODE_RIGHT_ALIGN, 2);
    //ConvertIntToDecimalStringN(gStringVar2, gPlayerPartyCount, STR_CONV_MODE_RIGHT_ALIGN, 2);
    //StringExpandPlaceholders(gStringVar4, gText_RogueDebugText);
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

    //SetMoney(&gSaveBlock1Ptr->money, 100);
    SetMoney(&gSaveBlock1Ptr->money, 60000);

    // TEMP - Should do this by script
    CreateMon(&starterMon, SPECIES_RATTATA, 10, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    GiveMonToPlayer(&starterMon);

//void CreateMon(struct Pokemon *mon, u16 species, u8 level, u8 fixedIV, u8 hasFixedPersonality, u32 fixedPersonality, u8 otIdType, u32 fixedOtId);
    //CreateMon(&starterMon, SPECIES_MEW, 1, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    //GiveMonToPlayer(&starterMon);
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
    
    gRogueRun.wildMonLevel = 5;
    gRogueRun.trainerMonLevel = 5;
    gRogueRun.playerMonLevel = 15;

    // (1 / N) chance of appearing
    //gRogueRun.trainerSpawnChance = 15;
    //gRogueRun.itemSpawnChance = 75;
    //gRogueRun.berrySpawnChance = 100;

    // Store current states
    gRogueSaveData.playerPartyCount = gPlayerPartyCount;
    memcpy(gRogueSaveData.playerParty, gPlayerParty, sizeof(gRogueSaveData.playerParty));

    gRogueSaveData.money = GetMoney(&gSaveBlock1Ptr->money);
    gRogueSaveData.registeredItem = gSaveBlock1Ptr->registeredItem;

    memcpy(gRogueSaveData.bagPocket_Items, gBagPockets[ITEMS_POCKET].itemSlots, sizeof(gRogueSaveData.bagPocket_Items));
    memcpy(gRogueSaveData.bagPocket_KeyItems, gBagPockets[KEYITEMS_POCKET].itemSlots, sizeof(gRogueSaveData.bagPocket_KeyItems));
    memcpy(gRogueSaveData.bagPocket_PokeBalls, gBagPockets[BALLS_POCKET].itemSlots, sizeof(gRogueSaveData.bagPocket_PokeBalls));
    memcpy(gRogueSaveData.bagPocket_TMHM, gBagPockets[TMHM_POCKET].itemSlots, sizeof(gRogueSaveData.bagPocket_TMHM));
    memcpy(gRogueSaveData.bagPocket_Berries, gBagPockets[BERRIES_POCKET].itemSlots, sizeof(gRogueSaveData.bagPocket_Berries));
}

static void EndRogueRun(void)
{
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    //gRogueRun.currentRoomIdx = 0;

    // Restore current states
    gPlayerPartyCount = gRogueSaveData.playerPartyCount;
    memcpy(gPlayerParty, gRogueSaveData.playerParty, sizeof(gRogueSaveData.playerParty));

    SetMoney(&gSaveBlock1Ptr->money, gRogueSaveData.money);
    gSaveBlock1Ptr->registeredItem = gRogueSaveData.registeredItem;

    memcpy(gBagPockets[ITEMS_POCKET].itemSlots, gRogueSaveData.bagPocket_Items, sizeof(gRogueSaveData.bagPocket_Items));
    memcpy(gBagPockets[KEYITEMS_POCKET].itemSlots, gRogueSaveData.bagPocket_KeyItems, sizeof(gRogueSaveData.bagPocket_KeyItems));
    memcpy(gBagPockets[BALLS_POCKET].itemSlots, gRogueSaveData.bagPocket_PokeBalls, sizeof(gRogueSaveData.bagPocket_PokeBalls));
    memcpy(gBagPockets[TMHM_POCKET].itemSlots, gRogueSaveData.bagPocket_TMHM, sizeof(gRogueSaveData.bagPocket_TMHM));
    memcpy(gBagPockets[BERRIES_POCKET].itemSlots, gRogueSaveData.bagPocket_Berries, sizeof(gRogueSaveData.bagPocket_Berries));
}

// TODO - Implement these route types

// Fishing
//      TYPE_WATER

// Field
//      TYPE_GRASS, TYPE_NORMAL

// Forest
//      TYPE_BUG, TYPE_GHOST, TYPE_POISON
        
// Cave
//      TYPE_ROCK, TYPE_ICE, TYPE_DRAGON

// Mountain
//      TYPE_GROUND, TYPE_FIRE, TYPE_FIGHTING

// Lake/Coast
//      TYPE_WATER, TYPE_FLYING

// City/Urban
//      TYPE_STEEL, TYPE_ELECTRIC, TYPE_PSYCHIC

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
        gRogueRun.currentRouteType = ROGUE_ROUTE_FIELD;

        // Wild/Trainer mons level at a flat rate
        ++gRogueRun.wildMonLevel;
        ++gRogueRun.playerMonLevel;

        // Trainer mons level faster, but never exceed player level
        gRogueRun.trainerMonLevel = min(gRogueRun.trainerMonLevel + 2, gRogueRun.playerMonLevel);

        RandomiseWildEncounters();
        ResetTrainerBattles();
        RandomiseEnabledTrainers();
        RandomiseEnabledItems();
        RandomiseBerryTrees();
    }
}


void Rogue_OnSetWarpData(struct WarpData *warp)
{
    // Configure random warp
    //if(warp->mapGroup == MAP_GROUP(ROGUE_HUB_TRANSITION) && warp->mapNum == MAP_NUM(ROGUE_HUB_TRANSITION) && warp->warpId == 1)

    if(Rogue_IsRunActive())
    {
        if((gRogueRun.currentRoomIdx % 2) == 0)
        {
            warp->mapGroup = MAP_GROUP(ROGUE_ROUTE_FIELD0);
            warp->mapNum = MAP_NUM(ROGUE_ROUTE_FIELD0);
        }
        else
        {
            warp->mapGroup = MAP_GROUP(ROGUE_ROUTE_FIELD1);
            warp->mapNum = MAP_NUM(ROGUE_ROUTE_FIELD1);
        }

        if(gRogueRun.currentRoomIdx != 0)
        {
            if((gRogueRun.currentRoomIdx % 3) == 0)
            {
                warp->mapGroup = MAP_GROUP(ROGUE_BOSS_0);
                warp->mapNum = MAP_NUM(ROGUE_BOSS_0);
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
    //if(gPlayerPartyCount >= 2)
    //{
    //    // Force double?
    //    gBattleTypeFlags |= BATTLE_TYPE_DOUBLE;
    //}

    //if (gNoOfApproachingTrainers == 2)
    //    gBattleTypeFlags = (BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS | BATTLE_TYPE_TRAINER);
    //else
    //    gBattleTypeFlags = (BATTLE_TYPE_TRAINER);

    //extern u16 gTrainerBattleOpponent_A;
    //extern u16 gTrainerBattleOpponent_B;

    // TODO - this doesn't really work as it messes up the trainer encounter flag..
    //if(gTrainerBattleOpponent_A != 0)
    //
    //   gTrainerBattleOpponent_A = TRAINER_DRAKE;
    //
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

void Rogue_CreateTrainerMon(u16 trainerNum, struct Pokemon *mon, u16 species, u8 level, u8 fixedIV, u8 hasFixedPersonality, u32 fixedPersonality, u8 otIdType, u32 fixedOtId)
{
    //gTrainers[trainerNum]

    u16 count = gRogueSpeciesTable[0].trainerSpeciesCount;
    u16 randIdx = Random() % count;

    species = gRogueSpeciesTable[0].trainerSpecies[randIdx];
    level = gRogueRun.trainerMonLevel;

    // TODO - Modify scale based on room

    CreateMon(mon, species, level, fixedIV, hasFixedPersonality, fixedPersonality, otIdType, fixedOtId);

    if(gPresetMonTable[species].presetCount != 0)
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
        
//extern u16 gRogueQueryBufferSize;
//extern bool8 gRogueQuerySpeciesState[];
//extern u16 gRogueQueryBuffer[];

        u8 levelVariation = min(6, gRogueRun.wildMonLevel - 1);
        const u16 count = ARRAY_COUNT(gRogueRun.wildEncounters);
        u16 randIdx = Random() % count;

        *species = gRogueRun.wildEncounters[randIdx];
        *level = gRogueRun.wildMonLevel - (Random() % levelVariation);
    }
}

//struct WarpEvent
//{
//    s16 x, y;
//    u8 elevation;
//    u8 warpId;
//    u8 mapNum;
//    u8 mapGroup;
//};

static void RandomiseWildEncounters(void)
{
//EWRAM_DATA struct RogueRunData gRogueRun = {};
//    u16 wildEncounters[5];
//    u16 fishingEncounters[2];


    // Query for the current route type
    RogueQuery_Clear();

    RogueQuery_SpeciesIsValid();
    RogueQuery_SpeciesIsNotLegendary();
    RogueQuery_SpeciesOfTypes(gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTable, gRogueRouteTable[gRogueRun.currentRouteType].wildTypeTableCount);
    RogueQuery_TransformToEggSpecies();

    // Evolve the species to just below the wild encounter level
    RogueQuery_EvolveSpeciesToLevel(1 + gRogueRun.currentRoomIdx);
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
    u8 chance = 10;
    return RandomChance(chance);
}

static bool8 RandomChanceItem()
{
    u8 chance = 75;
    return RandomChance(chance);
}

static bool8 RandomChanceBerry()
{
    u8 chance = 100;
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