#include "global.h"
#include "constants/items.h"

//#include "battle.h"
//#include "event_data.h"
//#include "data.h"
#include "item.h"
//#include "money.h"
//#include "pokedex.h"
//#include "string_util.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_charms.h"

bool8 IsCharmActive(u8 effectType)
{
    return GetCharmValue(effectType) != 0;
}

bool8 IsCurseActive(u8 effectType)
{
    return GetCurseValue(effectType) != 0;
}

u16 GetCharmValue(u8 effectType)
{
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            break;

        case EFFECT_FLINCH_CHANCE:
            return min(GetItemCountInBag(ITEM_FLINCH_CHARM), 10) * 10;
    }

    return 0;
}

u16 GetCurseValue(u8 effectType)
{
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            break;

        case EFFECT_FLINCH_CHANCE:
            return min(GetItemCountInBag(ITEM_FLINCH_CURSE), 10) * 10;
    }

    return 0;
}