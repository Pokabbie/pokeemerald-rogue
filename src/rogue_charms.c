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

        case EFFECT_SERENE_GRACE_CHANCE:
            return ITEM_GRACE_CHARM;

        // Unused
        // EFFECT_PARTY_SIZE
        // EFFECT_EVERSTONE_EVOS
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

        case EFFECT_SERENE_GRACE_CHANCE:
            return ITEM_GRACE_CURSE;

        case EFFECT_PARTY_SIZE:
            return ITEM_PARTY_CURSE;
        
        case EFFECT_EVERSTONE_EVOS:
            return ITEM_EVERSTONE_CURSE;
    }

    return ITEM_NONE;
}

static u16 CalcValueInternal(u8 effectType, u16 itemId, bool8 isCurse)
{
    u32 itemCount = min(100, (itemId == ITEM_NONE ? 0 : GetItemCountInBag(itemId)));

    // Custom rate scaling
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            return itemCount * 20;

        case EFFECT_FLINCH_CHANCE:
            return min(itemCount * (isCurse ? 5 : 10), 90);

        case EFFECT_SHED_SKIN_CHANCE:
            return min(itemCount * (isCurse ? 15 : 20), 100);

        case EFFECT_WILD_IV_RATE:
            return itemCount * 10;
            
        case EFFECT_CATCH_RATE:
            return itemCount * (isCurse ? 25 : 100);

        case EFFECT_SERENE_GRACE_CHANCE:
            return itemCount * (isCurse ? 50 : 75);
    }

    return itemCount;
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
    return CalcValueInternal(effectType, EffectToCharmItem(effectType), FALSE);
}

u16 GetCurseValue(u8 effectType)
{
    return CalcValueInternal(effectType, EffectToCurseItem(effectType), TRUE);
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

u16 Rogue_GetMaxPartySize(void)
{
    u16 count = min(GetCurseValue(EFFECT_PARTY_SIZE), PARTY_SIZE - 1);
    return PARTY_SIZE - count;
}

bool8 IsEffectDisabled(u8 effectType, bool8 isCurse)
{
    // These effects cannot be given out during runs
    //
    switch(effectType)
    {
        case EFFECT_PARTY_SIZE:
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
    while(itemId == ITEM_NONE || IsEffectDisabled(effectType, FALSE) || BufferContainsValue(historyBuffer, historyBufferCount, effectType));

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
    while(itemId == ITEM_NONE || IsEffectDisabled(effectType, TRUE) || BufferContainsValue(historyBuffer, historyBufferCount, effectType));

    historyBuffer[historyBufferCount] = effectType;

    return itemId;
}