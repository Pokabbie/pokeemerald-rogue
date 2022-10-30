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
    u16 count;

    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            break;

        case EFFECT_FLINCH_CHANCE:
            return min(GetItemCountInBag(ITEM_FLINCH_CHARM), 9) * 10;

        case EFFECT_CRIT_CHANCE:
            count = GetItemCountInBag(ITEM_CRIT_CHARM);

            if(count != 0)
            {
                // Free focus energy + extra stages
                return 2 + (count - 1);
            }
            break;

        case EFFECT_SHED_SKIN_CHANCE:
            return min(GetItemCountInBag(ITEM_SHED_SKIN_CHARM), 5) * 20;
    }

    return 0;
}

u16 GetCurseValue(u8 effectType)
{
    u16 count;

    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            break;

        case EFFECT_FLINCH_CHANCE:
            return min(GetItemCountInBag(ITEM_FLINCH_CURSE), 9) * 10;

        case EFFECT_CRIT_CHANCE:
            count = GetItemCountInBag(ITEM_CRIT_CURSE);

            if(count != 0)
            {
                // Free focus energy + extra stages
                return 2 + (count - 1);
            }
            break;

        case EFFECT_SHED_SKIN_CHANCE:
            return min(GetItemCountInBag(ITEM_SHED_SKIN_CURSE), 5) * 20;
    }

    return 0;
}