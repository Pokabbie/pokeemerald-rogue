#include "global.h"
#include "constants/event_objects.h"
#include "constants/rogue.h"
#include "event_data.h"
#include "fieldmap.h"
#include "field_screen_effect.h"
#include "overworld.h"
#include "strings.h"
#include "string_util.h"

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

#define PATH_MAP_OFFSET_X (MAP_OFFSET + 0)
#define PATH_MAP_OFFSET_Y (MAP_OFFSET + 1)

#define ROW_WIDTH 3
#define NODE_HEIGHT 2
// Assume CENTRE_NODE_HEIGHT is always 1

#define gSpecialVar_ScriptNodeID gSpecialVar_0x8000

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

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X + 1, y + PATH_MAP_OFFSET_Y, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X + 2, y + PATH_MAP_OFFSET_Y, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
}

static void DisableLadderMetatiles(u16 nodeX, u16 nodeY)
{
    u16 x, y, i, j;

    NodeToCoords(nodeX, nodeY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X, y + 1 + PATH_MAP_OFFSET_Y, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
}

static void DisableCrossoverMetatiles(u16 nodeX, u16 nodeY)
{
    u16 x, y, i, j;

    NodeToCoords(nodeX, nodeY, &x, &y);

    MapGridSetMetatileIdAt(x + PATH_MAP_OFFSET_X, y + PATH_MAP_OFFSET_Y, c_MetaTile_Water | MAPGRID_COLLISION_MASK);
}

static struct RogueAdvPathNode* GetNodeInfo(u16 x, u16 y)
{
    return &gRogueRun.advPath.nodes[y * MAX_PATH_COLUMNS + x];
}

static void ResetNodeInfo()
{
    u16 i;
    for(i = 0; i < ARRAY_COUNT(gRogueRun.advPath.nodes); ++i)
    {
        gRogueRun.advPath.nodes[i].isBridgeActive = FALSE;
        gRogueRun.advPath.nodes[i].isLadderActive = FALSE;
    }
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

        if(nextNodeInfo->isBridgeActive)
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
                    nodeInfo->isBridgeActive = TRUE;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;

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

                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isBridgeActive = TRUE;

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
                    nodeInfo->isBridgeActive = TRUE;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;

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
                nodeInfo->isBridgeActive = TRUE;
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

bool8 RogueAdv_GenerateAdventurePathsIfRequired()
{
    struct RogueAdvPathNode* nodeInfo;
    u8 i;
    u8 totalDistance;
    u8 minY, maxY;
    u8 rowDifficulties[MAX_PATH_COLUMNS];

    if(gRogueRun.advPath.currentNodeX < gRogueRun.advPath.currentColumnCount)
    {
        // Path is still valid
        return FALSE;
    }

    ResetNodeInfo();

    // Setup defaults
    totalDistance = 3 + RogueRandomRange(1, OVERWORLD_FLAG);
    memset(rowDifficulties, 5, sizeof(u8) * ARRAY_COUNT(rowDifficulties));

    // Exit node
    nodeInfo = GetNodeInfo(totalDistance, CENTRE_ROW_IDX + (RogueRandomRange(3, OVERWORLD_FLAG) - 1));
    nodeInfo->isBridgeActive = TRUE;

    for(i = 0; i < totalDistance; ++i)
    {
        GenerateAdventureColumnPath(totalDistance - i - 1, totalDistance, rowDifficulties);
        GenerateAdventureColumnEvents(totalDistance - i - 1, totalDistance, rowDifficulties);
    }

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
    gRogueRun.advPath.currentColumnCount = totalDistance;
    gRogueRun.advPath.currentNodeX = 0;
    gRogueRun.advPath.currentNodeY = (minY + maxY) / 2;
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

void RogueAdv_EnqueueNextWarp(struct WarpData *warp)
{
    // Should already be set correctly for RogueAdv_ExecuteNodeAction
    if(!gRogueRun.advPath.isOverviewActive)
    {
        u16 x, y;

        bool8 freshPath = RogueAdv_GenerateAdventurePathsIfRequired();

        NodeToCoords(gRogueRun.advPath.currentNodeX, gRogueRun.advPath.currentNodeY, &x, &y);

        // Always jump back to overview screen, after a different route
        warp->mapGroup = MAP_GROUP(ROGUE_ADVENTURE_PATHS);
        warp->mapNum = MAP_NUM(ROGUE_ADVENTURE_PATHS);
        warp->warpId = WARP_ID_NONE;
        warp->x = x + (freshPath ? 0 : 2);
        warp->y = y + 1;
    }
}

void RogueAdv_UpdateObjectGFX()
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
        if(nodeInfo->isBridgeActive)
        {
            currentID = maxID++;
            NodeToCoords(x, y, &mapX, &mapY);
            
            // TODO - Read pre-gen info here
            VarSet(VAR_OBJ_GFX_ID_0 + currentID, OBJ_EVENT_GFX_CUTTABLE_TREE);

            // Want them to sit on the bridge not the node
            SetObjEventTemplateCoords(currentID + 1, mapX + 1, mapY + 1);
        }
    }

    // TODO - move the rest out of the way
}

static struct RogueAdvPathNode* GetScriptNode()
{
    struct RogueAdvPathNode* nodeInfo;
    u16 x, y;
    u8 currentID;
    u8 targetNodeId = gSpecialVar_ScriptNodeID;
    u8 maxID = 0;

    for(x = 0; x < MAX_PATH_COLUMNS; ++x)
    for(y = 0; y < MAX_PATH_ROWS; ++y)
    {
        nodeInfo = GetNodeInfo(x, y);
        if(nodeInfo->isBridgeActive)
        {
            currentID = maxID++;

            if(currentID == targetNodeId)
            {
                return nodeInfo;
            }
        }
    }

    return NULL;
} 

static void GetScriptNodeCoords(u16* outX, u16* outY)
{
    struct RogueAdvPathNode* nodeInfo;
    u16 x, y;
    u8 currentID;
    u8 targetNodeId = gSpecialVar_ScriptNodeID;
    u8 maxID = 0;

    for(x = 0; x < MAX_PATH_COLUMNS; ++x)
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
                return;
            }
        }
    }

    *outX = 0;
    *outY = 0;
    return;
} 

void RogueAdv_BufferNodeMessage()
{
    u16 nodeX, nodeY;
    const u8 gText_TEMP[] = _("{STR_VAR_2},{STR_VAR_3}");
    struct RogueAdvPathNode* node = GetScriptNode();

    if(node)
    {
        GetScriptNodeCoords(&nodeX, &nodeY);
        ConvertUIntToDecimalStringN(gStringVar2, nodeX, STR_CONV_MODE_LEFT_ALIGN, 3);
        ConvertUIntToDecimalStringN(gStringVar3, nodeY, STR_CONV_MODE_LEFT_ALIGN, 3);


        StringExpandPlaceholders(gStringVar1, gText_TEMP);
    }
    else
    {
        ConvertUIntToDecimalStringN(gStringVar1, 123, STR_CONV_MODE_LEFT_ALIGN, 3);
    }
}

void RogueAdv_ExecuteNodeAction()
{
    u16 nodeX, nodeY;
    struct RogueAdvPathNode* node = GetScriptNode();

    if(node)
    {
        GetScriptNodeCoords(&nodeX, &nodeY);

        // Move to the selected node
        gRogueRun.advPath.currentNodeX = nodeX;
        gRogueRun.advPath.currentNodeY = nodeY;

        //SetWarpDestination(mapGroup, mapNum, warpId, x, y);
        SetWarpDestination(
            MAP_GROUP(ROGUE_ROUTE_FIELD0), 
            MAP_NUM(ROGUE_ROUTE_FIELD0), 
            0, 
            -1, -1
            );
        DoWarp();
        ResetInitialPlayerAvatarState();
        //u8 mapGroup = ScriptReadByte(ctx);
        //u8 mapNum = ScriptReadByte(ctx);
        //u8 warpId = ScriptReadByte(ctx);
        //u16 x = VarGet(ScriptReadHalfword(ctx));
        //u16 y = VarGet(ScriptReadHalfword(ctx));
//
        //SetWarpDestination(mapGroup, mapNum, warpId, x, y);
        //DoWarp();
        //ResetInitialPlayerAvatarState();
    }
}