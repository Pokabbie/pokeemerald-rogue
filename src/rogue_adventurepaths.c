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
#include "rogue_adventure.h"
#include "rogue_campaign.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"


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
    struct AdvPathConnectionSettings connectionsPerRoom[ADVPATH_ROOM_COUNT];
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
static void GenerateRoom(struct AdvPathRoomSettings* roomSettings, struct AdvPathSettings* pathSettings);
static struct AdvPathRoomSettings* GenerateChildRoom(struct AdvPathRoomSettings* parentRoom, struct AdvPathSettings* pathSettings);

static void AssignWeights_Standard(struct AdvPathRoomSettings* parentRoom, struct AdvPathSettings* pathSettings, u16* weights);
static void AssignWeights_Finalize(struct AdvPathRoomSettings* parentRoom, struct AdvPathSettings* pathSettings, u16* weights);

static u8 GenerateRoomConnectionMask(struct AdvPathRoomSettings* roomSettings, struct AdvPathSettings* pathSettings);
static bool8 DoesRoomExists(s8 x, s8 y, struct AdvPathSettings* pathSettings);

static u16 SelectObjectGfxForRoom(struct RogueAdvPathRoom* room);
static u8 SelectObjectMovementTypeForRoom(struct RogueAdvPathRoom* room);


static void GeneratePath(struct AdvPathSettings* pathSettings)
{
    struct AdvPathRoomSettings* bossRoom = &pathSettings->roomScratch[0];
    memset(bossRoom, 0, sizeof(bossRoom));

    AGB_ASSERT(pathSettings->generator != NULL);

    bossRoom->roomType = ADVPATH_ROOM_BOSS;
    GenerateRoom(bossRoom, pathSettings);

    gRogueAdvPath.roomCount = pathSettings->nodeCount;
    gRogueAdvPath.pathLength = pathSettings->totalLength;

    //if(gRogueRun.adventureRoomId == ADVPATH_INVALID_ROOM_ID)
    //{
    //    // Just entered this segment, so respawn at the start (Otherwise we're probably reloading from a rest save)
    //    gRogueRun.adventureRoomId = gRogueAdvPath.roomCount - 1;
    //}

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

static void GenerateRoom(struct AdvPathRoomSettings* roomSettings, struct AdvPathSettings* pathSettings)
{
    if(pathSettings->nodeCount >= ROGUE_ADVPATH_ROOM_CAPACITY)
    {
        // Cannot generate any more
        DebugPrint("ADVPATH: \tReached room/node capacity.");
        return;
    }
    else
    {
        u8 nodeId = pathSettings->nodeCount++;

        ++pathSettings->numOfRooms[roomSettings->roomType];
        DebugPrintf("ADVPATH: \tAdded room type %d (Total: %d)", roomSettings->roomType, pathSettings->numOfRooms[roomSettings->roomType]);

        // Populate the room params
        //
        {
            u16 weights[ADVPATH_SUBROOM_WEIGHT_COUNT];
            memset(weights, 0, sizeof(weights));

            switch(roomSettings->roomType)
            {
                case ADVPATH_ROOM_BOSS:
                    roomSettings->roomParams.perType.boss.trainerNum = Rogue_NextBossTrainerId();
                    break;

                case ADVPATH_ROOM_RESTSTOP:
                    weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 3;   //gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_RESTSTOP_BATTLE];
                    weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = 3;     //gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_RESTSTOP_SHOP];
                    weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = 1;     //gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_RESTSTOP_FULL];

                    roomSettings->roomParams.roomIdx = SelectIndexFromWeights(weights, ARRAY_COUNT(weights), RogueRandom());
                    break;

                case ADVPATH_ROOM_LEGENDARY:
                    roomSettings->roomParams.roomIdx = Rogue_SelectLegendaryEncounterRoom();
                    roomSettings->roomParams.perType.legendary.shinyState = RogueRandomRange(Rogue_GetShinyOdds(), OVERWORLD_FLAG) == 0;
                    break;

                case ADVPATH_ROOM_MINIBOSS:
                    roomSettings->roomParams.roomIdx = 0;
                    roomSettings->roomParams.perType.miniboss.trainerNum = Rogue_NextMinibossTrainerId();
                    break;

                case ADVPATH_ROOM_WILD_DEN:
                    roomSettings->roomParams.roomIdx = 0;
                    roomSettings->roomParams.perType.wildDen.species = Rogue_SelectWildDenEncounterRoom();
                    roomSettings->roomParams.perType.wildDen.shinyState = RogueRandomRange(Rogue_GetShinyOdds(), OVERWORLD_FLAG) == 0;
                    break;

                case ADVPATH_ROOM_ROUTE:
                {
                    roomSettings->roomParams.roomIdx = Rogue_SelectRouteRoom();

                    weights[ADVPATH_SUBROOM_ROUTE_CALM] = 2;    //gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_ROUTE_CALM];
                    weights[ADVPATH_SUBROOM_ROUTE_AVERAGE] = 2; //gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_ROUTE_AVERAGE];
                    weights[ADVPATH_SUBROOM_ROUTE_TOUGH] = 1;   //gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_ROUTE_TOUGH];

                    roomSettings->roomParams.perType.route.difficulty = SelectIndexFromWeights(weights, ARRAY_COUNT(weights), RogueRandom());
                    break;
                }
            }
        }

        // Write output for this room
        //
        gRogueAdvPath.rooms[nodeId].coords = roomSettings->currentCoords;
        gRogueAdvPath.rooms[nodeId].roomType = roomSettings->roomType;
        gRogueAdvPath.rooms[nodeId].roomParams = roomSettings->roomParams;
        gRogueAdvPath.rooms[nodeId].connectionMask = 0;
        gRogueAdvPath.rooms[nodeId].rngSeed = RogueRandom();


        // Generate children
        //
        if(roomSettings->currentCoords.x + 1 < pathSettings->totalLength)
        {
            struct Coords8 coords;
            struct AdvPathRoomSettings* childRoom;
            u8 connectionMask = GenerateRoomConnectionMask(roomSettings, pathSettings);

            gRogueAdvPath.rooms[nodeId].connectionMask = connectionMask;

            if((connectionMask & ROOM_CONNECTION_MASK_TOP) != 0 && !DoesRoomExists(roomSettings->currentCoords.x + 1, roomSettings->currentCoords.y + 1, pathSettings))
            {
                childRoom = GenerateChildRoom(roomSettings, pathSettings);
                childRoom->currentCoords.x = roomSettings->currentCoords.x + 1;
                childRoom->currentCoords.y = roomSettings->currentCoords.y + 1;

                GenerateRoom(childRoom, pathSettings);
            }

            if((connectionMask & ROOM_CONNECTION_MASK_MID) != 0 && !DoesRoomExists(roomSettings->currentCoords.x + 1, roomSettings->currentCoords.y + 0, pathSettings))
            {
                childRoom = GenerateChildRoom(roomSettings, pathSettings);
                childRoom->currentCoords.x = roomSettings->currentCoords.x + 1;
                childRoom->currentCoords.y = roomSettings->currentCoords.y + 0;

                GenerateRoom(childRoom, pathSettings);
            }

            if((connectionMask & ROOM_CONNECTION_MASK_BOT) != 0 && !DoesRoomExists(roomSettings->currentCoords.x + 1, roomSettings->currentCoords.y - 1, pathSettings))
            {
                childRoom = GenerateChildRoom(roomSettings, pathSettings);
                childRoom->currentCoords.x = roomSettings->currentCoords.x + 1;
                childRoom->currentCoords.y = roomSettings->currentCoords.y - 1;

                GenerateRoom(childRoom, pathSettings);
            }
        }
    }
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

static u8 GenerateRoomConnectionMask(struct AdvPathRoomSettings* roomSettings, struct AdvPathSettings* pathSettings)
{
    u8 mask, i;
    u8 connCount;
    u8 branchingChances[ROOM_CONNECTION_COUNT];
    u8 minConnCount = pathSettings->generator->connectionsPerRoom[roomSettings->roomType].minCount;
    u8 maxConnCount = pathSettings->generator->connectionsPerRoom[roomSettings->roomType].maxCount;

    // Use default settings
    if(minConnCount == 0 && maxConnCount == 0)
    {
        minConnCount = 1;
        maxConnCount = 3;

        for(i = 0; i < ROOM_CONNECTION_COUNT; ++i)
            branchingChances[i] = 40;
    }
    else
    {
        for(i = 0; i < ROOM_CONNECTION_COUNT; ++i)
            branchingChances[i] = pathSettings->generator->connectionsPerRoom[roomSettings->roomType].branchingChance[i];
    }

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

    return mask;
}

static struct AdvPathRoomSettings* GenerateChildRoom(struct AdvPathRoomSettings* parentRoom, struct AdvPathSettings* pathSettings)
{
    // Take from preallocated array
    struct AdvPathRoomSettings* newRoom = &pathSettings->roomScratch[pathSettings->nodeCount];
    memset(newRoom, 0, sizeof(newRoom));

    if(parentRoom->roomType == ADVPATH_ROOM_BOSS)
    {
        // These are intentionally empty "branching" nodes
        newRoom->roomType = ADVPATH_ROOM_NONE;
    }
    else
    {
        u16 weights[ADVPATH_ROOM_WEIGHT_COUNT];

        // Treat the empty spaces before the boss as the actual boss in the generation code
        // this is as we always have empty tiles right before the boss to created the initial branches
        if(parentRoom->roomType == ADVPATH_ROOM_NONE && parentRoom->currentCoords.x == 1)
            parentRoom->roomType = ADVPATH_ROOM_BOSS;

        AssignWeights_Standard(parentRoom, pathSettings, weights);
        AssignWeights_Finalize(parentRoom, pathSettings, weights);

        newRoom->roomType = SelectIndexFromWeights(weights, ARRAY_COUNT(weights), RogueRandom());
    }

    return newRoom;
}

static bool8 DoesRoomExists(s8 x, s8 y, struct AdvPathSettings* pathSettings)
{
    u8 i;

    for(i = 0; i < pathSettings->nodeCount; ++i)
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
        struct AdvPathSettings* pathSettings;
        struct AdvPathGenerator* generator;

        // If we have a valid room ID, then we're reloading a previous save
        bool8 isNewGeneration = gRogueRun.adventureRoomId == ADVPATH_INVALID_ROOM_ID;

        pathSettings = AllocZeroed(sizeof(struct AdvPathSettings));
        generator = AllocZeroed(sizeof(struct AdvPathGenerator));

        AGB_ASSERT(pathSettings != NULL);
        AGB_ASSERT(generator != NULL);

        pathSettings->generator = generator;
        pathSettings->totalLength = 3 + 2; // +2 to account for final encounter and initial split

        generator->connectionsPerRoom[ADVPATH_ROOM_NONE].minCount = 1;
        generator->connectionsPerRoom[ADVPATH_ROOM_NONE].maxCount = 1;
        generator->connectionsPerRoom[ADVPATH_ROOM_NONE].branchingChance[ROOM_CONNECTION_TOP] = 50;
        generator->connectionsPerRoom[ADVPATH_ROOM_NONE].branchingChance[ROOM_CONNECTION_MID] = 20;
        generator->connectionsPerRoom[ADVPATH_ROOM_NONE].branchingChance[ROOM_CONNECTION_BOT] = 50;

        generator->connectionsPerRoom[ADVPATH_ROOM_BOSS].minCount = 2;
        generator->connectionsPerRoom[ADVPATH_ROOM_BOSS].maxCount = 3;
        generator->connectionsPerRoom[ADVPATH_ROOM_BOSS].branchingChance[ROOM_CONNECTION_TOP] = 33;
        generator->connectionsPerRoom[ADVPATH_ROOM_BOSS].branchingChance[ROOM_CONNECTION_MID] = 33;
        generator->connectionsPerRoom[ADVPATH_ROOM_BOSS].branchingChance[ROOM_CONNECTION_BOT] = 33;

        // Select the correct seed
        {
            u8 i;
            u16 seed;
            SeedRogueRng(gRogueRun.baseSeed);

            seed = RogueRandom();
            for(i = 0; i < gRogueRun.currentDifficulty; ++i)
            {
                seed = RogueRandom();
            }

            // This is the seed for this path
            SeedRogueRng(seed);
        }

        DebugPrintf("ADVPATH: Generating path for seed %d.", gRngRogueValue);
        Rogue_ResetAdventurePathBuffers();
        GeneratePath(pathSettings);
        DebugPrint("ADVPATH: Finished generating path.");

        Free(pathSettings);
        Free(generator);

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

void RogueAdv_ApplyAdventureMetatiles()
{
    struct Coords16 treesCoords[24];
    u32 metatile;
    u16 x, y;
    u16 treeCount;
    u8 i, j;
    bool8 isValid;
    u8 totalHeight;

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


    totalHeight = gRogueAdvPath.pathMaxY - gRogueAdvPath.pathMinY + 1;

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
    if(gRogueRun.currentDifficulty < 8)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_0);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_0);
    }
    else if(gRogueRun.currentDifficulty < 12)
    {
        Rogue_GetPreferredElite4Map(trainerNum, &warp->mapGroup, &warp->mapNum);
    }
    else if(gRogueRun.currentDifficulty < 13)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_12);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_12);
    }
    else if(gRogueRun.currentDifficulty < 14)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_13);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_13);
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

        case ADVPATH_ROOM_MINIBOSS:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_MINI_BOSS);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_MINI_BOSS);
            break;

        case ADVPATH_ROOM_WILD_DEN:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_DEN);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_DEN);
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

        if(freshPath)
        {
            // Warp to initial start line
            // find start/end coords
            u8 i, x, y;
            u8 minY = (u8)-1;
            u8 maxY = 0;

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
    u8 totalHeight;

    writeIdx = 0;
    totalHeight = gRogueAdvPath.pathMaxY - gRogueAdvPath.pathMinY + 1;

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
                // Pack node into movement vars
                objectEvents[writeIdx].movementRangeX = i;//x;
                objectEvents[writeIdx].movementRangeY = 0;//y;
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

static u16 GetTypeForHint(struct RogueAdvPathRoom* room)
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
            switch(GetTypeForHint(room))
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

        case ADVPATH_ROOM_MINIBOSS:
            return OBJ_EVENT_GFX_NOLAND;

        case ADVPATH_ROOM_WILD_DEN:
            return OBJ_EVENT_GFX_GRASS_CUSHION;

        case ADVPATH_ROOM_GAMESHOW:
            return OBJ_EVENT_GFX_CONTEST_JUDGE;

        case ADVPATH_ROOM_DARK_DEAL:
            return OBJ_EVENT_GFX_DEVIL_MAN;

        case ADVPATH_ROOM_LAB:
            return OBJ_EVENT_GFX_PC;

        case ADVPATH_ROOM_BOSS:
            return OBJ_EVENT_GFX_BALL_CUSHION; // ?
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
    const u8 gText_AdjNormal[] = _("TYPICAL");
    const u8 gText_AdjFighting[] = _("MIGHTY");
    const u8 gText_AdjFlying[] = _("BREEZY");
    const u8 gText_AdjPoison[] = _("CORROSIVE");
    const u8 gText_AdjGround[] = _("COARSE");
    const u8 gText_AdjRock[] = _("RUGGED");
    const u8 gText_AdjBug[] = _("SWARMING");
    const u8 gText_AdjGhost[] = _("SPOOKY");
    const u8 gText_AdjSteel[] = _("SHARP");
    const u8 gText_AdjFire[] = _("WARM");
    const u8 gText_AdjWater[] = _("WET");
    const u8 gText_AdjGrass[] = _("VERDANT");
    const u8 gText_AdjElectric[] = _("ENERGETIC");
    const u8 gText_AdjPsychic[] = _("CONFUSING");
    const u8 gText_AdjIce[] = _("CHILLY");
    const u8 gText_AdjDragon[] = _("FIERCE");
    const u8 gText_AdjDark[] = _("GLOOMY");
#ifdef ROGUE_EXPANSION
    const u8 gText_AdjFairy[] = _("MAGICAL");
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

void RogueAdv_GetLastInteractedRoomParams()
{
    u16 lastTalkedId = VarGet(VAR_LAST_TALKED);
    u8 roomIdx = gSaveBlock1Ptr->objectEventTemplates[lastTalkedId].movementRangeX;

    gSpecialVar_ScriptNodeParam0 = gRogueAdvPath.rooms[roomIdx].roomType;
    gSpecialVar_ScriptNodeParam1 = gRogueAdvPath.rooms[roomIdx].roomParams.roomIdx;

    switch(gRogueAdvPath.rooms[roomIdx].roomType)
    {
        case ADVPATH_ROOM_ROUTE:
            gSpecialVar_ScriptNodeParam1 = gRogueAdvPath.rooms[roomIdx].roomParams.perType.route.difficulty;
            BufferTypeAdjective(GetTypeForHint(&gRogueAdvPath.rooms[roomIdx]));
            break;
    }
}

void RogueAdv_WarpLastInteractedRoom()
{
    struct WarpData warp;
    u16 lastTalkedId = VarGet(VAR_LAST_TALKED);
    u8 roomIdx = gSaveBlock1Ptr->objectEventTemplates[lastTalkedId].movementRangeX;

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

    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
}

static void AssignWeights_Standard(struct AdvPathRoomSettings* parentRoom, struct AdvPathSettings* pathSettings, u16* weights)
{
    u16 i;

    // Default weight
    memset(weights, 500, sizeof(weights));

    // Normal routes
    if(parentRoom->roomType == ADVPATH_ROOM_BOSS)
    {
        // Very unlikely at end
        weights[ADVPATH_ROOM_ROUTE] = 100;
    }
    else
    {            
        // If we've reached elite 4 we want to swap odds of none and routes
        if(gRogueRun.currentDifficulty >= 8)
        {
            // Unlikely but not impossible
            weights[ADVPATH_ROOM_ROUTE] = 400;
        }
        else
        {
            // Most common but gets less common over time
            weights[ADVPATH_ROOM_ROUTE] = 2000 - min(100 * gRogueRun.currentDifficulty, 500);
        }
    }

    // If we've reached elite 4 we want to swap odds of none and routes
    if(gRogueRun.currentDifficulty >= 8)
    {
        weights[ADVPATH_ROOM_NONE] = 1200;
    }
    else
    {
        // Unlikely but not impossible
        weights[ADVPATH_ROOM_NONE] = min(50 * (gRogueRun.currentDifficulty + 1), 200);
    }

    // Rest stops
    if(parentRoom->roomType == ADVPATH_ROOM_BOSS)
    {
        weights[ADVPATH_ROOM_RESTSTOP] = 1500;
    }
    else if(gRogueRun.currentDifficulty == 0)
    {
        weights[ADVPATH_ROOM_RESTSTOP] = 0;
    }
    else
    {
        // Unlikely but not impossible
        weights[ADVPATH_ROOM_RESTSTOP] = 100;
    }

    // Legendaries/Mini encounters
    if(gRogueRun.currentDifficulty == 0)
    {
        weights[ADVPATH_ROOM_MINIBOSS] = 0;
        weights[ADVPATH_ROOM_LEGENDARY] = 0;
        weights[ADVPATH_ROOM_WILD_DEN] = 40;
        weights[ADVPATH_ROOM_GAMESHOW] = 0;
        weights[ADVPATH_ROOM_DARK_DEAL] = 0;
        weights[ADVPATH_ROOM_LAB] = 0;
    }
    else
    {
        weights[ADVPATH_ROOM_MINIBOSS] = min(30 * gRogueRun.currentDifficulty, 700);
        weights[ADVPATH_ROOM_WILD_DEN] = min(25 * gRogueRun.currentDifficulty, 400);

        if(gRogueRun.currentDifficulty < 3)
            weights[ADVPATH_ROOM_LAB] = 0;
        else
            weights[ADVPATH_ROOM_LAB] = min(20 * gRogueRun.currentDifficulty, 70);

        // These should start trading with each other deeper into the run
        if(gRogueRun.currentDifficulty < 6)
        {
            weights[ADVPATH_ROOM_GAMESHOW] = 320 - 40 * min(8, gRogueRun.currentDifficulty);
            weights[ADVPATH_ROOM_DARK_DEAL] = 10;
        }
        else
        {
            weights[ADVPATH_ROOM_GAMESHOW] = 10;
            weights[ADVPATH_ROOM_DARK_DEAL] = 360 - 30 * min(5, gRogueRun.currentDifficulty - 6);
        }

        // Every 3rd encounter becomes more common
        if((gRogueRun.currentDifficulty % 3) != 0)
        {
            weights[ADVPATH_ROOM_DARK_DEAL] = 5;
        }

        switch (Rogue_GetConfigRange(DIFFICULTY_RANGE_LEGENDARY))
        {
        case DIFFICULTY_LEVEL_EASY:
            if((gRogueRun.currentDifficulty % 4) == 0)
                // Every 4 badges chances get really high
                weights[ADVPATH_ROOM_LEGENDARY] = 600;
            else
                // Otherwise the chances are just quite low
                weights[ADVPATH_ROOM_LEGENDARY] = 100;
            break;
        
        case DIFFICULTY_LEVEL_MEDIUM:
            // Pre E4 settings
            if(gRogueRun.currentDifficulty < 8)
            {
                if((gRogueRun.currentDifficulty % 3) == 0)
                    // Every 5 badges chances get really high
                    weights[ADVPATH_ROOM_LEGENDARY] = 800;
                else
                    // Otherwise the chances are just quite low
                    weights[ADVPATH_ROOM_LEGENDARY] = 20;
            }
            // E4 settings
            else 
            {
                if((gRogueRun.currentDifficulty % 9) == 0)
                    // Shortly in we have chance to get an uber legendary
                    weights[ADVPATH_ROOM_LEGENDARY] = 800;
                else
                    // Otherwise the chances are just quite low
                    weights[ADVPATH_ROOM_LEGENDARY] = 20;
            }
            break;

        case DIFFICULTY_LEVEL_HARD:
            if((gRogueRun.currentDifficulty % 5) == 0)
                // Every 5 badges chances get really high
                weights[ADVPATH_ROOM_LEGENDARY] = 800;
            else
                // Otherwise impossible
                weights[ADVPATH_ROOM_LEGENDARY] = 0;
            break;

        case DIFFICULTY_LEVEL_BRUTAL:
            // Impossible
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
            break;
        }
    }

    //if(nodeX == 0)
    //{
    //    // Impossible in first column
    //    weights[ADVPATH_ROOM_LEGENDARY] = 0;
    //}

    // Less likely in first column and/or last
    //if(nodeX == 0)
    //{
    //    weights[ADVPATH_ROOM_MINIBOSS] /= 2;
    //    weights[ADVPATH_ROOM_LEGENDARY] /= 2;
    //    weights[ADVPATH_ROOM_WILD_DEN] /= 2;
    //}

    if(parentRoom->roomType == ADVPATH_ROOM_BOSS)
    {
        weights[ADVPATH_ROOM_MINIBOSS] /= 3;
        weights[ADVPATH_ROOM_LEGENDARY] /= 2;
        weights[ADVPATH_ROOM_WILD_DEN] /= 3;
        weights[ADVPATH_ROOM_GAMESHOW] /= 4;
        weights[ADVPATH_ROOM_DARK_DEAL] /= 4;
        weights[ADVPATH_ROOM_LAB] /= 2;
    }

    // Now we've applied the default weights for this column, consider what out next encounter is
    switch(parentRoom->roomType)
    {
        case ADVPATH_ROOM_LEGENDARY:
            weights[ADVPATH_ROOM_RESTSTOP] = 0;
            weights[ADVPATH_ROOM_NONE] = 0;
            weights[ADVPATH_ROOM_WILD_DEN] = 0;
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
            weights[ADVPATH_ROOM_MINIBOSS] *= 2;
            break;

        case ADVPATH_ROOM_MINIBOSS:
            weights[ADVPATH_ROOM_MINIBOSS] = 0;
            weights[ADVPATH_ROOM_DARK_DEAL] *= 2;
            weights[ADVPATH_ROOM_LAB] *= 2;
            break;

        case ADVPATH_ROOM_GAMESHOW:
            weights[ADVPATH_ROOM_GAMESHOW] = 0;
            break;

        case ADVPATH_ROOM_DARK_DEAL:
            weights[ADVPATH_ROOM_DARK_DEAL] = 0;
            break;

        case ADVPATH_ROOM_LAB:
            weights[ADVPATH_ROOM_LAB] = 0;
            break;

        case ADVPATH_ROOM_RESTSTOP:
            weights[ADVPATH_ROOM_RESTSTOP] = 0;
            break;

        case ADVPATH_ROOM_ROUTE:
            weights[ADVPATH_ROOM_NONE] += 300;
            break;

        case ADVPATH_ROOM_NONE:
            weights[ADVPATH_ROOM_NONE] /= 2; // Unlikely to get multiple in a row
            break;
    }
}

static void AssignWeights_Finalize(struct AdvPathRoomSettings* parentRoom, struct AdvPathSettings* pathSettings, u16* weights)
{
    u16 i;
    
    if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_CLASSIC)
    {
        weights[ADVPATH_ROOM_RESTSTOP] /= 2;
        weights[ADVPATH_ROOM_WILD_DEN] = 0;
        weights[ADVPATH_ROOM_GAMESHOW] = 0;
        weights[ADVPATH_ROOM_DARK_DEAL] = 0;
        weights[ADVPATH_ROOM_MINIBOSS] = 0;
        weights[ADVPATH_ROOM_LAB] = 0;
    }
    else if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_POKEBALL_LIMIT)
    {
        weights[ADVPATH_ROOM_LAB] = 0;
    }

    // We have limited number of certain encounters
    switch (Rogue_GetConfigRange(DIFFICULTY_RANGE_LEGENDARY))
    {
    case DIFFICULTY_LEVEL_EASY:
        if(pathSettings->numOfRooms[ADVPATH_ROOM_LEGENDARY] >= 2)
        {
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
        }
        break;

    default:
        if(pathSettings->numOfRooms[ADVPATH_ROOM_LEGENDARY] >= 1)
        {
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
        }
        break;
    }


    if(pathSettings->numOfRooms[ADVPATH_ROOM_WILD_DEN] >= 2)
    {
        weights[ADVPATH_ROOM_WILD_DEN] = 0;
    }

    if(pathSettings->numOfRooms[ADVPATH_ROOM_MINIBOSS] >= 2)
    {
        weights[ADVPATH_ROOM_MINIBOSS] = 0;
    }

    if(pathSettings->numOfRooms[ADVPATH_ROOM_GAMESHOW] >= 2)
    {
        weights[ADVPATH_ROOM_GAMESHOW] = 0;
    }

    // Only 1 at once
    if(pathSettings->numOfRooms[ADVPATH_ROOM_DARK_DEAL] >= 1 || pathSettings->numOfRooms[ADVPATH_ROOM_LAB] >= 1)
    {
        weights[ADVPATH_ROOM_DARK_DEAL] = 0;
        weights[ADVPATH_ROOM_LAB] = 0;
    }

    if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_LATERMANNER)
    {
        weights[ADVPATH_ROOM_LEGENDARY] = 0;
        weights[ADVPATH_ROOM_WILD_DEN] = 0;
        weights[ADVPATH_ROOM_LAB] = 0;
    }
}