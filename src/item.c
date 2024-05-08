#include "global.h"
#include "item.h"
#include "berry.h"
#include "string_util.h"
#include "text.h"
#include "event_data.h"
#include "malloc.h"
#include "secret_base.h"
#include "item_menu.h"
#include "strings.h"
#include "load_save.h"
#include "item_use.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "constants/items.h"
#include "constants/hold_effects.h"

#include "data.h"
#include "party_menu.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_baked.h"
#include "rogue_quest.h"

extern u16 gUnknown_0203CF30[];

// this file's functions
static bool8 CheckPyramidBagHasItem(u16 itemId, u16 count);
static bool8 CheckPyramidBagHasSpace(u16 itemId, u16 count);
static bool8 BagPocketUsesReservedSlots(u8 pocket);

// EWRAM variables
EWRAM_DATA struct BagPocket gBagPockets[POCKETS_COUNT] = {0};

// rodata
#include "data/text/item_descriptions.h"
#include "data/items.h"
#include "data/rogue_items.h"

// code
u16 GetBagItemQuantity(u16 *quantity)
{
    return gSaveBlock2Ptr->encryptionKey ^ *quantity;
}

void SetBagItemQuantity(u16 *quantity, u16 newValue)
{
    *quantity =  newValue ^ gSaveBlock2Ptr->encryptionKey;
}

u16 GetPCItemQuantity(u16 *quantity)
{
    return *quantity;
}

void SetPCItemQuantity(u16 *quantity, u16 newValue)
{
    *quantity = newValue;
}

void ApplyNewEncryptionKeyToBagItems(u32 newKey)
{
    u16 i;
    u32 pocket, item;
    
    for(i = 0; i < BAG_ITEM_CAPACITY; ++i)
        ApplyNewEncryptionKeyToHword(&gSaveBlock1Ptr->bagPockets[i].quantity, newKey);
}

void ApplyNewEncryptionKeyToBagItems_(u32 newKey) // really GF?
{
    ApplyNewEncryptionKeyToBagItems(newKey);
}

void UpdateBagItemsPointers(void)
{
    u8 bagPocket;
    u8 itemPocket;
    u16 itemId;
    u16 itemIdx = 0;

    // Expects items to always be sorted
    for(bagPocket = 0; bagPocket < POCKETS_COUNT; ++bagPocket)
    {
        u16 startIdx = itemIdx;

        for(; itemIdx < BAG_ITEM_CAPACITY; ++itemIdx)
        {
            itemId = gSaveBlock1Ptr->bagPockets[itemIdx].itemId;

            if(itemId == ITEM_NONE)
                // Reach end
                break;

            itemPocket = ItemId_GetPocket(gSaveBlock1Ptr->bagPockets[itemIdx].itemId);

            // Convert from POCKET_XYZ to XYZ_POCKET which is offset by 1
            if(itemPocket == POCKET_NONE || itemPocket != (bagPocket + 1))
                // Reach end of this pocket
                break;
        }

        if(startIdx != itemIdx)
        {
            gBagPockets[bagPocket].itemSlots = &gSaveBlock1Ptr->bagPockets[startIdx];
            gBagPockets[bagPocket].capacity = (itemIdx - startIdx); // capacity is actually used as size now
        }
        else
        {
            gBagPockets[bagPocket].itemSlots = &gSaveBlock1Ptr->bagPockets[0];
            gBagPockets[bagPocket].capacity = 0;
        }
    }
}

bool8 ItemPocketUsesReservedSlots(u8 pocket)
{
    return BagPocketUsesReservedSlots(pocket - 1);
}

static bool8 BagPocketUsesReservedSlots(u8 pocket)
{
    switch (pocket)
    {
    case KEYITEMS_POCKET:
        return TRUE;
    }

    return FALSE;
}

u16 GetBagUnreservedFreeSlots()
{
    u8 bagPocket;
    u16 freeSlots = GetBagUnreservedTotalSlots();

    // Expects items to always be sorted
    for(bagPocket = 0; bagPocket < POCKETS_COUNT; ++bagPocket)
    {
        if(!BagPocketUsesReservedSlots(bagPocket))
            freeSlots -= gBagPockets[bagPocket].capacity;
    }

    return freeSlots;
}

u16 GetBagUnreservedTotalSlots()
{
    return Rogue_GetBagCapacity() - BAG_ITEM_RESERVED_SLOTS;
}

u16 GetBagReservedFreeSlots()
{
    u8 bagPocket;
    u16 freeSlots = GetBagReservedTotalSlots();

    // Expects items to always be sorted
    for(bagPocket = 0; bagPocket < POCKETS_COUNT; ++bagPocket)
    {
        if(BagPocketUsesReservedSlots(bagPocket))
            freeSlots -= gBagPockets[bagPocket].capacity;
    }

    return freeSlots;
}

u16 GetBagReservedTotalSlots()
{
    return BAG_ITEM_RESERVED_SLOTS;
}

void RemoveEmptyBagItems(void)
{
    u16 i;
    u16 j;

    for (i = 0; i < BAG_ITEM_CAPACITY - 1; i++)
    {
        for (j = i + 1; j < BAG_ITEM_CAPACITY; j++)
        {
            if (gSaveBlock1Ptr->bagPockets[i].itemId == 0)
            {
                struct ItemSlot temp = gSaveBlock1Ptr->bagPockets[i];
                gSaveBlock1Ptr->bagPockets[i] = gSaveBlock1Ptr->bagPockets[j];
                gSaveBlock1Ptr->bagPockets[j] = temp;
            }
        }
    }

    UpdateBagItemsPointers();
}

void ShrinkBagItems(void)
{
    bool8 repeat;
    u16 i;
    u16 j;
    u8 pocket;
    u16 quantityCapacity;
    u16 quantityA;
    u16 quantityB;

    // This code is pretty gross, but works so..... *shrug*
    // It just shrinks everything down into more managable stacks
    do
    {
        repeat = FALSE;
        RemoveEmptyBagItems();

        for (i = 1; i < BAG_ITEM_CAPACITY; i++)
        {
            // If the items are the same, attempt to shrink
            if(gSaveBlock1Ptr->bagPockets[i].itemId != ITEM_NONE && gSaveBlock1Ptr->bagPockets[i - 1].itemId == gSaveBlock1Ptr->bagPockets[i].itemId)
            {
                quantityA = GetBagItemQuantity(&gSaveBlock1Ptr->bagPockets[i - 1].quantity);
                quantityB = GetBagItemQuantity(&gSaveBlock1Ptr->bagPockets[i].quantity);

                pocket = ItemId_GetPocket(gSaveBlock1Ptr->bagPockets[i].itemId) - 1;
                quantityCapacity = Rogue_GetBagPocketAmountPerItem(pocket);

                while(quantityA < quantityCapacity && quantityB > 0)
                {
                    --quantityB;
                    ++quantityA;
                }

                SetBagItemQuantity(&gSaveBlock1Ptr->bagPockets[i - 1].quantity, quantityA);
                SetBagItemQuantity(&gSaveBlock1Ptr->bagPockets[i].quantity, quantityB);

                if(quantityB == 0)
                {
                    gSaveBlock1Ptr->bagPockets[i].itemId = ITEM_NONE;
                    repeat = TRUE; // Have to repeat to see if can fill this empty space
                }
            }
        }
    }
    while(repeat == TRUE);

    RemoveEmptyBagItems();
    UpdateBagItemsPointers();
}

void CopyItemName(u16 itemId, u8 *dst)
{
    CopyItemNameN(itemId, dst, ITEM_NAME_LENGTH);
}

void CopyItemNameN(u16 itemId, u8 *dst, u16 length)
{
    if((itemId >= ITEM_TM01 && itemId <= ITEM_HM08) || (itemId >= ITEM_TR01 && itemId <= ITEM_TR50))
    {
        u16 moveId = ItemIdToBattleMoveId(itemId);
        AGB_ASSERT(moveId < MOVES_COUNT);

        if(itemId >= ITEM_TR01 && itemId <= ITEM_TR50)
        {
            StringCopyN(dst, gText_TRPrefix, length);
        }
        else if(itemId >= ITEM_HM01 && itemId <= ITEM_HM08)
        {
            StringCopyN(dst, gText_HMPrefix, length);
        }
        else
        {
            StringCopyN(dst, gText_TMPrefix, length);
        }

        StringAppendN(dst, gMoveNames[moveId], length);
    }
    else
    {
        StringCopyN(dst, ItemId_GetName(itemId), length);
    }
}

void CopyItemNameHandlePlural(u16 itemId, u8 *dst, u32 quantity)
{
    //if (itemId == ITEM_POKE_BALL)
    //{
    //    if (quantity < 2)
    //        StringCopy(dst, ItemId_GetName(ITEM_POKE_BALL));
    //    else
    //        StringCopy(dst, gText_PokeBalls);
    //}
    //else
    {
        if (itemId >= FIRST_BERRY_INDEX && itemId <= LAST_BERRY_INDEX)
            GetBerryCountString(dst, gBerries[itemId - FIRST_BERRY_INDEX].name, quantity);
        else
        {
            if(quantity == 1)
            {
                CopyItemName(itemId, dst);
            }
            else
            {
                ConvertUIntToDecimalStringN(dst, quantity, STR_CONV_MODE_LEFT_ALIGN, BAG_ITEM_CAPACITY_DIGITS);
                dst = StringAppend(dst, gText_Space);
                CopyItemName(itemId, dst);
                StringAppend(dst, gText_Plural);
            }
        }
    }
}

void GetBerryCountString(u8 *dst, const u8 *berryName, u32 quantity)
{
    const u8 *berryString;
    u8 *txtPtr;

    if (quantity < 2)
        berryString = gText_Berry;
    else
        berryString = gText_Berries;

    txtPtr = StringCopy(dst, berryName);
    *txtPtr = CHAR_SPACE;
    StringCopy(txtPtr + 1, berryString);
}

bool8 IsBagPocketNonEmpty(u8 pocket)
{
    u8 i;

    for (i = 0; i < gBagPockets[pocket - 1].capacity; i++)
    {
        if (gBagPockets[pocket - 1].itemSlots[i].itemId != 0)
            return TRUE;
    }
    return FALSE;
}

bool8 CheckBagHasItem(u16 itemId, u16 count)
{
    u8 i;
    u8 pocket;

    if (ItemId_GetPocket(itemId) == 0)
        return FALSE;

    pocket = ItemId_GetPocket(itemId) - 1;

    // Check for item slots that contain the item
    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
        {
            u16 quantity;

            // Does this item slot contain enough of the item?
            quantity = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);
            if (quantity >= count)
                return TRUE;
            count -= quantity;

            // Does this item slot and all previous slots contain enough of the item?
            if (count == 0)
                return TRUE;
        }
    }
    return FALSE;
}

u16 GetItemCountInBag(u16 itemId)
{
    u8 i;
    u8 pocket;
    u16 count = 0;

    if (ItemId_GetPocket(itemId) == 0)
        return count;

    pocket = ItemId_GetPocket(itemId) - 1;

    // Check for item slots that contain the item
    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
        {
            count += GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);
        }
    }

    return count;
}

bool8 HasAtLeastOneBerry(void)
{
    u16 i;

    for (i = FIRST_BERRY_INDEX; i < ITEM_BRIGHT_POWDER; i++)
    {
        if (CheckBagHasItem(i, 1) == TRUE)
        {
            gSpecialVar_Result = TRUE;
            return TRUE;
        }
    }
    gSpecialVar_Result = FALSE;
    return FALSE;
}

bool8 CheckBagHasSpace(u16 itemId, u16 count)
{
    u8 i;
    u8 itemPocket;
    u16 freeSlots;

    itemPocket = ItemId_GetPocket(itemId);

    if (itemPocket == POCKET_NONE)
        return FALSE;

    if(ItemPocketUsesReservedSlots(itemPocket))
    {
        freeSlots = GetBagReservedFreeSlots();
    }
    else
    {
        freeSlots = GetBagUnreservedFreeSlots();
    }

    if(freeSlots > 0)
        // If we have free slots, we definately have space
        return TRUE;

    // Try find existing stack and check if can merge into
    {
        u8 pocket;
        u16 slotCapacity;
        u16 ownedCount;

        pocket = itemPocket - 1;
        slotCapacity = Rogue_GetBagPocketAmountPerItem(pocket);

        // Check space in any existing item slots that already contain this item
        for (i = 0; i < gBagPockets[pocket].capacity; ++i)
        {
            if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
            {
                ownedCount = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);
                if (ownedCount + count <= slotCapacity)
                    return TRUE;

                count -= (slotCapacity - ownedCount);
                if (count == 0)
                    break; //should be return TRUE, but that doesn't match
            }
        }
    }

    return FALSE;
}

static u16 GetPokeballSortScore(u16 itemId)
{
    switch (itemId)
    {
    case ITEM_POKE_BALL:
        return 0;
    case ITEM_GREAT_BALL:
        return 1;
    case ITEM_ULTRA_BALL:
        return 2;
    case ITEM_MASTER_BALL:
        return 3;

    default:
        return 10 + ItemId_GetPrice(itemId);
    }
}

static u16 GetBerrySortScore(u16 itemId)
{
    return ItemIdToBerryType(itemId);
}

bool8 SortItemPlaceBefore(u8 sortMode, u16 itemIdA, u16 itemIdB, u16 quantityA, u16 quantityB)
{
    if(itemIdA == itemIdB)
    {
        // Store largest stack first
        return quantityA > quantityB;
    }
    else
    {
        s32 sortScore;
        u8 pocketA = ItemId_GetPocket(itemIdA);
        u8 pocketB = ItemId_GetPocket(itemIdB);

        if(pocketA != pocketB)
        {
            // Always store items in pocket order, as we need this to calculate pointers/pocket size
            return pocketA < pocketB;
        }

        // Apply sort mode
        switch (sortMode)
        {
        case ITEM_SORT_MODE_TYPE:
            // Fallback to subsort below
            break;

        case ITEM_SORT_MODE_NAME:
            if(pocketA == POCKET_TM_HM)
            {
                u16 moveA = ItemIdToBattleMoveId(itemIdA);
                u16 moveB = ItemIdToBattleMoveId(itemIdB);
                AGB_ASSERT(moveA != ITEM_NONE && moveA < MOVES_COUNT);
                AGB_ASSERT(moveB != ITEM_NONE && moveB < MOVES_COUNT);
                sortScore = StringCompareN(gMoveNames[moveA], gMoveNames[moveB], MOVE_NAME_LENGTH);
            }
            else
                sortScore = StringCompareN(ItemId_GetName(itemIdA), ItemId_GetName(itemIdB), ITEM_NAME_LENGTH);

            if(sortScore != 0)
                return sortScore < 0;
            break;

        case ITEM_SORT_MODE_VALUE:
            {
                u8 priceA = ItemId_GetPrice(itemIdA);
                u8 priceB = ItemId_GetPrice(itemIdB);
                if(priceA != priceB)
                    return priceA > priceB;
            }
            break;

        case ITEM_SORT_MODE_AMOUNT:
            if(quantityA != quantityB)
                return quantityA > quantityB;
            break;

        default:
            AGB_ASSERT(FALSE);
            break;
        }

        // For poke balls & berries pocket, custom sort
        if(pocketA == POCKET_POKE_BALLS)
        {
            return GetPokeballSortScore(itemIdA) < GetPokeballSortScore(itemIdB);
        }
        if(pocketA == POCKET_BERRIES)
        {
            return GetBerrySortScore(itemIdA) < GetBerrySortScore(itemIdB);
        }

        // For TMs sort by move type, power, pp etc.
        if(pocketA == POCKET_TM_HM)
        {
            u16 moveA = ItemIdToBattleMoveId(itemIdA);
            u16 moveB = ItemIdToBattleMoveId(itemIdB);
            AGB_ASSERT(moveA != ITEM_NONE && moveA < MOVES_COUNT);
            AGB_ASSERT(moveB != ITEM_NONE && moveB < MOVES_COUNT);

            // Sort by type
            if(gBattleMoves[moveA].type != gBattleMoves[moveB].type)
                return gBattleMoves[moveA].type < gBattleMoves[moveB].type;

            // Sort by power
            if(gBattleMoves[moveA].power != gBattleMoves[moveB].power)
                return gBattleMoves[moveA].power > gBattleMoves[moveB].power;

            // Now fallback to alphabetical sort
            sortScore = StringCompareN(gMoveNames[moveA], gMoveNames[moveB], MOVE_NAME_LENGTH);

            if(sortScore != 0)
                return sortScore < 0;
        }

        // Fallback sub-sort so we have a consistent result
        //

        // Display evo items first (And ensure they are grouped together)
        {
            bool8 isEvoItemA = Rogue_IsEvolutionItem(itemIdA);
            bool8 isEvoItemB = Rogue_IsEvolutionItem(itemIdB);

            if(isEvoItemA && !isEvoItemB)
                return TRUE;

            if(!isEvoItemA && isEvoItemB)
                return FALSE;
        }

        // Within the pocket attempt to sort items
        {
            ItemUseFunc fieldUseFuncA = ItemId_GetFieldFunc(itemIdA);
            ItemUseFunc fieldUseFuncB = ItemId_GetFieldFunc(itemIdB);

            if(fieldUseFuncA != fieldUseFuncB)
            {
                // Prefer showing items which have a field func first
                return fieldUseFuncA > fieldUseFuncB;
            }
        }

        {
            u8 battleUsageA = ItemId_GetBattleUsage(itemIdA);
            u8 battleUsageB = ItemId_GetBattleUsage(itemIdB);

            if(battleUsageA != 0 || battleUsageB != 0)
            {
                if(battleUsageA != battleUsageB)
                {
                    return battleUsageA < battleUsageB;
                }
            }
        }

        {
            u8 holdEffectA = ItemId_GetHoldEffect(itemIdA);
            u8 holdEffectB = ItemId_GetHoldEffect(itemIdB);
            u8 holdEffectParamA = ItemId_GetHoldEffectParam(itemIdA);
            u8 holdEffectParamB = ItemId_GetHoldEffectParam(itemIdB);

            // Show best item of this type first
            if(holdEffectA != holdEffectB)
            {
                return holdEffectA > holdEffectB;
            }

            if(holdEffectParamA != holdEffectParamB)
            {
                return holdEffectParamA > holdEffectParamB;
            }

            // Sort by ID if all else has failed
            return itemIdA < itemIdB;
        }
    }
}

static bool8 SortItemSlotPlaceBefore(struct ItemSlot slotA, struct ItemSlot slotB)
{
    u16 quantityA = GetBagItemQuantity(&slotA.quantity);
    u16 quantityB = GetBagItemQuantity(&slotB.quantity);
    return SortItemPlaceBefore(gSaveBlock1Ptr->bagSortMode, slotA.itemId, slotB.itemId, quantityA, quantityB);
}

static void SortInsertItemSlot(struct ItemSlot itemSlot, struct ItemSlot* buffer, u16 currBufferCount, bool8 flipSort)
{
    if(currBufferCount == 0)
    {
        // Insert remaining item at the end
        buffer[currBufferCount] = itemSlot;
    }
    else if(currBufferCount == 1)
    {
        if(SortItemSlotPlaceBefore(itemSlot, buffer[0]) != flipSort)
        {
            buffer[currBufferCount] = buffer[0];
            buffer[0] = itemSlot;
        }
        else
        {
            buffer[currBufferCount] = itemSlot;
        }
    }
    else
    {
        u16 index = 0;
        u16 minIndex = 0;
        u16 maxIndex = currBufferCount - 1;

        // Insert sort, find the index to insert at
        while(minIndex != maxIndex)
        {
            AGB_ASSERT(minIndex < maxIndex);

            index = (maxIndex + minIndex) / 2;

            if(SortItemSlotPlaceBefore(itemSlot, buffer[index]) != flipSort)
            {
                if(maxIndex == index)
                    --maxIndex;
                else
                    maxIndex = index;
            }
            else
            {
                if(minIndex == index)
                    ++minIndex;
                else
                    minIndex = index;
            }
        }

        AGB_ASSERT(minIndex == maxIndex);

        // Special case to sort the end of the list
        if(minIndex == currBufferCount - 1)
        {
            if(SortItemSlotPlaceBefore(itemSlot, buffer[currBufferCount - 1]) != flipSort)
            {
                buffer[currBufferCount] = buffer[currBufferCount - 1];
                buffer[currBufferCount - 1] = itemSlot;
            }
            else
            {
                buffer[currBufferCount] = itemSlot;
            }
        }
        else
        {

            // Shift everything up
            for(index = currBufferCount; TRUE; --index)
            {
                buffer[index] = buffer[index - 1];

                if(index == minIndex + 1)
                    break;
            }

            buffer[minIndex] = itemSlot;
        }
    }
}

bool8 AddBagItem(u16 itemId, u16 count)
{
    u16 i;
    bool8 hasFreeSlots;
    u8 itemPocket = ItemId_GetPocket(itemId);

    if (itemPocket == POCKET_NONE)
        return FALSE;

    if(ItemPocketUsesReservedSlots(itemPocket))
    {
        hasFreeSlots = GetBagReservedFreeSlots() > 0;
    }
    else
    {
        hasFreeSlots = GetBagUnreservedFreeSlots() > 0;
    }

    // Try find existing stack and check if can merge into
    {
        u8 pocket;
        u16 slotCapacity;
        u16 itemCount;

        pocket = itemPocket - 1;
        slotCapacity = Rogue_GetBagPocketAmountPerItem(pocket);

        for (i = 0; i < gBagPockets[pocket].capacity; ++i)
        {
            if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
            {
                itemCount = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);

                // Slot has free space
                if(itemCount < slotCapacity)
                {
                    if(itemCount + count <= slotCapacity)
                    {
                        // Can fit entirely into this slot
                        itemCount += count;
                        SetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity, itemCount);

                        //QuestNotify_OnAddBagItem(itemId, count);

                        if(itemPocket == POCKET_KEY_ITEMS)
                            RecalcCharmCurseValues();

                        return TRUE;
                    }
                    else if(hasFreeSlots)
                    {
                        // Can only partially fit into the existing slot
                        count -= (slotCapacity - itemCount);
                        SetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity, slotCapacity);

                        //QuestNotify_OnAddBagItem(itemId, slotCapacity - itemCount);

                        if(itemPocket == POCKET_KEY_ITEMS)
                            RecalcCharmCurseValues();
                    }
                }
            }
        }
    }

    // Insert new sort if we can
    if(hasFreeSlots)
    {
        struct ItemSlot tempSlot;
        struct ItemSlot currItemSlot;
        currItemSlot.itemId = itemId;
        SetBagItemQuantity(&currItemSlot.quantity, count);

        for(i = 0; i < BAG_ITEM_CAPACITY; ++i)
        {
            if(gSaveBlock1Ptr->bagPockets[i].itemId == ITEM_NONE)
            {
                // Found end
                gSaveBlock1Ptr->bagPockets[i] = currItemSlot;
                UpdateBagItemsPointers();

                //QuestNotify_OnAddBagItem(itemId, count);

                if(ItemId_GetPocket(itemId) == POCKET_KEY_ITEMS)
                    RecalcCharmCurseValues();

                return TRUE;
            }
            else if(SortItemSlotPlaceBefore(currItemSlot, gSaveBlock1Ptr->bagPockets[i]))
            {
                // Swap, as currItemSlot should be infront
                tempSlot = gSaveBlock1Ptr->bagPockets[i];
                gSaveBlock1Ptr->bagPockets[i] = currItemSlot;
                currItemSlot = tempSlot;
            }
        }
    }

    // Failed to insert
    UpdateBagItemsPointers();
    return FALSE;
}

bool8 RemoveBagItem(u16 itemId, u16 count)
{
    u8 i;
    u16 totalQuantity = 0;

    if (ItemId_GetPocket(itemId) == POCKET_NONE || itemId == ITEM_NONE)
        return FALSE;

    {
        u8 pocket;
        u8 var;
        u16 ownedCount;
        struct BagPocket *itemPocket;

        pocket = ItemId_GetPocket(itemId) - 1;
        itemPocket = &gBagPockets[pocket];

        for (i = 0; i < itemPocket->capacity; i++)
        {
            if (itemPocket->itemSlots[i].itemId == itemId)
                totalQuantity += GetBagItemQuantity(&itemPocket->itemSlots[i].quantity);
        }

        if (totalQuantity < count)
            return FALSE;   // We don't have enough of the item

        if (CurMapIsSecretBase() == TRUE)
        {
            VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_BAG);
            VarSet(VAR_SECRET_BASE_LAST_ITEM_USED, itemId);
        }

        var = GetItemListPosition(pocket);
        if (itemPocket->capacity > var
         && itemPocket->itemSlots[var].itemId == itemId)
        {
            ownedCount = GetBagItemQuantity(&itemPocket->itemSlots[var].quantity);
            if (ownedCount >= count)
            {
                SetBagItemQuantity(&itemPocket->itemSlots[var].quantity, ownedCount - count);
                count = 0;
            }
            else
            {
                count -= ownedCount;
                SetBagItemQuantity(&itemPocket->itemSlots[var].quantity, 0);
            }

            if (GetBagItemQuantity(&itemPocket->itemSlots[var].quantity) == 0)
                itemPocket->itemSlots[var].itemId = ITEM_NONE;

            if (count == 0)
            {
                RemoveEmptyBagItems();

                //QuestNotify_OnRemoveBagItem(itemId, count);

                if(ItemId_GetPocket(itemId) == POCKET_KEY_ITEMS)
                    RecalcCharmCurseValues();

                return TRUE;
            }
        }

        for (i = 0; i < itemPocket->capacity; i++)
        {
            if (itemPocket->itemSlots[i].itemId == itemId)
            {
                ownedCount = GetBagItemQuantity(&itemPocket->itemSlots[i].quantity);
                if (ownedCount >= count)
                {
                    SetBagItemQuantity(&itemPocket->itemSlots[i].quantity, ownedCount - count);
                    count = 0;
                }
                else
                {
                    count -= ownedCount;
                    SetBagItemQuantity(&itemPocket->itemSlots[i].quantity, 0);
                }

                if (GetBagItemQuantity(&itemPocket->itemSlots[i].quantity) == 0)
                    itemPocket->itemSlots[i].itemId = ITEM_NONE;

                if (count == 0)
                {
                    RemoveEmptyBagItems();

                    //QuestNotify_OnRemoveBagItem(itemId, count);

                    if(ItemId_GetPocket(itemId) == POCKET_KEY_ITEMS)
                        RecalcCharmCurseValues();

                    return TRUE;
                }
            }
        }

        RemoveEmptyBagItems();

        //QuestNotify_OnRemoveBagItem(itemId, count);
        
        if(ItemId_GetPocket(itemId) == POCKET_KEY_ITEMS)
            RecalcCharmCurseValues();

        return TRUE;
    }
}

u8 GetPocketByItemId(u16 itemId)
{
    return ItemId_GetPocket(itemId);
}

void ClearItemSlots(struct ItemSlot *itemSlots, u16 itemCount)
{
    u16 i;

    for (i = 0; i < itemCount; i++)
    {
        itemSlots[i].itemId = ITEM_NONE;
        SetBagItemQuantity(&itemSlots[i].quantity, 0);
    }
}

static s32 FindFreePCItemSlot(void)
{
    s8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_NONE)
            return i;
    }
    return -1;
}

u8 CountUsedPCItemSlots(void)
{
    u8 usedSlots = 0;
    u8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId != ITEM_NONE)
            usedSlots++;
    }
    return usedSlots;
}

bool8 CheckPCHasItem(u16 itemId, u16 count)
{
    u8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId == itemId && GetPCItemQuantity(&gSaveBlock1Ptr->pcItems[i].quantity) >= count)
            return TRUE;
    }
    return FALSE;
}

bool8 AddPCItem(u16 itemId, u16 count)
{
    u8 i;
    s8 freeSlot;
    u16 ownedCount;
    struct ItemSlot *newItems;

    // Copy PC items
    newItems = AllocZeroed(sizeof(gSaveBlock1Ptr->pcItems));
    memcpy(newItems, gSaveBlock1Ptr->pcItems, sizeof(gSaveBlock1Ptr->pcItems));

    // Use any item slots that already contain this item
    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (newItems[i].itemId == itemId)
        {
            ownedCount = GetPCItemQuantity(&newItems[i].quantity);
            if (ownedCount + count <= MAX_PC_ITEM_CAPACITY)
            {
                SetPCItemQuantity(&newItems[i].quantity, ownedCount + count);
                memcpy(gSaveBlock1Ptr->pcItems, newItems, sizeof(gSaveBlock1Ptr->pcItems));
                Free(newItems);
                return TRUE;
            }
            count += ownedCount - MAX_PC_ITEM_CAPACITY;
            SetPCItemQuantity(&newItems[i].quantity, MAX_PC_ITEM_CAPACITY);
            if (count == 0)
            {
                memcpy(gSaveBlock1Ptr->pcItems, newItems, sizeof(gSaveBlock1Ptr->pcItems));
                Free(newItems);
                return TRUE;
            }
        }
    }

    // Put any remaining items into a new item slot.
    if (count > 0)
    {
        freeSlot = FindFreePCItemSlot();
        if (freeSlot == -1)
        {
            Free(newItems);
            return FALSE;
        }
        else
        {
            newItems[freeSlot].itemId = itemId;
            SetPCItemQuantity(&newItems[freeSlot].quantity, count);
        }
    }

    // Copy items back to the PC
    memcpy(gSaveBlock1Ptr->pcItems, newItems, sizeof(gSaveBlock1Ptr->pcItems));
    Free(newItems);
    return TRUE;
}

void RemovePCItem(u8 index, u16 count)
{
    gSaveBlock1Ptr->pcItems[index].quantity -= count;
    if (gSaveBlock1Ptr->pcItems[index].quantity == 0)
    {
        gSaveBlock1Ptr->pcItems[index].itemId = ITEM_NONE;
        CompactPCItems();
    }
}

void CompactPCItems(void)
{
    u16 i;
    u16 j;

    for (i = 0; i < PC_ITEMS_COUNT - 1; i++)
    {
        for (j = i + 1; j < PC_ITEMS_COUNT; j++)
        {
            if (gSaveBlock1Ptr->pcItems[i].itemId == 0)
            {
                struct ItemSlot temp = gSaveBlock1Ptr->pcItems[i];
                gSaveBlock1Ptr->pcItems[i] = gSaveBlock1Ptr->pcItems[j];
                gSaveBlock1Ptr->pcItems[j] = temp;
            }
        }
    }
}

void SwapRegisteredBike(void)
{
}

u16 BagGetItemIdByPocketPosition(u8 pocketId, u16 pocketPos)
{
    return gBagPockets[pocketId - 1].itemSlots[pocketPos].itemId;
}

u16 BagGetQuantityByPocketPosition(u8 pocketId, u16 pocketPos)
{
    return GetBagItemQuantity(&gBagPockets[pocketId - 1].itemSlots[pocketPos].quantity);
}

static void SwapItemSlots(struct ItemSlot *a, struct ItemSlot *b)
{
    struct ItemSlot temp;
    SWAP(*a, *b, temp);
}

void CompactItemsInBagPocket(struct BagPocket *bagPocket)
{
    u16 i, j;

    for (i = 0; i < bagPocket->capacity - 1; i++)
    {
        for (j = i + 1; j < bagPocket->capacity; j++)
        {
            if (GetBagItemQuantity(&bagPocket->itemSlots[i].quantity) == 0)
                SwapItemSlots(&bagPocket->itemSlots[i], &bagPocket->itemSlots[j]);
        }
    }
}

void SortItemsInBag()
{
    u16 i;
    bool8 anySorts;
    struct ItemSlot currItem;
    
    ShrinkBagItems();

    for(i = 0; i < BAG_ITEM_CAPACITY; ++i)
    {
        currItem = gSaveBlock1Ptr->bagPockets[i];

        if(currItem.itemId == ITEM_NONE)
            break;

        SortInsertItemSlot(currItem, gSaveBlock1Ptr->bagPockets, i, FALSE);
    }
}

void SortBerriesOrTMHMs(struct BagPocket *bagPocket)
{
    // We shouldn't do this?

    //u16 i, j;
//
    //for (i = 0; i < bagPocket->capacity - 1; i++)
    //{
    //    for (j = i + 1; j < bagPocket->capacity; j++)
    //    {
    //        if (GetBagItemQuantity(&bagPocket->itemSlots[i].quantity) != 0)
    //        {
    //            if (GetBagItemQuantity(&bagPocket->itemSlots[j].quantity) == 0)
    //                continue;
    //            if (bagPocket->itemSlots[i].itemId <= bagPocket->itemSlots[j].itemId)
    //                continue;
    //        }
    //        SwapItemSlots(&bagPocket->itemSlots[i], &bagPocket->itemSlots[j]);
    //    }
    //}
}

void MoveItemSlotInList(struct ItemSlot* itemSlots_, u32 from, u32 to_)
{
    // dumb assignments needed to match
    struct ItemSlot *itemSlots = itemSlots_;
    u32 to = to_;

    if (from != to)
    {
        s16 i, count;
        struct ItemSlot firstSlot = itemSlots[from];

        if (to > from)
        {
            to--;
            for (i = from, count = to; i < count; i++)
                itemSlots[i] = itemSlots[i + 1];
        }
        else
        {
            for (i = from, count = to; i > count; i--)
                itemSlots[i] = itemSlots[i - 1];
        }
        itemSlots[to] = firstSlot;
    }
}

void ClearBag(void)
{
    ClearItemSlots(gSaveBlock1Ptr->bagPockets, BAG_ITEM_CAPACITY);
    UpdateBagItemsPointers();
    RecalcCharmCurseValues();
}

u16 CountTotalItemQuantityInBag(u16 itemId)
{
    u16 i;
    u16 ownedCount = 0;
    struct BagPocket *bagPocket = &gBagPockets[ItemId_GetPocket(itemId) - 1];

    for (i = 0; i < bagPocket->capacity; i++)
    {
        if (bagPocket->itemSlots[i].itemId == itemId)
            ownedCount += GetBagItemQuantity(&bagPocket->itemSlots[i].quantity);
    }

    return ownedCount;
}

static bool8 CheckPyramidBagHasItem(u16 itemId, u16 count)
{
    u8 i;
    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (items[i] == itemId)
        {
            if (quantities[i] >= count)
                return TRUE;

            count -= quantities[i];
            if (count == 0)
                return TRUE;
        }
    }

    return FALSE;
}

static bool8 CheckPyramidBagHasSpace(u16 itemId, u16 count)
{
    u8 i;
    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (items[i] == itemId || items[i] == ITEM_NONE)
        {
            if (quantities[i] + count <= MAX_BAG_ITEM_CAPACITY)
                return TRUE;

            count = (quantities[i] + count) - MAX_BAG_ITEM_CAPACITY;
            if (count == 0)
                return TRUE;
        }
    }

    return FALSE;
}

bool8 AddPyramidBagItem(u16 itemId, u16 count)
{
    u16 i;

    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    u16 *newItems = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
    u8 *newQuantities = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

    memcpy(newItems, items, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
    memcpy(newQuantities, quantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

    for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (newItems[i] == itemId && newQuantities[i] < MAX_PYRAMID_BAG_ITEM_CAPACITY)
        {
            newQuantities[i] += count;
            if (newQuantities[i] > MAX_PYRAMID_BAG_ITEM_CAPACITY)
            {
                count = newQuantities[i] - MAX_PYRAMID_BAG_ITEM_CAPACITY;
                newQuantities[i] = MAX_PYRAMID_BAG_ITEM_CAPACITY;
            }
            else
            {
                count = 0;
            }

            if (count == 0)
                break;
        }
    }

    if (count > 0)
    {
        for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
        {
            if (newItems[i] == ITEM_NONE)
            {
                newItems[i] = itemId;
                newQuantities[i] = count;
                if (newQuantities[i] > MAX_PYRAMID_BAG_ITEM_CAPACITY)
                {
                    count = newQuantities[i] - MAX_PYRAMID_BAG_ITEM_CAPACITY;
                    newQuantities[i] = MAX_PYRAMID_BAG_ITEM_CAPACITY;
                }
                else
                {
                    count = 0;
                }

                if (count == 0)
                    break;
            }
        }
    }

    if (count == 0)
    {
        memcpy(items, newItems, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
        memcpy(quantities, newQuantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));
        Free(newItems);
        Free(newQuantities);
        return TRUE;
    }
    else
    {
        Free(newItems);
        Free(newQuantities);
        return FALSE;
    }
}

bool8 RemovePyramidBagItem(u16 itemId, u16 count)
{
    u16 i;

    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    i = gPyramidBagMenuState.cursorPosition + gPyramidBagMenuState.scrollPosition;
    if (items[i] == itemId && quantities[i] >= count)
    {
        quantities[i] -= count;
        if (quantities[i] == 0)
            items[i] = ITEM_NONE;
        return TRUE;
    }
    else
    {
        u16 *newItems = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
        u8 *newQuantities = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

        memcpy(newItems, items, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
        memcpy(newQuantities, quantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

        for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
        {
            if (newItems[i] == itemId)
            {
                if (newQuantities[i] >= count)
                {
                    newQuantities[i] -= count;
                    count = 0;
                    if (newQuantities[i] == 0)
                        newItems[i] = ITEM_NONE;
                }
                else
                {
                    count -= newQuantities[i];
                    newQuantities[i] = 0;
                    newItems[i] = ITEM_NONE;
                }

                if (count == 0)
                    break;
            }
        }

        if (count == 0)
        {
            memcpy(items, newItems, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
            memcpy(quantities, newQuantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));
            Free(newItems);
            Free(newQuantities);
            return TRUE;
        }
        else
        {
            Free(newItems);
            Free(newQuantities);
            return FALSE;
        }
    }
}

static u16 SanitizeItemId(u16 itemId)
{
    if (itemId >= ITEMS_COUNT)
        return ITEM_NONE;
    else
        return itemId;
}

const u8 *ItemId_GetName(u16 itemId)
{
    return Rogue_GetItemName(itemId);
}

u16 ItemId_GetId(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.itemId;
}

u16 ItemId_GetPrice(u16 itemId)
{
    return Rogue_GetPrice(itemId);
}

u8 ItemId_GetHoldEffect(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.holdEffect;
}

u8 ItemId_GetHoldEffectParam(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.holdEffectParam;
}

const u8 *ItemId_GetDescription(u16 itemId)
{
    return Rogue_GetItemDesc(itemId);
}

u8 ItemId_GetImportance(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.importance;
}

// unused
u8 ItemId_GetRegistrability(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.registrability;
}

u8 ItemId_GetPocket(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.pocket;
}

u8 ItemId_GetType(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.type;
}

ItemUseFunc ItemId_GetFieldFunc(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.fieldUseFunc;
}

u8 ItemId_GetBattleUsage(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.battleUsage;
}

ItemUseFunc ItemId_GetBattleFunc(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.battleUseFunc;
}

u8 ItemId_GetSecondaryId(u16 itemId)
{
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.secondaryId;
}
