#include "global.h"
#include "constants/items.h"

#include "item.h"
#include "random.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_charms.h"

static u16 EffectToCharmItem(u8 effectType)
{
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            return ITEM_SHOP_PRICE_CHARM;

        case EFFECT_FLINCH_CHANCE:
            return ITEM_FLINCH_CHARM;

        case EFFECT_CRIT_CHANCE:
            return ITEM_CRIT_CHARM;

        case EFFECT_SHED_SKIN_CHANCE:
            return ITEM_SHED_SKIN_CHARM;

        case EFFECT_WILD_IV_RATE:
            return ITEM_WILD_IV_CHARM;

        case EFFECT_CATCH_RATE:
            return ITEM_CATCHING_CHARM;
    }

    return ITEM_NONE;
}

static u16 EffectToCurseItem(u8 effectType)
{
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            return ITEM_SHOP_PRICE_CURSE;

        case EFFECT_FLINCH_CHANCE:
            return ITEM_FLINCH_CURSE;

        case EFFECT_CRIT_CHANCE:
            return ITEM_CRIT_CURSE;

        case EFFECT_SHED_SKIN_CHANCE:
            return ITEM_SHED_SKIN_CURSE;

        case EFFECT_WILD_IV_RATE:
            return ITEM_WILD_IV_CURSE;

        case EFFECT_CATCH_RATE:
            return ITEM_CATCHING_CURSE;
    }

    return ITEM_NONE;
}

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
    u16 itemCount = EffectToCharmItem(effectType);

    // Custom rate scaling
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            return itemCount * 20;

        case EFFECT_FLINCH_CHANCE:
            return min(itemCount, 9) * 10;

        case EFFECT_SHED_SKIN_CHANCE:
            return min(itemCount, 5) * 20;

        case EFFECT_WILD_IV_RATE:
            return itemCount * 10;
    }

    return itemCount;
}

u16 GetCurseValue(u8 effectType)
{
    u16 itemCount = EffectToCurseItem(effectType);

    // Custom rate scaling
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            return itemCount * 20;

        case EFFECT_FLINCH_CHANCE:
            return min(itemCount, 9) * 10;

        case EFFECT_SHED_SKIN_CHANCE:
            return min(itemCount, 5) * 20;

        case EFFECT_WILD_IV_RATE:
            return itemCount * 10;
    }

    return itemCount;
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

u16 Rogue_NextCharmItem(u16* historyBuffer, u16 historyBufferCount)
{
    u8 effectType;
    u16 itemId;

    do
    {
        effectType = Random() % EFFECT_COUNT;
        itemId = EffectToCharmItem(effectType);
    }
    while(itemId == ITEM_NONE || BufferContainsValue(historyBuffer, historyBufferCount, effectType));

    historyBuffer[historyBufferCount] = effectType;

    return itemId;
}

u16 Rogue_NextCurseItem(u16* historyBuffer, u16 historyBufferCount)
{
    u8 effectType;
    u16 itemId;

    do
    {
        effectType = Random() % EFFECT_COUNT;
        itemId = EffectToCurseItem(effectType);
    }
    while(itemId == ITEM_NONE || BufferContainsValue(historyBuffer, historyBufferCount, effectType));

    historyBuffer[historyBufferCount] = effectType;

    return itemId;
}