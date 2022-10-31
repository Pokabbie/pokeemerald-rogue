#include "global.h"
#include "constants/items.h"

#include "item.h"
#include "random.h"

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
            return GetItemCountInBag(ITEM_SHOP_PRICE_CHARM) * 20;

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
            return GetItemCountInBag(ITEM_SHOP_PRICE_CURSE) * 20;

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

static bool8 BufferContainsValue(u16* buffer, u16 count, u16 value)
{
    u16 i;

    for(i = 0; i < count; ++i)
    {
        if(buffer[i] == value)
            return TRUE;
    }

    return FALSE;
}

static void SelectCharmItemInternal(u16* outBuffer, u16 outCount, u16 firstItem, u16 lastItem)
{
    u16 i;
    u16 itemId;
    u16 itemCount = (lastItem - firstItem + 1);

    for(i = 0; i < outCount; ++i)
    {
        do
        {
            itemId = firstItem + Random() % itemCount;
        }
        while(i < itemCount && BufferContainsValue(outBuffer, i, itemId));

        outBuffer[i] = itemId;
    }
}

void Rogue_SelectCharmItems(u16* outBuffer, u16 count)
{
    SelectCharmItemInternal(outBuffer, count, FIRST_ITEM_CHARM, LAST_ITEM_CHARM);
}

void Rogue_SelectCurseItems(u16* outBuffer, u16 count)
{
    SelectCharmItemInternal(outBuffer, count, FIRST_ITEM_CURSE, LAST_ITEM_CURSE);
}