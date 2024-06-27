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

struct CharmCurseCounts
{
    u8 charmItems;
    u8 curseItems;
};

EWRAM_DATA struct CharmCurseCounts gCharmItemCounts[EFFECT_COUNT];

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

        case EFFECT_TORMENT_STATUS:
            return ITEM_TORMENT_CHARM;

        case EFFECT_PRESSURE_STATUS:
            return ITEM_PRESSURE_CHARM;

        case EFFECT_UNAWARE_STATUS:
            return ITEM_UNAWARE_CHARM;

        case EFFECT_ADAPTABILITY_RATE:
            return ITEM_ADAPTABILITY_CHARM;

        case EFFECT_EXTRA_LIFE:
            return ITEM_SACRED_ASH;

        case EFFECT_INFINITE_EXTRA_LIFE:
            return ITEM_INFINITE_EXTRA_LIFE_CHARM;

        case EFFECT_ALLOW_SAVE_SCUM:
            return ITEM_ALLOW_SAVE_SCUM_CHARM;

        // Unused
        // EFFECT_PARTY_SIZE
        // EFFECT_EVERSTONE_EVOS
        // EFFECT_BATTLE_ITEM_BAN
        // EFFECT_SPECIES_CLAUSE
        // EFFECT_ITEM_SHUFFLE
        // EFFECT_SNOWBALL_CURSES
        // EFFECT_RANDOMAN_ROUTE_SPAWN,
        // EFFECT_RANDOMAN_ALWAYS_SPAWN,
        // EFFECT_AUTO_MOVE_SELECT
        // EFFECT_ONE_HIT
        // EFFECT_SNAG_TRAINER_MON
        // EFFECT_WILD_EGG_SPECIES
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

        case EFFECT_TORMENT_STATUS:
            return ITEM_TORMENT_CURSE;

        case EFFECT_PRESSURE_STATUS:
            return ITEM_PRESSURE_CURSE;

        case EFFECT_UNAWARE_STATUS:
            return ITEM_UNAWARE_CURSE;

        case EFFECT_ADAPTABILITY_RATE:
            return ITEM_ADAPTABILITY_CURSE;

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
            return ITEM_SHUFFLE_CURSE;
        
        case EFFECT_SNOWBALL_CURSES:
            return ITEM_SNOWBALL_CURSE;
        
        case EFFECT_RANDOMAN_ROUTE_SPAWN:
            return ITEM_RANDOMAN_ROUTE_SPAWN_CURSE;
        
        case EFFECT_RANDOMAN_ALWAYS_SPAWN:
            return ITEM_RANDOMAN_ALWAYS_SPAWN_CURSE;
        
        case EFFECT_AUTO_MOVE_SELECT:
            return ITEM_AUTO_MOVE_CURSE;
        
        case EFFECT_ONE_HIT:
            return ITEM_ONE_HIT_CURSE;
        
        case EFFECT_SNAG_TRAINER_MON:
            return ITEM_SNAG_CURSE;
        
        case EFFECT_WILD_EGG_SPECIES:
            return ITEM_WILD_EGG_SPECIES_CURSE;
    }

    return ITEM_NONE;
}

static u16 CalcValueInternal(u8 effectType, u16 itemCount, bool8 isCurse)
{
    // Custom rate scaling
    switch(effectType)
    {
        case EFFECT_SHOP_PRICE:
            return itemCount * (isCurse ? 40 : 50);

        case EFFECT_FLINCH_CHANCE:
            return min(itemCount * (isCurse ? 10 : 15), 90);

        case EFFECT_SHED_SKIN_CHANCE:
            return min(itemCount * (isCurse ? 15 : 25), 90);

        case EFFECT_WILD_IV_RATE:
            return itemCount * 20;
            
        case EFFECT_CATCH_RATE:
            return itemCount * (isCurse ? 40 : 200);

        case EFFECT_SERENE_GRACE_CHANCE:
            return min(itemCount * (isCurse ? 50 : 75), 90);

        case EFFECT_WILD_ENCOUNTER_COUNT:
            return itemCount * (isCurse ? 1 : 2);

        case EFFECT_MOVE_PRIORITY_CHANCE:
            return min(itemCount * (isCurse ? 10 : 15), 90);

        case EFFECT_ADAPTABILITY_RATE:
            return min(itemCount * 5, 15); // 5 means 50%

        case EFFECT_ENDURE_CHANCE:
            return min(itemCount * (isCurse ? 20 : 40), 90);
    }

    return itemCount;
}

static u8 GetItemCountForEffectItem(u16 itemId)
{
    return min(100, (itemId == ITEM_NONE ? 0 : GetItemCountInBag(itemId)));
}

void RecalcCharmCurseValues(void)
{
    u8 effectType;
    DebugPrint("Recalcing Charm&Curse Values");

    for(effectType = 0; effectType < EFFECT_COUNT; ++effectType)
    {
        gCharmItemCounts[effectType].charmItems = GetItemCountForEffectItem(EffectToCharmItem(effectType));
        gCharmItemCounts[effectType].curseItems = GetItemCountForEffectItem(EffectToCurseItem(effectType));
        DebugPrintf("[%d] charm_item:%d curse_item:%d charm_value:%d curse_value:%d", 
            effectType,
            gCharmItemCounts[effectType].charmItems, 
            gCharmItemCounts[effectType].curseItems,
            CalcValueInternal(effectType, gCharmItemCounts[effectType].charmItems, FALSE),
            CalcValueInternal(effectType, gCharmItemCounts[effectType].curseItems, TRUE)
        );
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

    return CalcValueInternal(effectType, gCharmItemCounts[effectType].charmItems, FALSE);
}

u16 GetCurseValue(u8 effectType)
{
    if(!Rogue_IsRunActive())
        return 0;

    return CalcValueInternal(effectType, gCharmItemCounts[effectType].curseItems, TRUE);
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
        // Globally disabled effects (Can only be intentionally given)
        case EFFECT_PARTY_SIZE:
        case EFFECT_EXTRA_LIFE:
        case EFFECT_INFINITE_EXTRA_LIFE:
        case EFFECT_ALLOW_SAVE_SCUM:
        case EFFECT_SNOWBALL_CURSES:
        case EFFECT_RANDOMAN_ALWAYS_SPAWN:
        case EFFECT_AUTO_MOVE_SELECT:
        case EFFECT_ONE_HIT:
        case EFFECT_SNAG_TRAINER_MON:
        case EFFECT_WILD_EGG_SPECIES:
            return TRUE;

        // Disable these effects, once we already have one (They don't stack)
        case EFFECT_EVERSTONE_EVOS:
        case EFFECT_BATTLE_ITEM_BAN:
        case EFFECT_SPECIES_CLAUSE:
        case EFFECT_ITEM_SHUFFLE:
        case EFFECT_ENDURE_CHANCE:
        case EFFECT_TORMENT_STATUS:
        case EFFECT_PRESSURE_STATUS:
        case EFFECT_UNAWARE_STATUS:
        case EFFECT_RANDOMAN_ROUTE_SPAWN:
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
    if(IsCharmActive(EFFECT_INFINITE_EXTRA_LIFE))
    {
        // do nothing
        Rogue_PushPopup_TriggerExtraLife(FALSE);
    }
    else
    {
        u16 charmItemId = EffectToCharmItem(EFFECT_EXTRA_LIFE);
        AGB_ASSERT(charmItemId != ITEM_NONE);

        RemoveBagItem(charmItemId, 1);
        Rogue_PushPopup_TriggerExtraLife(TRUE);
    }

    gRogueAdvPath.isOverviewActive = FALSE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    SetWarpDestination(
        MAP_GROUP(ROGUE_ADVENTURE_PATHS), MAP_NUM(ROGUE_ADVENTURE_PATHS), 
        WARP_ID_NONE,
        0, 0
    );
}