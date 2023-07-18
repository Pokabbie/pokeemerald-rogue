#include "global.h"
#include "constants/layouts.h"
#include "constants/metatile_labels.h"
#include "constants/script_menu.h"

#include "event_data.h"
#include "fieldmap.h"
#include "menu.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"

#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_followmon.h"

#define TREE_TYPE_DENSE     0
#define TREE_TYPE_SPARSE    1

#define HOME_AREA_DISPLAY_MONS 4

static void MetatileSet_Tile(u16 xStart, u16 yStart, u16 tile);
static void MetatileFill_Tile(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u16 tile);
static void MetatileFill_TreesOverlapping(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u8 treeType);
static void MetatileFill_TreeStumps(u16 xStart, u16 yStart, u16 xEnd, u8 treeType);
static void MetatileFill_TreeCaps(u16 xStart, u16 yStart, u16 xEnd);

static void MetatileFill_CommonWarpExitVertical(u16 xStart, u16 yStart);
static void MetatileFill_CommonWarpExitHorizontal(u16 xStart, u16 yStart);

static void RogueHub_UpdateTownSquareAreaMetatiles();
static void RogueHub_UpdateAdventureEntranceAreaMetatiles();
static void RogueHub_UpdateHomeAreaMetatiles();
static void RogueHub_UpdateHomeInteriorMetatiles();
static void RogueHub_UpdateFarmingAreaMetatiles();

void RogueHub_Enter()
{
    //AGB_ASSERT(gRogueGlobalData.hubMap == NULL);
    //gRogueGlobalData.hubMap = AllocZeroed(sizeof(struct RogueHubMap));
}

void RogueHub_Exit()
{
    //AGB_ASSERT(gRogueGlobalData.hubMap != NULL);
    //free(gRogueGlobalData.hubMap);
    //gRogueGlobalData.hubMap = NULL;
}

void RogueHub_ClearProgress()
{
    memset(&gRogueGlobalData.hubMap, 0, sizeof(gRogueGlobalData.hubMap));

    // Build default area at 0,0
    RogueHub_BuildArea(HUB_AREA_TOWN_SQUARE, 0, 0);

    // TODO - Select placement based on trainer ID
    RogueHub_BuildArea(HUB_AREA_ADVENTURE_ENTRANCE, 1, 0);
}

bool8 RogueHub_HasUpgrade(u16 upgradeId)
{
    u16 idx = upgradeId / 8;
    u16 bit = upgradeId % 8;

    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(gRogueGlobalData.hubMap.upgradeFlags));
    return (gRogueGlobalData.hubMap.upgradeFlags[idx] & bitMask) != 0;
}

void RogueHub_SetUpgrade(u16 upgradeId, bool8 state)
{
    u16 idx = upgradeId / 8;
    u16 bit = upgradeId % 8;

    u8 bitMask = 1 << bit;
    
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueGlobalData.hubMap.upgradeFlags));
    if(state)
    {
        gRogueGlobalData.hubMap.upgradeFlags[idx] |= bitMask;
    }
    else
    {
        gRogueGlobalData.hubMap.upgradeFlags[idx] &= ~bitMask;
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

bool8 RogueHub_HasAreaBuilt(u8 area)
{
    u16 idx = area / 8;
    u16 bit = area % 8;

    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(gRogueGlobalData.hubMap.areaBuiltFlags));
    return (gRogueGlobalData.hubMap.areaBuiltFlags[idx] & bitMask) != 0;
}

void RogueHub_BuildArea(u8 area, s8 x, s8 y)
{
    u16 idx = area / 8;
    u16 bit = area % 8;

    u8 bitMask = 1 << bit;
    
    AGB_ASSERT(idx < ARRAY_COUNT(gRogueGlobalData.hubMap.areaBuiltFlags));
    
    gRogueGlobalData.hubMap.areaBuiltFlags[idx] |= bitMask;
    gRogueGlobalData.hubMap.areaCoords[area].x = x;
    gRogueGlobalData.hubMap.areaCoords[area].y = y;
}

bool8 RogueHub_HasAreaBuildRequirements(u8 area)
{
    u8 i;
    u8 check;

    for(i = 0; i < HUB_UPGRADE_MAX_REQUIREMENTS; ++i)
    {
        check = gRogueHubAreas[area].requiredUpgrades[i];
        if(check == HUB_UPGRADE_NONE)
            break;

        if(!RogueHub_HasUpgrade(check))
            return FALSE;
    }

    return TRUE;
}

u8 RogueHub_FindAreaAtCoord(s8 x, s8 y)
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(RogueHub_HasAreaBuilt(i))
        {
            if(gRogueGlobalData.hubMap.areaCoords[i].x == x && gRogueGlobalData.hubMap.areaCoords[i].y == y)
                return i;
        }
    }

    return HUB_AREA_NONE;
}

static bool8 CanAreaConnect(u8 area, u8 dir)
{
    // If both warps aren't set to 0, this is a valid warp
    return gRogueHubAreas[area].connectionWarps[dir][0] != 0 || gRogueHubAreas[area].connectionWarps[dir][1] != 0;
}

static u8 InvertConnDirection(u8 dir)
{
    switch (dir)
    {
    case HUB_AREA_CONN_SOUTH:
        return HUB_AREA_CONN_NORTH;
    case HUB_AREA_CONN_NORTH:
        return HUB_AREA_CONN_SOUTH;
    case HUB_AREA_CONN_WEST:
        return HUB_AREA_CONN_EAST;
    case HUB_AREA_CONN_EAST:
        return HUB_AREA_CONN_WEST;
    }

    return dir;
}

static void IncrementCoordsByDirection(struct Coords8* coords, u8 dir)
{
    switch (dir)
    {
    case HUB_AREA_CONN_SOUTH:
        --coords->y;
        break;
    case HUB_AREA_CONN_NORTH:
        ++coords->y;
        break;
    case HUB_AREA_CONN_WEST:
        --coords->x;
        break;
    case HUB_AREA_CONN_EAST:
        ++coords->x;
        break;
    }
}

bool8 RogueHub_AreaHasFreeConnection(u8 area, u8 dir)
{
    if(CanAreaConnect(area, dir))
    {
        struct Coords8 pos;
        pos.x = gRogueGlobalData.hubMap.areaCoords[area].x;
        pos.y = gRogueGlobalData.hubMap.areaCoords[area].y;
        IncrementCoordsByDirection(&pos, dir);

        return RogueHub_FindAreaAtCoord(pos.x, pos.y) == HUB_AREA_NONE;
    }

    return FALSE;
}

u8 RogueHub_GetAreaAtConnection(u8 area, u8 dir)
{
    if(CanAreaConnect(area, dir))
    {
        struct Coords8 pos;
        pos.x = gRogueGlobalData.hubMap.areaCoords[area].x;
        pos.y = gRogueGlobalData.hubMap.areaCoords[area].y;
        IncrementCoordsByDirection(&pos, dir);

        return RogueHub_FindAreaAtCoord(pos.x, pos.y);
    }

    return HUB_AREA_NONE;
}

static u8 GetAreaForLayout(u16 layout)
{
    u8 area;

    for(area = HUB_AREA_FIRST; area < HUB_AREA_COUNT; ++area)
    {
        if(gRogueHubAreas[area].primaryMapLayout == layout)
            return area;
    }

    return HUB_AREA_NONE;
}

u8 RogueHub_GetAreaFromCurrentMap()
{
    return GetAreaForLayout(gMapHeader.mapLayoutId);
}

void RogueHub_GetAvaliableAreasToBuild(u8* outAreas, u8* outCount)
{
    u8 i;
    u8 count = 0;
    u8 currentArea = RogueHub_GetAreaFromCurrentMap();

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        // Avoid any bugs with self referenced map loops
        if(i == currentArea)
            continue;

        if(!RogueHub_HasAreaBuilt(i) && RogueHub_HasAreaBuildRequirements(i))
        {
            outAreas[count++] = i;
        }
    }

    *outCount = count;
}

void RogueHub_GetAvaliableDirectionsToBuild(u8 fromArea, u8 toArea, u8* outDirs, u8* outCount)
{
    u8 i;
    u8 dir, invDir;
    u8 count = 0;
    u8 orderedDirs[] = 
    {
        HUB_AREA_CONN_NORTH,
        HUB_AREA_CONN_EAST,
        HUB_AREA_CONN_SOUTH,
        HUB_AREA_CONN_WEST,
    };

    if(fromArea != HUB_AREA_NONE && toArea != HUB_AREA_NONE && fromArea != toArea)
    {
        for(i = 0; i < ARRAY_COUNT(orderedDirs); ++i)
        {
            dir = orderedDirs[i];
            invDir = InvertConnDirection(dir);

            if(RogueHub_AreaHasFreeConnection(fromArea, dir) && CanAreaConnect(toArea, invDir))
            {
                outDirs[count++] = dir;
            }
        }
    }
    
    *outCount = count;
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

void RogueHub_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp)
{
    u8 area = GetAreaForLayout(mapHeader->mapLayoutId);

    if(area != HUB_AREA_NONE)
    {
        u8 dir;
        u8 warpArea = HUB_AREA_NONE;
        u8 enterDir = HUB_AREA_CONN_SOUTH;

        for(dir = HUB_AREA_CONN_SOUTH; dir <= HUB_AREA_CONN_EAST; ++dir)
        {
            if(CanAreaConnect(area, dir))
            {
                if(gRogueHubAreas[area].connectionWarps[dir][0] == warpId || gRogueHubAreas[area].connectionWarps[dir][1] == warpId)
                    break;
            }
        }

        // We're trying to warp out into a valid direction
        if(dir <= HUB_AREA_CONN_EAST)
        {
            warpArea = RogueHub_GetAreaAtConnection(area, dir);
            enterDir = InvertConnDirection(dir);
        }

        // Warping into a valid area, so setup a valid warp
        if(warpArea != HUB_AREA_NONE)
        {
            warp->mapGroup = gRogueHubAreas[warpArea].primaryMapGroup;
            warp->mapNum = gRogueHubAreas[warpArea].primaryMapNum;
            warp->warpId = gRogueHubAreas[warpArea].connectionWarps[enterDir][0];
        }
        else
        {
            // TODO - We should do safe warp back to main area if we get here
        }
    }
}

bool8 RogueHub_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection)
{
    // Convert from CONNECTION_ to HUB_AREA_CONN_
    u8 area = GetAreaForLayout(mapHeader->mapLayoutId);

    if(area != HUB_AREA_NONE)
    {
        u8 connDir = connection->direction - 1;
        
        // Hide any connections, unless we actually have the connection built
        return RogueHub_GetAreaAtConnection(area, connDir) != HUB_AREA_NONE;
    }

    return TRUE;
}

void RogueHub_ApplyMapMetatiles()
{
    switch (gMapHeader.mapLayoutId)
    {
    case LAYOUT_ROGUE_AREA_TOWN_SQUARE:
        RogueHub_UpdateTownSquareAreaMetatiles();
        break;
    case LAYOUT_ROGUE_AREA_ADVENTURE_ENTRANCE:
        RogueHub_UpdateAdventureEntranceAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_HOME:
        RogueHub_UpdateHomeAreaMetatiles();
        break;
    case LAYOUT_ROGUE_INTERIOR_HOME:
        RogueHub_UpdateHomeInteriorMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_FARMING_FIELD:
        RogueHub_UpdateFarmingAreaMetatiles();
        break;
    
    default:
        break;
    }

}

static void RogueHub_UpdateTownSquareAreaMetatiles()
{
    // Remove connectionss
    if(RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_TreesOverlapping(17, 0, 22, 0, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(17, 1, 22, TREE_TYPE_DENSE);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(28, 11);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitVertical(18, 22);
        MetatileFill_TreeCaps(18, 23, 21);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(0, 17);
    }
}

static void RogueHub_UpdateAdventureEntranceAreaMetatiles()
{
    // Remove connectionss
    if(RogueHub_GetAreaAtConnection(HUB_AREA_ADVENTURE_ENTRANCE, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(18, 9);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_ADVENTURE_ENTRANCE, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitVertical(8, 14);
        MetatileFill_TreeCaps(8, 15, 11);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_ADVENTURE_ENTRANCE, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(0, 9);
    }
}

static void RogueHub_UpdateHomeAreaMetatiles()
{
    // Remove connections
    if(RogueHub_GetAreaAtConnection(HUB_AREA_HOME, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(18, 15);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_HOME, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_TreeCaps(8, 19, 11);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_HOME, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(0, 15);
    }


    // Fill right field
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_GRASS_FIELD))
    {
        MetatileFill_TreesOverlapping(8, 2, 19, 9, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(8, 9, 11, TREE_TYPE_DENSE);
        MetatileFill_TreesOverlapping(12, 10, 19, 12, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(12, 13, 19, TREE_TYPE_DENSE);
    }

    // Fill shed (must unlock after field)
    else if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_GRASS_FIELD_SHED))
    {
        MetatileFill_TreesOverlapping(14, 2, 17, 4, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(14, 5, 17, TREE_TYPE_DENSE);
    }

    // Fill left field
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_BERRY_FIELD1))
    {
        // Unlocked no fields
        MetatileFill_TreesOverlapping(0, 5, 7, 8, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(0, 9, 7, TREE_TYPE_DENSE);
        MetatileFill_Tile(0, 10, 6, 14, METATILE_GeneralHub_Grass);
    }
    else if(!RogueHub_HasUpgrade(HUB_UPGRADE_HOME_BERRY_FIELD2))
    {
        // Unlocked right field
        MetatileFill_Tile(4, 9, 6, 14, METATILE_GeneralHub_Grass);
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
        // Replace with back wall
        MetatileSet_Tile(8, 0, 0x20E | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(8, 1, 0x216 | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(6, 0, 7, 0, 0x254 | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(6, 1, 7, 1, 0x25C | MAPGRID_COLLISION_MASK);

        // Replace with PC
        MetatileSet_Tile(8, 0, 0x251 | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(8, 1, 0x259 | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(8, 2, 0x261);

        MetatileSet_Tile(7, 1, 0x258 | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(7, 2, 0x260);

    }
}

static void RogueHub_UpdateFarmingAreaMetatiles()
{
    // Remove connectionss
    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_TreesOverlapping(7, 0, 12, 0, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(7, 1, 12, TREE_TYPE_DENSE);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(18, 9);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitVertical(8, 14);
        MetatileFill_TreeCaps(8, 15, 11);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_CommonWarpExitHorizontal(0, 9);
    }


    // Fill right field
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD))
    {
        MetatileFill_TreesOverlapping(12, 1, 19, 6, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(13, 7, 19, TREE_TYPE_DENSE);
        MetatileFill_TreeStumps(12, 7, 12, TREE_TYPE_SPARSE);
        
        MetatileFill_Tile(12, 8, 19, 8, METATILE_GeneralHub_Grass);
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

static void MetatileFill_CommonWarpExit(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd)
{
    u16 x, y;
    u32 tile;

    for(x = xStart; x <= xEnd; ++x)
    {
        for(y = yStart; y <= yEnd; ++y)
        {
            tile = MapGridGetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET);

            switch (tile)
            {
            // Tree tops
            case 0x00E:
            case 0x00F:
            case 0x03E:
            case 0x03F:
            case 0x047:
                MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_GeneralHub_Grass);
                break;

            // Tree bottoms
            case 0x016:
            case 0x017:
            case 0x03D:
                MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_GeneralHub_Grass);
                break;

            // Tree overlap tree left
            case 0x1F4:
                MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, 0x1CE);
                break;
            // Tree overlap tree right
            case 0x1F5:
                MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, 0x1CF);
                break;

            // Grass warps/shadows
            case METATILE_GeneralHub_GrassWarpNorth:
            case METATILE_GeneralHub_GrassWarpEast:
            case METATILE_GeneralHub_GrassWarpSouth:
            case METATILE_GeneralHub_GrassWarpWest:
            case METATILE_GeneralHub_GrassWarpShadowNorth:
            case METATILE_GeneralHub_GrassWarpShadowEast:
            case METATILE_GeneralHub_GrassWarpShadowSouth:
            case METATILE_GeneralHub_GrassWarpShadowWest:
                MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, METATILE_GeneralHub_Grass);
                break;
            
            default:
                break;
            }
        }
    }
}

static void MetatileFill_CommonWarpExitVertical(u16 xStart, u16 yStart)
{
    MetatileFill_CommonWarpExit(xStart, yStart, xStart + 3, yStart + 1);
}

static void MetatileFill_CommonWarpExitHorizontal(u16 xStart, u16 yStart)
{
    MetatileFill_CommonWarpExit(xStart, yStart, xStart + 1, yStart + 4);
}

void RogueHub_GetAreaBuildsMultichoice(struct MenuAction* outList, u8* outCount, u8 listCapcity)
{
    u8 i;
    u8 count;
    u8 areas[HUB_AREA_COUNT];

    RogueHub_GetAvaliableAreasToBuild(&areas[0], &count);

    for(i = 0; i < count; ++i)
    {
        outList[i].text = &gRogueHubAreas[areas[i]].areaName[0];
    }

    outList[count].text = gText_MenuExit;
    *outCount = count + 1;
}

void RogueHub_GetAreaBuildDirectionMultichoice(struct MenuAction* outList, u8* outCount, u8 listCapcity)
{
    u8 i;
    u8 count;
    u8 directions[4];
    u16 hubAreaToBuild = VarGet(VAR_0x8004);

    if(hubAreaToBuild >= HUB_AREA_COUNT)
    {
        hubAreaToBuild = HUB_AREA_NONE;
    }

    RogueHub_GetAvaliableDirectionsToBuild(RogueHub_GetAreaFromCurrentMap(), hubAreaToBuild, &directions[0], &count);

    for(i = 0; i < count; ++i)
    {
        switch (directions[i])
        {
            case HUB_AREA_CONN_SOUTH:
                outList[i].text = &gText_ExpandSouth[0];
                break;
            case HUB_AREA_CONN_NORTH:
                outList[i].text = &gText_ExpandNorth[0];
                break;
            case HUB_AREA_CONN_EAST:
                outList[i].text = &gText_ExpandEast[0];
                break;
            case HUB_AREA_CONN_WEST:
                outList[i].text = &gText_ExpandWest[0];
                break;
            default:
                // Should never reach here
                outList[i].text = &gText_Toss2[0];
                break;
        }
    }

    outList[count].text = gText_MenuExit;
    *outCount = count + 1;
}

void RogueHub_GetAreaFromMultichoiceResult()
{
    u16 result = VarGet(VAR_RESULT);

    if(result == MULTI_B_PRESSED)
    {
        VarSet(VAR_RESULT, HUB_AREA_NONE);
    }
    else
    {
        u8 count;
        u8 areas[HUB_AREA_COUNT];

        RogueHub_GetAvaliableAreasToBuild(&areas[0], &count);

        if(result < count)
        {
            VarSet(VAR_RESULT, areas[result]);
        }
        else
        {
            VarSet(VAR_RESULT, HUB_AREA_NONE);
        }
    }
}

void RogueHub_GetAreaDirectionFromMultichoiceResult()
{
    u16 result = VarGet(VAR_RESULT);
    u16 hubAreaToBuild = VarGet(VAR_0x8004);

    if(hubAreaToBuild >= HUB_AREA_COUNT)
    {
        hubAreaToBuild = HUB_AREA_NONE;
    }

    if(result == MULTI_B_PRESSED)
    {
        VarSet(VAR_RESULT, HUB_AREA_CONN_NONE);
    }
    else
    {
        u8 count;
        u8 directions[4];

        RogueHub_GetAvaliableDirectionsToBuild(RogueHub_GetAreaFromCurrentMap(), hubAreaToBuild, &directions[0], &count);

        if(result < count)
        {
            VarSet(VAR_RESULT, directions[result]);
        }
        else
        {
            VarSet(VAR_RESULT, HUB_AREA_CONN_NONE);
        }
    }
}

void RogueHub_BuildHubArea()
{
    u16 result = VarGet(VAR_0x8004);
    u8 dir = VarGet(VAR_0x8005);
    u8 currentArea = RogueHub_GetAreaFromCurrentMap();

    if(currentArea != HUB_AREA_NONE && !RogueHub_HasAreaBuilt(result) && dir <= HUB_AREA_CONN_EAST)
    {
        struct Coords8 pos;
        pos.x = gRogueGlobalData.hubMap.areaCoords[currentArea].x;
        pos.y = gRogueGlobalData.hubMap.areaCoords[currentArea].y;
        IncrementCoordsByDirection(&pos, dir);

        RogueHub_BuildArea(result, pos.x, pos.y);
        VarSet(VAR_RESULT, TRUE);
    }
    else
    {
        VarSet(VAR_RESULT, FALSE);
    }
}

void RogueHub_BufferAreaDescriptionText()
{
    u16 upgrade = VarGet(VAR_0x8004);

    VarSet(VAR_RESULT, FALSE);

    if(upgrade < HUB_AREA_COUNT && gRogueHubAreas[upgrade].descText)
    {
        StringCopy(gStringVar4, gRogueHubAreas[upgrade].descText);
        VarSet(VAR_RESULT, TRUE);
    }
}

void RogueHub_BufferAreaCompleteText()
{
    u16 upgrade = VarGet(VAR_0x8004);

    VarSet(VAR_RESULT, FALSE);

    if(upgrade < HUB_AREA_COUNT && gRogueHubAreas[upgrade].completeText)
    {
        StringCopy(gStringVar4, gRogueHubAreas[upgrade].completeText);
        VarSet(VAR_RESULT, TRUE);
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

void RogueHub_BufferUpgradeDescriptionText()
{
    u16 upgrade = VarGet(VAR_0x8004);

    VarSet(VAR_RESULT, FALSE);

    if(upgrade < HUB_UPGRADE_COUNT && gRogueHubUpgrades[upgrade].descText)
    {
        StringCopy(gStringVar4, gRogueHubUpgrades[upgrade].descText);
        VarSet(VAR_RESULT, TRUE);
    }
}

void RogueHub_BufferUpgradeCompleteText()
{
    u16 upgrade = VarGet(VAR_0x8004);

    VarSet(VAR_RESULT, FALSE);

    if(upgrade < HUB_UPGRADE_COUNT && gRogueHubUpgrades[upgrade].completeText)
    {
        StringCopy(gStringVar4, gRogueHubUpgrades[upgrade].completeText);
        VarSet(VAR_RESULT, TRUE);
    }
}

void RogueHub_SetRandomFollowMonsFromPC()
{
    // Try get from current box first
    u8 tryCount;
    u16 species;
    u32 checkMask;
    u32 tryMask = 0;
    u8 foundCount = 0;

    // Start with the final box as thats where most players keep their "cool" mons
    u8 boxId = TOTAL_BOXES_COUNT - 1;

    for(tryCount = 0; tryCount < 64; ++tryCount)
    {
        u8 pos = Random() % IN_BOX_COUNT;
        checkMask = (1 << pos);

        // If we've gotten this far, just try to take from the current box
        if(tryCount == 32 && boxId != StorageGetCurrentBox())
        {
            tryMask = 0;
            boxId = StorageGetCurrentBox();
        }

        // If we haven't already tried this pos
        if((tryMask & checkMask) == 0)
        {
            tryMask |= checkMask;
            species = GetBoxMonDataAt(boxId, pos, MON_DATA_SPECIES);

            if(species != SPECIES_NONE)
            {
                FollowMon_SetGraphics(foundCount, species, GetBoxMonDataAt(boxId, pos, MON_DATA_IS_SHINY));
                if(++foundCount >= HOME_AREA_DISPLAY_MONS)
                    break;
            }
        }
    }

    // Fill in the rest of the slots with nothing
    for(; foundCount < HOME_AREA_DISPLAY_MONS; ++foundCount)
    {
        FollowMon_SetGraphics(foundCount, SPECIES_NONE, FALSE);
    }
}