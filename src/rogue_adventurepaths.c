#include "global.h"
#include "constants/event_objects.h"
#include "constants/rogue.h"
#include "event_data.h"
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

// Difficulty rating is from 1-10 (5 being average, 1 easy, 10 hard)
struct AdvEventScratch
{
    u8 roomType;
    u8 nextRoomType;
};

struct AdvMapScratch
{
    u8 legendaryCount;
    u8 wildDenCount;
    u8 gameShowCount;
    u8 graveYardCount;
    u8 minibossCount;
    u8 labCount;
    struct AdvEventScratch* readNodes;
    struct AdvEventScratch* writeNodes;
};

static void GetBranchingChance(u8 columnIdx, u8 columnCount, u8 roomType, u8* breakChance, u8* extraSplitChance)
{
    *breakChance = 0;
    *extraSplitChance = 0;

    switch(roomType)
    {
        case ADVPATH_ROOM_BOSS:
            if(columnIdx == columnCount - 1)
            {
                *breakChance = 100;
                *extraSplitChance = 10;
            }
            else
            {
                *breakChance = 100;
                *extraSplitChance = gRogueRun.currentDifficulty < 8 ? 90 : 50;
            }
            break;

        case ADVPATH_ROOM_NONE:
            *breakChance = 5;
            break;

        case ADVPATH_ROOM_ROUTE:
            if(columnIdx >= columnCount - 2)
            {
                *breakChance = 50;
                *extraSplitChance = 34;
            }
            else
            {
                *breakChance = 20;
                *extraSplitChance = 20;
            }
            break;

        case ADVPATH_ROOM_RESTSTOP:
            *breakChance = 5;
            *extraSplitChance = 0;
            break;

        case ADVPATH_ROOM_LEGENDARY:
            *breakChance = 0;
            *extraSplitChance = 50;
            break;

        case ADVPATH_ROOM_MINIBOSS:
        case ADVPATH_ROOM_WILD_DEN:
        case ADVPATH_ROOM_GAMESHOW:
        case ADVPATH_ROOM_GRAVEYARD:
            *breakChance = 2;
            *extraSplitChance = 50;
            break;

        case ADVPATH_ROOM_LAB:
            *breakChance = 2;
            *extraSplitChance = 0;
            break;
    }

#ifdef ROGUE_DEBUG
    //*breakChance = 0;
    //*extraSplitChance = 0;
#endif
}

static void GenerateAdventureColumnPath(u8 columnIdx, u8 columnCount, struct AdvMapScratch* scratch)
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
                    scratch->writeNodes[i].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    scratch->writeNodes[i + 1].nextRoomType = nextRoomType;

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
                    scratch->writeNodes[i].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isBridgeActive = TRUE;
                    scratch->writeNodes[i - 1].nextRoomType = nextRoomType;

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
                    scratch->writeNodes[i - 1].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    scratch->writeNodes[i + 1].nextRoomType = nextRoomType;

                    // 3rd bridge might appear on occasion
                    if(RogueRandomChance(extraChance, OVERWORLD_FLAG))
                    {
                        nodeInfo = GetNodeInfo(columnIdx, i);
                        nodeInfo->isBridgeActive = TRUE;
                        scratch->writeNodes[i].nextRoomType = nextRoomType;
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
                    scratch->writeNodes[i - 1].nextRoomType = nextRoomType;
                    
                    nodeInfo = GetNodeInfo(columnIdx + 1, i - 1);
                    nodeInfo->isLadderActive = TRUE;
                }
                // Below
                else if(offset == 2)
                {
                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    scratch->writeNodes[i + 1].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx + 1, i);
                    nodeInfo->isLadderActive = TRUE;
                }
                // Aligned
                else
                {
                    nodeInfo = GetNodeInfo(columnIdx, i);
                    nodeInfo->isBridgeActive = TRUE;
                    scratch->writeNodes[i].nextRoomType = nextRoomType;
                }
            }
        }
    }

}

static void ChooseNewEvent(u8 nodeX, u8 nodeY, u8 columnCount, struct AdvMapScratch* scratch, struct AdvEventScratch* prevScratch, struct AdvEventScratch* currScratch)
{
    u16 weights[ADVPATH_ROOM_COUNT];
    u16 totalWeight;
    u16 targetWeight;
    u8 i;

    // 500 is default weight
    memset(&weights[0], 500, sizeof(u16) * ARRAY_COUNT(weights));

    if(nodeX == columnCount - 1)
    {
        // This column is purely to allow for larger branches
        currScratch->roomType = ADVPATH_ROOM_NONE;
        return;
    }


    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        // Turn everything off
        memset(&weights[0], 0, sizeof(u16) * ARRAY_COUNT(weights));

        // We should only be here for the first loop, as we should have no other encounters
        if(currScratch->nextRoomType == ADVPATH_ROOM_BOSS)
        {
            weights[ADVPATH_ROOM_RESTSTOP] = 1500;
        }
        else
        {
            weights[ADVPATH_ROOM_ROUTE] = 1500;
            weights[ADVPATH_ROOM_LEGENDARY] = 200;
            weights[ADVPATH_ROOM_WILD_DEN] = 250;
            weights[ADVPATH_ROOM_GAMESHOW] = 70;
            weights[ADVPATH_ROOM_GRAVEYARD] = 70;
            weights[ADVPATH_ROOM_LAB] = 0;
        }
    }
    else
    {
        // Normal routes
        if(currScratch->nextRoomType == ADVPATH_ROOM_BOSS)
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
        if(currScratch->nextRoomType == ADVPATH_ROOM_BOSS)
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
            weights[ADVPATH_ROOM_GRAVEYARD] = 0;
            weights[ADVPATH_ROOM_LAB] = 0;
        }
        else
        {
            weights[ADVPATH_ROOM_MINIBOSS] = min(30 * gRogueRun.currentDifficulty, 700);
            weights[ADVPATH_ROOM_WILD_DEN] = min(25 * gRogueRun.currentDifficulty, 400);
            weights[ADVPATH_ROOM_LAB] = min(20 * gRogueRun.currentDifficulty, 60);

            // These should start trading with each other deeper into the run
            if(gRogueRun.currentDifficulty < 6)
            {
                weights[ADVPATH_ROOM_GAMESHOW] = 320 - 40 * min(8, gRogueRun.currentDifficulty);
                weights[ADVPATH_ROOM_GRAVEYARD] = 10;
            }
            else
            {
                weights[ADVPATH_ROOM_GAMESHOW] = 10;
                weights[ADVPATH_ROOM_GRAVEYARD] = 330 - 30 * min(5, gRogueRun.currentDifficulty - 6);
            }

            if(FlagGet(FLAG_ROGUE_EASY_LEGENDARIES))
            {
                if((gRogueRun.currentDifficulty % 4) == 0)
                    // Every 4 badges chances get really high
                    weights[ADVPATH_ROOM_LEGENDARY] = 600;
                else
                    // Otherwise the chances are just quite low
                    weights[ADVPATH_ROOM_LEGENDARY] = 100;
            }
            else if(FlagGet(FLAG_ROGUE_HARD_LEGENDARIES))
            {
                if((gRogueRun.currentDifficulty % 5) == 0)
                    // Every 5 badges chances get really high
                    weights[ADVPATH_ROOM_LEGENDARY] = 800;
                else
                    // Otherwise impossible
                    weights[ADVPATH_ROOM_LEGENDARY] = 0;
            }
            else 
            {
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
        if(currScratch->nextRoomType == ADVPATH_ROOM_BOSS)
        {
            weights[ADVPATH_ROOM_MINIBOSS] /= 3;
            weights[ADVPATH_ROOM_LEGENDARY] /= 2;
            weights[ADVPATH_ROOM_WILD_DEN] /= 3;
            weights[ADVPATH_ROOM_GAMESHOW] /= 4;
            weights[ADVPATH_ROOM_GRAVEYARD] /= 4;
            weights[ADVPATH_ROOM_LAB] /= 2;
        }

        // Now we've applied the default weights for this column, consider what out next encounter is
        switch(currScratch->nextRoomType)
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
                weights[ADVPATH_ROOM_GRAVEYARD] *= 2;
                weights[ADVPATH_ROOM_LAB] *= 2;
                break;

            case ADVPATH_ROOM_GAMESHOW:
                weights[ADVPATH_ROOM_GAMESHOW] = 0;
                break;

            case ADVPATH_ROOM_GRAVEYARD:
                weights[ADVPATH_ROOM_GRAVEYARD] = 0;
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

    // We have limited number of certain encounters
    if(FlagGet(FLAG_ROGUE_EASY_LEGENDARIES))
    {
        if(scratch->legendaryCount >= 2)
        {
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
        }
    }
    else
    {
        if(scratch->legendaryCount >= 1)
        {
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
        }
    }

    if(scratch->wildDenCount >= 2)
    {
        weights[ADVPATH_ROOM_WILD_DEN] = 0;
    }

    if(scratch->minibossCount >= 2)
    {
        weights[ADVPATH_ROOM_MINIBOSS] = 0;
    }

    if(scratch->gameShowCount >= 2)
    {
        weights[ADVPATH_ROOM_GAMESHOW] = 0;
    }

    if(scratch->graveYardCount >= 1)
    {
        weights[ADVPATH_ROOM_GRAVEYARD] = 0;
    }

    if(scratch->labCount >= 1)
    {
        weights[ADVPATH_ROOM_LAB] = 0;
    }

    totalWeight = 0;
    for(i = 0; i < ADVPATH_ROOM_COUNT; ++i)
    {
        totalWeight += weights[i];
    }

    targetWeight = 1 + RogueRandomRange(totalWeight, OVERWORLD_FLAG);
    totalWeight = 0;

    for(i = 0; i < ADVPATH_ROOM_COUNT; ++i)
    {
        totalWeight += weights[i];

        if(targetWeight <= totalWeight)
        {
            // Found the room we want
            currScratch->roomType = i;
            break;
        }
    }

#ifdef ROGUE_DEBUG
    //if(currScratch->roomType == ADVPATH_ROOM_ROUTE)
    //    currScratch->roomType = ADVPATH_ROOM_RESTSTOP;
#endif
}

static void CreateEventParams(u16 nodeX, u16 nodeY, struct RogueAdvPathNode* nodeInfo, struct AdvMapScratch* scratch)
{
    u16 temp;

    switch(nodeInfo->roomType)
    {
        // Handled below as this is a special case
        //case ADVPATH_ROOM_BOSS:

        case ADVPATH_ROOM_RESTSTOP:
            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                // Always full rest stop
                nodeInfo->roomParams.roomIdx = 0; // Heals
            }
            else
            {
                temp = RogueRandomRange(7, OVERWORLD_FLAG);
                
                if(gRogueRun.currentDifficulty >= 12)
                {
                    // Always always a full rest stop
                    if(temp == 0)
                        nodeInfo->roomParams.roomIdx = 1; // Shops
                    else if(temp == 1)
                        nodeInfo->roomParams.roomIdx = 2; // Battle prep.
                    else
                        nodeInfo->roomParams.roomIdx = 0; // Heals
                }
                else if(temp == 0)
                {
                    // Small chance for full rest stop
                    nodeInfo->roomParams.roomIdx = 0; // Heals
                }
                else
                {
                    // We want to ping pong the options rather then have them appear in the same order
                    if(nodeY % 2 == 0)
                    {
                        if(temp <= 2)
                            nodeInfo->roomParams.roomIdx = 1; // Shops
                        else
                            nodeInfo->roomParams.roomIdx = 2; // Battle prep.
                    }
                    else
                    {
                        if(temp <= 2)
                            nodeInfo->roomParams.roomIdx = 2; // Battle prep.
                        else
                            nodeInfo->roomParams.roomIdx = 1; // Shops
                    }
                }
            }
            break;

        case ADVPATH_ROOM_LEGENDARY:
            nodeInfo->roomParams.roomIdx = Rogue_SelectLegendaryEncounterRoom();
            break;

        case ADVPATH_ROOM_MINIBOSS:
            nodeInfo->roomParams.roomIdx = Rogue_SelectMiniBossEncounterRoom();
            break;

        case ADVPATH_ROOM_WILD_DEN:
            nodeInfo->roomParams.roomIdx = Rogue_SelectWildDenEncounterRoom();
            break;

        case ADVPATH_ROOM_GAMESHOW:
            break;

        case ADVPATH_ROOM_GRAVEYARD:
            break;

        case ADVPATH_ROOM_LAB:
            break;

        case ADVPATH_ROOM_ROUTE:
        {
            nodeInfo->roomParams.roomIdx = Rogue_SelectRouteRoom();

            if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            {
                // All calm to maximise encounters
                nodeInfo->roomParams.perType.route.difficulty = 0;
            }
            else if(gRogueRun.currentDifficulty >= 8)
            {
                // Low chance if getting a calm route, but otherwise is difficult
                switch(RogueRandomRange(6, OVERWORLD_FLAG))
                {
                    case 0:
                        nodeInfo->roomParams.perType.route.difficulty = 0;
                        break;

                    default:
                        nodeInfo->roomParams.perType.route.difficulty = 2;
                        break;
                };
            }
            else
            {
                switch(RogueRandomRange(6, OVERWORLD_FLAG))
                {
                    case 0:
                    case 1:
                        nodeInfo->roomParams.perType.route.difficulty = 0;
                        break;

                    case 2:
                    case 3:
                    case 4:
                        nodeInfo->roomParams.perType.route.difficulty = 1;
                        break;

                    //case 5:
                    default:
                        nodeInfo->roomParams.perType.route.difficulty = 2;
                        break;
                };
            }
            break;
        }
    }
}

static void GenerateAdventureColumnEvents(u8 columnIdx, u8 columnCount, struct AdvMapScratch* scratch)
{
    struct RogueAdvPathNode* nodeInfo;
    u8 i;

    for(i = 0; i < MAX_PATH_ROWS; ++i)
    {
        nodeInfo = GetNodeInfo(columnIdx, i);
        if(nodeInfo->isBridgeActive)
        {
            ChooseNewEvent(columnIdx, i, columnCount, scratch, &scratch->readNodes[i], &scratch->writeNodes[i]);

            // Post event choose
            nodeInfo->roomType = scratch->writeNodes[i].roomType;
            
            switch(nodeInfo->roomType)
            {
                case ADVPATH_ROOM_LEGENDARY:
                    ++scratch->legendaryCount;
                    break;

                case ADVPATH_ROOM_WILD_DEN:
                    ++scratch->wildDenCount;
                    break;

                case ADVPATH_ROOM_MINIBOSS:
                    ++scratch->minibossCount;
                    break;

                case ADVPATH_ROOM_GAMESHOW:
                    ++scratch->gameShowCount;
                    break;

                case ADVPATH_ROOM_GRAVEYARD:
                    ++scratch->graveYardCount;
                    break;

                case ADVPATH_ROOM_LAB:
                    ++scratch->labCount;
                    break;
            }

            CreateEventParams(columnIdx, i, nodeInfo, scratch);
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
    u8 i;
    u8 totalDistance;
    u8 minY, maxY;
    struct AdvMapScratch scratch;
    struct AdvEventScratch* rowEventScratchA = malloc(sizeof(struct AdvEventScratch) * MAX_PATH_ROWS);
    struct AdvEventScratch* rowEventScratchB = malloc(sizeof(struct AdvEventScratch) * MAX_PATH_ROWS);

    if(gRogueAdvPath.currentNodeX < gRogueAdvPath.currentColumnCount)
    {
        // Path is still valid
        return FALSE;
    }

    memset(&scratch, 0, sizeof(scratch));

    SetRogueSeedForPath();

    ResetNodeInfo();

    // Setup defaults
    totalDistance = 4;

    if(FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
    {
        if(gRogueRun.currentDifficulty == 0)
        {
            totalDistance = 6;
        }
        else
        {
            totalDistance = 0;
        }
    }

    // Exit node
    nodeInfo = GetNodeInfo(totalDistance, CENTRE_ROW_IDX);
    nodeInfo->isBridgeActive = TRUE;
    nodeInfo->roomType = ADVPATH_ROOM_BOSS;
    nodeInfo->roomParams.roomIdx = Rogue_SelectBossEncounter();

    for(i = 0; i < totalDistance; ++i)
    {
        scratch.readNodes = (i == 0 ? &rowEventScratchA[0] : &rowEventScratchB[0]);
        scratch.writeNodes =  (i == 0 ? &rowEventScratchB[0] : &rowEventScratchA[0]);

        memset(scratch.writeNodes, 0, sizeof(scratch.writeNodes[0]) * MAX_PATH_ROWS);

        GenerateAdventureColumnPath(totalDistance - i - 1, totalDistance, &scratch);
        GenerateAdventureColumnEvents(totalDistance - i - 1, totalDistance, &scratch);
    }

    free(rowEventScratchA);
    free(rowEventScratchB);

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


static void SetBossRoomWarp(u8 bossId, struct WarpData* warp)
{
    if(gRogueRun.currentDifficulty < 8)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_0);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_0);
    }
    else if(gRogueRun.currentDifficulty < 12)
    {
        const struct RogueTrainerEncounter* trainer = &gRogueBossEncounters.trainers[bossId];
        u8 type = trainer->incTypes[0];

        if((trainer->trainerFlags & TRAINER_FLAG_THIRDSLOT_WEATHER) != 0)
        {
            type = trainer->incTypes[2];
        }

        warp->mapGroup = gRogueTypeToEliteRoom[type].group;
        warp->mapNum = gRogueTypeToEliteRoom[type].num;
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
                SetBossRoomWarp(node->roomParams.roomIdx, warp);
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

            case ADVPATH_ROOM_GRAVEYARD:
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

static u16 SelectGFXForNode(struct RogueAdvPathNode* nodeInfo)
{
    switch(nodeInfo->roomType)
    {
        case ADVPATH_ROOM_NONE:
            return 0;
            
        case ADVPATH_ROOM_ROUTE:
        {
            switch(nodeInfo->roomParams.perType.route.difficulty)
            {
                case 0:
                    return OBJ_EVENT_GFX_CUTTABLE_TREE;
                case 1:
                    return OBJ_EVENT_GFX_BREAKABLE_ROCK;
                case 2:
                    return OBJ_EVENT_GFX_PUSHABLE_BOULDER;
                default:
                    return OBJ_EVENT_GFX_CUTTABLE_TREE;
            };
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

        case ADVPATH_ROOM_GRAVEYARD:
            return OBJ_EVENT_GFX_DEVIL_MAN;

        case ADVPATH_ROOM_LAB:
            return OBJ_EVENT_GFX_PC;

        case ADVPATH_ROOM_BOSS:
            return OBJ_EVENT_GFX_BALL_CUSHION; // ?
    }

    return 0;
}

static u16 GetInitialGFXColumn()
{
    if(gRogueAdvPath.justGenerated)
        return 0;
    else
        return gRogueAdvPath.currentNodeX + 1;
}

static u16 GetGFXVarCount()
{
    return VAR_OBJ_GFX_ID_F - VAR_OBJ_GFX_ID_0 + 1;
}

void RogueAdv_UpdateObjectGFX()
{
    struct RogueAdvPathNode* nodeInfo;
    u8 currentID;
    u16 x, y;
    u16 mapX, mapY;
    u16 objGFX;
    u8 maxID = 0;

    // Only place future GFX
    for(x = GetInitialGFXColumn(); x < MAX_PATH_COLUMNS; ++x)
    for(y = 0; y < MAX_PATH_ROWS; ++y)
    {
        nodeInfo = GetNodeInfo(x, y);
        if(nodeInfo->isBridgeActive)
        {
            currentID = maxID++;
            NodeToCoords(x, y, &mapX, &mapY);

            objGFX = SelectGFXForNode(nodeInfo);

            if(objGFX && currentID < GetGFXVarCount())
            {
                VarSet(VAR_OBJ_GFX_ID_0 + currentID, objGFX);

                // Want them to sit on the bridge not the node
                SetObjEventTemplateCoords(currentID + 1, mapX + 2, mapY + 1);
            }
        }
    }
}


static struct RogueAdvPathNode* GetScriptNodeWithCoords(u16* outX, u16* outY)
{
    struct RogueAdvPathNode* nodeInfo;
    u16 x, y;
    u8 currentID;
    u8 targetNodeId = gSpecialVar_ScriptNodeID;
    u8 maxID = 0;

    for(x = GetInitialGFXColumn(); x < MAX_PATH_COLUMNS; ++x)
    for(y = 0; y < MAX_PATH_ROWS; ++y)
    {
        nodeInfo = GetNodeInfo(x, y);
        if(nodeInfo->isBridgeActive)
        {
            currentID = maxID++;

            if(currentID == targetNodeId)
            {
                *outX = x;
                *outY = y;
                return nodeInfo;
            }
        }
    }

    *outX = 0;
    *outY = 0;
    return NULL;
}

static struct RogueAdvPathNode* GetScriptNode()
{
    u16 x, y;
    return GetScriptNodeWithCoords(&x, &y);
}

static void BufferRoomType(u8* dst, u8 roomType)
{
    const u8 gText_AdvRoom_None[] = _("None");
    const u8 gText_AdvRoom_Route[] = _("Route");
    const u8 gText_AdvRoom_RestStop[] = _("Rest");
    const u8 gText_AdvRoom_Lengendary[] = _("Legend");
    const u8 gText_AdvRoom_MiniBoss[] = _("Mini Boss");
    const u8 gText_AdvRoom_Boss[] = _("Boss");

    switch(roomType)
    {
        case ADVPATH_ROOM_NONE:
            StringCopy(dst, gText_AdvRoom_None);
            return;

        case ADVPATH_ROOM_ROUTE:
            StringCopy(dst, gText_AdvRoom_Route);
            return;

        case ADVPATH_ROOM_RESTSTOP:
            StringCopy(dst, gText_AdvRoom_RestStop);
            return;

        case ADVPATH_ROOM_LEGENDARY:
            StringCopy(dst, gText_AdvRoom_Lengendary);
            return;

        case ADVPATH_ROOM_MINIBOSS:
            StringCopy(dst, gText_AdvRoom_MiniBoss);
            return;

        case ADVPATH_ROOM_BOSS:
            StringCopy(dst, gText_AdvRoom_Boss);
            return;
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

void RogueAdv_GetNodeParams()
{
    u16 nodeX, nodeY;
    struct RogueAdvPathNode* node = GetScriptNodeWithCoords(&nodeX, &nodeY);

    if(node)
    {
        gSpecialVar_ScriptNodeParam0 = node->roomType;
        gSpecialVar_ScriptNodeParam1 = node->roomParams.roomIdx;

        switch(node->roomType)
        {
            case ADVPATH_ROOM_ROUTE:
                gSpecialVar_ScriptNodeParam1 = node->roomParams.perType.route.difficulty;
                BufferTypeAdjective(gRogueRouteTable.routes[node->roomParams.roomIdx].wildTypeTable[(nodeX + nodeY) % 3]);
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
    u16 nodeX, nodeY;
    struct WarpData warp;
    struct RogueAdvPathNode* node = GetScriptNodeWithCoords(&nodeX, &nodeY);

    // Fill with dud warp
    warp.mapGroup = MAP_GROUP(ROGUE_HUB_TRANSITION);
    warp.mapNum = MAP_NUM(ROGUE_HUB_TRANSITION);
    warp.warpId = 0;
    warp.x = -1;
    warp.y = -1;

    if(node)
    {
        // Move to the selected node
        gRogueAdvPath.currentNodeX = nodeX;
        gRogueAdvPath.currentNodeY = nodeY;
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
    u16 i;
    struct RogueAdvPathNode* node;

    u16 nodeX, nodeY;

    for(i = 0; i < 100; ++i)
    {
        VarSet(gSpecialVar_ScriptNodeID, Random() % 16);

        // Look for encounter in next column (Don't care if these can actually connect or not)
        node = GetScriptNodeWithCoords(&nodeX, &nodeY);
        if(node && nodeX == GetInitialGFXColumn() && node->roomType != ADVPATH_ROOM_NONE)
        {
            RogueAdv_ExecuteNodeAction();
            return;
        }
    }

    // Failed so fallback to next gym warp
    for(i = 0; i < 16; ++i)
    {
        VarSet(gSpecialVar_ScriptNodeID, i);

        node = GetScriptNodeWithCoords(&nodeX, &nodeY);
        if(node && node->roomType == ADVPATH_ROOM_BOSS)
        {
            RogueAdv_ExecuteNodeAction();
            return;
        }
    }

    VarSet(gSpecialVar_ScriptNodeID, 0);
    RogueAdv_ExecuteNodeAction();
#endif
}