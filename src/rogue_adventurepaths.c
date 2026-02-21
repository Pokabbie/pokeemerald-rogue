#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/metatile_labels.h"
#include "constants/trainer_types.h"
#include "constants/rogue.h"
#include "gba/isagbprint.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_screen_effect.h"
#include "malloc.h"
#include "overworld.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_controller.h"

#include "rogue_adventurepaths.h"
#include "rogue_campaign.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"
#include "rogue_query.h"
#include "rogue_quest.h"


#define ROOM_TO_WORLD_X 3
#define ROOM_TO_WORLD_Y 2

#define PATH_MAP_OFFSET_X (4)
#define PATH_MAP_OFFSET_Y (4)

#define ADJUST_COORDS_X(val) (gRogueAdvPath.pathLength - val - 1)   // invert so we place the first node at the end
#define ADJUST_COORDS_Y(val) (val - gRogueAdvPath.pathMinY + 1)     // start at coord 0


#define ROOM_TO_METATILE_X(val) ((ADJUST_COORDS_X(val) * ROOM_TO_WORLD_X) + MAP_OFFSET + PATH_MAP_OFFSET_X)
#define ROOM_TO_METATILE_Y(val) ((ADJUST_COORDS_Y(val) * ROOM_TO_WORLD_Y) + MAP_OFFSET + PATH_MAP_OFFSET_Y)

#define ROOM_TO_OBJECT_EVENT_X(val) ((ADJUST_COORDS_X(val) * ROOM_TO_WORLD_X) + PATH_MAP_OFFSET_X + 2)
#define ROOM_TO_OBJECT_EVENT_Y(val) ((ADJUST_COORDS_Y(val) * ROOM_TO_WORLD_Y) + PATH_MAP_OFFSET_Y)

#define ROOM_TO_WARP_X(val) (ROOM_TO_OBJECT_EVENT_X(val) + 1)
#define ROOM_TO_WARP_Y(val) (ROOM_TO_OBJECT_EVENT_Y(val))

#define ROOM_CONNECTION_TOP     0
#define ROOM_CONNECTION_MID     1
#define ROOM_CONNECTION_BOT     2
#define ROOM_CONNECTION_COUNT   3

#define ROOM_CONNECTION_MASK_TOP     (1 << ROOM_CONNECTION_TOP)
#define ROOM_CONNECTION_MASK_MID     (1 << ROOM_CONNECTION_MID)
#define ROOM_CONNECTION_MASK_BOT     (1 << ROOM_CONNECTION_BOT)

#define MAX_CONNECTION_GENERATOR_COLUMNS 5

#define gSpecialVar_ScriptNodeID        gSpecialVar_0x8004
#define gSpecialVar_ScriptNodeParam0    gSpecialVar_0x8005
#define gSpecialVar_ScriptNodeParam1    gSpecialVar_0x8006

struct AdvPathConnectionSettings
{
    u8 minCount;
    u8 maxCount;
    u8 branchingChance[ROOM_CONNECTION_COUNT];
};

struct AdvPathGenerator
{
    struct AdvPathConnectionSettings connectionsSettingsPerColumn[MAX_CONNECTION_GENERATOR_COLUMNS];
};

struct AdvPathRoomSettings
{
    struct Coords8 currentCoords;
    struct RogueAdvPathRoomParams roomParams;
    u8 roomType;
};

struct AdvPathSettings
{
    const struct AdvPathGenerator* generator;
    struct AdvPathRoomSettings roomScratch[ROGUE_ADVPATH_ROOM_CAPACITY];
    u8 numOfRooms[ADVPATH_ROOM_COUNT];
    struct Coords8 currentCoords;
    u8 totalLength;
    u8 nodeCount;
};


struct MetatileOffset
{
    s16 x;
    s16 y;
    u32 metatile;
};

struct MetatileConnection
{
    u16 centre;
    u16 left;
    u16 right;
    u16 up;
    u16 down;
};

static const struct MetatileOffset sTreeDecorationMetatiles[] = 
{
    { 0, 0, METATILE_GeneralHub_Tree_BottomLeft_Sparse },
    { 1, 0, METATILE_GeneralHub_Tree_BottomRight_Sparse },
    { 0, -1, METATILE_GeneralHub_Tree_TopLeft_Sparse },
    { 1, -1, METATILE_GeneralHub_Tree_TopRight_Sparse },
    { 0, -2, METATILE_GeneralHub_Tree_TopLeft_CapGrass },
    { 1, -2, METATILE_GeneralHub_Tree_TopRight_CapGrass },
};

static bool8 IsObjectEventVisible(struct RogueAdvPathRoom* room);
static bool8 ShouldBlockObjectEvent(struct RogueAdvPathRoom* room);
static void BufferTypeAdjective(u8 type);

static void GeneratePath(struct AdvPathSettings* pathSettings);
static void GenerateFloorLayout(struct Coords8 currentCoords, struct AdvPathSettings* pathSettings);
static void GenerateRoomPlacements(struct AdvPathSettings* pathSettings);
static void GenerateRoomInstance(u8 roomId, u8 roomType);
static u8 CountRoomConnections(u8 mask);

static u8 GenerateRoomConnectionMask(struct Coords8 coords, struct AdvPathSettings* pathSettings);
static bool8 DoesRoomExists(s8 x, s8 y);

static u16 SelectObjectGfxForRoom(struct RogueAdvPathRoom* room);
static u8 SelectObjectMovementTypeForRoom(struct RogueAdvPathRoom* room);

static u8 GetPathGenerationDifficulty()
{
    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
    {
        if(Rogue_GetCurrentDifficulty() == 0)
        {
            // Generate full path under late game difficulty balance
            return ROGUE_ELITE_START_DIFFICULTY - 1;
        }
    }

    // Skip ahead for the fake out
    if(Rogue_AssumeFinalQuestFakeChamp())
        return Rogue_GetCurrentDifficulty() + 1;
    else
        return Rogue_GetCurrentDifficulty();
}

static void GeneratePath(struct AdvPathSettings* pathSettings)
{
    struct AdvPathRoomSettings* bossRoom = &pathSettings->roomScratch[0];
    memset(bossRoom, 0, sizeof(*bossRoom));

    AGB_ASSERT(pathSettings->generator != NULL);

    bossRoom->roomType = ADVPATH_ROOM_BOSS;

    // Generate base layout
    {
        struct Coords8 coords;
        coords.x = 0;
        coords.y = 0;

        gRogueAdvPath.roomCount = 0;
        gRogueAdvPath.pathLength = pathSettings->totalLength;

        GenerateFloorLayout(coords, pathSettings);
        GenerateRoomPlacements(pathSettings);
    }

    // Store min/max Y coords
    {
        u8 i;

        for(i = 0; i < gRogueAdvPath.roomCount; ++i)
        {
            if(i == 0)
            {
                gRogueAdvPath.pathMinY = gRogueAdvPath.rooms[i].coords.y;
                gRogueAdvPath.pathMaxY = gRogueAdvPath.rooms[i].coords.y;
            }
            else
            {
                gRogueAdvPath.pathMinY = min(gRogueAdvPath.pathMinY, gRogueAdvPath.rooms[i].coords.y);
                gRogueAdvPath.pathMaxY = max(gRogueAdvPath.pathMaxY, gRogueAdvPath.rooms[i].coords.y);
            }
        }
    }
}

static void GenerateFloorLayout(struct Coords8 currentCoords, struct AdvPathSettings* pathSettings)
{
    if(pathSettings->nodeCount >= ROGUE_ADVPATH_ROOM_CAPACITY)
    {
        // Cannot generate any more
        DebugPrint("ADVPATH: \tReached room/node capacity.");
        return;
    }
    else
    {
        u8 nodeId = gRogueAdvPath.roomCount++;

        // Write base settings for this room (These will likely be overriden later)
        gRogueAdvPath.rooms[nodeId].coords = currentCoords;
        gRogueAdvPath.rooms[nodeId].roomType = ADVPATH_ROOM_NONE;
        gRogueAdvPath.rooms[nodeId].connectionMask = 0;
        gRogueAdvPath.rooms[nodeId].rngSeed = RogueRandom();

        
        // Generate children
        //
        if(currentCoords.x + 1 < pathSettings->totalLength)
        {
            struct Coords8 newCoords;
            u8 connectionMask;

            newCoords.x = currentCoords.x + 1;
            newCoords.y = currentCoords.y;

            connectionMask = GenerateRoomConnectionMask(currentCoords, pathSettings);
            gRogueAdvPath.rooms[nodeId].connectionMask = connectionMask;

            newCoords.y = currentCoords.y + 1;
            if((connectionMask & ROOM_CONNECTION_MASK_TOP) != 0 && !DoesRoomExists(newCoords.x, newCoords.y))
            {
                GenerateFloorLayout(newCoords, pathSettings);
            }
            
            newCoords.y = currentCoords.y + 0;
            if((connectionMask & ROOM_CONNECTION_MASK_MID) != 0 && !DoesRoomExists(newCoords.x, newCoords.y))
            {
                GenerateFloorLayout(newCoords, pathSettings);
            }

            newCoords.y = currentCoords.y - 1;
            if((connectionMask & ROOM_CONNECTION_MASK_BOT) != 0 && !DoesRoomExists(newCoords.x, newCoords.y))
            {
                GenerateFloorLayout(newCoords, pathSettings);
            }
        }
    }
}

static bool8 IsPrecededByRoomType(struct RogueAdvPathRoom* room, u8 roomType)
{
    u8 i;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(gRogueAdvPath.rooms[i].coords.x == room->coords.x + 1)
        {
            // ROOM_CONNECTION_MASK_TOP
            if((room->connectionMask & ROOM_CONNECTION_MASK_TOP) != 0 && gRogueAdvPath.rooms[i].coords.y == room->coords.y + 1)
            {
                if(gRogueAdvPath.rooms[i].roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_MID
            else if((room->connectionMask & ROOM_CONNECTION_MASK_MID) != 0 && gRogueAdvPath.rooms[i].coords.y == room->coords.y + 0)
            {
                if(gRogueAdvPath.rooms[i].roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_BOT
            else if((room->connectionMask & ROOM_CONNECTION_MASK_BOT) != 0 && gRogueAdvPath.rooms[i].coords.y == room->coords.y - 1)
            {
                if(gRogueAdvPath.rooms[i].roomType == roomType)
                    return TRUE;
            }
        }
    }

    return FALSE;
}

static bool8 IsProceededByRoomType(struct RogueAdvPathRoom* room, u8 roomType)
{
    u8 i;

    if(room->coords.x == 0)
        return FALSE;
    else if(room->coords.x == 1)
        return roomType == ADVPATH_ROOM_BOSS;

    // Check the inverse mask to see if we are connected
    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(gRogueAdvPath.rooms[i].coords.x == room->coords.x - 1)
        {
            // ROOM_CONNECTION_MASK_TOP
            if((gRogueAdvPath.rooms[i].connectionMask & ROOM_CONNECTION_MASK_BOT) != 0 && gRogueAdvPath.rooms[i].coords.y == room->coords.y + 1)
            {
                if(gRogueAdvPath.rooms[i].roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_MID
            else if((gRogueAdvPath.rooms[i].connectionMask & ROOM_CONNECTION_MASK_MID) != 0 && gRogueAdvPath.rooms[i].coords.y == room->coords.y + 0)
            {
                if(gRogueAdvPath.rooms[i].roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_BOT
            else if((gRogueAdvPath.rooms[i].connectionMask & ROOM_CONNECTION_MASK_TOP) != 0 && gRogueAdvPath.rooms[i].coords.y == room->coords.y - 1)
            {
                if(gRogueAdvPath.rooms[i].roomType == roomType)
                    return TRUE;
            }
        }
    }

    return FALSE;
}

static u8 CountRoomType(u16 roomType)
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(gRogueAdvPath.rooms[i].roomType == roomType)
            ++count;
    }

    return count;
}

static u8 CountSubRoomType(u16 roomType, u16 roomIndex)
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(gRogueAdvPath.rooms[i].roomType == roomType && gRogueAdvPath.rooms[i].roomParams.roomIdx == roomIndex)
            ++count;
    }

    return count;
}

static u8 SelectRoomType_CalculateWeight(u16 weightIndex, u16 roomType, void* data)
{
    u8 count;

    switch (roomType)
    {
    case ADVPATH_ROOM_RESTSTOP:
        count = CountRoomType(roomType);

        // Always want at least 1 rest stop
        if(count == 0)
            return 100;
        // Prefer a 2nd rest stop
        else if(count == 1)
            return 20;
        // If we already have 4 perfer most other encounters
        else if(count >= 4)
            return 1;
        break;

    // Only allow 1 but we really want to place it
    case ADVPATH_ROOM_LEGENDARY:
        count = CountRoomType(roomType);
        if(count == 0)
            return 200;
        else
            return 0;
        break;

    // Only allow 1 but we really want to place it (less so than other encounters)
    case ADVPATH_ROOM_TEAM_HIDEOUT:
        count = CountRoomType(roomType);
        if(count == 0)
            return 50;
        else
            return 0;
        break;

    // Only allow 1 but we prefer it over others
    case ADVPATH_ROOM_HONEY_TREE:
        count = CountRoomType(roomType);
        if(count == 0)
        {
            // Every other badge we want to increase weight otherwise decrease weight but not impossible
            if((GetPathGenerationDifficulty() - 1) % 2 == 0)
                return 15;
            else
                return 1;
        }
        else
            return 0;
        break;

    // Only allow 1 and cycle weighting every third difficulty
    case ADVPATH_ROOM_DARK_DEAL:
        count = CountRoomType(roomType);
        if(count != 0)
            return 0;
        else if((GetPathGenerationDifficulty() % 2) != 0)
            return 15;
        else
            return 1;
        break;

    // Only allow 1 and cycle weighting every third difficulty (offset from dark deal rates)
    case ADVPATH_ROOM_LAB:
        count = CountRoomType(roomType);
        if(count != 0)
            return 0;
        else if(((GetPathGenerationDifficulty() + 1) % 2) != 0)
            return 20;
        else
            return 1;
        break;


    // Only allow 1 of this type at once
    case ADVPATH_ROOM_GAMESHOW:
    case ADVPATH_ROOM_CATCHING_CONTEST:
    case ADVPATH_ROOM_SIGN:
    case ADVPATH_ROOM_BATTLE_SIM:
        count = CountRoomType(roomType);
        if(count != 0)
            return 0;
        break;

    // We really want this to spawn when we allow it to
    case ADVPATH_ROOM_SHRINE:
        count = CountRoomType(roomType);
        if(count != 0)
            return 0;
        return 100;

    default:
        AGB_ASSERT(FALSE);
        break;
    }

    return 5;
}

static u16 SelectRoomType(u16* activeTypeBuffer, u16 activeTypeCount)
{
    u16 i;
    u16 result;

    RogueCustomQuery_Begin();

    for(i = 0; i < activeTypeCount; ++i)
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, activeTypeBuffer[i]);

    RogueWeightQuery_Begin();
    {
        RogueWeightQuery_CalculateWeights(SelectRoomType_CalculateWeight, NULL);
        result = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());
    }
    RogueWeightQuery_End();

    RogueCustomQuery_End();

    return result;
}

static u8 ReplaceRoomEncounters_CalculateWeight(u16 weightIndex, u16 roomId, void* data)
{
    s16 weight = 10;
    u8 roomType = *((u8*)data);
    struct RogueAdvPathRoom* existingRoom = &gRogueAdvPath.rooms[roomId];

    switch (roomType)
    {
    case ADVPATH_ROOM_RESTSTOP:
        // Like being placed in the final column but can occasionally end up in other one
        if(existingRoom->coords.x <= 2)
            weight += 90;

        // Don't want to place in first column
        if(existingRoom->coords.x + 1 == gRogueAdvPath.pathLength)
            weight -= 40;

        // Don't place after or before or other rest stop
        if(IsPrecededByRoomType(existingRoom, ADVPATH_ROOM_RESTSTOP) || IsProceededByRoomType(existingRoom, ADVPATH_ROOM_RESTSTOP))
            weight = 0;
   
        if(IsPrecededByRoomType(existingRoom, ADVPATH_ROOM_LEGENDARY) || IsProceededByRoomType(existingRoom, ADVPATH_ROOM_LEGENDARY))
            weight = 0;
        break;

    case ADVPATH_ROOM_LEGENDARY:
        // Like being placed in the final column but can occasionally end up in other one
        if(existingRoom->coords.x <= 2)
            weight += 80;

        // Don't want to place in first column
        if(existingRoom->coords.x + 1 == gRogueAdvPath.pathLength)
            weight -= 40;

        // Prefer route where we are locked into this path
        if(CountRoomConnections(existingRoom->connectionMask) == 1)
            weight += 40;

        // We like having the legend be behind the team hideout
        if(IsPrecededByRoomType(existingRoom, ADVPATH_ROOM_TEAM_HIDEOUT))
            weight += 200;
        break;

    case ADVPATH_ROOM_TEAM_HIDEOUT:
        // Don't want to place in final column
        if(existingRoom->coords.x <= 2)
            weight -= 40;

        // Prefer route where we are locked into this path
        if(CountRoomConnections(existingRoom->connectionMask) == 1)
            weight += 10;

        // We like having the legend be behind the team hideout
        if(IsProceededByRoomType(existingRoom, ADVPATH_ROOM_LEGENDARY))
            weight += 200;
        break;

    case ADVPATH_ROOM_SHRINE:
    case ADVPATH_ROOM_LAB:
        // Like being placed in the final column but can occasionally end up in other one
        if(existingRoom->coords.x <= 2)
            weight += 80;
        break;

    case ADVPATH_ROOM_CATCHING_CONTEST:
    case ADVPATH_ROOM_GAMESHOW:
    case ADVPATH_ROOM_BATTLE_SIM:
        // Don't want to place in first column
        if(existingRoom->coords.x + 1 == gRogueAdvPath.pathLength)
            weight -= 40;
        // Like being placed in the middle columns but can occasionally end up in other one
        else if(existingRoom->coords.x > 2)
            weight += 80;
        break;

    case ADVPATH_ROOM_SIGN:
        // Prefer being placed in first column
        if(existingRoom->coords.x + 1 == gRogueAdvPath.pathLength)
            weight += 80;

        // Like being placed in the middle columns but can occasionally end up in other one
        if(existingRoom->coords.x > 2)
            weight += 40;
        break;
    }

    // If we've got this encounter immediately before or after prefer not this one
    if(IsPrecededByRoomType(existingRoom, roomType))
        weight -= 5;
    if(IsProceededByRoomType(existingRoom, roomType))
        weight -= 5;


    return (u8)(min(255, max(0, weight)));
}

static void ReplaceRoomEncounter(u8 fromRoomType, u8 toRoomType)
{
    u16 replaceIndex = (u16)-1;
    u8 replaceRoomType = 0;

    RoguePathsQuery_Begin();
    RoguePathsQuery_Reset(QUERY_FUNC_INCLUDE);
    RoguePathsQuery_IsOfType(QUERY_FUNC_INCLUDE, fromRoomType);

    RogueWeightQuery_Begin();
    {
        u16 index;

        RogueWeightQuery_CalculateWeights(ReplaceRoomEncounters_CalculateWeight, &toRoomType);

        if(RogueWeightQuery_HasAnyWeights())
        {
            index = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, index);

            replaceIndex = index;
            replaceRoomType = toRoomType;
        }
    }
    RogueWeightQuery_End();

    RoguePathsQuery_End();

    // Now replace after the query has been freed, as we may use Query API internally
    if(replaceIndex != (u16)-1)
    {
        GenerateRoomInstance(replaceIndex, replaceRoomType);
    }
}

static void GenerateRoomPlacements(struct AdvPathSettings* pathSettings)
{
    u8 i;
    u8 freeRoomCount = 0;
    u8 validEncounterCount = 0;
    u16 validEncounterList[ADVPATH_ROOM_COUNT];
    u16 minReplaceCount = 1;

    // Place gym at very end
    GenerateRoomInstance(0, ADVPATH_ROOM_BOSS);

    // Place routes on all tiles for now, so other encounters can choose to replace them
    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        // Don't place them immediately before the gym
        if(gRogueAdvPath.rooms[i].coords.x > 1)
        {
            GenerateRoomInstance(i, ADVPATH_ROOM_ROUTE);
            ++freeRoomCount;
        }
    }

    // Now we're going to replace the routes based on the ideal placement
    // The order of these is important to decide the placement

    // For gauntlet, place full rest stop at end always
    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
    {
        for(i = 0; i < gRogueAdvPath.roomCount; ++i)
        {
            if(gRogueAdvPath.rooms[i].roomType == ADVPATH_ROOM_ROUTE && gRogueAdvPath.rooms[i].coords.x <= 2)
            {
                GenerateRoomInstance(i, ADVPATH_ROOM_RESTSTOP);
                --freeRoomCount;
            }
        }
    }

    // Randomly replace a routes with empty tiles
    {
        u8 chance;
        u8 chanceFalloff;
        
        if(GetPathGenerationDifficulty() >=  ROGUE_CHAMP_START_DIFFICULTY)
        {
            chance = 30;
            chanceFalloff = 4;
        }
        else if(GetPathGenerationDifficulty() >=  ROGUE_ELITE_START_DIFFICULTY)
        {
            chance = 20;
            chanceFalloff = 5;
        }
        else
        {
            chance = 5;
            chanceFalloff = 5;
        }

        if(GetPathGenerationDifficulty() == 0)
            chance = 0;

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
            chance = 0;

        if(chance != 0)
        {
            for(i = 0; i < gRogueAdvPath.roomCount; ++i)
            {
                if(gRogueAdvPath.rooms[i].roomType == ADVPATH_ROOM_ROUTE && RogueRandomChance(chance, 0))
                {
                    GenerateRoomInstance(i, ADVPATH_ROOM_NONE);
                    --freeRoomCount;

                    if(chance <= chanceFalloff)
                        chance = 1;
                    else
                        chance -= chanceFalloff;
                }
            }
        }
    }

    // Populate special encounters into a single list
    //
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET) // In gauntlet we place these manually
    {
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_RESTSTOP;
        ++minReplaceCount;
    }

    // Legends
    for(i = 0; i < ADVPATH_LEGEND_COUNT; ++i)
    {
        if(gRogueRun.legendarySpecies[i] != SPECIES_NONE && gRogueRun.legendaryDifficulties[i] == GetPathGenerationDifficulty())
        {
            validEncounterList[validEncounterCount++] = ADVPATH_ROOM_LEGENDARY;
            ++minReplaceCount;
            break;
        }
    }

    // Team Encounters
    for(i = 0; i < ADVPATH_TEAM_ENCOUNTER_COUNT; ++i)
    {
        if(gRogueRun.teamEncounterDifficulties[i] == GetPathGenerationDifficulty())
        {
            validEncounterList[validEncounterCount++] = ADVPATH_ROOM_TEAM_HIDEOUT;
            ++minReplaceCount;
            break;
        }
    }

    // Honey tree
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET && GetPathGenerationDifficulty() >= 1 && RogueRandomChance(60, 0))
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_HONEY_TREE;

    // Catching contest
    if(RogueRandomChance(33, 0))
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_CATCHING_CONTEST;

    // Mysterious Sign
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET && GetPathGenerationDifficulty() < ROGUE_ELITE_START_DIFFICULTY && RogueRandomChance(40, 0))
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_SIGN;

    // Shrine (Gauntlet will always offer this encounter)
    if((Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET) || GetPathGenerationDifficulty() == gRogueRun.shrineSpawnDifficulty)
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_SHRINE;

    // Battle sim
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET && GetPathGenerationDifficulty() >= 1 && RogueRandomChance(33, 0))
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_BATTLE_SIM;

    {
        bool8 allowDarkDeal = (GetPathGenerationDifficulty() % 3 != 0);
        bool8 allowLab = (GetPathGenerationDifficulty() % 3 != 1);
        bool8 allowGameShow = RogueRandomChance(50, 0);

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
        {
            allowDarkDeal = TRUE;
            allowLab = FALSE;
            allowGameShow = FALSE;
        }

        allowDarkDeal = (allowDarkDeal && RogueRandomChance(25, 0));
        allowLab = (allowLab && RogueRandomChance(25, 0));


        if(allowLab)
        {
            // Lab
            if(GetPathGenerationDifficulty() >= ROGUE_GYM_MID_DIFFICULTY - 1)
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_LAB;
        }

        // Dark deal / Game show
        if(GetPathGenerationDifficulty() >= ROGUE_GYM_MID_DIFFICULTY + 2)
        {
            // Only dark deals
            if(allowDarkDeal)
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_DARK_DEAL;
        }
        else if(GetPathGenerationDifficulty() >= ROGUE_GYM_MID_DIFFICULTY - 1)
        {
            // Mix of both
            if(allowDarkDeal)
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_DARK_DEAL;

            if(allowGameShow)
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_GAMESHOW;
        }
        else
        {
            // Only game show
            if(allowGameShow)
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_GAMESHOW;
        }
    }

    // Replace % of route with special encounters
    {
        u16 replacePerc = 0;
        u16 replaceCount = freeRoomCount;

        switch (RogueRandom() % 3)
        {
        case 0:
            replacePerc = 25;
            break;
        case 1:
            replacePerc = 33;
            break;
        case 2:
            replacePerc = 50;
            break;
        }

        replaceCount = (replaceCount * replacePerc) / 100;
        replaceCount = max(replaceCount, minReplaceCount);
        replaceCount = max(replaceCount, freeRoomCount);

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
        {
            replaceCount = min(replaceCount, validEncounterCount);
        }

        for(i = 0; i < (u8)replaceCount; ++i)
        {
            u16 encounterType = SelectRoomType(validEncounterList, validEncounterCount);
            ReplaceRoomEncounter(ADVPATH_ROOM_ROUTE, encounterType);
            --freeRoomCount;
        }
    }

    // Wild dens
    {
        u8 chance;
        u8 chanceFalloff;
        u8 minRouteCount;

        // Recoute number of regular routes remaining
        freeRoomCount = 0;
        for(i = 0; i < gRogueAdvPath.roomCount; ++i)
        {
            if(gRogueAdvPath.rooms[i].roomType == ADVPATH_ROOM_ROUTE)
                ++freeRoomCount;
        }

        // If players get encounters they basically have to get lucky with wild den
        if(GetPathGenerationDifficulty() >=  ROGUE_CHAMP_START_DIFFICULTY)
        {
            chance = 90;
            chanceFalloff = 15;
            minRouteCount = 1;
        }
        else if(GetPathGenerationDifficulty() >=  ROGUE_ELITE_START_DIFFICULTY)
        {
            chance = 60;
            chanceFalloff = 10;
            minRouteCount = 2;
        }
        else if(GetPathGenerationDifficulty() >=  1)
        {
            chance = 40;
            chanceFalloff = 20;
            minRouteCount = 3;
        }
        else
        {
            chance = 5;
            chanceFalloff = 0;
            minRouteCount = 3;
        }

        // Always make sure there is at least 1 regular route which can be chosen
        for(i = 0; i < gRogueAdvPath.roomCount && freeRoomCount > minRouteCount; ++i)
        {
            if(gRogueAdvPath.rooms[i].roomType == ADVPATH_ROOM_ROUTE && RogueRandomChance(chance, 0))
            {
                GenerateRoomInstance(i, ADVPATH_ROOM_WILD_DEN);
                --freeRoomCount;

                if(chance <= chanceFalloff)
                    chance = 1;
                else
                    chance -= chanceFalloff;
            }
        }
    }
}

static u8 FindRoomOfType(u16 type)
{
    u16 i;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if( gRogueAdvPath.rooms[i].roomType == type)
            return i;
    }

    AGB_ASSERT(FALSE);
    return 0;
}

static void GenerateRoomInstance(u8 roomId, u8 roomType)
{
    u16 weights[ADVPATH_SUBROOM_WEIGHT_COUNT];
    memset(weights, 0, sizeof(weights));

    // Count other
    //++pathSettings->numOfRooms[roomSettings->roomType];
    //DebugPrintf("ADVPATH: \tAdded room type %d (Total: %d)", roomType, pathSettings->numOfRooms[roomSettings->roomType]);

    // Erase any previously set params
    gRogueAdvPath.rooms[roomId].roomType = ADVPATH_ROOM_NONE; // set room type below so counting methods don't break
    memset(&gRogueAdvPath.rooms[roomId].roomParams, 0, sizeof(gRogueAdvPath.rooms[roomId].roomParams));

    switch(roomType)
    {
        case ADVPATH_ROOM_BOSS:
            // Specifically use the correct difficulty here regardless of if we are faking or not
            AGB_ASSERT(Rogue_GetCurrentDifficulty() < ARRAY_COUNT(gRogueRun.bossTrainerNums));
            gRogueAdvPath.rooms[roomId].roomParams.perType.boss.trainerNum = gRogueRun.bossTrainerNums[Rogue_GetCurrentDifficulty()];
            break;

        case ADVPATH_ROOM_RESTSTOP:
            weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 15;
            weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = 15;
            weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] = 15;
            weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = 0;

            if(GetPathGenerationDifficulty() >= ROGUE_GYM_START_DIFFICULTY + 2)
            {
                // Only activate after 2nd badge
                weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = 1;
            }
            else if(GetPathGenerationDifficulty() >= ROGUE_ELITE_START_DIFFICULTY)
            {
                // Ever so slightly more common during E4 phase
                weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = 20;
            }

            // Prefer showing each rest stop type before having duplicates
            if(CountSubRoomType(ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_BATTLE) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 0;

            if(CountSubRoomType(ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_SHOP) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = 0;

            if(CountSubRoomType(ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_DAYCARE) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] = 0;

            if(weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] == 0 && weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] == 0 && weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] == 0)
            {
                // Make sure we place all other types first before placing duplicates
                weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 3;
                weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = 3;
                weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] = 3;
            }

            // If we have a full rest stop, make it only appear once
            if(CountSubRoomType(ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_FULL) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = 0;

            // For champ we will always spawn full rest stops, for balance
            if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET || GetPathGenerationDifficulty() >= ROGUE_CHAMP_START_DIFFICULTY)
            {
                gRogueAdvPath.rooms[roomId].roomParams.roomIdx = ADVPATH_SUBROOM_RESTSTOP_FULL;
            }
            else
            {
                gRogueAdvPath.rooms[roomId].roomParams.roomIdx = SelectIndexFromWeights(weights, ARRAY_COUNT(weights), RogueRandom());
            }
            break;

        case ADVPATH_ROOM_LEGENDARY:
            {
                u8 legendId = Rogue_GetCurrentLegendaryEncounterId();
                u16 species = gRogueRun.legendarySpecies[legendId];
                gRogueAdvPath.rooms[roomId].roomParams.roomIdx = Rogue_GetLegendaryRoomForSpecies(species);
                gRogueAdvPath.rooms[roomId].roomParams.perType.legendary.shinyState = Rogue_RollShinyState(SHINY_ROLL_STATIC);
            }
            break;

        case ADVPATH_ROOM_TEAM_HIDEOUT:
            {
                u8 encounterId = Rogue_GetCurrentTeamHideoutEncounterId();
                gRogueAdvPath.rooms[roomId].roomParams.roomIdx = gRogueRun.teamEncounterRooms[encounterId];
            }
            break;

        case ADVPATH_ROOM_MINIBOSS:
            AGB_ASSERT(FALSE);
            gRogueAdvPath.rooms[roomId].roomParams.roomIdx = 0;
            gRogueAdvPath.rooms[roomId].roomParams.perType.miniboss.trainerNum = 0;
            break;

        case ADVPATH_ROOM_WILD_DEN:
            gRogueAdvPath.rooms[roomId].roomParams.roomIdx = 0;
            gRogueAdvPath.rooms[roomId].roomParams.perType.wildDen.species = Rogue_SelectWildDenEncounterRoom();
            gRogueAdvPath.rooms[roomId].roomParams.perType.wildDen.shinyState = Rogue_RollShinyState(SHINY_ROLL_STATIC);
            break;

        case ADVPATH_ROOM_HONEY_TREE:
            gRogueAdvPath.rooms[roomId].roomParams.roomIdx = 0;
            gRogueAdvPath.rooms[roomId].roomParams.perType.honeyTree.species = Rogue_SelectHoneyTreeEncounterRoom();
            gRogueAdvPath.rooms[roomId].roomParams.perType.honeyTree.shinyState = Rogue_RollShinyState(SHINY_ROLL_STATIC);
            break;

        case ADVPATH_ROOM_ROUTE:
        {
            gRogueAdvPath.rooms[roomId].roomParams.roomIdx = Rogue_SelectRouteRoom(GetPathGenerationDifficulty());
            DebugPrintf("Route [%d] = %d", roomId, gRogueAdvPath.rooms[roomId].roomParams.roomIdx);

            if(GetPathGenerationDifficulty() > ROGUE_ELITE_START_DIFFICULTY)
            {
                weights[ADVPATH_SUBROOM_ROUTE_CALM] = 0;
                weights[ADVPATH_SUBROOM_ROUTE_AVERAGE] = 1;
                weights[ADVPATH_SUBROOM_ROUTE_TOUGH] = 8;
            }
            else
            {
                weights[ADVPATH_SUBROOM_ROUTE_CALM] = 3;
                weights[ADVPATH_SUBROOM_ROUTE_AVERAGE] = 4;
                weights[ADVPATH_SUBROOM_ROUTE_TOUGH] = 1;
            }

            gRogueAdvPath.rooms[roomId].roomParams.perType.route.difficulty = SelectIndexFromWeights(weights, ARRAY_COUNT(weights), RogueRandom());
            break;
        }

        case ADVPATH_ROOM_SIGN:
            // Use same RNG seed as boss so we can generate their team
            gRogueAdvPath.rooms[roomId].rngSeed = gRogueAdvPath.rooms[FindRoomOfType(ADVPATH_ROOM_BOSS)].rngSeed;
            break;
    }

    // Set room type at the end to avoid breaking code that considers sub rooms
    gRogueAdvPath.rooms[roomId].roomType = roomType;
}

static u8 CountRoomConnections(u8 mask)
{
    u8 count = 0;

    if(mask == 0)
        return 0;

    if((mask & ROOM_CONNECTION_MASK_TOP) != 0)
        ++count;

    if((mask & ROOM_CONNECTION_MASK_MID) != 0)
        ++count;

    if((mask & ROOM_CONNECTION_MASK_BOT) != 0)
        ++count;

    return count;
}

static u8 GenerateRoomConnectionMask(struct Coords8 coords, struct AdvPathSettings* pathSettings)
{
    u8 mask;
    u8 connCount;
    u8 minConnCount = pathSettings->generator->connectionsSettingsPerColumn[min(coords.x, MAX_CONNECTION_GENERATOR_COLUMNS - 1)].minCount;
    u8 maxConnCount = pathSettings->generator->connectionsSettingsPerColumn[min(coords.x, MAX_CONNECTION_GENERATOR_COLUMNS - 1)].maxCount;
    u8 const* branchingChances = pathSettings->generator->connectionsSettingsPerColumn[min(coords.x, MAX_CONNECTION_GENERATOR_COLUMNS - 1)].branchingChance;

    do
    {
        mask = 0;

        if(RogueRandomChance(branchingChances[ROOM_CONNECTION_TOP], OVERWORLD_FLAG))
            mask |= ROOM_CONNECTION_MASK_TOP;

        if(RogueRandomChance(branchingChances[ROOM_CONNECTION_MID], OVERWORLD_FLAG))
            mask |= ROOM_CONNECTION_MASK_MID;

        if(RogueRandomChance(branchingChances[ROOM_CONNECTION_BOT], OVERWORLD_FLAG))
            mask |= ROOM_CONNECTION_MASK_BOT;

        connCount = CountRoomConnections(mask);
    }
    // keep going until we have the required number of connections
    while(!(connCount >= minConnCount && connCount <= maxConnCount));

    AGB_ASSERT(mask != 0);

    return mask;
}

static bool8 DoesRoomExists(s8 x, s8 y)
{
    u8 i;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(gRogueAdvPath.rooms[i].coords.x == x && gRogueAdvPath.rooms[i].coords.y == y)
            return TRUE;
    }

    return FALSE;
}

bool8 RogueAdv_GenerateAdventurePathsIfRequired()
{
    if(gRogueRun.adventureRoomId != ADVPATH_INVALID_ROOM_ID && (gRogueAdvPath.roomCount != 0 && gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomType != ADVPATH_ROOM_BOSS))
    {
        // Path is still valid
        gRogueAdvPath.justGenerated = FALSE;
        return FALSE;
    }
    else
    {
        struct AdvPathSettings pathSettings = {0};
        struct AdvPathGenerator generator = {0};

        // If we have a valid room ID, then we're reloading a previous save
        bool8 isNewGeneration = gRogueRun.adventureRoomId == ADVPATH_INVALID_ROOM_ID;

        pathSettings.generator = &generator;
        pathSettings.totalLength = 3 + 2; // +2 to account for final encounter and initial split

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
        {
            if(Rogue_GetCurrentDifficulty() == 0)
                pathSettings.totalLength = 5 + 2;
            else
                pathSettings.totalLength = 2;
        }

        // Select the correct seed
        {
            u8 i;
            u16 seed;
            SeedRogueRng(gRogueRun.baseSeed * 235 + 31897);

            seed = RogueRandom();
            for(i = 0; i < GetPathGenerationDifficulty(); ++i)
            {
                seed = RogueRandom();
            }

            // This is the seed for this path
            SeedRogueRng(seed);
        }

        // Select some branching presets for the layout generation
        {
            u8 i;

            // Gym split
            generator.connectionsSettingsPerColumn[0].minCount = 2;
            generator.connectionsSettingsPerColumn[0].maxCount = 3;
            generator.connectionsSettingsPerColumn[0].branchingChance[ROOM_CONNECTION_TOP] = 33;
            generator.connectionsSettingsPerColumn[0].branchingChance[ROOM_CONNECTION_MID] = 33;
            generator.connectionsSettingsPerColumn[0].branchingChance[ROOM_CONNECTION_BOT] = 33;
            
            for(i = 1; i < MAX_CONNECTION_GENERATOR_COLUMNS; ++i)
            {
                if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
                {
                    // Mixed but not too wide
                    generator.connectionsSettingsPerColumn[i].minCount = 1;
                    generator.connectionsSettingsPerColumn[i].maxCount = 2;
                    generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                    generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 40;
                    generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                }
                else
                {
                    // Random column variant switches
                    switch (RogueRandom() % 6)
                    {
                    // Mixed/Standard
                    case 0:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 3;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    // Branches
                    case 1:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 2;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 0;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    // Lines
                    case 2:
                        generator.connectionsSettingsPerColumn[i].minCount = 2;
                        generator.connectionsSettingsPerColumn[i].maxCount = 2;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 10;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 50;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 10;
                        break;

                    // Wiggling line
                    case 3:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 1;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 0;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    // Fork
                    case 4:
                        generator.connectionsSettingsPerColumn[i].minCount = 3;
                        generator.connectionsSettingsPerColumn[i].maxCount = 3;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 100;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 100;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 100;
                        break;

                    // Mixed/Standard Alt
                    case 5:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 3;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 50;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 10;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 50;
                        break;
                    
                    default:
                        AGB_ASSERT(FALSE);
                        break;
                    }
                }
            }
        }

        DebugPrintf("ADVPATH: Generating path for seed %d.", gRngRogueValue);
        Rogue_ResetAdventurePathBuffers();
        GeneratePath(&pathSettings);
        DebugPrint("ADVPATH: Finished generating path.");

        gRogueAdvPath.justGenerated = isNewGeneration;

        if(!isNewGeneration)
        {
            // Remember the room type/params
            gRogueAdvPath.currentRoomType = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomType;
            memcpy(&gRogueAdvPath.currentRoomParams, &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomParams, sizeof(gRogueAdvPath.currentRoomParams));
        }

        return isNewGeneration;
    }
}

void RogueAdv_Debug_ForceRegenerateAdventurePaths()
{
#ifdef ROGUE_DEBUG
    struct WarpData warp;

    gRogueAdvPath.roomCount = 0;
    gRogueAdvPath.isOverviewActive = FALSE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    
    // Fill with dud warp
    warp.mapGroup = MAP_GROUP(ROGUE_HUB_TRANSITION);
    warp.mapNum = MAP_NUM(ROGUE_HUB_TRANSITION);
    warp.warpId = 0;
    warp.x = -1;
    warp.y = -1;

    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
#endif
}

u8 RogueAdv_GetTileNum()
{
    if(!gRogueAdvPath.isOverviewActive && gRogueRun.adventureRoomId < gRogueAdvPath.roomCount)
    {
        return gRogueAdvPath.pathLength - gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x - 1;
    }

    // Fallback so we are viewing the same thing
    return 0;
}

bool8 RogueAdv_IsViewingPath()
{
    return gRogueAdvPath.isOverviewActive != 0;
}

void RogueAdv_ApplyAdventureMetatiles()
{
    struct Coords16 treesCoords[24];
    u32 metatile;
    u16 x, y;
    u16 treeCount;
    u8 i, j;
    bool8 isValid;

    // Detect trees, as we will likely need to remove them later
    treeCount = 0;

    for(y = 0; y < gMapHeader.mapLayout->height; ++y)
    for(x = 0; x < gMapHeader.mapLayout->width; ++x)
    {
        metatile = MapGridGetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET);

        if(metatile == sTreeDecorationMetatiles[0].metatile)
        {
            treesCoords[treeCount].x = x;
            treesCoords[treeCount].y = y;
            ++treeCount;

            AGB_ASSERT(treeCount < ARRAY_COUNT(treesCoords));
        }
    }

    // Draw room path
    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        // Move coords into world space
        x = ROOM_TO_METATILE_X(gRogueAdvPath.rooms[i].coords.x);
        y = ROOM_TO_METATILE_Y(gRogueAdvPath.rooms[i].coords.y);

        // Main tile where object will be placed
        //
        
        if(ShouldBlockObjectEvent(&gRogueAdvPath.rooms[i]))
        {
            // Place rock to block way back
            MapGridSetMetatileIdAt(x + 2, y, METATILE_General_SandPit_Stone | MAPGRID_COLLISION_MASK);
        }
        else
        {
            MapGridSetMetatileIdAt(x + 2, y, METATILE_General_SandPit_Center);
        }

        // Place connecting tiles infront
        //
        // ROOM_CONNECTION_MASK_MID (Always needed)
        MapGridSetMetatileIdAt(x + 1, y + 0, METATILE_General_SandPit_Center);
        
        if((gRogueAdvPath.rooms[i].connectionMask & ROOM_CONNECTION_MASK_TOP) != 0)
            MapGridSetMetatileIdAt(x + 1, y + 1, METATILE_General_SandPit_Center);

        if((gRogueAdvPath.rooms[i].connectionMask & ROOM_CONNECTION_MASK_BOT) != 0)
            MapGridSetMetatileIdAt(x + 1, y - 1, METATILE_General_SandPit_Center);

        // Place connecting tiles behind (Unless we're the final node)
        //
        if(i != 0)
        {
            for(j = 1; j < ROOM_TO_WORLD_X; ++j)
            {
                if(j == 1 && IsObjectEventVisible(&gRogueAdvPath.rooms[i]))
                    // Place stone to block interacting from the back
                    MapGridSetMetatileIdAt(x + 2 + j, y, METATILE_General_SandPit_Stone | MAPGRID_COLLISION_MASK);
                else
                    MapGridSetMetatileIdAt(x + 2 + j, y, METATILE_General_SandPit_Center);
            }
        }
    }

    // Draw initial start line
    {
        // find start/end coords
        u8 minY = (u8)-1;
        u8 maxY = 0;

        x = 0;
        y = 0;

        for(i = 0; i < gRogueAdvPath.roomCount; ++i)
        {
            // Count if in first column
            if(gRogueAdvPath.rooms[i].coords.x == gRogueAdvPath.pathLength - 1)
            {
                // Move coords into world space
                x = ROOM_TO_METATILE_X(gRogueAdvPath.rooms[i].coords.x);
                y = ROOM_TO_METATILE_Y(gRogueAdvPath.rooms[i].coords.y);

                minY = min(minY, y);
                maxY = max(maxY, y);
            }
        }

        for(i = minY; i <= maxY; ++i)
        {
            MapGridSetMetatileIdAt(x + 1, i, METATILE_General_SandPit_Center);
        }
    }

    // Remove any decorations that may have been split in parts by the path placement
    for(j = 0; j < treeCount; ++j)
    {
        x = treesCoords[j].x + MAP_OFFSET;
        y = treesCoords[j].y + MAP_OFFSET;

        // Check for any missing tiles
        isValid = TRUE;

        for(i = 0; i < ARRAY_COUNT(sTreeDecorationMetatiles); ++i)
        {
            if(MapGridGetMetatileIdAt(x + sTreeDecorationMetatiles[i].x, y + sTreeDecorationMetatiles[i].y) != sTreeDecorationMetatiles[i].metatile)
            {
                isValid = FALSE;
                break;
            }
        }

        // If we're missing a tile remove rest of the tree
        if(!isValid)
        {
            for(i = 0; i < ARRAY_COUNT(sTreeDecorationMetatiles); ++i)
            {
                if(MapGridGetMetatileIdAt(x + sTreeDecorationMetatiles[i].x, y + sTreeDecorationMetatiles[i].y) == sTreeDecorationMetatiles[i].metatile)
                {
                    MapGridSetMetatileIdAt(x + sTreeDecorationMetatiles[i].x, y + sTreeDecorationMetatiles[i].y, METATILE_General_Grass | MAPGRID_COLLISION_MASK);
                }
            }
        }
    }
}


static void SetBossRoomWarp(u16 trainerNum, struct WarpData* warp)
{
    if(GetPathGenerationDifficulty() == ROGUE_FINAL_CHAMP_DIFFICULTY)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_13);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_13);
    }
    else if(GetPathGenerationDifficulty() == ROGUE_CHAMP_START_DIFFICULTY)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_12);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_12);
    }
    else if(GetPathGenerationDifficulty() >= ROGUE_ELITE_START_DIFFICULTY)
    {
        Rogue_GetPreferredElite4Map(trainerNum, &warp->mapGroup, &warp->mapNum);
    }
    else
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_0);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_0);
    }
}

static void ApplyCurrentNodeWarp(struct WarpData *warp)
{
    struct RogueAdvPathRoom* room = &gRogueAdvPath.rooms[gRogueRun.adventureRoomId];

    SeedRogueRng(room->rngSeed);

    switch(room->roomType)
    {
        case ADVPATH_ROOM_BOSS:
            SetBossRoomWarp(room->roomParams.perType.boss.trainerNum, warp);
            break;

        case ADVPATH_ROOM_RESTSTOP:
            warp->mapGroup = gRogueRestStopEncounterInfo.mapTable[room->roomParams.roomIdx].group;
            warp->mapNum = gRogueRestStopEncounterInfo.mapTable[room->roomParams.roomIdx].num;
            break;

        case ADVPATH_ROOM_ROUTE:
            warp->mapGroup = gRogueRouteTable.routes[room->roomParams.roomIdx].map.group;
            warp->mapNum = gRogueRouteTable.routes[room->roomParams.roomIdx].map.num;
            break;

        case ADVPATH_ROOM_LEGENDARY:
            warp->mapGroup = gRogueLegendaryEncounterInfo.mapTable[room->roomParams.roomIdx].group;
            warp->mapNum = gRogueLegendaryEncounterInfo.mapTable[room->roomParams.roomIdx].num;
            break;

        case ADVPATH_ROOM_TEAM_HIDEOUT:
            warp->mapGroup = gRogueTeamEncounterInfo.mapTable[room->roomParams.roomIdx].group;
            warp->mapNum = gRogueTeamEncounterInfo.mapTable[room->roomParams.roomIdx].num;
            break;

        case ADVPATH_ROOM_MINIBOSS:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_MINI_BOSS);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_MINI_BOSS);
            break;

        case ADVPATH_ROOM_WILD_DEN:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_DEN);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_DEN);
            break;

        case ADVPATH_ROOM_HONEY_TREE:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_HONEY_TREE);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_HONEY_TREE);
            break;

        case ADVPATH_ROOM_GAMESHOW:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_GAME_SHOW);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_GAME_SHOW);
            break;

        case ADVPATH_ROOM_DARK_DEAL:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_GRAVEYARD);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_GRAVEYARD);
            break;

        case ADVPATH_ROOM_LAB:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_LAB);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_LAB);
            break;

        case ADVPATH_ROOM_SHRINE:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_SHRINE);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_SHRINE);
            break;

        case ADVPATH_ROOM_CATCHING_CONTEST:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_CATCHING_CONTEST);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_CATCHING_CONTEST);
            break;

        case ADVPATH_ROOM_SIGN:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_SIGN);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_SIGN);
            break;

        case ADVPATH_ROOM_BATTLE_SIM:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_BATTLE_SIM);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_BATTLE_SIM);
            break;
    }
}

u8 RogueAdv_OverrideNextWarp(struct WarpData *warp)
{
    // Should already be set correctly for RogueAdv_WarpLastInteractedRoom
    if(!gRogueAdvPath.isOverviewActive)
    {
        bool8 freshPath = RogueAdv_GenerateAdventurePathsIfRequired();

        // Always jump back to overview screen, after a different route
        warp->mapGroup = MAP_GROUP(ROGUE_ADVENTURE_PATHS);
        warp->mapNum = MAP_NUM(ROGUE_ADVENTURE_PATHS);
        warp->warpId = WARP_ID_NONE;

        if(freshPath || gRogueRun.adventureRoomId == ADVPATH_INVALID_ROOM_ID)
        {
            // Warp to initial start line
            // find start/end coords
            u8 i, x, y;
            u8 minY = (u8)-1;
            u8 maxY = 0;

            x = 0;
            y = 0;

            for(i = 0; i < gRogueAdvPath.roomCount; ++i)
            {
                // Count if in first column
                if(gRogueAdvPath.rooms[i].coords.x == gRogueAdvPath.pathLength - 1)
                {
                    // Move coords into world space
                    x = ROOM_TO_WARP_X(gRogueAdvPath.rooms[i].coords.x); 
                    y = ROOM_TO_WARP_Y(gRogueAdvPath.rooms[i].coords.y);

                    minY = min(minY, y);
                    maxY = max(maxY, y);
                }
            }

            warp->x = x - 2;
            warp->y = minY + (maxY - minY) / 2;
        }
        else
        {
            warp->x = ROOM_TO_WARP_X(gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x);
            warp->y = ROOM_TO_WARP_Y(gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.y);
        }

        // Trigger before we wipe the room type
        RogueQuest_OnTrigger(QUEST_TRIGGER_EXIT_ENCOUNTER);

        gRogueAdvPath.currentRoomType = ADVPATH_ROOM_NONE;
        return ROGUE_WARP_TO_ADVPATH;
    }
    else
    {
        ApplyCurrentNodeWarp(warp);
        return ROGUE_WARP_TO_ROOM;
    }
}

extern const u8 Rogue_AdventurePaths_InteractRoom[];

void RogueAdv_ModifyObjectEvents(struct MapHeader *mapHeader, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity)
{
    u8 i;
    u8 writeIdx;
    u8 x, y;

    writeIdx = 0;

    // Draw room path
    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        // Move coords into world space
        x = ROOM_TO_OBJECT_EVENT_X(gRogueAdvPath.rooms[i].coords.x);
        y = ROOM_TO_OBJECT_EVENT_Y(gRogueAdvPath.rooms[i].coords.y);

        if(writeIdx < objectEventCapacity)
        {
            if(IsObjectEventVisible(&gRogueAdvPath.rooms[i]))
            {
                objectEvents[writeIdx].localId = writeIdx;
                objectEvents[writeIdx].graphicsId = SelectObjectGfxForRoom(&gRogueAdvPath.rooms[i]);
                objectEvents[writeIdx].x = x;
                objectEvents[writeIdx].y = y;
                objectEvents[writeIdx].elevation = 3;
                objectEvents[writeIdx].trainerType = TRAINER_TYPE_NONE;
                objectEvents[writeIdx].movementType = SelectObjectMovementTypeForRoom(&gRogueAdvPath.rooms[i]);

                // Pack node into this var
                objectEvents[writeIdx].trainerRange_berryTreeId = i;
                objectEvents[writeIdx].script = Rogue_AdventurePaths_InteractRoom;

                ++writeIdx;
            }
        }
        else
        {
            DebugPrintf("WARNING: Cannot add adventure path object %d (out of range %d)", writeIdx, objectEventCapacity);
        }
    }

    *objectEventCount = writeIdx;
}

bool8 RogueAdv_CanUseEscapeRope(void)
{
    if(!gRogueAdvPath.isOverviewActive)
    {
        // We are in transition i.e. just started the run
        if(gRogueAdvPath.roomCount == 0)
            return FALSE;
        
        switch(gRogueAdvPath.currentRoomType)
        {
            case ADVPATH_ROOM_BOSS:
                return FALSE;

            default:
                return TRUE;
        }
    }

    return FALSE;
}

u8 Rogue_GetTypeForHintForRoom(struct RogueAdvPathRoom const* room)
{
    return gRogueRouteTable.routes[room->roomParams.roomIdx].wildTypeTable[(room->coords.x + room->coords.y) % ARRAY_COUNT(gRogueRouteTable.routes[0].wildTypeTable)];
}

static u16 SelectObjectGfxForRoom(struct RogueAdvPathRoom* room)
{
    switch(room->roomType)
    {
        case ADVPATH_ROOM_NONE:
            return 0;
            
        case ADVPATH_ROOM_ROUTE:
        {
            switch(Rogue_GetTypeForHintForRoom(room))
            {
                case TYPE_BUG:
                    return OBJ_EVENT_GFX_ROUTE_BUG;
                case TYPE_DARK:
                    return OBJ_EVENT_GFX_ROUTE_DARK;
                case TYPE_DRAGON:
                    return OBJ_EVENT_GFX_ROUTE_DRAGON;
                case TYPE_ELECTRIC:
                    return OBJ_EVENT_GFX_ROUTE_ELECTRIC;
#ifdef ROGUE_EXPANSION
                case TYPE_FAIRY:
                    return OBJ_EVENT_GFX_ROUTE_FAIRY;
#endif
                case TYPE_FIGHTING:
                    return OBJ_EVENT_GFX_ROUTE_FIGHTING;
                case TYPE_FIRE:
                    return OBJ_EVENT_GFX_ROUTE_FIRE;
                case TYPE_FLYING:
                    return OBJ_EVENT_GFX_ROUTE_FLYING;
                case TYPE_GHOST:
                    return OBJ_EVENT_GFX_ROUTE_GHOST;
                case TYPE_GRASS:
                    return OBJ_EVENT_GFX_ROUTE_GRASS;
                case TYPE_GROUND:
                    return OBJ_EVENT_GFX_ROUTE_GROUND;
                case TYPE_ICE:
                    return OBJ_EVENT_GFX_ROUTE_ICE;
                case TYPE_NORMAL:
                    return OBJ_EVENT_GFX_ROUTE_NORMAL;
                case TYPE_POISON:
                    return OBJ_EVENT_GFX_ROUTE_POISON;
                case TYPE_PSYCHIC:
                    return OBJ_EVENT_GFX_ROUTE_PSYCHIC;
                case TYPE_ROCK:
                    return OBJ_EVENT_GFX_ROUTE_ROCK;
                case TYPE_STEEL:
                    return OBJ_EVENT_GFX_ROUTE_STEEL;
                case TYPE_WATER:
                    return OBJ_EVENT_GFX_ROUTE_WATER;

                default:
                //case TYPE_MYSTERY:
                    return OBJ_EVENT_GFX_ROUTE_MYSTERY;
            }
        }

        case ADVPATH_ROOM_RESTSTOP:
            return gRogueRestStopEncounterInfo.mapTable[room->roomParams.roomIdx].encounterId;

        case ADVPATH_ROOM_LEGENDARY:
            return OBJ_EVENT_GFX_TRICK_HOUSE_STATUE;

        case ADVPATH_ROOM_TEAM_HIDEOUT:
        {
            u8 gender = (room->coords.x + room->coords.y) % 2;

            switch (gRogueRun.teamEncounterNum)
            {
            case TEAM_NUM_KANTO_ROCKET:
            case TEAM_NUM_JOHTO_ROCKET:
                return gender ? OBJ_EVENT_GFX_ROCKET_M : OBJ_EVENT_GFX_ROCKET_F;

            case TEAM_NUM_AQUA:
                return gender ? OBJ_EVENT_GFX_AQUA_MEMBER_M : OBJ_EVENT_GFX_AQUA_MEMBER_F;

            case TEAM_NUM_MAGMA:
                return gender ? OBJ_EVENT_GFX_MAGMA_MEMBER_M : OBJ_EVENT_GFX_MAGMA_MEMBER_F;

            case TEAM_NUM_GALACTIC:
                return gender ? OBJ_EVENT_GFX_TEAM_GALACTIC_GRUNT_M : OBJ_EVENT_GFX_TEAM_GALACTIC_GRUNT_F;

            default:
                AGB_ASSERT(FALSE);
                return OBJ_EVENT_GFX_ROCKET_M;
            }
        }

        case ADVPATH_ROOM_MINIBOSS:
            return OBJ_EVENT_GFX_NOLAND;

        case ADVPATH_ROOM_WILD_DEN:
            return OBJ_EVENT_GFX_GRASS_DEFAULT;

        case ADVPATH_ROOM_HONEY_TREE:
            return OBJ_EVENT_GFX_GOLD_GRASS;

        case ADVPATH_ROOM_GAMESHOW:
            return OBJ_EVENT_GFX_CONTEST_JUDGE;

        case ADVPATH_ROOM_DARK_DEAL:
            return OBJ_EVENT_GFX_DEVIL_MAN;

        case ADVPATH_ROOM_LAB:
            return OBJ_EVENT_GFX_PC;

        case ADVPATH_ROOM_SHRINE:
            return OBJ_EVENT_GFX_MISC_CHANNELER;

        case ADVPATH_ROOM_CATCHING_CONTEST:
            return OBJ_EVENT_GFX_MISC_BUG_CATCHER;

        case ADVPATH_ROOM_SIGN:
            return OBJ_EVENT_GFX_SMALL_SIGN;

        case ADVPATH_ROOM_BATTLE_SIM:
            return OBJ_EVENT_GFX_YOUNGSTER;

        case ADVPATH_ROOM_BOSS:
            return OBJ_EVENT_GFX_BATTLE_STATUE;
    }

    return 0;
}

static u8 SelectObjectMovementTypeForRoom(struct RogueAdvPathRoom* room)
{
    switch(room->roomType)
    {
        case ADVPATH_ROOM_ROUTE:
        {
            switch(room->roomParams.perType.route.difficulty)
            {
                case 1: // ADVPATH_SUBROOM_ROUTE_AVERAGE
                    return MOVEMENT_TYPE_FACE_UP;
                case 2: // ADVPATH_SUBROOM_ROUTE_TOUGH
                    return MOVEMENT_TYPE_FACE_LEFT;
                default: // ADVPATH_SUBROOM_ROUTE_CALM
                    return MOVEMENT_TYPE_NONE;
            };
        }
    }

    return MOVEMENT_TYPE_NONE;
}

static bool8 IsObjectEventVisible(struct RogueAdvPathRoom* room)
{
    if(room->roomType == ADVPATH_ROOM_NONE)
        return FALSE;

    if(gRogueAdvPath.justGenerated)
    {
        // Everything is visible
        return TRUE;
    }
    else
    {
        u8 focusX = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x;
        return room->coords.x < focusX;
    }
}

static bool8 ShouldBlockObjectEvent(struct RogueAdvPathRoom* room)
{
    if(gRogueAdvPath.justGenerated)
    {
        return FALSE;
    }
    else
    {
        u8 focusX = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x;
        return room->coords.x == focusX;
    }
}

static void BufferTypeAdjective(u8 type)
{
    const u8 gText_AdjNormal[] = _("Typical");
    const u8 gText_AdjFighting[] = _("Mighty");
    const u8 gText_AdjFlying[] = _("Breezy");
    const u8 gText_AdjPoison[] = _("Corrosive");
    const u8 gText_AdjGround[] = _("Coarse");
    const u8 gText_AdjRock[] = _("Rugged");
    const u8 gText_AdjBug[] = _("Swarming");
    const u8 gText_AdjGhost[] = _("Spooky");
    const u8 gText_AdjSteel[] = _("Sharp");
    const u8 gText_AdjFire[] = _("Warm");
    const u8 gText_AdjWater[] = _("Wet");
    const u8 gText_AdjGrass[] = _("Verdant");
    const u8 gText_AdjElectric[] = _("Energetic");
    const u8 gText_AdjPsychic[] = _("Confusing");
    const u8 gText_AdjIce[] = _("Chilly");
    const u8 gText_AdjDragon[] = _("Fierce");
    const u8 gText_AdjDark[] = _("Gloomy");
#ifdef ROGUE_EXPANSION
    const u8 gText_AdjFairy[] = _("Magical");
#endif
    const u8 gText_AdjNone[] = _("???");

    switch(type)
    {
        case TYPE_NORMAL:
            StringCopy(gStringVar1, gText_AdjNormal);
            break;

        case TYPE_FIGHTING:
            StringCopy(gStringVar1, gText_AdjFighting);
            break;

        case TYPE_FLYING:
            StringCopy(gStringVar1, gText_AdjFlying);
            break;

        case TYPE_POISON:
            StringCopy(gStringVar1, gText_AdjPoison);
            break;

        case TYPE_GROUND:
            StringCopy(gStringVar1, gText_AdjGround);
            break;

        case TYPE_ROCK:
            StringCopy(gStringVar1, gText_AdjRock);
            break;

        case TYPE_BUG:
            StringCopy(gStringVar1, gText_AdjBug);
            break;

        case TYPE_GHOST:
            StringCopy(gStringVar1, gText_AdjGhost);
            break;

        case TYPE_STEEL:
            StringCopy(gStringVar1, gText_AdjSteel);
            break;

        case TYPE_FIRE:
            StringCopy(gStringVar1, gText_AdjFire);
            break;

        case TYPE_WATER:
            StringCopy(gStringVar1, gText_AdjWater);
            break;

        case TYPE_GRASS:
            StringCopy(gStringVar1, gText_AdjGrass);
            break;

        case TYPE_ELECTRIC:
            StringCopy(gStringVar1, gText_AdjElectric);
            break;

        case TYPE_PSYCHIC:
            StringCopy(gStringVar1, gText_AdjPsychic);
            break;

        case TYPE_ICE:
            StringCopy(gStringVar1, gText_AdjIce);
            break;

        case TYPE_DRAGON:
            StringCopy(gStringVar1, gText_AdjDragon);
            break;

        case TYPE_DARK:
            StringCopy(gStringVar1, gText_AdjDark);
            break;

#ifdef ROGUE_EXPANSION
        case TYPE_FAIRY:
            StringCopy(gStringVar1, gText_AdjFairy);
            break;
#endif

        default:
            StringCopy(gStringVar1, gText_AdjNone);
            break;
    }
}

static u8 GetRoomIndexFromLastInteracted()
{
    u16 lastTalkedId = VarGet(VAR_LAST_TALKED);

    // We have to lookup into the template as this var gets zeroed when it's not being used in a valid way
    return gSaveBlock1Ptr->objectEventTemplates[lastTalkedId].trainerRange_berryTreeId;

    //u8 objEventId = GetObjectEventIdByLocalIdAndMap(lastTalkedId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    //u8 roomIdx = gObjectEvents[objEventId].trainerRange_berryTreeId;
    //return roomIdx;
}

void RogueAdv_GetLastInteractedRoomParams()
{
    u8 roomIdx = GetRoomIndexFromLastInteracted();

    gSpecialVar_ScriptNodeParam0 = gRogueAdvPath.rooms[roomIdx].roomType;
    gSpecialVar_ScriptNodeParam1 = gRogueAdvPath.rooms[roomIdx].roomParams.roomIdx;

    switch(gRogueAdvPath.rooms[roomIdx].roomType)
    {
        case ADVPATH_ROOM_ROUTE:
            gSpecialVar_ScriptNodeParam1 = gRogueAdvPath.rooms[roomIdx].roomParams.perType.route.difficulty;
            BufferTypeAdjective(Rogue_GetTypeForHintForRoom(&gRogueAdvPath.rooms[roomIdx]));
            break;
    }
}

bool8 Rogue_SafeSmartCheckInternal();

void RogueAdv_WarpLastInteractedRoom()
{
    struct WarpData warp;
    u8 roomIdx = GetRoomIndexFromLastInteracted();

    if(Rogue_SafeSmartCheckInternal())
    {
        // Move to the selected node
        gRogueRun.adventureRoomId = roomIdx;
        gRogueAdvPath.currentRoomType = gRogueAdvPath.rooms[roomIdx].roomType;
        memcpy(&gRogueAdvPath.currentRoomParams, &gRogueAdvPath.rooms[roomIdx].roomParams, sizeof(gRogueAdvPath.currentRoomParams));

        // Fill with dud warp
        warp.mapGroup = MAP_GROUP(ROGUE_HUB_TRANSITION);
        warp.mapNum = MAP_NUM(ROGUE_HUB_TRANSITION);
        warp.warpId = 0;
        warp.x = -1;
        warp.y = -1;
    }
    else
    {
        warp.mapGroup = MAP_GROUP(ROGUE_AREA_ADVENTURE_ENTRANCE);
        warp.mapNum = MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE);
        warp.warpId = 0;
        warp.x = -1;
        warp.y = -1;
    }


    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
}