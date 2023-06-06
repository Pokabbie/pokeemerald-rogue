#include "global.h"
#include "constants/layouts.h"
#include "constants/metatile_labels.h"
#include "constants/script_menu.h"

//#include "constants/event_objects.h"
//#include "constants/event_object_movement.h"
//#include "constants/map_types.h"
//#include "constants/rogue.h"
//#include "constants/songs.h"
//
#include "event_data.h"
//#include "event_object_movement.h"
#include "fieldmap.h"
#include "menu.h"
//#include "field_player_avatar.h"
//#include "follow_me.h"
//#include "metatile_behavior.h"
//#include "pokemon.h"
//#include "safari_zone.h"
//#include "sound.h"
#include "strings.h"
//#include "random.h"
//#include "script.h"

//#include "rogue.h"
#include "rogue_hub.h"
//#include "rogue_controller.h"
//#include "rogue_followmon.h"
//#include "rogue_popup.h"

#define TREE_TYPE_DENSE     0
#define TREE_TYPE_SPARSE    1

struct RogueHubMap
{
    u8 upgradeFlags[1 + HUB_UPGRADE_COUNT / 8];
};

EWRAM_DATA struct RogueHubMap gRogueHubMap;

static void MetatileSet_Tile(u16 xStart, u16 yStart, u16 tile);
static void MetatileFill_Tile(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u16 tile);
static void MetatileFill_TreesOverlapping(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u8 treeType);
static void MetatileFill_TreeStumps(u16 xStart, u16 yStart, u16 xEnd, u8 treeType);
static void MetatileFill_TreeCaps(u16 xStart, u16 yStart, u16 xEnd);

static void RogueHub_UpdateHomeAreaMetatiles();
static void RogueHub_UpdateHomeInteriorMetatiles();

void RogueHub_Enter()
{
    //AGB_ASSERT(gRogueHubMap == NULL);
    //gRogueHubMap = AllocZeroed(sizeof(struct RogueHubMap));
}

void RogueHub_Exit()
{
    //AGB_ASSERT(gRogueHubMap != NULL);
    //free(gRogueHubMap);
    //gRogueHubMap = NULL;
}

bool8 RogueHub_HasUpgrade(u16 upgradeId)
{
    u16 idx = upgradeId / 8;
    u16 bit = upgradeId % 8;

    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(gRogueHubMap.upgradeFlags));
    return gRogueHubMap.upgradeFlags[idx] & bitMask;
}

void RogueHub_SetUpgrade(u16 upgradeId, bool8 state)
{
    u16 idx = upgradeId / 8;
    u16 bit = upgradeId % 8;

    u8 bitMask = 1 << bit;
    
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueHubMap.upgradeFlags));
    if(state)
    {
        gRogueHubMap.upgradeFlags[idx] |= bitMask;
    }
    else
    {
        gRogueHubMap.upgradeFlags[idx] &= ~bitMask;
    }
}

bool8 RogueHub_HasUpgradeRequirements(u16 upgradeId)
{
    u8 i;
    u8 check;

    for(i = 0; i < HUB_UPGRADE_MAX_REQUIREMENTS; ++i)
    {
        check = gRogueHubUpgrades[upgradeId].requiredUpgrades[i];
        if(check == HUB_UPGRADE_NONE)
            break;

        if(!RogueHub_HasUpgrade(check))
            return FALSE;
    }

    return TRUE;
}

u8 RogueHub_GetAreaFromCurrentMap()
{
    switch (gMapHeader.mapLayoutId)
    {
    case LAYOUT_ROGUE_AREA_HOME:
        return HUB_AREA_HOME;
    }

    return HUB_AREA_COUNT;
}

void RogueHub_GetAvaliableUpgrades(u8 area, u8* outUpgrades, u8* outUpgradeCount)
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < HUB_UPGRADE_COUNT; ++i)
    {
        if(gRogueHubUpgrades[i].targetArea == area && !RogueHub_HasUpgrade(i) && RogueHub_HasUpgradeRequirements(i))
        {
            outUpgrades[count++] = i;
        }
    }

    *outUpgradeCount = count;
}

void RogueHub_ApplyMapMetatiles()
{
    switch (gMapHeader.mapLayoutId)
    {
    case LAYOUT_ROGUE_AREA_HOME:
        RogueHub_UpdateHomeAreaMetatiles();
        break;
    case LAYOUT_ROGUE_INTERIOR_HOME:
        RogueHub_UpdateHomeInteriorMetatiles();
        break;
    
    default:
        break;
    }

}

static void RogueHub_UpdateHomeAreaMetatiles()
{
    // Fill right field
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_GRASS_FIELD))
    {
        MetatileFill_TreesOverlapping(8, 3, 19, 9, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(8, 9, 11, TREE_TYPE_DENSE);
        MetatileFill_TreesOverlapping(12, 10, 19, 12, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(12, 13, 19, TREE_TYPE_DENSE);
    }

    // Fill shed (must unlock after field)
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_GRASS_FIELD_SHED))
    {
        MetatileFill_TreesOverlapping(14, 3, 17, 6, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(15, 7, 17, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(14, 7, 14, TREE_TYPE_SPARSE);
    }

    // Fill left field
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_BERRY_FIELD1))
    {
        // Unlocked no fields
        MetatileFill_TreesOverlapping(0, 1, 7, 8, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(0, 9, 7, TREE_TYPE_DENSE);
        MetatileFill_Tile(0, 10, 6, 14, METATILE_GeneralHub_Grass);
    }
    else if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_BERRY_FIELD2))
    {
        // Unlocked bottom field
        MetatileFill_TreesOverlapping(0, 1, 7, 7, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(0, 7, 7, TREE_TYPE_DENSE);
        MetatileFill_Tile(0, 8, 6, 8, METATILE_GeneralHub_Grass);
    }

    // Remove house
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_LOWER_FLOOR))
    {
        MetatileFill_Tile(7, 10, 11, 14, METATILE_GeneralHub_Grass);
    }
    // Remove 2nd storey from house
    else if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_UPPER_FLOOR))
    {
        // back
        MetatileFill_Tile(7, 10, 11, 10, METATILE_GeneralHub_Grass);
        MetatileSet_Tile(7, 11, 0x260); // left 
        MetatileSet_Tile(11, 11, 0x261); // right
        MetatileFill_Tile(8, 11, 10, 11, 0x209); // tiles

        // front
        MetatileSet_Tile(7, 12, 0x268 | MAPGRID_COLLISION_MASK); // left 
        MetatileSet_Tile(11, 12, 0x269 | MAPGRID_COLLISION_MASK); // right
        MetatileFill_Tile(8, 12, 10, 12, 0x211 | MAPGRID_COLLISION_MASK); // tiles
    }
}

static void RogueHub_UpdateHomeInteriorMetatiles()
{
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_UPPER_FLOOR))
    {
        MetatileSet_Tile(8, 0, 0x20E | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(8, 1, 0x216 | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(6, 0, 7, 0, 0x254 | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(6, 1, 7, 1, 0x25C | MAPGRID_COLLISION_MASK);
    }
}

// Metatile util functions
//

static void MetatileSet_Tile(u16 x, u16 y, u16 tile)
{
    MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, tile);
}

static void MetatileFill_Tile(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u16 tile)
{
    u16 x, y;

    for(x = xStart; x <= xEnd; ++x)
    {
        for(y = yStart; y <= yEnd; ++y)
        {
            MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, tile);
        }
    }
}

static void MetatileFill_TreesOverlapping(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u8 treeType)
{
    u16 x, y, tile;

    for(x = xStart; x <= xEnd; ++x)
    {
        for(y = yStart; y <= yEnd; ++y)
        {
            if(treeType == TREE_TYPE_DENSE)
            {
                if((y % 2) == 0)
                {
                    tile = (x % 2) == 0 ? METATILE_GeneralHub_Tree_TopLeft_Dense : METATILE_GeneralHub_Tree_TopRight_Dense;
                }
                else
                {
                    tile = (x % 2) == 0 ? METATILE_GeneralHub_Tree_BottomLeft_Dense_Overlapped : METATILE_GeneralHub_Tree_BottomRight_Dense_Overlapped;
                }
            }
            else
            {
                if((y % 2) == 0)
                {
                    tile = (x % 2) == 0 ? METATILE_GeneralHub_Tree_TopLeft_Sparse : METATILE_GeneralHub_Tree_TopRight_Sparse;
                }
                else
                {
                    tile = (x % 2) == 0 ? METATILE_GeneralHub_Tree_BottomLeft_Sparse_Overlapped : METATILE_GeneralHub_Tree_BottomRight_Sparse_Overlapped;
                }
            }

            MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, tile | MAPGRID_COLLISION_MASK);
        }
    }
}

static void MetatileFill_TreeStumps(u16 xStart, u16 yStart, u16 xEnd, u8 treeType)
{
    u16 x, tile;

    for(x = xStart; x <= xEnd; ++x)
    {
        if(treeType == TREE_TYPE_DENSE)
            tile = (x % 2) == 0 ? METATILE_GeneralHub_Tree_BottomLeft_Dense : METATILE_GeneralHub_Tree_BottomRight_Dense;
        else
            tile = (x % 2) == 0 ? METATILE_GeneralHub_Tree_BottomLeft_Sparse : METATILE_GeneralHub_Tree_BottomRight_Sparse;

        MapGridSetMetatileIdAt(x + MAP_OFFSET, yStart + MAP_OFFSET, tile | MAPGRID_COLLISION_MASK);
    }
}

static void MetatileFill_TreeCaps(u16 xStart, u16 yStart, u16 xEnd)
{
    u16 x, tile;

    for(x = xStart; x <= xEnd; ++x)
    {
        tile = (x % 2) == 0 ? METATILE_GeneralHub_Tree_TopLeft_CapGrass : METATILE_GeneralHub_Tree_TopRight_CapGrass;
        MapGridSetMetatileIdAt(x + MAP_OFFSET, yStart + MAP_OFFSET, tile);
    }
}

void RogueHub_GetAreaUpgradesMultichoice(struct MenuAction* outList, u8* outCount, u8 listCapcity)
{
    u8 i;
    u8 count;
    u8 upgrades[MAX_HUB_UPGRADE_TREES_PER_AREA];
    u8 area = RogueHub_GetAreaFromCurrentMap();

    RogueHub_GetAvaliableUpgrades(area, &upgrades[0], &count);

    for(i = 0; i < count; ++i)
    {
        outList[i].text = &gRogueHubUpgrades[upgrades[i]].upgradeName[0];
    }

    outList[count].text = gText_MenuExit;
    *outCount = count + 1;
}

void RogueHub_GetUpgradeFromMultichoiceResult()
{
    u16 result = VarGet(VAR_RESULT);

    if(result == MULTI_B_PRESSED)
    {
        VarSet(VAR_RESULT, HUB_UPGRADE_NONE);
    }
    else
    {
        u8 count;
        u8 upgrades[MAX_HUB_UPGRADE_TREES_PER_AREA];
        u8 area = RogueHub_GetAreaFromCurrentMap();

        RogueHub_GetAvaliableUpgrades(area, &upgrades[0], &count);

        if(result < count)
        {
            VarSet(VAR_RESULT, upgrades[result]);
        }
        else
        {
            VarSet(VAR_RESULT, HUB_UPGRADE_NONE);
        }
    }
}

void RogueHub_ApplyHubUpgrade()
{
    u16 result = VarGet(VAR_0x8004);

    if(!RogueHub_HasUpgrade(result))
    {
        RogueHub_SetUpgrade(result, TRUE);
        VarSet(VAR_RESULT, TRUE);
    }
    else
    {
        VarSet(VAR_RESULT, FALSE);
    }
}