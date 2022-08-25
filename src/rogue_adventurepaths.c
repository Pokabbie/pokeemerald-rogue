#include "global.h"
#include "constants/rogue.h"
#include "fieldmap.h"

#include "rogue_controller.h"

#include "rogue_adventurepaths.h"

// Node refers to horizontal paths
// Ladder refers to vertical
// e.g.
//   <- Crossover
//  X ____ <- Node
//  |
//  | <- Ladder
//  |


#define MAX_PATH_ROWS ROGUE_MAX_ADVPATH_ROWS
#define MAX_PATH_COLUMNS ROGUE_MAX_ADVPATH_COLUMNS
#define CENTRE_ROW_IDX ((MAX_PATH_ROWS - 1) / 2)

#define PATH_MAP_OFFSET (MAP_OFFSET + 1)

#define ROW_WIDTH 3
#define OUTER_NODE_HEIGHT 2
// Assume CENTRE_NODE_HEIGHT is always 1

const u16 c_MetaTile_Sign = 0x003;
const u16 c_MetaTile_Grass = 0x001;
const u16 c_MetaTile_Water = 0x170;

static void NodeToCoords(u16 nodeX, u16 nodeY, u16* x, u16* y)
{
    *x = nodeX * ROW_WIDTH;

    if(nodeY == CENTRE_ROW_IDX)
    {
        *y = (CENTRE_ROW_IDX * OUTER_NODE_HEIGHT);
    }
    else if(nodeY < CENTRE_ROW_IDX)
    {
        *y = nodeY * OUTER_NODE_HEIGHT;
    }
    else
    {
        *y = (CENTRE_ROW_IDX * OUTER_NODE_HEIGHT) + (nodeY - CENTRE_ROW_IDX) * OUTER_NODE_HEIGHT;
    }
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

    if(LadderY == CENTRE_ROW_IDX)
    {
    }
    else if(LadderY < CENTRE_ROW_IDX)
    {
        MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y + 1 + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
    }
    else
    {
        MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y - 1 + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
    }
}

static void DisableCrossoverMetatiles(u16 LadderX, u16 LadderY)
{
    u16 x, y, i, j;

    LadderToCoords(LadderX, LadderY, &x, &y);

    if(LadderY == CENTRE_ROW_IDX)
    {
        MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
    }
    else if(LadderY < CENTRE_ROW_IDX)
    {
        MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
    }
    else
    {
        MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET, y + PATH_MAP_OFFSET, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
    }
}

static struct RogueAdvPathNode* GetNodeInfo(u16 x, u16 y)
{
    return &gRogueRun.advPathNodes[y + x * MAX_PATH_ROWS];
}

static void ResetNodeInfo()
{
    memcpy(&gRogueRun.advPathNodes[0], 0, sizeof(struct RogueAdvPathNode) * ARRAY_COUNT(gRogueRun.advPathNodes));
}

void Rogue_GenerateAdventurePaths()
{
    struct RogueAdvPathNode* nodeInfo;

    ResetNodeInfo();

    nodeInfo = GetNodeInfo(0, CENTRE_ROW_IDX);
    nodeInfo->isNodeActive = TRUE;
    nodeInfo->isLadderActive = TRUE;

    nodeInfo = GetNodeInfo(1, CENTRE_ROW_IDX -1);
    nodeInfo->isNodeActive = TRUE;
    nodeInfo->isLadderActive = TRUE;

    nodeInfo = GetNodeInfo(2, CENTRE_ROW_IDX - 1);
    nodeInfo->isNodeActive = TRUE;
    nodeInfo->isLadderActive = FALSE;

    nodeInfo = GetNodeInfo(1, CENTRE_ROW_IDX + 1);
    nodeInfo->isNodeActive = TRUE;
    nodeInfo->isLadderActive = TRUE;
//

    //nodeInfo = GetNodeInfo(1, CENTRE_ROW_IDX);
    //nodeInfo->isNodeActive = TRUE;
    //nodeInfo->isLadderActive = TRUE;
//
    //nodeInfo = GetNodeInfo(1, CENTRE_ROW_IDX - 1);
    //nodeInfo->isNodeActive = TRUE;
    //nodeInfo->isLadderActive = TRUE;
//
    //nodeInfo = GetNodeInfo(1, CENTRE_ROW_IDX + 1);
    //nodeInfo->isNodeActive = TRUE;
    //nodeInfo->isLadderActive = FALSE;
}

void Rogue_ApplyAdventureMetatiles()
{
    u8 x, y;
    struct RogueAdvPathNode* nodeInfo;
    struct RogueAdvPathNode* prevNodeInfo;

    Rogue_GenerateAdventurePaths();

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
        
        if(x != 0 && !nodeInfo->isNodeActive && !nodeInfo->isLadderActive)
        {
            prevNodeInfo = GetNodeInfo(x - 1, y);

            if(!prevNodeInfo->isNodeActive)
            {
                DisableCrossoverMetatiles(x, y);
            }
        }
    }
}