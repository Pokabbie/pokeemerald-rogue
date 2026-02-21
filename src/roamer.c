#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "roamer.h"

// Despite having a variable to track it, the roamer is
// hard-coded to only ever be in map group 0
#define ROAMER_MAP_GROUP 0

enum
{
    MAP_GRP, // map group
    MAP_NUM, // map number
};

void ClearRoamerData(void)
{
}

void ClearRoamerLocationData(void)
{
}

static void CreateInitialRoamerMon(bool16 createLatios)
{
}

// gSpecialVar_0x8004 here corresponds to the options in the multichoice MULTI_TV_LATI (0 for 'Red', 1 for 'Blue')
void InitRoamer(void)
{
    ClearRoamerData();
    ClearRoamerLocationData();
    CreateInitialRoamerMon(gSpecialVar_0x8004);
}

void UpdateLocationHistoryForRoamer(void)
{
}

void RoamerMoveToOtherLocationSet(void)
{
}

void RoamerMove(void)
{
}

bool8 IsRoamerAt(u8 mapGroup, u8 mapNum)
{
    return FALSE;
}

void CreateRoamerMonInstance(void)
{
}

bool8 TryStartRoamerEncounter(void)
{
    if (IsRoamerAt(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum) == TRUE && (Random() % 4) == 0)
    {
        CreateRoamerMonInstance();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void UpdateRoamerHPStatus(struct Pokemon *mon)
{
}

void SetRoamerInactive(void)
{
}

void GetRoamerLocation(u8 *mapGroup, u8 *mapNum)
{
    *mapGroup = 0;
    *mapNum = 0;
}
