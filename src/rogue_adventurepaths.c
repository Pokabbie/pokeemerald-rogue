#include "global.h"
#include "constants/event_objects.h"
#include "constants/rogue.h"
#include "event_data.h"
#include "fieldmap.h"
#include "overworld.h"

#include "rogue_controller.h"

#include "rogue_adventurepaths.h"

// Node refers to horizontal paths
// Ladder refers to vertical
// e.g.
//  \/-- Crossover
//  X ____ <- Node
//  |
//  | <- Ladder
//  |


#define MAX_PATH_ROWS ROGUE_MAX_ADVPATH_ROWS
#define MAX_PATH_COLUMNS ROGUE_MAX_ADVPATH_COLUMNS
#define CENTRE_ROW_IDX ((MAX_PATH_ROWS - 1) / 2)

#define PATH_MAP_OFFSET (MAP_OFFSET + 1)

#define ROW_WIDTH 3
#define NODE_HEIGHT 2
// Assume CENTRE_NODE_HEIGHT is always 1

const u16 c_MetaTile_Sign = 0x003;
const u16 c_MetaTile_Grass = 0x001;
const u16 c_MetaTile_Water = 0x170;

static void NodeToCoords(u16 nodeX, u16 nodeY, u16* x, u16* y)
{
    *x = nodeX * ROW_WIDTH;
    *y = nodeY * NODE_HEIGHT;
}

static void LadderToCoords(u16 LadderX, u16 LadderY, u16* x, u16* y)
{
    NodeToCoords(LadderX, LadderY, x, y);
    --*x;
}

static void DisableNodeMetatiles(u16 nodeX, u16 nodeY)
{
    u16 x, y, i, j;

    NodeToCoords(nodeX, nodeY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
    MapGridSetMetatileIdAt(x + 1 + PATH_MAP_OFFSET, y + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
}

static void DisableLadderMetatiles(u16 LadderX, u16 LadderY)
{
    u16 x, y, i, j;

    LadderToCoords(LadderX, LadderY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y + 1 + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
}

static void DisableCrossoverMetatiles(u16 LadderX, u16 LadderY)
{
    u16 x, y, i, j;

    LadderToCoords(LadderX, LadderY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
}

static struct RogueAdvPathNode* GetNodeInfo(u16 x, u16 y)
{
    return &gRogueRun.advPathNodes[y * MAX_PATH_COLUMNS + x];
}

static void ResetNodeInfo()
{
    memcpy(&gRogueRun.advPathNodes[0], 0, sizeof(struct RogueAdvPathNode) * ARRAY_COUNT(gRogueRun.advPathNodes));
}

// Difficulty rating is from 1-10 (5 being average, 1 easy, 10 hard)
static void GenerateAdventureColumnPath(u8 columnIdx, u8 columnCount, u8* rowDifficulties)
{
    u8 i;
    u8 chance;
    u8 difficulty;
    struct RogueAdvPathNode* nextNodeInfo;
    struct RogueAdvPathNode* nodeInfo;

    for(i = 0; i < MAX_PATH_ROWS; ++i)
    {
        // This has already been decided
        nextNodeInfo = GetNodeInfo(columnIdx + 1, i);

        if(nextNodeInfo->isNodeActive)
        {
            difficulty = rowDifficulties[i];

            // Calculate the split chance
            if(columnIdx == columnCount - 1)
            {
                chance = 100;
            }
            else if(difficulty >= 4 && difficulty <= 6)
            {
                chance = 85;
            }
            else if(difficulty <= 2)
            {
                // Too easy
                chance = 0;
            }
            else if(difficulty >= 8)
            {
                // Too Hard
                chance = 0;
            }
            else
            {
                // Small chance
                chance = 10;
            }

            if(RogueRandomChance(chance, OVERWORLD_FLAG))
            {
                if(i == 0)
                {
                    // Split
                    // ==|==
                    //   |
                    // ==|

                    nodeInfo = GetNodeInfo(columnIdx, i);
                    nodeInfo->isNodeActive = TRUE;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isNodeActive = TRUE;

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
                    nodeInfo->isNodeActive = TRUE;

                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isNodeActive = TRUE;

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
                    // TODO - Another chance of splitting into 3?
                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isNodeActive = TRUE;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isNodeActive = TRUE;

                    nodeInfo = GetNodeInfo(columnIdx + 1, i - 1);
                    nodeInfo->isLadderActive = TRUE;
                    
                    nodeInfo = GetNodeInfo(columnIdx + 1, i);
                    nodeInfo->isLadderActive = TRUE;
                }
            }
            else
            {
                // Just continue the same path
                nodeInfo = GetNodeInfo(columnIdx, i);
                nodeInfo->isNodeActive = TRUE;
            }
        }
    }

}

static void GenerateAdventureColumnEvents(u8 columnIdx, u8 columnCount, u8* rowDifficulties)
{
    u8 i;

    // TODO
    for(i = 0; i < MAX_PATH_COLUMNS; ++i)
    {
        rowDifficulties[i] = 1 + RogueRandomRange(9, OVERWORLD_FLAG);
    }
}

void Rogue_GenerateAdventurePathsIfRequired()
{
    struct RogueAdvPathNode* nodeInfo;
    u8 i;
    u8 totalDistance;
    u8 rowDifficulties[MAX_PATH_COLUMNS];

    ResetNodeInfo();

    // Setup defaults
    totalDistance = 3 + RogueRandomRange(1, OVERWORLD_FLAG);
    memset(rowDifficulties, 5, sizeof(u8) * ARRAY_COUNT(rowDifficulties));

    // Exit node
    nodeInfo = GetNodeInfo(totalDistance, CENTRE_ROW_IDX + (RogueRandomRange(3, OVERWORLD_FLAG) - 1));
    nodeInfo->isNodeActive = TRUE;

    for(i = 0; i < totalDistance; ++i)
    {
        GenerateAdventureColumnPath(totalDistance - i - 1, totalDistance, rowDifficulties);
        GenerateAdventureColumnEvents(totalDistance - i - 1, totalDistance, rowDifficulties);
    }

    //nodeInfo = GetNodeInfo(0, CENTRE_ROW_IDX);
    //nodeInfo->isNodeActive = TRUE;
    //nodeInfo->isLadderActive = TRUE;
}

void Rogue_ApplyAdventureMetatiles()
{
    u8 x, y;
    struct RogueAdvPathNode* nodeInfo;
    struct RogueAdvPathNode* prevNodeInfo;

    for(x = 0; x < MAX_PATH_COLUMNS; ++x)
    for(y = 0; y < MAX_PATH_ROWS; ++y)
    {
        nodeInfo = GetNodeInfo(x, y);
        if(!nodeInfo->isNodeActive)
        {
            DisableNodeMetatiles(x, y);
        }

        if(!nodeInfo->isLadderActive)
        {
            DisableLadderMetatiles(x, y);
        }
        
        if(!nodeInfo->isNodeActive && !nodeInfo->isLadderActive)
        {
            if(x == 0)
            {
                DisableCrossoverMetatiles(x, y);
            }
            else
            {
                prevNodeInfo = GetNodeInfo(x - 1, y);

                if(!prevNodeInfo->isNodeActive)
                {
                    DisableCrossoverMetatiles(x, y);
                }
            }
        }
    }
}

void Rogue_UpdateObjectGFX()
{
    //OBJ_EVENT_GFX_TRICK_HOUSE_STATUE
    //OBJ_EVENT_GFX_MART_EMPLOYEE
    //OBJ_EVENT_GFX_NURSE
    //OBJ_EVENT_GFX_MYSTERY_GIFT_MAN
    //OBJ_EVENT_GFX_DEOXYS_TRIANGLE
    //OBJ_EVENT_GFX_CONTEST_JUDGE
    //OBJ_EVENT_GFX_ITEM_BALL
    //OBJ_EVENT_GFX_BALL_CUSHION
    //OBJ_EVENT_GFX_CUTTABLE_TREE
//setvar VAR_OBJ_GFX_ID_1, OBJ_EVENT_GFX_LATIOS

    // Local ID is 1 above the GFX id

    //SetObjEventTemplateCoords(localId, x, y);

    struct RogueAdvPathNode* nodeInfo;
    u8 currentID;
    u16 x, y;
    u16 mapX, mapY;
    u8 maxID = 0;

    for(x = 0; x < MAX_PATH_COLUMNS; ++x)
    for(y = 0; y < MAX_PATH_ROWS; ++y)
    {
        nodeInfo = GetNodeInfo(x, y);
        if(nodeInfo->isNodeActive)
        {
            currentID = maxID++;
            NodeToCoords(x, y, &mapX, &mapY);
            
            // TODO - Read pre-gen info here
            VarSet(VAR_OBJ_GFX_ID_0 + currentID, OBJ_EVENT_GFX_CUTTABLE_TREE);

            SetObjEventTemplateCoords(currentID + 1, mapX + 1, mapY + 1);
        }
    }

    // TODO - move the rest out of the way
}