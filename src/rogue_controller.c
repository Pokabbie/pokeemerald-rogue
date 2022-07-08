#include "rogue_controller.h"

#include "global.h"
#include "event_data.h"

//static void RandomiseWarps(void);


void Rogue_OnNewGame(void)
{
    FlagSet(FLAG_RECEIVED_RUNNING_SHOES);
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_SYS_POKEMON_GET);

    EnableNationalPokedex();
}

void Rogue_OnLoadMap(void)
{
    // Seems to be working? Need to track against flag here though, as this gets called for started maps
    //FlagSet(FLAG_SYS_POKEMON_GET);
    
        //*((s32*)((void*)0)) = 123;

    s32 i;
    struct WarpEvent *warpEvent = gMapHeader.events->warps;
    u8 warpCount = gMapHeader.events->warpCount;
    
    for (i = 0; i < warpCount; i++, warpEvent++)
    {
        if(i == 0)
        {
            // Skip first warp as that should always be left as the entry warp
            continue;
        }

        // Should be Prof lab
        warpEvent->warpId = 0;
        warpEvent->mapNum = 4;
        warpEvent->mapGroup = 0;
    }

    // TODO - Do something
}

//struct WarpEvent
//{
//    s16 x, y;
//    u8 elevation;
//    u8 warpId;
//    u8 mapNum;
//    u8 mapGroup;
//};

//static s8 GetWarpEventAtPosition(struct MapHeader *mapHeader, u16 x, u16 y, u8 elevation)
//{
//    s32 i;
//    struct WarpEvent *warpEvent = mapHeader->events->warps;
//    u8 warpCount = mapHeader->events->warpCount;
//
//    for (i = 0; i < warpCount; i++, warpEvent++)
//    {
//        if ((u16)warpEvent->x == x && (u16)warpEvent->y == y)
//        {
//            if (warpEvent->elevation == elevation || warpEvent->elevation == 0)
//                return i;
//        }
//    }
//    return WARP_ID_NONE;
//}