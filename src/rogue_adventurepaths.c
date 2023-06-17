#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
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

// Bridge refers to horizontal paths
// Ladder refers to vertical
// e.g.
//  \/-- Node/Crossover
//  X ____ <- Bridge
//  |
//  | <- Ladder
//  |


#define MAX_PATH_ROWS ROGUE_MAX_ADVPATH_ROWS
#define MAX_PATH_COLUMNS ROGUE_MAX_ADVPATH_COLUMNS
#define CENTRE_ROW_IDX ((MAX_PATH_ROWS - 1) / 2)

#define PATH_MAP_OFFSET_X (MAP_OFFSET + 1)
#define PATH_MAP_OFFSET_Y (MAP_OFFSET + 1)

#define ROW_WIDTH 3
#define NODE_HEIGHT 2
// Assume CENTRE_NODE_HEIGHT is always 1

#define gSpecialVar_ScriptNodeID        gSpecialVar_0x8004
#define gSpecialVar_ScriptNodeParam0    gSpecialVar_0x8005
#define gSpecialVar_ScriptNodeParam1    gSpecialVar_0x8006

const u16 c_MetaTile_Sign = 0x003;
const u16 c_MetaTile_Grass = 0x001;
const u16 c_MetaTile_Water = 0x170;
const u16 c_MetaTile_Stone = 0x0E2;


// Difficulty rating is from 1-10 (5 being average, 1 easy, 10 hard)
struct AdvEventScratch
{
    u8 roomType;
    u8 nextRoomType;
};

struct AdvMapScratch
{
    bool8 readWriteFlip;
    const struct RogueAdvPathGenerator* generator;
    u8 roomCount[ADVPATH_ROOM_WEIGHT_COUNT];
    struct AdvEventScratch nodesA[MAX_PATH_ROWS];
    struct AdvEventScratch nodesB[MAX_PATH_ROWS];
};

// TODO - Make more generic and usable by any map to save RAM
struct AdvPathLayoutData
{
    u16 objectCount;
    struct ObjectEventTemplate* objects;
};

EWRAM_DATA struct AdvMapScratch* gAdvPathScratch = NULL;
EWRAM_DATA struct AdvPathLayoutData gAdvPathLayoutData = { 0, NULL };

static void AllocAdvPathScratch(void);
static void FreeAdvPathScratch(void);
static void FlipAdvPathNodes(void);
static struct AdvEventScratch* GetScratchReadNode(u16 i);
static struct AdvEventScratch* GetScratchWriteNode(u16 i);

static u16 GetInitialGFXColumn();
static u16 SelectGFXForNode(struct RogueAdvPathNode* nodeInfo, u16 nodeX, u16 nodeY);
static u8 SelectMovementTypeForNode(struct RogueAdvPathNode* nodeInfo, u16 nodeX, u16 nodeY);
static u16 GetTypeForHint(struct RogueAdvPathNode* node, u16 nodeX, u16 nodeY);
static void BufferTypeAdjective(u8 type);

static void AssignWeights_Generator(u8 nodeX, u8 nodeY, u8 columnCount, u16* weights, struct AdvEventScratch* writeNodeScratch);
static void AssignWeights_Standard(u8 nodeX, u8 nodeY, u8 columnCount, u16* weights, struct AdvEventScratch* writeNodeScratch);
static void AssignWeights_Finalize(u8 nodeX, u8 nodeY, u8 columnCount, u16* weights, struct AdvEventScratch* writeNodeScratch);


static void NodeToCoords(u16 nodeX, u16 nodeY, u16* x, u16* y)
{
    *x = nodeX * ROW_WIDTH;
    *y = nodeY * NODE_HEIGHT;
}

static void DisableBridgeMetatiles(u16 nodeX, u16 nodeY)
{
    u16 x, y, i, j;

    NodeToCoords(nodeX, nodeY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X + 1, y + PATH_MAP_OFFSET_Y, c_MetaTile_Grass | MAPGRID_COLLISION_MASK);
    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X + 2, y + PATH_MAP_OFFSET_Y, c_MetaTile_Grass | MAPGRID_COLLISION_MASK);
}

static void DisableLadderMetatiles(u16 nodeX, u16 nodeY)
{
    u16 x, y, i, j;

    NodeToCoords(nodeX, nodeY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X, y + 1 + PATH_MAP_OFFSET_Y, c_MetaTile_Grass | MAPGRID_COLLISION_MASK);
}

static void DisableCrossoverMetatiles(u16 nodeX, u16 nodeY)
{
    u16 x, y, i, j;

    NodeToCoords(nodeX, nodeY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X, y + PATH_MAP_OFFSET_Y, c_MetaTile_Grass | MAPGRID_COLLISION_MASK);
}

static void PlaceStoneMetatile(u16 nodeX, u16 nodeY)
{
    u16 x, y, i, j;

    NodeToCoords(nodeX, nodeY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X + 2, y + PATH_MAP_OFFSET_Y, c_MetaTile_Stone | MAPGRID_COLLISION_MASK);
}

static struct RogueAdvPathNode* GetNodeInfo(u16 x, u16 y)
{
    return &gRogueAdvPath.nodes[y * MAX_PATH_COLUMNS + x];
}

static void ResetNodeInfo()
{
    u16 i;
    for(i = 0; i < ARRAY_COUNT(gRogueAdvPath.nodes); ++i)
    {
        gRogueAdvPath.nodes[i].isBridgeActive = FALSE;
        gRogueAdvPath.nodes[i].isLadderActive = FALSE;
        gRogueAdvPath.nodes[i].roomType = ADVPATH_ROOM_NONE;
        memset(&gRogueAdvPath.nodes[i].roomParams, 0, sizeof(struct RogueAdvPathRoomParams));
    }
}

static void GetBranchingChance(u8 columnIdx, u8 columnCount, u8 roomType, u8* breakChance, u8* extraSplitChance)
{
    *breakChance = 50;
    *extraSplitChance = 10;

    switch(roomType)
    {
        case ADVPATH_ROOM_BOSS:
            if(columnIdx == columnCount - 1)
            {
                *breakChance = 100;
                *extraSplitChance = 10;
            }
            break;

        case ADVPATH_ROOM_NONE:
            *breakChance = 0;
            *extraSplitChance = 0;
            break;

        case ADVPATH_ROOM_RESTSTOP:
            *breakChance = 10;
            *extraSplitChance = 0;
            break;

        case ADVPATH_ROOM_LEGENDARY:
        case ADVPATH_ROOM_DARK_DEAL:
        case ADVPATH_ROOM_LAB:
            *breakChance = 20;
            *extraSplitChance = 50;
            break;
    }

#ifdef ROGUE_DEBUG
    //*breakChance = 0;
    //*extraSplitChance = 0;
#endif
}

static void GenerateAdventureColumnPath(u8 columnIdx, u8 columnCount)
{
    u8 i;
    u8 breakChance;
    u8 extraChance;
    u8 nextRoomType;
    struct RogueAdvPathNode* nextNodeInfo;
    struct RogueAdvPathNode* nodeInfo;

    for(i = 0; i < MAX_PATH_ROWS; ++i)
    {
        // This has already been decided
        nextNodeInfo = GetNodeInfo(columnIdx + 1, i);

        if(nextNodeInfo->isBridgeActive)
        {
            // Calculate the split chance
            if(columnIdx >= columnCount - 2) // First column before boss will be empty purely to give branches extra width
            {
                nextRoomType = ADVPATH_ROOM_BOSS;
            }
            else 
            {
                nextRoomType = nextNodeInfo->roomType;
            }

            // Calculate the split chance
            GetBranchingChance(columnIdx, columnCount, nextRoomType, &breakChance, &extraChance);

            if(RogueRandomChance(breakChance, OVERWORLD_FLAG))
            {
                if(i == 0)
                {
                    // Split
                    // ==|==
                    //   |
                    // ==|
                    nodeInfo = GetNodeInfo(columnIdx, i);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i)->nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i + 1)->nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx + 1, i);
                    nodeInfo->isLadderActive = TRUE;
                }
                else if(i == MAX_PATH_ROWS - 1)
                {
                    // Split
                    // ==|
                    //   |
                    // ==|==

                    nodeInfo = GetNodeInfo(columnIdx, i);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i)->nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i - 1)->nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx + 1, i - 1);
                    nodeInfo->isLadderActive = TRUE;
                }
                else
                {
                    // Split
                    // ==|
                    //   |
                    //   |==
                    //   |
                    // ==|
                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i - 1)->nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i + 1)->nextRoomType = nextRoomType;

                    // 3rd bridge might appear on occasion
                    if(RogueRandomChance(extraChance, OVERWORLD_FLAG))
                    {
                        nodeInfo = GetNodeInfo(columnIdx, i);
                        nodeInfo->isBridgeActive = TRUE;
                        GetScratchWriteNode(i)->nextRoomType = nextRoomType;
                    }

                    nodeInfo = GetNodeInfo(columnIdx + 1, i - 1);
                    nodeInfo->isLadderActive = TRUE;
                    
                    nodeInfo = GetNodeInfo(columnIdx + 1, i);
                    nodeInfo->isLadderActive = TRUE;

                }
            }
            else
            {
                u8 offset = 1;

                // Move path up or down but don't split
                if(RogueRandomChance(33, OVERWORLD_FLAG))
                {
                    if(RogueRandomChance(50, OVERWORLD_FLAG))
                    {
                        if(i != 0)
                            --offset;
                    }
                    else
                    {
                        if(i != MAX_PATH_ROWS - 1)
                            ++offset;
                    }
                }

                // Above
                if(offset == 0)
                {
                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i - 1)->nextRoomType = nextRoomType;
                    
                    nodeInfo = GetNodeInfo(columnIdx + 1, i - 1);
                    nodeInfo->isLadderActive = TRUE;
                }
                // Below
                else if(offset == 2)
                {
                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i + 1)->nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx + 1, i);
                    nodeInfo->isLadderActive = TRUE;
                }
                // Aligned
                else
                {
                    nodeInfo = GetNodeInfo(columnIdx, i);
                    nodeInfo->isBridgeActive = TRUE;
                    GetScratchWriteNode(i)->nextRoomType = nextRoomType;
                }
            }
        }
    }
}

static u16 SelectIndexFromWeights(u16* weights, u16 count)
{
    u16 totalWeight;
    u16 targetWeight;
    u8 i;

    totalWeight = 0;
    for(i = 0; i < count; ++i)
    {
        totalWeight += weights[i];
    }

    targetWeight = RogueRandomRange(totalWeight, OVERWORLD_FLAG);
    totalWeight = 0;

    for(i = 0; i < count; ++i)
    {
        totalWeight += weights[i];

        if(targetWeight <= totalWeight)
        {
            return i;
        }
    }

    return 0;
}

static void ChooseNewEvent(u8 nodeX, u8 nodeY, u8 columnCount)
{
    u16 weights[ADVPATH_ROOM_WEIGHT_COUNT];
    struct AdvEventScratch* writeNodeScratch = GetScratchWriteNode(nodeY);

    if(nodeX == columnCount - 1)
    {
        // This column is purely to allow for larger branches
        writeNodeScratch->roomType = ADVPATH_ROOM_NONE;
        return;
    }

    // New idea??
    if(FALSE)
    {
        AssignWeights_Generator(nodeX, nodeY, columnCount, &weights[0], writeNodeScratch);
    }
    else
    {
        // TODO - Support gauntlet

        AssignWeights_Standard(nodeX, nodeY, columnCount, &weights[0], writeNodeScratch);
        AssignWeights_Finalize(nodeX, nodeY, columnCount, &weights[0], writeNodeScratch);
    }

    writeNodeScratch->roomType = SelectIndexFromWeights(weights, ARRAY_COUNT(weights));
    
    if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_MINIBOSS_BATTLER)
    {
        if(writeNodeScratch->roomType == ADVPATH_ROOM_ROUTE)
            writeNodeScratch->roomType = ADVPATH_ROOM_MINIBOSS;
    }

#ifdef ROGUE_DEBUG
    //if(writeNodeScratch->roomType == ADVPATH_ROOM_ROUTE)
    //    writeNodeScratch->roomType = ADVPATH_ROOM_RESTSTOP;
#endif
}

static void CreateEventParams(u16 nodeX, u16 nodeY, struct RogueAdvPathNode* nodeInfo)
{
    u16 weights[ADVPATH_SUBROOM_WEIGHT_COUNT];

    memset(weights, 0, sizeof(weights));

    switch(nodeInfo->roomType)
    {
        case ADVPATH_ROOM_RESTSTOP:
            weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_RESTSTOP_BATTLE];
            weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_RESTSTOP_SHOP];
            weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_RESTSTOP_FULL];

            nodeInfo->roomParams.roomIdx = SelectIndexFromWeights(weights, ARRAY_COUNT(weights)) - ADVPATH_SUBROOM_RESTSTOP_BATTLE;
            break;

        case ADVPATH_ROOM_LEGENDARY:
            nodeInfo->roomParams.roomIdx = Rogue_SelectLegendaryEncounterRoom();
            nodeInfo->roomParams.perType.legendary.shinyState = RogueRandomRange(Rogue_GetShinyOdds(), OVERWORLD_FLAG) == 0;
            break;

        case ADVPATH_ROOM_MINIBOSS:
            nodeInfo->roomParams.roomIdx = 0;
            nodeInfo->roomParams.perType.miniboss.trainerNum = Rogue_NextMinibossTrainerId();
            break;

        case ADVPATH_ROOM_WILD_DEN:
            nodeInfo->roomParams.roomIdx = 0;
            nodeInfo->roomParams.perType.wildDen.species = Rogue_SelectWildDenEncounterRoom();
            nodeInfo->roomParams.perType.wildDen.shinyState = RogueRandomRange(Rogue_GetShinyOdds(), OVERWORLD_FLAG) == 0;
            break;

        case ADVPATH_ROOM_ROUTE:
        {
            nodeInfo->roomParams.roomIdx = Rogue_SelectRouteRoom();

            weights[ADVPATH_SUBROOM_ROUTE_CALM] = gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_ROUTE_CALM];
            weights[ADVPATH_SUBROOM_ROUTE_AVERAGE] = gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_ROUTE_AVERAGE];
            weights[ADVPATH_SUBROOM_ROUTE_TOUGH] = gAdvPathScratch->generator->subRoomWeights[ADVPATH_SUBROOM_ROUTE_TOUGH];

            nodeInfo->roomParams.perType.route.difficulty = SelectIndexFromWeights(weights, ARRAY_COUNT(weights)) - ADVPATH_SUBROOM_ROUTE_CALM;
            break;
        }
    }
}

static void GenerateAdventureColumnEvents(u8 columnIdx, u8 columnCount)
{
    struct RogueAdvPathNode* nodeInfo;
    u8 i;

    for(i = 0; i < MAX_PATH_ROWS; ++i)
    {
        nodeInfo = GetNodeInfo(columnIdx, i);
        if(nodeInfo->isBridgeActive)
        {
            ChooseNewEvent(columnIdx, i, columnCount);

            // Post event choose
            nodeInfo->roomType = GetScratchWriteNode(i)->roomType;
            ++gAdvPathScratch->roomCount[nodeInfo->roomType];

            CreateEventParams(columnIdx, i, nodeInfo);
        }
    }
}

static void SetRogueSeedForPath()
{
    gRngRogueValue = Rogue_GetStartSeed() + (u32)gRogueRun.currentDifficulty * 31;
}

static void SetRogueSeedForNode()
{
    SetRogueSeedForPath();
    gRngRogueValue += (u32)gRogueAdvPath.currentNodeX * 71 + (u32)gRogueAdvPath.currentNodeY * 21;
}

bool8 RogueAdv_GenerateAdventurePathsIfRequired()
{
    struct RogueAdvPathNode* nodeInfo;
    struct RogueAdventurePhase* adventurePhase;
    u8 i;
    u8 totalDistance;
    u8 minY, maxY;

    if(gRogueAdvPath.currentNodeX < gRogueAdvPath.currentColumnCount)
    {
        // Path is still valid
        return FALSE;
    }

    AllocAdvPathScratch();
    gAdvPathScratch->generator = &Rogue_GetAdventurePhase()->pathGenerator;

    SetRogueSeedForPath();

    ResetNodeInfo();

    // Setup defaults
    totalDistance = 1 + gAdvPathScratch->generator->minLength + RogueRandomRange(gAdvPathScratch->generator->maxLength - gAdvPathScratch->generator->minLength + 1, OVERWORLD_FLAG);

    // Exit node
    nodeInfo = GetNodeInfo(totalDistance, CENTRE_ROW_IDX);
    nodeInfo->isBridgeActive = TRUE;
    nodeInfo->roomType = ADVPATH_ROOM_BOSS;
    nodeInfo->roomParams.roomIdx = 0;
    nodeInfo->roomParams.perType.boss.trainerNum = Rogue_NextBossTrainerId();

    for(i = 0; i < totalDistance; ++i)
    {
        FlipAdvPathNodes();
        GenerateAdventureColumnPath(totalDistance - i - 1, totalDistance);
        GenerateAdventureColumnEvents(totalDistance - i - 1, totalDistance);
    }

    FreeAdvPathScratch();

    minY = MAX_PATH_ROWS;
    maxY = 0;

    for(i = 0; i < MAX_PATH_ROWS; ++i)
    {
        nodeInfo = GetNodeInfo(0, i);
        if(nodeInfo->isBridgeActive)
        {
            minY = min(minY, i);
            maxY = max(maxY, i);
        }
    }
    
    // Activate all the ladders for first column, as we'll want to run along this to choose a path
    for(i = minY; i < maxY; ++i)
    {
        nodeInfo = GetNodeInfo(0, i);
        nodeInfo->isLadderActive = TRUE;
    }

    // Place in centre of options
    gRogueAdvPath.currentColumnCount = totalDistance;
    gRogueAdvPath.currentNodeX = 0;
    gRogueAdvPath.currentNodeY = (minY + maxY) / 2;
    return TRUE;
}

void RogueAdv_ApplyAdventureMetatiles()
{
    u8 x, y;
    struct RogueAdvPathNode* nodeInfo;
    struct RogueAdvPathNode* prevNodeInfo;

    for(x = 0; x < MAX_PATH_COLUMNS; ++x)
    for(y = 0; y < MAX_PATH_ROWS; ++y)
    {
        nodeInfo = GetNodeInfo(x, y);
        if(!nodeInfo->isBridgeActive)
        {
            DisableBridgeMetatiles(x, y);
        }
        else
        {
            if(GetInitialGFXColumn() == x + 1)
            {
                PlaceStoneMetatile(x, y);
            }
        }

        if(nodeInfo->roomType != ADVPATH_ROOM_NONE && x >= GetInitialGFXColumn())
        {
            PlaceStoneMetatile(x, y);
        }

        if(!nodeInfo->isLadderActive)
        {
            DisableLadderMetatiles(x, y);
        }
        
        if(!nodeInfo->isBridgeActive && !nodeInfo->isLadderActive)
        {
            if(x == 0)
            {
                DisableCrossoverMetatiles(x, y);
            }
            else
            {
                prevNodeInfo = GetNodeInfo(x - 1, y);

                if(!prevNodeInfo->isBridgeActive)
                {
                    DisableCrossoverMetatiles(x, y);
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
    struct RogueAdvPathNode* node = GetNodeInfo(gRogueAdvPath.currentNodeX, gRogueAdvPath.currentNodeY);

    if(node)
    {
        SetRogueSeedForNode();

        switch(node->roomType)
        {
            case ADVPATH_ROOM_BOSS:
                SetBossRoomWarp(node->roomParams.perType.boss.trainerNum, warp);
                break;

            case ADVPATH_ROOM_RESTSTOP:
                warp->mapGroup = gRogueRestStopEncounterInfo.mapTable[node->roomParams.roomIdx].group;
                warp->mapNum = gRogueRestStopEncounterInfo.mapTable[node->roomParams.roomIdx].num;
                break;

            case ADVPATH_ROOM_ROUTE:
                warp->mapGroup = gRogueRouteTable.routes[node->roomParams.roomIdx].map.group;
                warp->mapNum = gRogueRouteTable.routes[node->roomParams.roomIdx].map.num;
                break;

            case ADVPATH_ROOM_LEGENDARY:
                warp->mapGroup = gRogueLegendaryEncounterInfo.mapTable[node->roomParams.roomIdx].group;
                warp->mapNum = gRogueLegendaryEncounterInfo.mapTable[node->roomParams.roomIdx].num;
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
        
        // Now we've interacted hide this node
        //node->roomType = ADVPATH_ROOM_NONE;
    }
}

u8 RogueAdv_OverrideNextWarp(struct WarpData *warp)
{
    if(gAdvPathLayoutData.objects != NULL)
    {
        free(gAdvPathLayoutData.objects);
        gAdvPathLayoutData.objectCount = 0;
        gAdvPathLayoutData.objects = NULL;
    }

    // Should already be set correctly for RogueAdv_ExecuteNodeAction
    if(!gRogueAdvPath.isOverviewActive)
    {
        u16 x, y;

        bool8 freshPath = RogueAdv_GenerateAdventurePathsIfRequired();
        gRogueAdvPath.justGenerated = freshPath;

        NodeToCoords(gRogueAdvPath.currentNodeX, gRogueAdvPath.currentNodeY, &x, &y);

        // Always jump back to overview screen, after a different route
        warp->mapGroup = MAP_GROUP(ROGUE_ADVENTURE_PATHS);
        warp->mapNum = MAP_NUM(ROGUE_ADVENTURE_PATHS);
        warp->warpId = WARP_ID_NONE;
        warp->x = x + (freshPath ? 1 : 4);
        warp->y = y + 1;

        gRogueAdvPath.currentRoomType = ADVPATH_ROOM_NONE;
        return ROGUE_WARP_TO_ADVPATH;
    }
    else
    {
        ApplyCurrentNodeWarp(warp);
        return ROGUE_WARP_TO_ROOM;
    }
}

extern const u8 Rogue_AdventurePaths_InteractNode[];

void RogueAdv_ModifyObjectEvents(struct MapHeader *mapHeader, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity)
{
    struct RogueAdvPathNode* nodeInfo;
    u16 i, x, y, mapX, mapY;
    u16 encounterCount = 0;

    for(i = 0; i < ARRAY_COUNT(gRogueAdvPath.nodes); ++i)
    {
        nodeInfo = &gRogueAdvPath.nodes[i];
        if(nodeInfo->roomType != ADVPATH_ROOM_NONE)
        {
            x = i % MAX_PATH_COLUMNS;
            y = i / MAX_PATH_COLUMNS;
            NodeToCoords(x, y, &mapX, &mapY);

            if(x >= GetInitialGFXColumn())
            {
                u16 idx = encounterCount++;
                if(idx < objectEventCapacity)
                {
                    objectEvents[idx].localId = idx;
                    objectEvents[idx].graphicsId = SelectGFXForNode(nodeInfo, x, y);
                    objectEvents[idx].x = mapX + 2;//+ MAP_OFFSET;
                    objectEvents[idx].y = mapY + 1;//+ MAP_OFFSET;
                    objectEvents[idx].elevation = 3;
                    objectEvents[idx].trainerType = TRAINER_TYPE_NONE;
                    objectEvents[idx].movementType = SelectMovementTypeForNode(nodeInfo, x, y);
                    // Pack node into movement vars
                    objectEvents[idx].movementRangeX = x;
                    objectEvents[idx].movementRangeY = y;
                    objectEvents[idx].script = Rogue_AdventurePaths_InteractNode;
                }
                else
                {
                    DebugPrintf("WARNING: Cannot add adventure path object %d (out of range %d)", idx, objectEventCapacity);
                }
            }
        }
    }

    *objectEventCount = encounterCount;
}

bool8 RogueAdv_CanUseEscapeRope(void)
{
    if(!gRogueAdvPath.isOverviewActive)
    {
        // We are in transition i.e. just started the run
        if(gRogueAdvPath.currentColumnCount == 0)
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

static u16 SelectGFXForNode(struct RogueAdvPathNode* nodeInfo, u16 nodeX, u16 nodeY)
{
    switch(nodeInfo->roomType)
    {
        case ADVPATH_ROOM_NONE:
            return 0;
            
        case ADVPATH_ROOM_ROUTE:
        {
            switch(GetTypeForHint(nodeInfo, nodeX, nodeY))
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
            return gRogueRestStopEncounterInfo.mapTable[nodeInfo->roomParams.roomIdx].encounterId;

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

static u8 SelectMovementTypeForNode(struct RogueAdvPathNode* nodeInfo, u16 nodeX, u16 nodeY)
{
    switch(nodeInfo->roomType)
    {
        case ADVPATH_ROOM_ROUTE:
        {
            switch(nodeInfo->roomParams.perType.route.difficulty)
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

static u16 GetInitialGFXColumn()
{
    if(gRogueAdvPath.justGenerated)
        return 0;
    else
        return gRogueAdvPath.currentNodeX + 1;
}

static u16 GetTypeForHint(struct RogueAdvPathNode* node, u16 nodeX, u16 nodeY)
{
    return gRogueRouteTable.routes[node->roomParams.roomIdx].wildTypeTable[(nodeX + nodeY) % 3];
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

static struct RogueAdvPathNode* GetScriptInteractNode(u16* outX, u16* outY)
{
    struct RogueAdvPathNode* node; 
    u16 lastTalkedId = VarGet(VAR_LAST_TALKED);

    *outX = gSaveBlock1Ptr->objectEventTemplates[lastTalkedId].movementRangeX;
    *outY = gSaveBlock1Ptr->objectEventTemplates[lastTalkedId].movementRangeY;
    return GetNodeInfo(*outX, *outY);
}

void RogueAdv_GetNodeParams()
{
    u16 nodeX, nodeY;
    struct RogueAdvPathNode* node = GetScriptInteractNode(&nodeX, &nodeY);

    if(node)
    {
        gSpecialVar_ScriptNodeParam0 = node->roomType;
        gSpecialVar_ScriptNodeParam1 = node->roomParams.roomIdx;

        switch(node->roomType)
        {
            case ADVPATH_ROOM_ROUTE:
                gSpecialVar_ScriptNodeParam1 = node->roomParams.perType.route.difficulty;
                BufferTypeAdjective(GetTypeForHint(node, nodeX, nodeY));
                break;
        }
    }
    else
    {
        gSpecialVar_ScriptNodeParam0 = (u16)-1;
        gSpecialVar_ScriptNodeParam1 = (u16)-1;
    }
}

void RogueAdv_ExecuteNodeAction()
{
    struct RogueAdvPathNode* node; 
    struct WarpData warp;
    u16 x, y;
    
    node = GetScriptInteractNode(&x, &y);

    // Fill with dud warp
    warp.mapGroup = MAP_GROUP(ROGUE_HUB_TRANSITION);
    warp.mapNum = MAP_NUM(ROGUE_HUB_TRANSITION);
    warp.warpId = 0;
    warp.x = -1;
    warp.y = -1;

    if(node)
    {
        // Move to the selected node
        gRogueAdvPath.currentNodeX = x;
        gRogueAdvPath.currentNodeY = y;
        gRogueAdvPath.currentRoomType = node->roomType;
        memcpy(&gRogueAdvPath.currentRoomParams, &node->roomParams, sizeof(struct RogueAdvPathRoomParams));
    }

    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
}

void RogueAdv_DebugExecuteRandomNextNode()
{
#ifdef ROGUE_DEBUG
    //u16 i;
    //struct RogueAdvPathNode* node;
//
    //u16 nodeX, nodeY;
//
    //for(i = 0; i < 100; ++i)
    //{
    //    VarSet(gSpecialVar_ScriptNodeID, Random() % 16);
//
    //    // Look for encounter in next column (Don't care if these can actually connect or not)
    //    node = GetScriptNodeWithCoords(&nodeX, &nodeY);
    //    if(node && nodeX == GetInitialGFXColumn() && node->roomType != ADVPATH_ROOM_NONE)
    //    {
    //        RogueAdv_ExecuteNodeAction();
    //        return;
    //    }
    //}
//
    //// Failed so fallback to next gym warp
    //for(i = 0; i < 16; ++i)
    //{
    //    VarSet(gSpecialVar_ScriptNodeID, i);
//
    //    node = GetScriptNodeWithCoords(&nodeX, &nodeY);
    //    if(node && node->roomType == ADVPATH_ROOM_BOSS)
    //    {
    //        RogueAdv_ExecuteNodeAction();
    //        return;
    //    }
    //}
//
    //VarSet(gSpecialVar_ScriptNodeID, 0);
    //RogueAdv_ExecuteNodeAction();
#endif
}

static void AllocAdvPathScratch(void)
{
    AGB_ASSERT(gAdvPathScratch == NULL);
    gAdvPathScratch = AllocZeroed(sizeof(struct AdvMapScratch));
}

static void FreeAdvPathScratch(void)
{
    AGB_ASSERT(gAdvPathScratch != NULL);
    free(gAdvPathScratch);
    gAdvPathScratch = NULL;
}

static void FlipAdvPathNodes(void)
{
    AGB_ASSERT(gAdvPathScratch != NULL);

    gAdvPathScratch->readWriteFlip = !gAdvPathScratch->readWriteFlip;

    memset(GetScratchWriteNode(0), 0, sizeof(gAdvPathScratch->nodesA));
}

static struct AdvEventScratch* GetScratchReadNode(u16 i)
{
    AGB_ASSERT(i < ARRAY_COUNT(gAdvPathScratch->nodesA));

    if(gAdvPathScratch->readWriteFlip)
        return &gAdvPathScratch->nodesA[i];
    else
        return &gAdvPathScratch->nodesB[i];
}

static struct AdvEventScratch* GetScratchWriteNode(u16 i)
{
    AGB_ASSERT(i < ARRAY_COUNT(gAdvPathScratch->nodesA));

    if(!gAdvPathScratch->readWriteFlip)
        return &gAdvPathScratch->nodesA[i];
    else
        return &gAdvPathScratch->nodesB[i];
}

static void AssignWeights_Generator(u8 nodeX, u8 nodeY, u8 columnCount, u16* weights, struct AdvEventScratch* writeNodeScratch)
{
    u16 i;

    // Copy generators initial weights
    memcpy(weights, gAdvPathScratch->generator->roomWeights, sizeof(weights));

    // TODO - Adjust weights
    for(i = 0; i < ADVPATH_ROOM_WEIGHT_COUNT; ++i)
    {
        // We can have as many rooms as we want for this type
        if(gAdvPathScratch->generator->maxRoomCount[i] == 0)
            continue;

        // We've reached capacity
        if(gAdvPathScratch->roomCount[i] >= gAdvPathScratch->generator->maxRoomCount[i])
            weights[i] = 0;
    }

    // We can't have 2 empties in a row
    if(writeNodeScratch->nextRoomType == ADVPATH_ROOM_NONE)
    {
        weights[ADVPATH_ROOM_NONE] = 0;
    }
}

static void AssignWeights_Standard(u8 nodeX, u8 nodeY, u8 columnCount, u16* weights, struct AdvEventScratch* writeNodeScratch)
{
    u16 i;

    // Default weight
    memset(weights, 500, sizeof(weights));

    // Normal routes
    if(writeNodeScratch->nextRoomType == ADVPATH_ROOM_BOSS)
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

    // NONE / Skip encounters
    if(nodeX == columnCount - 2) 
    {
        // Final encounter cannot be none, to avoid GFX obj running out
        weights[ADVPATH_ROOM_NONE] = 0;
    }
    else
    {
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
    }

    // Rest stops
    if(writeNodeScratch->nextRoomType == ADVPATH_ROOM_BOSS)
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

    if(nodeX == 0)
    {
        // Impossible in first column
        weights[ADVPATH_ROOM_LEGENDARY] = 0;
    }

    // Less likely in first column and/or last
    if(nodeX == 0)
    {
        weights[ADVPATH_ROOM_MINIBOSS] /= 2;
        weights[ADVPATH_ROOM_LEGENDARY] /= 2;
        weights[ADVPATH_ROOM_WILD_DEN] /= 2;
    }
    if(writeNodeScratch->nextRoomType == ADVPATH_ROOM_BOSS)
    {
        weights[ADVPATH_ROOM_MINIBOSS] /= 3;
        weights[ADVPATH_ROOM_LEGENDARY] /= 2;
        weights[ADVPATH_ROOM_WILD_DEN] /= 3;
        weights[ADVPATH_ROOM_GAMESHOW] /= 4;
        weights[ADVPATH_ROOM_DARK_DEAL] /= 4;
        weights[ADVPATH_ROOM_LAB] /= 2;
    }

    // Now we've applied the default weights for this column, consider what out next encounter is
    switch(writeNodeScratch->nextRoomType)
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

static void AssignWeights_Finalize(u8 nodeX, u8 nodeY, u8 columnCount, u16* weights, struct AdvEventScratch* writeNodeScratch)
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
        if(gAdvPathScratch->roomCount[ADVPATH_ROOM_LEGENDARY] >= 2)
        {
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
        }
        break;

    default:
        if(gAdvPathScratch->roomCount[ADVPATH_ROOM_LEGENDARY] >= 1)
        {
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
        }
        break;
    }


    if(gAdvPathScratch->roomCount[ADVPATH_ROOM_WILD_DEN] >= 2)
    {
        weights[ADVPATH_ROOM_WILD_DEN] = 0;
    }

    if(gAdvPathScratch->roomCount[ADVPATH_ROOM_MINIBOSS] >= 2)
    {
        weights[ADVPATH_ROOM_MINIBOSS] = 0;
    }

    if(gAdvPathScratch->roomCount[ADVPATH_ROOM_GAMESHOW] >= 2)
    {
        weights[ADVPATH_ROOM_GAMESHOW] = 0;
    }

    // Only 1 at once
    if(gAdvPathScratch->roomCount[ADVPATH_ROOM_DARK_DEAL] >= 1 || gAdvPathScratch->roomCount[ADVPATH_ROOM_LAB] >= 1)
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