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
#include "rogue_charms.h"
#include "rogue_baked.h"
#include "rogue_quest.h"

extern u16 gUnknown_0203CF30[];

// this file's functions
static bool8 CheckPyramidBagHasItem(u16 itemId, u16 count);
static bool8 CheckPyramidBagHasSpace(u16 itemId, u16 count);
static bool8 ItemPocketUsesReservedSlots(u8 pocket);
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

static bool8 ItemPocketUsesReservedSlots(u8 pocket)
{
    return BagPocketUsesReservedSlots(pocket - 1);
}

static bool8 BagPocketUsesReservedSlots(u8 pocket)
{
    switch (pocket)
    {
    case CHARMS_POCKET:
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
    // TODO - Adjust bag slots when in adventure
    return BAG_ITEM_CAPACITY - BAG_ITEM_RESERVED_SLOTS;
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

void CompactBagItems(void)
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

void CopyItemName(u16 itemId, u8 *dst)
{
    StringCopy(dst, ItemId_GetName(itemId));

    if(itemId >= ITEM_TM01 && itemId <= ITEM_HM08)
    {
        u16 moveId = ItemIdToBattleMoveId(itemId);

        if(itemId >= ITEM_HM01 && itemId <= ITEM_HM08)
        {
            StringCopy(dst, gText_HMPrefix);
        }
        else
        {
            StringCopy(dst, gText_TMPrefix);
        }

        StringAppend(dst, gMoveNames[moveId]);
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

static u16 GetBagFreeSlotCount()
{
    u8 i;
    u16 count = 0;

    for(i = 0; i < POCKETS_COUNT; ++i)
    {
        count += gBagPockets[i].capacity;
    }

    return BAG_ITEM_CAPACITY - count;
}

bool8 CheckBagHasSpace(u16 itemId, u16 count)
{
    u8 i;
    u16 freeSlots;

    if (ItemId_GetPocket(itemId) == POCKET_NONE)
        return FALSE;

    freeSlots = GetBagFreeSlotCount();

    if(freeSlots > 0)
        // If we have free slots, we definately have space
        return TRUE;

    // Try find existing stack and check if can merge into
    {
        u8 pocket;
        u16 slotCapacity;
        u16 ownedCount;

        pocket = ItemId_GetPocket(itemId) - 1;
        if (pocket != BERRIES_POCKET)
            slotCapacity = MAX_BAG_ITEM_CAPACITY;
        else
            slotCapacity = MAX_BERRY_CAPACITY;

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

static u16 GetPokeballSortScore(struct Item* item)
{
    switch (item->itemId)
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
        return 10 + item->price;
    }
}

static bool8 SortItem(struct ItemSlot slotA, struct ItemSlot slotB)
{
    if(slotA.itemId == slotB.itemId)
    {
        // Store largest stack first
        return GetBagItemQuantity(&slotA.quantity) > GetBagItemQuantity(&slotB.quantity);
    }
    else
    {
        struct Item itemA;
        struct Item itemB;
        Rogue_ModifyItem(slotA.itemId, &itemA);
        Rogue_ModifyItem(slotB.itemId, &itemB);

        if(itemA.pocket != itemB.pocket)
        {
            // Always store items in pocket order, as we need this to calculate pointers/pocket size
            return itemA.pocket < itemB.pocket;
        }

        // For poke balls pocket, custom sort
        if(itemA.pocket == POCKET_POKE_BALLS)
        {
            return GetPokeballSortScore(&itemA) < GetPokeballSortScore(&itemB);
        }

        // Within the pocket attempt to sort items
        {
            if(itemA.fieldUseFunc != itemB.fieldUseFunc)
            {
                // Prefer showing items which have a field func first
                return itemA.fieldUseFunc > itemB.fieldUseFunc;
            }

            if(itemA.battleUsage != 0 || itemB.battleUsage != 0)
            {
                if(itemA.battleUsage != itemB.battleUsage)
                {
                    return itemA.battleUsage < itemB.battleUsage;
                }

                if(itemA.battleUseFunc != itemB.battleUseFunc)
                {
                    // Prefer showing items which have a battle func first
                    return itemA.battleUseFunc > itemB.battleUseFunc;
                }
            }

            if(itemA.holdEffectParam != itemB.holdEffectParam)
            {
                // Show best item of this type first
                return itemA.holdEffectParam > itemB.holdEffectParam;
            }

            // Sort by ID if all else has failed
            return slotA.itemId < slotB.itemId;
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
        if (pocket != BERRIES_POCKET)
            slotCapacity = MAX_BAG_ITEM_CAPACITY;
        else
            slotCapacity = MAX_BERRY_CAPACITY;

        for (i = 0; i < gBagPockets[pocket].capacity; ++i)
        {
            if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
            {
                itemCount = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);

                // Slot has free space
                if(itemCount < slotCapacity)
                {
                    if(itemCount + count < slotCapacity)
                    {
                        // Can fit entirely into this slot
                        itemCount += count;
                        SetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity, itemCount);

                        QuestNotify_OnAddBagItem(itemId, count);

                        if(itemPocket == POCKET_CHARMS)
                            RecalcCharmCurseValues();

                        return TRUE;
                    }
                    else if(hasFreeSlots)
                    {
                        // Can only partially fit into the existing slot
                        count -= (slotCapacity - itemCount);
                        SetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity, slotCapacity);

                        QuestNotify_OnAddBagItem(itemId, slotCapacity - itemCount);

                        if(itemPocket == POCKET_CHARMS)
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

                QuestNotify_OnAddBagItem(itemId, count);

                if(ItemId_GetPocket(itemId) == POCKET_CHARMS)
                    RecalcCharmCurseValues();

                return TRUE;
            }
            else if(SortItem(currItemSlot, gSaveBlock1Ptr->bagPockets[i]))
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
                CompactBagItems();

                QuestNotify_OnRemoveBagItem(itemId, count);

                if(ItemId_GetPocket(itemId) == POCKET_CHARMS)
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
                    CompactBagItems();

                    QuestNotify_OnRemoveBagItem(itemId, count);

                    if(ItemId_GetPocket(itemId) == POCKET_CHARMS)
                        RecalcCharmCurseValues();

                    return TRUE;
                }
            }
        }

        CompactBagItems();

        QuestNotify_OnRemoveBagItem(itemId, count);
        
        if(ItemId_GetPocket(itemId) == POCKET_CHARMS)
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
    switch (gSaveBlock1Ptr->registeredItem)
    {
    case ITEM_MACH_BIKE:
        gSaveBlock1Ptr->registeredItem = ITEM_ACRO_BIKE;
        break;
    case ITEM_ACRO_BIKE:
        gSaveBlock1Ptr->registeredItem = ITEM_MACH_BIKE;
        break;
    }
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

void SortBerriesOrTMHMs(struct BagPocket *bagPocket)
{
    u16 i, j;

    for (i = 0; i < bagPocket->capacity - 1; i++)
    {
        for (j = i + 1; j < bagPocket->capacity; j++)
        {
            if (GetBagItemQuantity(&bagPocket->itemSlots[i].quantity) != 0)
            {
                if (GetBagItemQuantity(&bagPocket->itemSlots[j].quantity) == 0)
                    continue;
                if (bagPocket->itemSlots[i].itemId <= bagPocket->itemSlots[j].itemId)
                    continue;
            }
            SwapItemSlots(&bagPocket->itemSlots[i], &bagPocket->itemSlots[j]);
        }
    }
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
    struct Item item;
    Rogue_ModifyItem(itemId, &item);
    return item.price;
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
