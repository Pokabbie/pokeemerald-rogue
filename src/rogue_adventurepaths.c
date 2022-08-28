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

#define PATH_MAP_OFFSET_X (MAP_OFFSET + 1)
#define PATH_MAP_OFFSET_Y (MAP_OFFSET + 1)

#define ROW_WIDTH 3
#define NODE_HEIGHT 2
// Assume CENTRE_NODE_HEIGHT is always 1

#define gSpecialVar_ScriptNodeID        gSpecialVar_0x8001
#define gSpecialVar_ScriptNodeParam0    gSpecialVar_0x8002
#define gSpecialVar_ScriptNodeParam1    gSpecialVar_0x8003

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
    u8 difficulty;
    u8 difficultyContributions;
    u8 roomType;
    u8 nextRoomType;
};

static void GetBranchingChance(u8 columnIdx, u8 columnCount, u8 roomType, u8 difficulty, u8* breakChance, u8* extraSplitChance)
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
            *breakChance = 2;
            *extraSplitChance = 50;
            break;
    }
}

static void GenerateAdventureColumnPath(u8 columnIdx, u8 columnCount, struct AdvEventScratch* readScratch, struct AdvEventScratch* writeScratch)
{
    u8 i;
    u8 breakChance;
    u8 extraChance;
    u8 nextRoomType;
    u8 difficulty;
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
                difficulty = 5;
                nextRoomType = ADVPATH_ROOM_BOSS;
            }
            else 
            {
                difficulty = readScratch[i].difficulty;
                nextRoomType = nextNodeInfo->roomType;
            }

            // Calculate the split chance
            GetBranchingChance(columnIdx, columnCount, nextRoomType, difficulty, &breakChance, &extraChance);

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
                    writeScratch[i].difficulty += difficulty;
                    writeScratch[i].difficultyContributions++;
                    writeScratch[i].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    writeScratch[i + 1].difficulty += difficulty;
                    writeScratch[i + 1].difficultyContributions++;
                    writeScratch[i + 1].nextRoomType = nextRoomType;

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
                    writeScratch[i].difficulty += difficulty;
                    writeScratch[i].difficultyContributions++;
                    writeScratch[i].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i - 1);
                    nodeInfo->isBridgeActive = TRUE;
                    writeScratch[i - 1].difficulty += difficulty;
                    writeScratch[i - 1].difficultyContributions++;
                    writeScratch[i - 1].nextRoomType = nextRoomType;

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
                    writeScratch[i - 1].difficulty += difficulty;
                    writeScratch[i - 1].difficultyContributions++;
                    writeScratch[i - 1].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    writeScratch[i + 1].difficulty += difficulty;
                    writeScratch[i + 1].difficultyContributions++;
                    writeScratch[i + 1].nextRoomType = nextRoomType;

                    // 3rd bridge might appear on occasion
                    if(RogueRandomChance(extraChance, OVERWORLD_FLAG))
                    {
                        nodeInfo = GetNodeInfo(columnIdx, i);
                        nodeInfo->isBridgeActive = TRUE;
                        writeScratch[i].difficulty += difficulty;
                        writeScratch[i].difficultyContributions++;
                        writeScratch[i].nextRoomType = nextRoomType;
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
                    writeScratch[i - 1].difficulty += difficulty;
                    writeScratch[i - 1].difficultyContributions++;
                    writeScratch[i - 1].nextRoomType = nextRoomType;
                    
                    nodeInfo = GetNodeInfo(columnIdx + 1, i - 1);
                    nodeInfo->isLadderActive = TRUE;
                }
                // Below
                else if(offset == 2)
                {
                    nodeInfo = GetNodeInfo(columnIdx, i + 1);
                    nodeInfo->isBridgeActive = TRUE;
                    writeScratch[i + 1].difficulty += difficulty;
                    writeScratch[i + 1].difficultyContributions++;
                    writeScratch[i + 1].nextRoomType = nextRoomType;

                    nodeInfo = GetNodeInfo(columnIdx + 1, i);
                    nodeInfo->isLadderActive = TRUE;
                }
                // Aligned
                else
                {
                    nodeInfo = GetNodeInfo(columnIdx, i);
                    nodeInfo->isBridgeActive = TRUE;
                    writeScratch[i].difficulty += difficulty;
                    writeScratch[i].difficultyContributions++;
                    writeScratch[i].nextRoomType = nextRoomType;
                }
            }
        }
    }

}

static void ChooseNewEvent(u8 nodeX, u8 nodeY, u8 columnCount, struct AdvEventScratch* prevScratch, struct AdvEventScratch* currScratch)
{
    u8 weights[ADVPATH_ROOM_COUNT];
    u16 totalWeight;
    u16 targetWeight;
    u8 i;

    // 50 is default weight
    memset(&weights[0], 5, sizeof(u8) * ARRAY_COUNT(weights));

    if(nodeX == columnCount - 1)
    {
        // This column is purely to allow for larger branches
        currScratch->roomType = ADVPATH_ROOM_NONE;
        return;
    }


    // Normal routes
    if(currScratch->nextRoomType == ADVPATH_ROOM_BOSS)
    {
        // Less likely near the end
        weights[ADVPATH_ROOM_ROUTE] = 20;
    }
    else
    {
        // Most common
        weights[ADVPATH_ROOM_ROUTE] = 150;
    }

    // NONE / Skip encounters
    if(gRogueRun.currentDifficulty >= 8)
    {
        if(nodeY < CENTRE_ROW_IDX)
        {
            // Lower routes are much more likely to have skips
            weights[ADVPATH_ROOM_NONE] = 100;
        }
        else
        {
            // Slightly below average
            weights[ADVPATH_ROOM_NONE] = 20;
        }
    }
    else
    {
        // Unlikely but not impossible
        weights[ADVPATH_ROOM_NONE] = 20;
    }

    // Rest stops
    if(currScratch->nextRoomType == ADVPATH_ROOM_BOSS)
    {
        weights[ADVPATH_ROOM_RESTSTOP] = 100;
    }
    else
    {
        // Unlikely but not impossible
        weights[ADVPATH_ROOM_RESTSTOP] = 20;
    }

    // Legendaries/Mini encounters
    if(gRogueRun.currentDifficulty == 0)
    {
        weights[ADVPATH_ROOM_MINIBOSS] = 0;
        weights[ADVPATH_ROOM_LEGENDARY] = 0;
    }
    else
    {
        if(currScratch->nextRoomType == ADVPATH_ROOM_BOSS)
        {
            weights[ADVPATH_ROOM_MINIBOSS] = 70;
            weights[ADVPATH_ROOM_LEGENDARY] = 70;
        }
        else
        {
            weights[ADVPATH_ROOM_MINIBOSS] = 30;
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
        }
    }

    // Now we've applied the default weights for this column, consider what out next encounter is
    switch(currScratch->nextRoomType)
    {
        case ADVPATH_ROOM_LEGENDARY:
            weights[ADVPATH_ROOM_MINIBOSS] *= 2;
            weights[ADVPATH_ROOM_RESTSTOP] = 0;
            weights[ADVPATH_ROOM_NONE] = 0;
            break;

        case ADVPATH_ROOM_MINIBOSS:
            weights[ADVPATH_ROOM_LEGENDARY] = 0;
            break;

        case ADVPATH_ROOM_RESTSTOP:
            weights[ADVPATH_ROOM_RESTSTOP] = 0;
            break;

        case ADVPATH_ROOM_ROUTE:
            weights[ADVPATH_ROOM_NONE] += 30;
            break;

        case ADVPATH_ROOM_NONE:
            weights[ADVPATH_ROOM_NONE] /= 2; // Unlikely to get multiple in a row
            break;
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
}

static void CreateEventParams(struct RogueAdvPathNode* nodeInfo, struct AdvEventScratch* prevScratch, struct AdvEventScratch* currScratch)
{
    nodeInfo->roomType = currScratch->roomType;

    switch(nodeInfo->roomType)
    {
        case ADVPATH_ROOM_RESTSTOP:
            nodeInfo->roomParams.roomIdx= RogueRandomRange(gRogueRestStopEncounterInfo.mapCount, OVERWORLD_FLAG);
            break;
    }
}

static void GenerateAdventureColumnEvents(u8 columnIdx, u8 columnCount, struct AdvEventScratch* readScratch, struct AdvEventScratch* writeScratch)
{
    struct RogueAdvPathNode* nodeInfo;
    u8 i;

    for(i = 0; i < MAX_PATH_ROWS; ++i)
    {
        // Average the input difficulty
        if(writeScratch[i].difficultyContributions)
            writeScratch[i].difficulty /= writeScratch[i].difficultyContributions;
        else
            writeScratch[i].difficulty = 5;

        nodeInfo = GetNodeInfo(columnIdx, i);
        if(nodeInfo->isBridgeActive)
        {
            ChooseNewEvent(columnIdx, i, columnCount, &readScratch[i], &writeScratch[i]);
            CreateEventParams(nodeInfo, &readScratch[i], &writeScratch[i]);
        }
    }
}

bool8 RogueAdv_GenerateAdventurePathsIfRequired()
{
    struct RogueAdvPathNode* nodeInfo;
    u8 i;
    u8 totalDistance;
    u8 minY, maxY;
    struct AdvEventScratch rowEventScratchA[MAX_PATH_ROWS];
    struct AdvEventScratch rowEventScratchB[MAX_PATH_ROWS];

    if(gRogueAdvPath.currentNodeX < gRogueAdvPath.currentColumnCount)
    {
        // Path is still valid
        return FALSE;
    }

    ResetNodeInfo();

    // Setup defaults
    totalDistance = 4;

    // Exit node
    nodeInfo = GetNodeInfo(totalDistance, CENTRE_ROW_IDX);
    nodeInfo->isBridgeActive = TRUE;
    nodeInfo->roomType = ADVPATH_ROOM_BOSS;

    for(i = 0; i < totalDistance; ++i)
    {
        struct AdvEventScratch* readScratch = (i == 0 ? &rowEventScratchA[0] : &rowEventScratchB[0]);
        struct AdvEventScratch* writeScratch =  (i == 0 ? &rowEventScratchB[0] : &rowEventScratchA[0]);

        memset(writeScratch, 0, sizeof(struct AdvEventScratch) * MAX_PATH_ROWS);

        GenerateAdventureColumnPath(totalDistance - i - 1, totalDistance, readScratch, writeScratch);
        GenerateAdventureColumnEvents(totalDistance - i - 1, totalDistance, readScratch, writeScratch);
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

bool8 RogueAdv_OverrideNextWarp(struct WarpData *warp)
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
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static u16 SelectGFXForNode(struct RogueAdvPathNode* nodeInfo)
{
    switch(nodeInfo->roomType)
    {
        case ADVPATH_ROOM_NONE:
            return 0;
            
        case ADVPATH_ROOM_ROUTE:
            return OBJ_EVENT_GFX_CUTTABLE_TREE;

        case ADVPATH_ROOM_RESTSTOP:
            return gRogueRestStopEncounterInfo.mapTable[nodeInfo->roomParams.roomIdx].encounterId;

        case ADVPATH_ROOM_LEGENDARY:
            return OBJ_EVENT_GFX_TRICK_HOUSE_STATUE;

        case ADVPATH_ROOM_MINIBOSS:
            return OBJ_EVENT_GFX_NOLAND; //OBJ_EVENT_GFX_WALLY; // ?? OBJ_EVENT_GFX_YOUNGSTER

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

            if(objGFX)
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

void RogueAdv_GetNodeParams()
{
    struct RogueAdvPathNode* node = GetScriptNode();

    if(node)
    {
        gSpecialVar_ScriptNodeParam0 = node->roomType;
        gSpecialVar_ScriptNodeParam1 = node->roomParams.roomIdx;
    }
    else
    {
        gSpecialVar_ScriptNodeParam0 = (u16)-1;
        gSpecialVar_ScriptNodeParam1 = (u16)-1;
    }
}

void RogueAdv_BufferNodeMessage()
{
    u16 nodeX, nodeY;
    const u8 gText_TEMP[] = _("{STR_VAR_2},{STR_VAR_3}: ");
    struct RogueAdvPathNode* node = GetScriptNodeWithCoords(&nodeX, &nodeY);

    if(node)
    {
        ConvertUIntToDecimalStringN(gStringVar2, nodeX, STR_CONV_MODE_LEFT_ALIGN, 3);
        ConvertUIntToDecimalStringN(gStringVar3, nodeY, STR_CONV_MODE_LEFT_ALIGN, 3);

        BufferRoomType(gStringVar4, node->roomType);

        StringExpandPlaceholders(gStringVar1, gText_TEMP);
        StringAppend(gStringVar1, gStringVar4);
    }
    else
    {
        ConvertUIntToDecimalStringN(gStringVar1, 123, STR_CONV_MODE_LEFT_ALIGN, 3);
    }
}

void RogueAdv_ExecuteNodeAction()
{
    u16 nodeX, nodeY;
    struct WarpData warp;
    struct RogueAdvPathNode* node = GetScriptNodeWithCoords(&nodeX, &nodeY);

    // Fill with default warp
    warp.mapGroup = MAP_GROUP(ROGUE_ROUTE_FIELD0);
    warp.mapNum = MAP_NUM(ROGUE_ROUTE_FIELD0);
    warp.warpId = 0;
    warp.x = -1;
    warp.y = -1;

    if(node)
    {
        // Move to the selected node
        gRogueAdvPath.currentNodeX = nodeX;
        gRogueAdvPath.currentNodeY = nodeY;
        gRogueAdvPath.currentRoomType = node->roomType;

        switch(node->roomType)
        {
            case ADVPATH_ROOM_BOSS:
                Rogue_SelectBossRoomWarp(&warp);
                break;

            case ADVPATH_ROOM_RESTSTOP:
                warp.mapGroup = gRogueRestStopEncounterInfo.mapTable[node->roomParams.roomIdx].group;
                warp.mapNum = gRogueRestStopEncounterInfo.mapTable[node->roomParams.roomIdx].num;
                break;
        }
        
        // Now we've interacted hide this node
        node->roomType = ADVPATH_ROOM_NONE;
    }

    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
}