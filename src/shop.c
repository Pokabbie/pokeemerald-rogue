#include "global.h"
#include "bg.h"
#include "data.h"
#include "decompress.h"
#include "decoration.h"
#include "decoration_inventory.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "fieldmap.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "item_menu.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "money.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "scanline_effect.h"
#include "script.h"
#include "shop.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "text_window.h"
#include "tv.h"
#include "constants/decorations.h"
#include "constants/items.h"
#include "constants/metatile_behaviors.h"
#include "constants/rogue_hub.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/event_objects.h"

#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_hub.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_settings.h"

typedef void (*ShopCallback)();

#define TAG_SCROLL_ARROW   2100
#define TAG_ITEM_ICON_BASE 2110

#define INVALID_ITEM_COUNT ((u16)-1)

static EWRAM_DATA struct MartInfo sMartInfo = {0};
static EWRAM_DATA struct ShopData *sShopData = NULL;
static EWRAM_DATA struct ListMenuItem *sListMenuItems = NULL;
static EWRAM_DATA u8 (*sItemNames)[ITEM_NAME_LENGTH + 4] = {0};
static EWRAM_DATA u8 sPurchaseHistoryId = 0;
EWRAM_DATA struct ItemSlot gMartPurchaseHistory[SMARTSHOPPER_NUM_ITEMS] = {0};
static EWRAM_DATA ShopCallback sFreeCallback = NULL;

static void Task_ShopMenu(u8 taskId);
static void Task_HandleShopMenuQuit(u8 taskId);
static void CB2_InitBuyMenu(void);
static void Task_GoToBuyOrSellMenu(u8 taskId);
static void MapPostLoadHook_ReturnToShopMenu(void);
static void Task_ReturnToShopMenu(u8 taskId);
static void ShowShopMenuAfterExitingBuyOrSellMenu(u8 taskId);
static void BuyMenuDrawGraphics(void);
static void BuyMenuAddScrollIndicatorArrows(void);
static void Task_BuyMenu(u8 taskId);
static void BuyMenuBuildListMenuTemplate(void);
static void BuyMenuInitBgs(void);
static void BuyMenuInitWindows(void);
static void BuyMenuDecompressBgGraphics(void);
static void BuyMenuSetListEntry(struct ListMenuItem*, u16, u8*);
static void BuyMenuAddItemIcon(u16, u8);
static void BuyMenuRemoveItemIcon(u16, u8);
static void BuyMenuPrint(u8 windowId, const u8 *text, u8 x, u8 y, s8 speed, u8 colorSet);
static void BuyMenuDrawMapGraphics(void);
static void BuyMenuCopyMenuBgToBg1TilemapBuffer(void);
static void BuyMenuCollectObjectEventData(void);
static void BuyMenuDrawObjectEvents(void);
static void BuyMenuDrawMapBg(void);
static bool8 BuyMenuCheckForOverlapWithMenuBg(int, int);
static void BuyMenuDrawMapMetatile(s16, s16, const u16*, u8);
static void BuyMenuDrawMapMetatileLayer(u16 *dest, s16 offset1, s16 offset2, const u16 *src);
static bool8 BuyMenuCheckIfObjectEventOverlapsMenuBg(s16 *);
static void ExitBuyMenu(u8 taskId);
static void Task_ExitBuyMenu(u8 taskId);
static void BuyMenuTryMakePurchase(u8 taskId);
static void BuyMenuReturnToItemList(u8 taskId);
static void Task_BuyHowManyDialogueInit(u8 taskId);
static void BuyMenuConfirmPurchase(u8 taskId);
static void BuyMenuPrintItemQuantityAndPrice(u8 taskId);
static bool8 BuyMenuIsBuildDirectionValid(u8 area, u8 dir);
static void BuyMenuPrintBuildDirection(u8 taskId);
static void Task_BuyHowManyDialogueHandleInput(u8 taskId);
static void Task_BuyWhichDirectionDialogueHandleInput(u8 taskId);
static void BuyMenuSubtractMoney(u8 taskId);
static void RecordItemPurchase(u8 taskId);
static void Task_ReturnToItemListAfterItemPurchase(u8 taskId);
static void Task_ReturnToItemListAfterDecorationPurchase(u8 taskId);
static void Task_HandleShopMenuBuy(u8 taskId);
static void Task_HandleShopMenuSell(u8 taskId);
static void Task_HandleShopMenuUpgrades(u8 taskId);
static void Task_HandleShopMenuAreas(u8 taskId);
static void BuyMenuPrintItemDescriptionAndShowItemIcon(s32 item, bool8 onInit, struct ListMenu *list);
static void BuyMenuPrintPriceInList(u8 windowId, u32 itemId, u8 y);

static void CopyShopItemName(u16 item, u8* name);
static const u8* GetShopItemDescription(u16 item);
static bool8 BuyShopItem(u16 item, u16 count);
static u32 GetShopItemPrice(u16 item);
static bool8 IsZeroPriceMarkedAsFree();

static u32 GetShopCurrencyAmount();
static void RemoveShopCurrencyAmount(u32 amount);

static const struct YesNoFuncTable sShopPurchaseYesNoFuncs =
{
    BuyMenuTryMakePurchase,
    BuyMenuReturnToItemList
};

static const struct MenuAction sShopMenuActions_BuySellQuit[] =
{
    { gText_ShopBuy, {.void_u8=Task_HandleShopMenuBuy} },
    { gText_ShopSell, {.void_u8=Task_HandleShopMenuSell} },
    { gText_ShopQuit, {.void_u8=Task_HandleShopMenuQuit} }
};

static const struct MenuAction sShopMenuActions_BuyQuit[] =
{
    { gText_ShopBuy, {.void_u8=Task_HandleShopMenuBuy} },
    { gText_ShopQuit, {.void_u8=Task_HandleShopMenuQuit} }
};

static const struct MenuAction sShopMenuActions_BuildQuit[] =
{
    { gText_ShopUpgrade, {.void_u8=Task_HandleShopMenuUpgrades} },
    { gText_ShopAreas, {.void_u8=Task_HandleShopMenuAreas} },
    { gText_ShopQuit, {.void_u8=Task_HandleShopMenuQuit} }
};


static const struct WindowTemplate sShopMenuWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 9,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x0008,
    },
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 9,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x0008,
    }
};

static const struct ListMenuTemplate sShopBuyMenuListTemplate =
{
    .items = NULL,
    .moveCursorFunc = BuyMenuPrintItemDescriptionAndShowItemIcon,
    .itemPrintFunc = BuyMenuPrintPriceInList,
    .totalItems = 0,
    .maxShowed = 0,
    .windowId = 1,
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 0,
    .cursorShadowPal = 3,
    .lettersSpacing = 0,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_MULTIPLE_SCROLL_L_R,
    .fontId = FONT_NARROW,
    .cursorKind = 0
};

static const struct BgTemplate sShopBuyMenuBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 0,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 3,
        .charBaseIndex = 0,
        .mapBaseIndex = 28,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    }
};

static const struct WindowTemplate sShopBuyMenuWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 10,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x001E,
    },
    {
        .bg = 0,
        .tilemapLeft = 14,
        .tilemapTop = 2,
        .width = 15,
        .height = 16,
        .paletteNum = 15,
        .baseBlock = 0x0032,
    },
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 13,
        .width = 14,
        .height = 6,
        .paletteNum = 15,
        .baseBlock = 0x0122,
    },
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 11,
        .width = 12,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x0176,
    },
    {
        .bg = 0,
        .tilemapLeft = 18,
        .tilemapTop = 11,
        .width = 10,
        .height = 2,
        .paletteNum = 15,
        .baseBlock = 0x018E,
    },
    {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 27,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 0x01A2,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct WindowTemplate sShopBuyMenuYesNoWindowTemplates =
{
    .bg = 0,
    .tilemapLeft = 21,
    .tilemapTop = 9,
    .width = 5,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x020E,
};

static const u8 sShopBuyMenuTextColors[][3] =
{
    {1, 2, 3},
    {0, 2, 3},
    {0, 3, 2}
};

static u8 CreateShopMenu(u8 martType)
{
    int numMenuItems;

    LockPlayerFieldControls();
    sMartInfo.martType = martType;

    if (martType == MART_TYPE_NORMAL)
    {
        struct WindowTemplate winTemplate;
        winTemplate = sShopMenuWindowTemplates[0];
        winTemplate.width = GetMaxWidthInMenuTable(sShopMenuActions_BuySellQuit, ARRAY_COUNT(sShopMenuActions_BuySellQuit));
        sMartInfo.windowId = AddWindow(&winTemplate);
        sMartInfo.menuActions = sShopMenuActions_BuySellQuit;
        numMenuItems = ARRAY_COUNT(sShopMenuActions_BuySellQuit);
    }
    else if (martType == MART_TYPE_HUB_AREAS || martType == MART_TYPE_HUB_UPGRADES)
    {
        struct WindowTemplate winTemplate;
        winTemplate = sShopMenuWindowTemplates[0];
        winTemplate.width = GetMaxWidthInMenuTable(sShopMenuActions_BuildQuit, ARRAY_COUNT(sShopMenuActions_BuildQuit));
        sMartInfo.windowId = AddWindow(&winTemplate);
        sMartInfo.menuActions = sShopMenuActions_BuildQuit;
        numMenuItems = ARRAY_COUNT(sShopMenuActions_BuildQuit);
    }
    else if(martType == MART_TYPE_SINGLE_PURCHASE)
    {
        FadeScreen(FADE_TO_BLACK, 0);
        return CreateTask(Task_HandleShopMenuBuy, 8);
    }
    else // MART_TYPE_PURCHASE_ONLY and DECOR
    {
        struct WindowTemplate winTemplate;
        winTemplate = sShopMenuWindowTemplates[1];
        winTemplate.width = GetMaxWidthInMenuTable(sShopMenuActions_BuyQuit, ARRAY_COUNT(sShopMenuActions_BuyQuit));
        sMartInfo.windowId = AddWindow(&winTemplate);
        sMartInfo.menuActions = sShopMenuActions_BuyQuit;
        numMenuItems = ARRAY_COUNT(sShopMenuActions_BuyQuit);
    }

    SetStandardWindowBorderStyle(sMartInfo.windowId, 0);
    PrintMenuTable(sMartInfo.windowId, numMenuItems, sMartInfo.menuActions);
    InitMenuInUpperLeftCornerNormal(sMartInfo.windowId, numMenuItems, 0);
    PutWindowTilemap(sMartInfo.windowId);
    CopyWindowToVram(sMartInfo.windowId, COPYWIN_MAP);

    return CreateTask(Task_ShopMenu, 8);
}

static void SetShopMenuCallback(void (* callback)(void))
{
    sMartInfo.callback = callback;
}

static u16 StaticShopItemListCallback(u16 index)
{
    const u16* items = (const u16*)sMartInfo.listItemData;
    return items[index];
}

static void ResetMartInfo()
{
    sMartInfo.dynamicMartCategory = 0;
    sMartInfo.minPrice = 0;
    sMartInfo.anythingBought = FALSE;
    sMartInfo.listItemCallback = NULL;
    sMartInfo.listItemData = NULL;
    sMartInfo.listItemTerminator = 0;
    sMartInfo.itemCount = INVALID_ITEM_COUNT;
}

static void SetShopItemsFromStaticList(const u16 *items, u16 terminatorItem)
{
    sMartInfo.dynamicMartCategory = 0;
    sMartInfo.listItemCallback = StaticShopItemListCallback;
    sMartInfo.listItemData = (void*)items;
    sMartInfo.listItemTerminator = terminatorItem;
}

static void SetShopItemsFromCallback(u16 (*listItemCallback)(u16), u16 terminatorItem, void* userData)
{
    sMartInfo.dynamicMartCategory = 0;
    sMartInfo.listItemCallback = listItemCallback;
    sMartInfo.listItemData = userData;
    sMartInfo.listItemTerminator = terminatorItem;
}

static void Task_ShopMenu(u8 taskId)
{
    s8 inputCode = Menu_ProcessInputNoWrap();
    switch (inputCode)
    {
    case MENU_NOTHING_CHOSEN:
        break;
    case MENU_B_PRESSED:
        PlaySE(SE_SELECT);
        Task_HandleShopMenuQuit(taskId);
        break;
    default:
        sMartInfo.menuActions[inputCode].func.void_u8(taskId);
        break;
    }
}

static void Task_HandleShopMenuBuy(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    data[8] = (u32)CB2_InitBuyMenu >> 16;
    data[9] = (u32)CB2_InitBuyMenu;
    gTasks[taskId].func = Task_GoToBuyOrSellMenu;
    FadeScreen(FADE_TO_BLACK, 0);
}

static void Task_HandleShopMenuSell(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    data[8] = (u32)CB2_GoToSellMenu >> 16;
    data[9] = (u32)CB2_GoToSellMenu;
    gTasks[taskId].func = Task_GoToBuyOrSellMenu;
    FadeScreen(FADE_TO_BLACK, 0);
}

static void Task_HandleShopMenuUpgrades(u8 taskId)
{
    sMartInfo.martType = MART_TYPE_HUB_UPGRADES;
    gTasks[taskId].func = Task_HandleShopMenuBuy;
}

static void Task_HandleShopMenuAreas(u8 taskId)
{
    sMartInfo.martType = MART_TYPE_HUB_AREAS;
    gTasks[taskId].func = Task_HandleShopMenuBuy;
}

void CB2_ExitSellMenu(void)
{
    gFieldCallback = MapPostLoadHook_ReturnToShopMenu;
    SetMainCallback2(CB2_ReturnToField);
}

static void Task_HandleShopMenuQuit(u8 taskId)
{
    if (sMartInfo.martType != MART_TYPE_SINGLE_PURCHASE)
    {
        ClearStdWindowAndFrameToTransparent(sMartInfo.windowId, 2);
        RemoveWindow(sMartInfo.windowId);
    }

    TryPutSmartShopperOnAir();
    UnlockPlayerFieldControls();
    DestroyTask(taskId);

    if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        RogueQuest_OnTrigger(QUEST_TRIGGER_MISC_UPDATE);
    }

    if(sMartInfo.anythingBought)
        VarSet(VAR_RESULT, TRUE);
    else
        VarSet(VAR_RESULT, FALSE);

    if (sMartInfo.callback)
        sMartInfo.callback();
}

static void Task_GoToBuyOrSellMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2((void *)((u16)data[8] << 16 | (u16)data[9]));
    }
}

static void MapPostLoadHook_ReturnToShopMenu(void)
{
    FadeInFromBlack();
    CreateTask(Task_ReturnToShopMenu, 8);
}

static void Task_ReturnToShopMenu(u8 taskId)
{
    if (IsWeatherNotFadingIn() == TRUE)
    {
        if(sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
        {
            gTasks[taskId].func = Task_HandleShopMenuQuit;
            return;
        }

        if (sMartInfo.martType == MART_TYPE_DECOR2)
            DisplayItemMessageOnField(taskId, gText_CanIHelpWithAnythingElse, ShowShopMenuAfterExitingBuyOrSellMenu);
        else if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
            DisplayItemMessageOnField(taskId, gText_WhatWouldYouLikeToBuild, ShowShopMenuAfterExitingBuyOrSellMenu);
        else
            DisplayItemMessageOnField(taskId, gText_AnythingElseICanHelp, ShowShopMenuAfterExitingBuyOrSellMenu);
    }
}

static void ShowShopMenuAfterExitingBuyOrSellMenu(u8 taskId)
{
    CreateShopMenu(sMartInfo.martType);
    DestroyTask(taskId);
}

static void CB2_BuyMenu(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlankCB_BuyMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

#define tItemCount data[1]
#define tItemId data[5]
#define tListTaskId data[7]

static void CB2_InitBuyMenu(void)
{
    u8 taskId;

    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        CpuFastFill(0, (void *)OAM, OAM_SIZE);
        ScanlineEffect_Stop();
        ResetTempTileDataBuffers();
        FreeAllSpritePalettes();
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        ClearScheduledBgCopiesToVram();
        sShopData = AllocZeroed(sizeof(struct ShopData));
        sShopData->scrollIndicatorsTaskId = TASK_NONE;
        sShopData->itemSpriteIds[0] = SPRITE_NONE;
        sShopData->itemSpriteIds[1] = SPRITE_NONE;
        BuyMenuBuildListMenuTemplate();
        BuyMenuInitBgs();
        FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 0x20, 0x20);
        FillBgTilemapBufferRect_Palette0(1, 0, 0, 0, 0x20, 0x20);
        FillBgTilemapBufferRect_Palette0(2, 0, 0, 0, 0x20, 0x20);
        FillBgTilemapBufferRect_Palette0(3, 0, 0, 0, 0x20, 0x20);
        BuyMenuInitWindows();
        BuyMenuDecompressBgGraphics();
        gMain.state++;
        break;
    case 1:
        if (!FreeTempTileDataBuffersIfPossible())
            gMain.state++;
        break;
    default:
        BuyMenuDrawGraphics();
        BuyMenuAddScrollIndicatorArrows();
        taskId = CreateTask(Task_BuyMenu, 8);
        gTasks[taskId].tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, 0, 0);
        BlendPalettes(PALETTES_ALL, 0x10, RGB_BLACK);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        SetVBlankCallback(VBlankCB_BuyMenu);
        SetMainCallback2(CB2_BuyMenu);
        break;
    }
}

static void BuyMenuFreeMemory(void)
{
    if(sFreeCallback != NULL)
    {
        sFreeCallback();
        sFreeCallback = NULL;
    }

    Free(sShopData);
    Free(sListMenuItems);
    Free(sItemNames);
    FreeAllWindowBuffers();

    sShopData = NULL;
    sListMenuItems = NULL;
    sItemNames = NULL;
}

static void BuyMenuBuildListMenuTemplate(void)
{
    u16 i, itemId;

    if(sMartInfo.listItemCallback != StaticShopItemListCallback)
    {
        // If the list isn't static, we want to recalculate the length here
        sMartInfo.itemCount = INVALID_ITEM_COUNT;
    }

    // Calculate item count first for allocs (Only need to do this once though)
    if(sMartInfo.itemCount == INVALID_ITEM_COUNT)
    {
        sMartInfo.itemCount = 0;

        for(i = 0; TRUE; ++i)
        {
            itemId = sMartInfo.listItemCallback(i);

            if(itemId == sMartInfo.listItemTerminator)
                break;
        }

        sMartInfo.itemCount = i;
    }

    // Generate lists
    if(sListMenuItems != NULL)
    {
        Free(sListMenuItems);
        sListMenuItems = NULL;
    }
    if(sItemNames != NULL)
    {
        Free(sItemNames);
        sItemNames = NULL;
    }
    
    sListMenuItems = Alloc((sMartInfo.itemCount + 1) * sizeof(*sListMenuItems));
    sItemNames = Alloc((sMartInfo.itemCount + 1) * sizeof(*sItemNames));
    for(i = 0; i < sMartInfo.itemCount; ++i)
    {
        itemId = sMartInfo.listItemCallback(i);
        AGB_ASSERT(itemId != sMartInfo.listItemTerminator);

        BuyMenuSetListEntry(&sListMenuItems[i], itemId, sItemNames[i]);
    }

    // Append quit option
    StringCopyN(sItemNames[i], gText_Cancel2, ITEM_NAME_LENGTH + 4);
    sListMenuItems[i].name = sItemNames[i];
    sListMenuItems[i].id = LIST_CANCEL;

    gMultiuseListMenuTemplate = sShopBuyMenuListTemplate;
    gMultiuseListMenuTemplate.items = sListMenuItems;
    gMultiuseListMenuTemplate.totalItems = sMartInfo.itemCount + 1;
    if (gMultiuseListMenuTemplate.totalItems > 8)
        gMultiuseListMenuTemplate.maxShowed = 8;
    else
        gMultiuseListMenuTemplate.maxShowed = gMultiuseListMenuTemplate.totalItems;

    sShopData->itemsShowed = gMultiuseListMenuTemplate.maxShowed;
}

static void BuyMenuSetListEntry(struct ListMenuItem *menuItem, u16 item, u8 *name)
{
    CopyShopItemName(item, name);
    menuItem->name = name;
    menuItem->id = item;
}

static void BuyMenuPrintItemDescriptionAndShowItemIcon(s32 item, bool8 onInit, struct ListMenu *list)
{
    const u8 *description;
    if (onInit != TRUE)
        PlaySE(SE_SELECT);

    if (item != LIST_CANCEL)
        BuyMenuAddItemIcon(item, sShopData->iconSlot);
    else
        BuyMenuAddItemIcon(-1, sShopData->iconSlot);

    BuyMenuRemoveItemIcon(item, sShopData->iconSlot ^ 1);
    sShopData->iconSlot ^= 1;
    if (item != LIST_CANCEL)
    {
        description = GetShopItemDescription(item);
    }
    else
    {
        description = gText_QuitShopping;
    }

    FillWindowPixelBuffer(2, PIXEL_FILL(0));

    BuyMenuPrint(2, description, 3, 1, 0, 0);
}

static u32 Mart_GetItemPrice(u16 itemId)
{
    if(ItemId_GetPrice(itemId) == 0)
    {
        // Will always be free
        return 0;
    }
    else
    {
        u32 totalPerc = 0;
        u32 basePrice = max(ItemId_GetPrice(itemId), sMartInfo.minPrice);

        u16 decValue = GetCharmValue(EFFECT_SHOP_PRICE);
        u16 incValue = GetCurseValue(EFFECT_SHOP_PRICE);

        totalPerc = 100 + incValue;
        if(decValue > totalPerc)
            totalPerc = 0;
        else
            totalPerc -= decValue;

        // At this point max at all the prices
        if(totalPerc >= 2500)
            return 99999;

        // Can only go down to 10$ or up to 99999$
        return max(10, min(99999, (basePrice * totalPerc) / 100));
    }
}

static void BuyMenuPrintPriceInList(u8 windowId, u32 itemId, u8 y)
{
    u8 x;

    if (itemId != LIST_CANCEL)
    {
        u32 price = GetShopItemPrice(itemId);

        if(price == 0)
        {
            if(IsZeroPriceMarkedAsFree())
            {
                x = GetStringRightAlignXOffset(FONT_NARROW, gText_PokedollarFree, 0x78);
                AddTextPrinterParameterized4(windowId, FONT_NARROW, x, y, 0, 0, sShopBuyMenuTextColors[1], TEXT_SKIP_DRAW, gText_PokedollarFree);
            }
            else
            {
                x = GetStringRightAlignXOffset(FONT_NARROW, gText_PokedollarAlreadyOwned, 0x78);
                AddTextPrinterParameterized4(windowId, FONT_NARROW, x, y, 0, 0, sShopBuyMenuTextColors[1], TEXT_SKIP_DRAW, gText_PokedollarAlreadyOwned);
            }
        }
        else
        {
            ConvertIntToDecimalStringN(
                gStringVar1,
                GetShopItemPrice(itemId),
                STR_CONV_MODE_LEFT_ALIGN,
                5);

            if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
                StringExpandPlaceholders(gStringVar4, gText_BuildVar1);
            else
                StringExpandPlaceholders(gStringVar4, gText_PokedollarVar1);

            x = GetStringRightAlignXOffset(FONT_NARROW, gStringVar4, 0x78);
            AddTextPrinterParameterized4(windowId, FONT_NARROW, x, y, 0, 0, sShopBuyMenuTextColors[1], TEXT_SKIP_DRAW, gStringVar4);
        }
    }
}

static void BuyMenuAddScrollIndicatorArrows(void)
{
    if (sShopData->scrollIndicatorsTaskId == TASK_NONE && sMartInfo.itemCount + 1 > 8)
    {
        sShopData->scrollIndicatorsTaskId = AddScrollIndicatorArrowPairParameterized(
            SCROLL_ARROW_UP,
            172,
            12,
            148,
            sMartInfo.itemCount - 7,
            TAG_SCROLL_ARROW,
            TAG_SCROLL_ARROW,
            &sShopData->scrollOffset);
    }
}

static void BuyMenuRemoveScrollIndicatorArrows(void)
{
    if (sShopData->scrollIndicatorsTaskId != TASK_NONE)
    {
        RemoveScrollIndicatorArrowPair(sShopData->scrollIndicatorsTaskId);
        sShopData->scrollIndicatorsTaskId = TASK_NONE;
    }
}

static void BuyMenuPrintCursor(u8 scrollIndicatorsTaskId, u8 colorSet)
{
    u8 y = ListMenuGetYCoordForPrintingArrowCursor(scrollIndicatorsTaskId);
    BuyMenuPrint(1, gText_SelectorArrow2, 0, y, 0, colorSet);
}

static void BuyMenuAddItemIcon(u16 item, u8 iconSlot)
{
    u8 spriteId;
    u8 *spriteIdPtr = &sShopData->itemSpriteIds[iconSlot];
    if (*spriteIdPtr != SPRITE_NONE)
        return;

    if (sMartInfo.martType == MART_TYPE_NORMAL || item == 0xFFFF || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        spriteId = AddItemIconSprite(iconSlot + TAG_ITEM_ICON_BASE, iconSlot + TAG_ITEM_ICON_BASE, item);
        if (spriteId != MAX_SPRITES)
        {
            *spriteIdPtr = spriteId;
            gSprites[spriteId].x2 = 24;
            gSprites[spriteId].y2 = 88;
        }
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        u16 targetArea = sMartInfo.martType == MART_TYPE_HUB_UPGRADES ? gRogueHubUpgrades[item].targetArea : item;
        const u32* image = gRogueHubAreas[targetArea].iconImage;
        const u32* palette = gRogueHubAreas[targetArea].iconPalette;

        AGB_ASSERT(targetArea < HUB_AREA_COUNT);

        if(image != NULL && palette != NULL)
        {
            spriteId = AddIconSprite(iconSlot + TAG_ITEM_ICON_BASE, iconSlot + TAG_ITEM_ICON_BASE, image, palette);
            if (spriteId != MAX_SPRITES)
            {
                *spriteIdPtr = spriteId;
                gSprites[spriteId].x2 = 24;
                gSprites[spriteId].y2 = 88;
            }
        }
    }
    else
    {
        spriteId = AddDecorationIconObject(item, 20, 84, 1, iconSlot + TAG_ITEM_ICON_BASE, iconSlot + TAG_ITEM_ICON_BASE);
        if (spriteId != MAX_SPRITES)
            *spriteIdPtr = spriteId;
    }
}

static void BuyMenuRemoveItemIcon(u16 item, u8 iconSlot)
{
    u8 *spriteIdPtr = &sShopData->itemSpriteIds[iconSlot];
    if (*spriteIdPtr == SPRITE_NONE)
        return;

    FreeSpriteTilesByTag(iconSlot + TAG_ITEM_ICON_BASE);
    FreeSpritePaletteByTag(iconSlot + TAG_ITEM_ICON_BASE);
    DestroySprite(&gSprites[*spriteIdPtr]);
    *spriteIdPtr = SPRITE_NONE;
}

static void BuyMenuInitBgs(void)
{
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sShopBuyMenuBgTemplates, ARRAY_COUNT(sShopBuyMenuBgTemplates));
    SetBgTilemapBuffer(1, sShopData->tilemapBuffers[1]);
    SetBgTilemapBuffer(2, sShopData->tilemapBuffers[3]);
    SetBgTilemapBuffer(3, sShopData->tilemapBuffers[2]);
    SetGpuReg(REG_OFFSET_BG0HOFS, 0);
    SetGpuReg(REG_OFFSET_BG0VOFS, 0);
    SetGpuReg(REG_OFFSET_BG1HOFS, 0);
    SetGpuReg(REG_OFFSET_BG1VOFS, 0);
    SetGpuReg(REG_OFFSET_BG2HOFS, 0);
    SetGpuReg(REG_OFFSET_BG2VOFS, 0);
    SetGpuReg(REG_OFFSET_BG3HOFS, 0);
    SetGpuReg(REG_OFFSET_BG3VOFS, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    ShowBg(1);
    ShowBg(2);
    ShowBg(3);
}

static void BuyMenuDecompressBgGraphics(void)
{
    DecompressAndCopyTileDataToVram(1, gShopMenu_Gfx, 0x3A0, 0x3E3, 0);
    LZDecompressWram(gShopMenu_Tilemap, sShopData->tilemapBuffers[0]);
    LoadCompressedPalette(gShopMenu_Pal, 0xC0, 0x20);
}

static void BuyMenuInitWindows(void)
{
    InitWindows(sShopBuyMenuWindowTemplates);
    DeactivateAllTextPrinters();
    LoadUserWindowBorderGfx(0, 1, 0xD0);
    LoadMessageBoxGfx(0, 0xA, 0xE0);
    PutWindowTilemap(0);
    PutWindowTilemap(1);
    PutWindowTilemap(2);
}

static void BuyMenuPrint(u8 windowId, const u8 *text, u8 x, u8 y, s8 speed, u8 colorSet)
{
    gTextFlags.replaceScrollWithNewLine = TRUE;
    AddTextPrinterParameterized4(windowId, FONT_NORMAL, x, y, 0, 0, sShopBuyMenuTextColors[colorSet], speed, text);
    gTextFlags.replaceScrollWithNewLine = FALSE;
}

static void BuyMenuDisplayMessage(u8 taskId, const u8 *text, TaskFunc callback)
{
    DisplayMessageAndContinueTask(taskId, 5, 10, 14, FONT_NORMAL, GetPlayerTextSpeedDelay(), text, callback);
    ScheduleBgCopyTilemapToVram(0);
}

static void BuyMenuDrawGraphics(void)
{
    BuyMenuDrawMapGraphics();
    BuyMenuCopyMenuBgToBg1TilemapBuffer();
    AddMoneyLabelObject(19, 11);

    if(sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
        PrintMoneyAmountInMoneyBoxWithBorderCustom(0, 1, 13, GetShopCurrencyAmount(), gText_BuildVar1);
    else
        PrintMoneyAmountInMoneyBoxWithBorder(0, 1, 13, GetShopCurrencyAmount());

    ScheduleBgCopyTilemapToVram(0);
    ScheduleBgCopyTilemapToVram(1);
    ScheduleBgCopyTilemapToVram(2);
    ScheduleBgCopyTilemapToVram(3);
}

static void BuyMenuDrawMapGraphics(void)
{
    BuyMenuCollectObjectEventData();
    BuyMenuDrawObjectEvents();
    BuyMenuDrawMapBg();
}

static void BuyMenuDrawMapBg(void)
{
    s16 i;
    s16 j;
    s16 x;
    s16 y;
    const struct MapLayout *mapLayout;
    u16 metatile;
    u8 metatileLayerType;

    mapLayout = gMapHeader.mapLayout;
    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    x -= 4;
    y -= 4;

    for (j = 0; j < 10; j++)
    {
        for (i = 0; i < 15; i++)
        {
            metatile = MapGridGetMetatileIdAt(x + i, y + j);
            if (BuyMenuCheckForOverlapWithMenuBg(i, j) == TRUE)
                metatileLayerType = MapGridGetMetatileLayerTypeAt(x + i, y + j);
            else
                metatileLayerType = METATILE_LAYER_TYPE_COVERED;

            if (metatile < NUM_METATILES_IN_PRIMARY)
            {
                BuyMenuDrawMapMetatile(i, j, (u16*)mapLayout->primaryTileset->metatiles + metatile * 8, metatileLayerType);
            }
            else
            {
                BuyMenuDrawMapMetatile(i, j, (u16*)Rogue_ModifyOverworldTileset(mapLayout->secondaryTileset)->metatiles + ((metatile - NUM_METATILES_IN_PRIMARY) * 8), metatileLayerType);
            }
        }
    }
}

static void BuyMenuDrawMapMetatile(s16 x, s16 y, const u16 *src, u8 metatileLayerType)
{
    u16 offset1 = x * 2;
    u16 offset2 = y * 64;

    switch (metatileLayerType)
    {
    case METATILE_LAYER_TYPE_NORMAL:
        BuyMenuDrawMapMetatileLayer(sShopData->tilemapBuffers[3], offset1, offset2, src);
        BuyMenuDrawMapMetatileLayer(sShopData->tilemapBuffers[1], offset1, offset2, src + 4);
        break;
    case METATILE_LAYER_TYPE_COVERED:
        BuyMenuDrawMapMetatileLayer(sShopData->tilemapBuffers[2], offset1, offset2, src);
        BuyMenuDrawMapMetatileLayer(sShopData->tilemapBuffers[3], offset1, offset2, src + 4);
        break;
    case METATILE_LAYER_TYPE_SPLIT:
        BuyMenuDrawMapMetatileLayer(sShopData->tilemapBuffers[2], offset1, offset2, src);
        BuyMenuDrawMapMetatileLayer(sShopData->tilemapBuffers[1], offset1, offset2, src + 4);
        break;
    }
}

static void BuyMenuDrawMapMetatileLayer(u16 *dest, s16 offset1, s16 offset2, const u16 *src)
{
    // This function draws a whole 2x2 metatile.
    dest[offset1 + offset2] = src[0]; // top left
    dest[offset1 + offset2 + 1] = src[1]; // top right
    dest[offset1 + offset2 + 32] = src[2]; // bottom left
    dest[offset1 + offset2 + 33] = src[3]; // bottom right
}

static void BuyMenuCollectObjectEventData(void)
{
    s16 facingX;
    s16 facingY;
    u8 y;
    u8 x;
    u8 r8 = 0;

    GetXYCoordsOneStepInFrontOfPlayer(&facingX, &facingY);
    for (y = 0; y < OBJECT_EVENTS_COUNT; y++)
        sShopData->viewportObjects[y][OBJ_EVENT_ID] = OBJECT_EVENTS_COUNT;
    for (y = 0; y < 5; y++)
    {
        for (x = 0; x < 7; x++)
        {
            u8 objEventId = GetObjectEventIdByXY(facingX - 4 + x, facingY - 2 + y);

            if (objEventId != OBJECT_EVENTS_COUNT)
            {
                sShopData->viewportObjects[r8][OBJ_EVENT_ID] = objEventId;
                sShopData->viewportObjects[r8][X_COORD] = x;
                sShopData->viewportObjects[r8][Y_COORD] = y;
                sShopData->viewportObjects[r8][LAYER_TYPE] = MapGridGetMetatileLayerTypeAt(facingX - 4 + x, facingY - 2 + y);

                switch (gObjectEvents[objEventId].facingDirection)
                {
                    case DIR_SOUTH:
                        sShopData->viewportObjects[r8][ANIM_NUM] = 0;
                        break;
                    case DIR_NORTH:
                        sShopData->viewportObjects[r8][ANIM_NUM] = 1;
                        break;
                    case DIR_WEST:
                        sShopData->viewportObjects[r8][ANIM_NUM] = 2;
                        break;
                    case DIR_EAST:
                    default:
                        sShopData->viewportObjects[r8][ANIM_NUM] = 3;
                        break;
                }
                r8++;
            }
        }
    }
}

static void BuyMenuDrawObjectEvents(void)
{
    u8 i;
    u8 spriteId;
    const struct ObjectEventGraphicsInfo *graphicsInfo;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        if (sShopData->viewportObjects[i][OBJ_EVENT_ID] == OBJECT_EVENTS_COUNT)
            continue;

        graphicsInfo = GetObjectEventGraphicsInfo(gObjectEvents[sShopData->viewportObjects[i][OBJ_EVENT_ID]].graphicsId);

        spriteId = CreateObjectGraphicsSprite(
            gObjectEvents[sShopData->viewportObjects[i][OBJ_EVENT_ID]].graphicsId,
            SpriteCallbackDummy,
            (u16)sShopData->viewportObjects[i][X_COORD] * 16 + 8,
            (u16)sShopData->viewportObjects[i][Y_COORD] * 16 + 48 - graphicsInfo->height / 2,
            2);

        if (BuyMenuCheckIfObjectEventOverlapsMenuBg(sShopData->viewportObjects[i]) == TRUE)
        {
            gSprites[spriteId].subspriteTableNum = 4;
            gSprites[spriteId].subspriteMode = SUBSPRITES_ON;
        }

        // RogueNote: don't start anim for truck as it breaks 
        if(gObjectEvents[sShopData->viewportObjects[i][OBJ_EVENT_ID]].graphicsId != OBJ_EVENT_GFX_TRUCK)
        {
            StartSpriteAnim(&gSprites[spriteId], sShopData->viewportObjects[i][ANIM_NUM]);
        }

        // TODO - add riding sprite??
    }
}

static bool8 BuyMenuCheckIfObjectEventOverlapsMenuBg(s16 *object)
{
    if (!BuyMenuCheckForOverlapWithMenuBg(object[X_COORD], object[Y_COORD] + 2) && object[LAYER_TYPE] != METATILE_LAYER_TYPE_COVERED)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void BuyMenuCopyMenuBgToBg1TilemapBuffer(void)
{
    s16 i;
    u16 *dest = sShopData->tilemapBuffers[1];
    const u16 *src = sShopData->tilemapBuffers[0];

    for (i = 0; i < 1024; i++)
    {
        if (src[i] != 0)
        {
            dest[i] = src[i] + 0xC3E3;
        }
    }
}

static bool8 BuyMenuCheckForOverlapWithMenuBg(int x, int y)
{
    const u16 *metatile = sShopData->tilemapBuffers[0];
    int offset1 = x * 2;
    int offset2 = y * 64;

    if (metatile[offset2 + offset1] == 0 &&
        metatile[offset2 + offset1 + 32] == 0 &&
        metatile[offset2 + offset1 + 1] == 0 &&
        metatile[offset2 + offset1 + 33] == 0)
    {
        return TRUE;
    }

    return FALSE;
}

static void Task_BuyMenu(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        s32 itemId = ListMenu_ProcessInput(tListTaskId);
        ListMenuGetScrollAndRow(tListTaskId, &sShopData->scrollOffset, &sShopData->selectedRow);

        switch (itemId)
        {
        case LIST_NOTHING_CHOSEN:
            break;
        case LIST_CANCEL:
            PlaySE(SE_SELECT);
            ExitBuyMenu(taskId);
            break;
        default:
            PlaySE(SE_SELECT);
            tItemId = itemId;
            ClearWindowTilemap(2);
            BuyMenuRemoveScrollIndicatorArrows();
            BuyMenuPrintCursor(tListTaskId, 2);

            sShopData->totalCost = GetShopItemPrice(itemId);

            if(sShopData->totalCost == 0 && !IsZeroPriceMarkedAsFree())
            {
                BuyMenuDisplayMessage(taskId, gText_AlreadyOwnThis, BuyMenuReturnToItemList);
            }
            else if (GetShopCurrencyAmount() < sShopData->totalCost)
            {
                BuyMenuDisplayMessage(taskId, gText_YouDontHaveMoney, BuyMenuReturnToItemList);
            }
            else
            {
                CopyShopItemName(itemId, gStringVar1);

                if (sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
                {
                    CopyShopItemName(itemId, gStringVar1);
                    //if (ItemId_GetPocket(itemId) == POCKET_TM_HM)
                    //{
                    //    StringCopy(gStringVar2, gMoveNames[ItemIdToBattleMoveId(itemId)]);
                    //    BuyMenuDisplayMessage(taskId, gText_Var1CertainlyHowMany2, Task_BuyHowManyDialogueInit);
                    //}
                    //else
                    {
                        BuyMenuDisplayMessage(taskId, gText_Var1CertainlyHowMany, Task_BuyHowManyDialogueInit);
                    }
                }
                else if(sMartInfo.martType == MART_TYPE_HUB_AREAS)
                {
                    CopyShopItemName(itemId, gStringVar1);

                    ConvertIntToDecimalStringN(gStringVar2, sShopData->totalCost, STR_CONV_MODE_LEFT_ALIGN, 6);
                    StringCopy(gStringVar3, gRogueHubAreas[RogueHub_GetAreaFromCurrentMap()].areaName);

                    StringExpandPlaceholders(gStringVar4, gText_Var1FromVar2FromVar3BuildDirection);
                    BuyMenuDisplayMessage(taskId, gStringVar4, Task_BuyHowManyDialogueInit);
                }
                else if(sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
                {
                    CopyShopItemName(itemId, gStringVar1);
                    StringCopy(gStringVar2, gRogueHubAreas[gRogueHubUpgrades[itemId].targetArea].areaName);

                    ConvertIntToDecimalStringN(gStringVar3, sShopData->totalCost, STR_CONV_MODE_LEFT_ALIGN, 6);

                    StringExpandPlaceholders(gStringVar4, gText_BuildVar1InVar2ThatllBeVar3);
                    BuyMenuDisplayMessage(taskId, gStringVar4, BuyMenuConfirmPurchase);
                }
                else 
                {
                    CopyShopItemName(itemId, gStringVar1);
                    ConvertIntToDecimalStringN(gStringVar2, sShopData->totalCost, STR_CONV_MODE_LEFT_ALIGN, 6);

                    if (sMartInfo.martType == MART_TYPE_DECOR)
                        StringExpandPlaceholders(gStringVar4, gText_Var1IsItThatllBeVar2);
                    else // MART_TYPE_DECOR2
                        StringExpandPlaceholders(gStringVar4, gText_YouWantedVar1ThatllBeVar2);

                    BuyMenuDisplayMessage(taskId, gStringVar4, BuyMenuConfirmPurchase);
                }
            }
            break;
        }
    }
}

static void Task_BuyHowManyDialogueInit(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    u16 maxQuantity;

    if(sMartInfo.martType == MART_TYPE_HUB_AREAS)
    {
        StringCopy(gStringVar1, gRogueHubAreas[RogueHub_GetAreaFromCurrentMap()].areaName);
        StringExpandPlaceholders(gStringVar4, gText_FromVar1);
    }
    else
    {
        u16 quantityInBag = CountTotalItemQuantityInBag(tItemId);
        ConvertIntToDecimalStringN(gStringVar1, quantityInBag, STR_CONV_MODE_RIGHT_ALIGN, MAX_ITEM_DIGITS + 1);
        StringExpandPlaceholders(gStringVar4, gText_InBagVar1);
    }

    DrawStdFrameWithCustomTileAndPalette(3, FALSE, 1, 13);
    BuyMenuPrint(3, gStringVar4, 0, 1, 0, 0);

    tItemCount = 1;
    DrawStdFrameWithCustomTileAndPalette(4, FALSE, 1, 13);

    if(sMartInfo.martType == MART_TYPE_HUB_AREAS)
        BuyMenuPrintBuildDirection(taskId);
    else
        BuyMenuPrintItemQuantityAndPrice(taskId);

    ScheduleBgCopyTilemapToVram(0);

    // We're going to use this prompt to select a direction
    if(sMartInfo.martType == MART_TYPE_HUB_AREAS)
    {
        // We're going to use this prompt to select a direction
        gTasks[taskId].func = Task_BuyWhichDirectionDialogueHandleInput;
    }
    else
    {
        if(sShopData->totalCost == 0)
            maxQuantity = MAX_SHOP_ITEM_CAPACITY;
        else
            maxQuantity = GetShopCurrencyAmount() / sShopData->totalCost;

        // Max quantity is based on item stack size
        if(sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
        {
            maxQuantity = min(maxQuantity, Rogue_GetBagPocketAmountPerItem(ItemId_GetPocket(tItemId) - 1));

            // Can only buy 1 of infinite items
            if(tItemId <= ITEM_TM01 && tItemId >= ITEM_HM08)
                maxQuantity = 1;
        }

        if (maxQuantity > MAX_SHOP_ITEM_CAPACITY)
        {
            sShopData->maxQuantity = MAX_SHOP_ITEM_CAPACITY;
        }
        else
        {
            sShopData->maxQuantity = maxQuantity;
        }

        gTasks[taskId].func = Task_BuyHowManyDialogueHandleInput;
    }
}

static void Task_BuyHowManyDialogueHandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (AdjustQuantityAccordingToDPadInput(&tItemCount, sShopData->maxQuantity) == TRUE)
    {
        sShopData->totalCost = GetShopItemPrice(tItemId) * tItemCount;
        BuyMenuPrintItemQuantityAndPrice(taskId);
    }
    else
    {
        if (JOY_NEW(A_BUTTON))
        {
            PlaySE(SE_SELECT);
            ClearStdWindowAndFrameToTransparent(4, 0);
            ClearStdWindowAndFrameToTransparent(3, 0);
            ClearWindowTilemap(4);
            ClearWindowTilemap(3);
            PutWindowTilemap(1);
            CopyShopItemName(tItemId, gStringVar1);
            ConvertIntToDecimalStringN(gStringVar2, tItemCount, STR_CONV_MODE_LEFT_ALIGN, SHOP_ITEM_CAPACITY_DIGITS);
            ConvertIntToDecimalStringN(gStringVar3, sShopData->totalCost, STR_CONV_MODE_LEFT_ALIGN, 6);
            BuyMenuDisplayMessage(taskId, gText_Var1AndYouWantedVar2, BuyMenuConfirmPurchase);
        }
        else if (JOY_NEW(B_BUTTON))
        {
            PlaySE(SE_SELECT);
            ClearStdWindowAndFrameToTransparent(4, 0);
            ClearStdWindowAndFrameToTransparent(3, 0);
            ClearWindowTilemap(4);
            ClearWindowTilemap(3);
            BuyMenuReturnToItemList(taskId);
        }
    }
}

static void Task_BuyWhichDirectionDialogueHandleInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (JOY_NEW(DPAD_UP))
    {
        tItemCount = HUB_AREA_CONN_NORTH;
        BuyMenuPrintBuildDirection(taskId);
    }
    else if (JOY_NEW(DPAD_DOWN))
    {
        tItemCount = HUB_AREA_CONN_SOUTH;
        BuyMenuPrintBuildDirection(taskId);
    }
    else if (JOY_NEW(DPAD_LEFT))
    {
        tItemCount = HUB_AREA_CONN_WEST;
        BuyMenuPrintBuildDirection(taskId);
    }
    else if (JOY_NEW(DPAD_RIGHT))
    {
        tItemCount = HUB_AREA_CONN_EAST;
        BuyMenuPrintBuildDirection(taskId);
    }
    if (JOY_NEW(A_BUTTON))
    {
        if(BuyMenuIsBuildDirectionValid(tItemId,tItemCount))
        {
            PlaySE(SE_SELECT);
            ClearStdWindowAndFrameToTransparent(4, 0);
            ClearStdWindowAndFrameToTransparent(3, 0);
            ClearWindowTilemap(4);
            ClearWindowTilemap(3);
            PutWindowTilemap(1);

            CopyShopItemName(tItemId, gStringVar1);

            switch (tItemCount)
            {
            case HUB_AREA_CONN_SOUTH:
                StringCopy(gStringVar2, gText_South);
                break;

            case HUB_AREA_CONN_NORTH:
                StringCopy(gStringVar2, gText_North);
                break;

            case HUB_AREA_CONN_WEST:
                StringCopy(gStringVar2, gText_West);
                break;

            case HUB_AREA_CONN_EAST:
                StringCopy(gStringVar2, gText_East);
                break;
            }
            
            ConvertIntToDecimalStringN(gStringVar3, sShopData->totalCost, STR_CONV_MODE_LEFT_ALIGN, 6);
            BuyMenuDisplayMessage(taskId, gText_BuildVar1ToVar2ForVar3, BuyMenuConfirmPurchase);
        }
        else
        {
            PlaySE(SE_FAILURE);
        }
    }
    else if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        ClearStdWindowAndFrameToTransparent(4, 0);
        ClearStdWindowAndFrameToTransparent(3, 0);
        ClearWindowTilemap(4);
        ClearWindowTilemap(3);
        BuyMenuReturnToItemList(taskId);
    }
}

static void BuyMenuConfirmPurchase(u8 taskId)
{
    CreateYesNoMenuWithCallbacks(taskId, &sShopBuyMenuYesNoWindowTemplates, 1, 0, 0, 1, 13, &sShopPurchaseYesNoFuncs);
}

static void BuyMenuTryMakePurchase(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    PutWindowTilemap(1);

    if (sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY)
    {
        if (BuyShopItem(tItemId, tItemCount) != FALSE)
        {
            BuyMenuDisplayMessage(taskId, gText_HereYouGoThankYou, BuyMenuSubtractMoney);
            RecordItemPurchase(taskId);
        }
        else
        {
            BuyMenuDisplayMessage(taskId, gText_NoMoreRoomForThis, BuyMenuReturnToItemList);
        }
    }
    else if (sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        if (BuyShopItem(tItemId, tItemCount) != FALSE)
        {
            BuyMenuDisplayMessage(taskId, gText_ThanksIllGiveLater, BuyMenuSubtractMoney);
            RecordItemPurchase(taskId);
        }
        else
        {
            BuyMenuDisplayMessage(taskId, gText_NoMoreRoomForThis, BuyMenuReturnToItemList);
        }
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        if (BuyShopItem(tItemId, tItemCount) != FALSE)
        {
            if(sMartInfo.martType == MART_TYPE_HUB_AREAS)
            {
                StringCopyN(gStringVar1, gRogueHubAreas[tItemId].areaName, ITEM_NAME_LENGTH + 4);
                BuyMenuDisplayMessage(taskId, gText_ThankYouBuildOrderSent, BuyMenuSubtractMoney);
            }
            else
            {
                StringCopyN(gStringVar1, gRogueHubAreas[gRogueHubUpgrades[tItemId].targetArea].areaName, ITEM_NAME_LENGTH + 4);
                BuyMenuDisplayMessage(taskId, gText_ThankYouUpgradeOrderSent, BuyMenuSubtractMoney);
            }
        }
        else
        {
            // Should never reach here
            BuyMenuDisplayMessage(taskId, gText_SpaceForVar1Full, BuyMenuReturnToItemList);
        }
    }
    else
    {
        if (BuyShopItem(tItemId, tItemCount) != FALSE)
        {
            if (sMartInfo.martType == MART_TYPE_DECOR)
                BuyMenuDisplayMessage(taskId, gText_ThankYouIllSendItHome, BuyMenuSubtractMoney);
            else // MART_TYPE_DECOR2
                BuyMenuDisplayMessage(taskId, gText_ThanksIllSendItHome, BuyMenuSubtractMoney);
        }
        else
        {
            BuyMenuDisplayMessage(taskId, gText_SpaceForVar1Full, BuyMenuReturnToItemList);
        }
    }
}

static void BuyMenuSubtractMoney(u8 taskId)
{
    RemoveShopCurrencyAmount(sShopData->totalCost);
    PlaySE(SE_SHOP);

    sMartInfo.anythingBought = TRUE;

    if(sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
        PrintMoneyAmountInMoneyBoxCustom(0, GetShopCurrencyAmount(), 0, gText_BuildVar1);
    else
        PrintMoneyAmountInMoneyBox(0, GetShopCurrencyAmount(), 0);

    if (sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        gTasks[taskId].func = Task_ReturnToItemListAfterItemPurchase;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        gTasks[taskId].func = Task_ReturnToItemListAfterDecorationPurchase;
    }
    else
    {
        gTasks[taskId].func = Task_ReturnToItemListAfterDecorationPurchase;
    }
}

static void Task_ReturnToItemListAfterItemPurchase(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        if (sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
        {
            ClearDialogWindowAndFrameToTransparent(5, 0);
            PutWindowTilemap(1);
            PutWindowTilemap(2);
            ScheduleBgCopyTilemapToVram(0);
            ExitBuyMenu(taskId);
        }
        else
        {
            u16 ballCount = tItemCount / 10;
            PlaySE(SE_SELECT);
            if (ItemId_GetPocket(tItemId) == POCKET_POKE_BALLS && ballCount != 0 && AddBagItem(ITEM_PREMIER_BALL, ballCount) == TRUE)
            {
                CopyItemNameHandlePlural(ITEM_PREMIER_BALL, gStringVar1, ballCount);
                if(ballCount > 1)
                    BuyMenuDisplayMessage(taskId, gText_ThrowInPremierBalls, BuyMenuReturnToItemList);
                else
                    BuyMenuDisplayMessage(taskId, gText_ThrowInPremierBall, BuyMenuReturnToItemList);
            }
            else
            {
                BuyMenuReturnToItemList(taskId);
            }
        }
    }
}

static void Task_ReturnToItemListAfterDecorationPurchase(u8 taskId)
{
    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BuyMenuReturnToItemList(taskId);
    }
}

static void BuyMenuReturnToItemList(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    // Recontruct if shop content is dynamic (Force always on after purchase?)
    //if(sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        u16 previousItemCount = sMartInfo.itemCount;

        BuyMenuBuildListMenuTemplate();
        DestroyListMenuTask(tListTaskId, &sShopData->scrollOffset, &sShopData->selectedRow);

        if(sMartInfo.itemCount < previousItemCount)
        {
            // Need to shift cursor up to avoid overflowing the bottom of the list
            if(sShopData->scrollOffset == 0)
            {
                //if(sShopData->selectedRow != 0)
                //    --sShopData->selectedRow;
            }
            else
            {
                --sShopData->scrollOffset;
            }
        }

        tListTaskId = ListMenuInit(&gMultiuseListMenuTemplate, sShopData->scrollOffset, sShopData->selectedRow);
    }

    ClearDialogWindowAndFrameToTransparent(5, 0);
    BuyMenuPrintCursor(tListTaskId, 1);
    PutWindowTilemap(1);
    PutWindowTilemap(2);
    ScheduleBgCopyTilemapToVram(0);
    BuyMenuAddScrollIndicatorArrows();
    gTasks[taskId].func = Task_BuyMenu;
}

static void BuyMenuPrintItemQuantityAndPrice(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    FillWindowPixelBuffer(4, PIXEL_FILL(1));
    PrintMoneyAmount(4, 38, 1, sShopData->totalCost, TEXT_SKIP_DRAW);
    ConvertIntToDecimalStringN(gStringVar1, tItemCount, STR_CONV_MODE_LEADING_ZEROS, SHOP_ITEM_CAPACITY_DIGITS);
    StringExpandPlaceholders(gStringVar4, gText_xVar1);
    BuyMenuPrint(4, gStringVar4, 0, 1, 0, 0);
}

static bool8 BuyMenuIsBuildDirectionValid(u8 buildArea, u8 dir)
{
    u8 fromArea = RogueHub_GetAreaFromCurrentMap();
    return RogueHub_CanBuildConnectionBetween(fromArea, buildArea, dir);
}

static void BuyMenuPrintBuildDirection(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    FillWindowPixelBuffer(4, PIXEL_FILL(1));

    if(BuyMenuIsBuildDirectionValid(tItemId, tItemCount))
    {
        switch (tItemCount)
        {
        case HUB_AREA_CONN_SOUTH:
            BuyMenuPrint(4, gText_ExpandSouth, 0, 1, 0, 0);
            break;

        case HUB_AREA_CONN_NORTH:
            BuyMenuPrint(4, gText_ExpandNorth, 0, 1, 0, 0);
            break;

        case HUB_AREA_CONN_WEST:
            BuyMenuPrint(4, gText_ExpandWest, 0, 1, 0, 0);
            break;

        case HUB_AREA_CONN_EAST:
            BuyMenuPrint(4, gText_ExpandEast, 0, 1, 0, 0);
            break;
        }
    }
    else
    {
        switch (tItemCount)
        {
        case HUB_AREA_CONN_SOUTH:
            BuyMenuPrint(4, gText_CannotExpandSouth, 0, 1, 0, 0);
            break;

        case HUB_AREA_CONN_NORTH:
            BuyMenuPrint(4, gText_CannotExpandNorth, 0, 1, 0, 0);
            break;

        case HUB_AREA_CONN_WEST:
            BuyMenuPrint(4, gText_CannotExpandWest, 0, 1, 0, 0);
            break;

        case HUB_AREA_CONN_EAST:
            BuyMenuPrint(4, gText_CannotExpandEast, 0, 1, 0, 0);
            break;
        }
    }
}

static void ExitBuyMenu(u8 taskId)
{
    gFieldCallback = MapPostLoadHook_ReturnToShopMenu;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = Task_ExitBuyMenu;
}

static void Task_ExitBuyMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        RemoveMoneyLabelObject();
        BuyMenuFreeMemory();
        SetMainCallback2(CB2_ReturnToField);
        DestroyTask(taskId);
    }
}

static void ClearItemPurchases(void)
{
    sPurchaseHistoryId = 0;
    memset(gMartPurchaseHistory, 0, sizeof(gMartPurchaseHistory));
}

static void RecordItemPurchase(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    u16 i;

    for (i = 0; i < ARRAY_COUNT(gMartPurchaseHistory); i++)
    {
        if (gMartPurchaseHistory[i].itemId == tItemId && gMartPurchaseHistory[i].quantity != 0)
        {
            if (gMartPurchaseHistory[i].quantity + tItemCount > 255)
                gMartPurchaseHistory[i].quantity = 255;
            else
                gMartPurchaseHistory[i].quantity += tItemCount;
            return;
        }
    }

    if (sPurchaseHistoryId < ARRAY_COUNT(gMartPurchaseHistory))
    {
        gMartPurchaseHistory[sPurchaseHistoryId].itemId = tItemId;
        gMartPurchaseHistory[sPurchaseHistoryId].quantity = tItemCount;
        sPurchaseHistoryId++;
    }
}

#undef tItemCount
#undef tItemId
#undef tListTaskId

static void CheckPokemartState()
{
    AGB_ASSERT(sShopData == NULL);
    AGB_ASSERT(sListMenuItems == NULL);
    AGB_ASSERT(sItemNames == NULL);
}

void CreatePokemartMenu(const u16 *itemsForSale)
{
    CheckPokemartState();
    ResetMartInfo();

    CreateShopMenu(MART_TYPE_NORMAL);
    SetShopItemsFromStaticList(itemsForSale, ITEM_NONE);
    ClearItemPurchases();
    SetShopMenuCallback(ScriptContext_Enable);
}

void CreateDecorationShop1Menu(const u16 *itemsForSale)
{
    CheckPokemartState();
    ResetMartInfo();

    CreateShopMenu(MART_TYPE_DECOR);
    SetShopItemsFromStaticList(itemsForSale, ITEM_NONE);
    SetShopMenuCallback(ScriptContext_Enable);
}

void CreateDecorationShop2Menu(const u16 *itemsForSale)
{
    CheckPokemartState();
    ResetMartInfo();

    CreateShopMenu(MART_TYPE_DECOR2);
    SetShopItemsFromStaticList(itemsForSale, ITEM_NONE);
    SetShopMenuCallback(ScriptContext_Enable);
}

void CreatePokemartMenuWithMinPrice(const u16 *itemsForSale, u16 minPrice)
{
    CheckPokemartState();
    ResetMartInfo();

    sMartInfo.minPrice = minPrice;

    CreateShopMenu(MART_TYPE_NORMAL);
    SetShopItemsFromStaticList(itemsForSale, ITEM_NONE);
    ClearItemPurchases();
    SetShopMenuCallback(ScriptContext_Enable);
}

static u16 HubUpgradeShopItemListCallback(u16 index)
{
    if(sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        sMartInfo.listItemTerminator = HUB_UPGRADE_NONE;

        if(index < HUB_UPGRADE_COUNT)
        {
            u8 i, listIndex;
            listIndex = 0;

            for(i = 0; i < HUB_UPGRADE_COUNT; ++i)
            {
                if(!RogueHub_HasUpgrade(i) && RogueHub_HasUpgradeRequirements(i) && (RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_SHOPS) || !gRogueHubUpgrades[i].isHidden))
                {
                    if(listIndex == index)
                        return i;
                    else
                        ++listIndex;
                }
            }
        }

        return HUB_UPGRADE_NONE;
    }
    else // MART_TYPE_HUB_AREAS
    {
        sMartInfo.listItemTerminator = HUB_AREA_NONE;

        if(index < HUB_AREA_COUNT)
        {
            u8 i, listIndex;
            listIndex = 0;

            for(i = 0; i < HUB_AREA_COUNT; ++i)
            {
                if(!RogueHub_HasAreaBuilt(i) && RogueHub_HasAreaBuildRequirements(i))
                {
                    if(listIndex == index)
                        return i;
                    else
                        ++listIndex;
                }
            }
        }

        return HUB_AREA_NONE;
    }

    return HUB_UPGRADE_NONE;
}

static void FreeQueryShopItemList()
{
    if(sMartInfo.listItemData != NULL)
    {
        RogueListQuery_End();
        Rogue_CloseMartQuery();
        sMartInfo.listItemData = NULL;
    }
}

static u16 QueryShopItemListCallback(u16 index)
{
    u16 const* listPtr;

    // Run the query once and cache it for later
    if(sMartInfo.listItemData == NULL)
    {
        Rogue_OpenMartQuery(sMartInfo.dynamicMartCategory, &sMartInfo.minPrice);
        {
            u8 sortMode = ITEM_SORT_MODE_TYPE;
            bool8 flipSort = FALSE;

            switch (sMartInfo.dynamicMartCategory)
            {
            case ROGUE_SHOP_BERRIES:
            case ROGUE_SHOP_HELD_ITEMS:
            case ROGUE_SHOP_BATTLE_ENHANCERS:
            case ROGUE_SHOP_CHARMS:
            case ROGUE_SHOP_CURSES:
            case ROGUE_SHOP_TMS:
            case ROGUE_SHOP_COURIER:
                sortMode = ITEM_SORT_MODE_NAME;
                break;

            //case ROGUE_SHOP_BALLS:
            //    sortMode = ITEM_SORT_MODE_VALUE;
            //    flipSort = TRUE;
            //    break;
            }

            RogueListQuery_Begin();
            sMartInfo.listItemData = RogueListQuery_CollapseItems(sortMode, flipSort);
        }

        // End inner and outer query when we leave shop, as we might need some dynamic allocs
        sFreeCallback = FreeQueryShopItemList;
    }

    listPtr = (u16 const*)sMartInfo.listItemData;
    return listPtr[index];
}


void CreateDynamicPokemartMenu(const u16 category)
{
    CheckPokemartState();
    ResetMartInfo();

    if(category == ROGUE_SHOP_HUB_UPGRADES)
    {
        CreateShopMenu(MART_TYPE_HUB_UPGRADES);
        SetShopItemsFromCallback(HubUpgradeShopItemListCallback, HUB_UPGRADE_NONE, NULL);
        sMartInfo.dynamicMartCategory = category;
    }
    else
    {
        u16 martType = MART_TYPE_NORMAL;

        if(category == ROGUE_SHOP_CHARMS || category == ROGUE_SHOP_CURSES)
            martType = MART_TYPE_PURCHASE_ONLY;

        if(category == ROGUE_SHOP_COURIER)
            martType = MART_TYPE_SINGLE_PURCHASE;

        CreateShopMenu(martType);
        SetShopItemsFromCallback(QueryShopItemListCallback, ITEM_NONE, NULL);
        sMartInfo.dynamicMartCategory = category;
        ClearItemPurchases();
    }
    SetShopMenuCallback(ScriptContext_Enable);
}

// Item type callbacks
//

static void CopyShopItemName(u16 item, u8* name)
{
    if (sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        CopyItemNameN(item, name, ITEM_NAME_LENGTH + 4);
        return;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_AREAS)
    {
        StringCopyN(name, gRogueHubAreas[item].areaName, ITEM_NAME_LENGTH + 4);
        return;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        StringCopyN(name, gRogueHubUpgrades[item].upgradeName, ITEM_NAME_LENGTH + 4);
        return;
    }
    else
    {
        //StringCopyN(name, gDecorations[item].name, ITEM_NAME_LENGTH + 4);
        return;
    }

    //StringCopyN(name, gText_EmptyString7, ITEM_NAME_LENGTH + 4);
}

static const u8* GetShopItemDescription(u16 item)
{
    const u8* str = NULL;

    if (sMartInfo.martType == MART_TYPE_NORMAL  || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        str = ItemId_GetDescription(item);
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_AREAS)
    {
        str = gRogueHubAreas[item].descText;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        str = gRogueHubUpgrades[item].descText;
    }
    //else
        //str = gDecorations[item].description;

    if(str == NULL)
        str = gText_EmptyString7;

    return str;
}

static u32 GetShopItemPrice(u16 item)
{
    if (sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        u32 price = Mart_GetItemPrice(item) >> IsPokeNewsActive(POKENEWS_SLATEPORT);

        if(sMartInfo.dynamicMartCategory == ROGUE_SHOP_TMS)
        {
            // Override TMs/HMs price if we have them
            if(item >= ITEM_TM01 && item <= ITEM_HM08 && CheckBagHasItem(item, 1))
                price = 0;
        }

        return price;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_AREAS)
    {
        return gRogueHubAreas[item].buildCost;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        return gRogueHubUpgrades[item].buildCost;
    }
    else
    {
        //return gDecorations[item].price;
    }

    return 0;
}

static bool8 IsZeroPriceMarkedAsFree()
{
    if (sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY || sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        if(sMartInfo.dynamicMartCategory == ROGUE_SHOP_TMS)
        {
            // Theses aren't free just mark as already bought
            return FALSE;
        }
    }

    // Assume 0 is free by default
    return TRUE;
}

static bool8 BuyShopItem(u16 item, u16 count)
{
    if (sMartInfo.martType == MART_TYPE_NORMAL || sMartInfo.martType == MART_TYPE_PURCHASE_ONLY)
    {
        IncrementGameStat(GAME_STAT_ITEMS_BOUGHT); // todo - make this item count
        return AddBagItem(item, count);
    }
    else if(sMartInfo.martType == MART_TYPE_SINGLE_PURCHASE)
    {
        AGB_ASSERT(VarGet(VAR_ROGUE_COURIER_ITEM) == ITEM_NONE);
        VarSet(VAR_ROGUE_COURIER_ITEM, item);
        VarSet(VAR_ROGUE_COURIER_COUNT, count);
        return TRUE;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_AREAS)
    {
        RogueHub_BuildAreaInConnDir(item, count);
        return TRUE;
    }
    else if (sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        RogueHub_SetUpgrade(item, TRUE);
        return TRUE;
    }
    else
    {
        return DecorationAdd(item);
    }
}

static u32 GetShopCurrencyAmount()
{
    if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        return GetItemCountInBag(ITEM_BUILDING_SUPPLIES);
    }

    return GetMoney(&gSaveBlock1Ptr->money);
}

static void RemoveShopCurrencyAmount(u32 amount)
{
    if (sMartInfo.martType == MART_TYPE_HUB_AREAS || sMartInfo.martType == MART_TYPE_HUB_UPGRADES)
    {
        RemoveBagItem(ITEM_BUILDING_SUPPLIES, amount);
    }
    else
    {
        RemoveMoney(&gSaveBlock1Ptr->money, amount);
        Rogue_OnSpendMoney(amount);
    }
}