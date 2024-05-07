#include "global.h"
#include "constants/items.h"

#include "item.h"
#include "overworld.h"
#include "random.h"

#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_popup.h"

EWRAM_DATA u16 gCharmValues[EFFECT_COUNT];
EWRAM_DATA u16 gCurseValues[EFFECT_COUNT];

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

        case EFFECT_WILD_ENCOUNTER_COUNT:
            return ITEM_WILD_ENCOUNTER_CHARM;

        case EFFECT_MOVE_PRIORITY_CHANCE:
            return ITEM_MOVE_PRIORITY_CHARM;

        case EFFECT_ENDURE_CHANCE:
            return ITEM_ENDURE_CHARM;

        case EFFECT_EXTRA_LIFE:
            return ITEM_SACRED_ASH;

        // Unused
        // EFFECT_PARTY_SIZE
        // EFFECT_EVERSTONE_EVOS
        // EFFECT_BATTLE_ITEM_BAN
        // EFFECT_SPECIES_CLAUSE
        // EFFECT_ITEM_SHUFFLE
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

        case EFFECT_WILD_ENCOUNTER_COUNT:
            return ITEM_WILD_ENCOUNTER_CURSE;

        case EFFECT_MOVE_PRIORITY_CHANCE:
            return ITEM_MOVE_PRIORITY_CURSE;

        case EFFECT_ENDURE_CHANCE:
            return ITEM_ENDURE_CURSE;

        // Just curse effects
        case EFFECT_PARTY_SIZE:
            return ITEM_PARTY_CURSE;
        
        case EFFECT_EVERSTONE_EVOS:
            return ITEM_EVERSTONE_CURSE;
        
        case EFFECT_BATTLE_ITEM_BAN:
            return ITEM_BATTLE_ITEM_CURSE;
        
        case EFFECT_SPECIES_CLAUSE:
            return ITEM_SPECIES_CLAUSE_CURSE;
        
        case EFFECT_ITEM_SHUFFLE:
            return ITEM_ITEM_SHUFFLE_CURSE;
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
            return itemCount * (isCurse ? 40 : 40);

        case EFFECT_FLINCH_CHANCE:
            return min(itemCount * (isCurse ? 10 : 10), 90);

        case EFFECT_SHED_SKIN_CHANCE:
            return min(itemCount * (isCurse ? 15 : 20), 100);

        case EFFECT_WILD_IV_RATE:
            return itemCount * 20;
            
        case EFFECT_CATCH_RATE:
            return itemCount * (isCurse ? 25 : 100);

        case EFFECT_SERENE_GRACE_CHANCE:
            return itemCount * (isCurse ? 50 : 75);

        case EFFECT_WILD_ENCOUNTER_COUNT:
            return itemCount * (isCurse ? 1 : 2);

        case EFFECT_MOVE_PRIORITY_CHANCE:
            return itemCount * (isCurse ? 10 : 10);

        case EFFECT_ENDURE_CHANCE:
            return itemCount * (isCurse ? 20 : 20);
    }

    return itemCount;
}

void RecalcCharmCurseValues(void)
{
    u8 effectType;
    DebugPrint("Recalcing Charm&Curse Values");

    for(effectType = 0; effectType < EFFECT_COUNT; ++effectType)
    {
        gCharmValues[effectType] = CalcValueInternal(effectType, EffectToCharmItem(effectType), FALSE);
        gCurseValues[effectType] = CalcValueInternal(effectType, EffectToCurseItem(effectType), TRUE);
        DebugPrintf("[%d] charm:%d curse:%d", effectType, gCharmValues[effectType], gCurseValues[effectType]);
    }
}

bool8 IsCharmActive(u8 effectType)
{
    return GetCharmValue(effectType) != 0;
}

bool8 IsCurseActive(u8 effectType)
{
    return GetCurseValue(effectType) != 0;
}

bool8 AnyCharmsActive()
{
    u8 effectType;
    u16 itemId;

    for(effectType = 0; effectType < EFFECT_COUNT; ++effectType)
    {
        if(IsCharmActive(effectType))
            return TRUE;
    }

    return FALSE;
}

bool8 AnyCursesActive()
{
    u8 effectType;
    u16 itemId;

    for(effectType = 0; effectType < EFFECT_COUNT; ++effectType)
    {
        if(IsCharmActive(effectType))
            return TRUE;
    }

    return FALSE;
}

u16 GetCharmValue(u8 effectType)
{
    if(!Rogue_IsRunActive())
        return 0;

    return gCharmValues[effectType];
}

u16 GetCurseValue(u8 effectType)
{
    if(!Rogue_IsRunActive())
        return 0;

    return gCurseValues[effectType];
}

void Rogue_RemoveCharmsFromBag(void)
{
    u8 effectType;
    u16 itemId;

    for(effectType = 0; effectType < EFFECT_COUNT; ++effectType)
    {
        itemId = EffectToCharmItem(effectType);

        if(itemId != ITEM_NONE)
        {
            u16 count = GetItemCountInBag(itemId);

            if(count != 0)
                RemoveBagItem(itemId, count);
        }
    }
}

void Rogue_RemoveCursesFromBag(void)
{
    u8 effectType;
    u16 itemId;

    for(effectType = 0; effectType < EFFECT_COUNT; ++effectType)
    {
        itemId = EffectToCurseItem(effectType);

        if(itemId != ITEM_NONE)
        {
            u16 count = GetItemCountInBag(itemId);

            if(count != 0)
                RemoveBagItem(itemId, count);
        }
    }
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
    if(Rogue_IsCatchingContestActive())
    {
        return 1;
    }
    else
    {
        u16 count = min(GetCurseValue(EFFECT_PARTY_SIZE), PARTY_SIZE - 1);
        return PARTY_SIZE - count;
    }
}

bool8 IsEffectDisabled(u8 effectType, bool8 isCurse)
{
    // These effects cannot be given out during runs
    //
    switch(effectType)
    {
        case EFFECT_PARTY_SIZE:
        case EFFECT_EXTRA_LIFE:
            return TRUE;

        // Disable these effects, once we already have one (They don't stack)
        case EFFECT_EVERSTONE_EVOS:
        case EFFECT_BATTLE_ITEM_BAN:
        case EFFECT_SPECIES_CLAUSE:
        case EFFECT_ITEM_SHUFFLE:
            if(isCurse)
                return CheckBagHasItem(EffectToCurseItem(effectType), 1);
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


void Rogue_ExecuteExtraLife()
{
    u16 charmItemId = EffectToCharmItem(EFFECT_EXTRA_LIFE);
    AGB_ASSERT(charmItemId != ITEM_NONE);

    RemoveBagItem(charmItemId, 1);
    Rogue_PushPopup_TriggerExtraLife();

    gRogueAdvPath.isOverviewActive = FALSE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    SetWarpDestination(
        MAP_GROUP(ROGUE_ADVENTURE_PATHS), MAP_NUM(ROGUE_ADVENTURE_PATHS), 
        WARP_ID_NONE,
        0, 0
    );
}