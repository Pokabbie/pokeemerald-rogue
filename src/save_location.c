#include "global.h"
#include "save_location.h"

#define LIST_END 0xFFFF

static bool32 IsCurMapInLocationList(const u16 *list)
{
    s32 i;
    u16 map = (gSaveBlock1Ptr->location.mapGroup << 8) + gSaveBlock1Ptr->location.mapNum;

    for (i = 0; list[i] != LIST_END; i++)
    {
        if (list[i] == map)
            return TRUE;
    }

    return FALSE;
}

static const u16 sSaveLocationPokeCenterList[] =
{
    LIST_END,
};

static bool32 IsCurMapPokeCenter(void)
{
    return IsCurMapInLocationList(sSaveLocationPokeCenterList);
}

static const u16 sSaveLocationReloadLocList[] = // There's only 1 location, and it's presumed its for the save reload feature for battle tower.
{
    LIST_END,
};

static bool32 IsCurMapReloadLocation(void)
{
    return IsCurMapInLocationList(sSaveLocationReloadLocList);
}

// Nulled out list. Unknown what this would have been.
static const u16 sEmptyMapList[] =
{
    LIST_END,
};

static bool32 IsCurMapInEmptyList(void)
{
    return IsCurMapInLocationList(sEmptyMapList);
}

static void TrySetPokeCenterWarpStatus(void)
{
    if (!IsCurMapPokeCenter())
        gSaveBlock2Ptr->specialSaveWarpFlags &= ~POKECENTER_SAVEWARP;
    else
        gSaveBlock2Ptr->specialSaveWarpFlags |= POKECENTER_SAVEWARP;
}

static void TrySetReloadWarpStatus(void)
{
    if (!IsCurMapReloadLocation())
        gSaveBlock2Ptr->specialSaveWarpFlags &= ~LOBBY_SAVEWARP;
    else
        gSaveBlock2Ptr->specialSaveWarpFlags |= LOBBY_SAVEWARP;
}

// Unknown save warp flag. Never set because map list is empty.
static void TrySetUnknownWarpStatus(void)
{
    if (!IsCurMapInEmptyList())
        gSaveBlock2Ptr->specialSaveWarpFlags &= ~UNK_SPECIAL_SAVE_WARP_FLAG_3;
    else
        gSaveBlock2Ptr->specialSaveWarpFlags |= UNK_SPECIAL_SAVE_WARP_FLAG_3;
}

void TrySetMapSaveWarpStatus(void)
{
    TrySetPokeCenterWarpStatus();
    TrySetReloadWarpStatus();
    TrySetUnknownWarpStatus();
}

// In FRLG, only bits 0, 4, and 5 are set when the pokedex is received.
// Bits 1, 2, 3, and 15 are instead set by SetPostgameFlags.
// These flags are read by Pokemon Colosseum/XD for linking. XD Additionally requires FLAG_SYS_GAME_CLEAR
void SetUnlockedPokedexFlags(void)
{
    gSaveBlock2Ptr->gcnLinkFlags |= (1 << 15);
    gSaveBlock2Ptr->gcnLinkFlags |= (1 << 0);
    gSaveBlock2Ptr->gcnLinkFlags |= (1 << 1);
    gSaveBlock2Ptr->gcnLinkFlags |= (1 << 2);
    gSaveBlock2Ptr->gcnLinkFlags |= (1 << 4);
    gSaveBlock2Ptr->gcnLinkFlags |= (1 << 5);
    gSaveBlock2Ptr->gcnLinkFlags |= (1 << 3);
}

void SetChampionSaveWarp(void)
{
    gSaveBlock2Ptr->specialSaveWarpFlags |= CHAMPION_SAVEWARP;
}
