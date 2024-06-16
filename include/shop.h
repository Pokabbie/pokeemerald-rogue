#ifndef GUARD_SHOP_H
#define GUARD_SHOP_H

extern EWRAM_DATA struct ItemSlot gMartPurchaseHistory[3];

enum
{
    MART_TYPE_NORMAL, // normal mart
    MART_TYPE_PURCHASE_ONLY, // normal mart, but can only buy
    MART_TYPE_DECOR,
    MART_TYPE_DECOR2,
    MART_TYPE_HUB_AREAS,
    MART_TYPE_HUB_UPGRADES,
    MART_TYPE_SINGLE_PURCHASE,
};

// shop view window NPC info enum
enum
{
    OBJ_EVENT_ID,
    X_COORD,
    Y_COORD,
    ANIM_NUM,
    LAYER_TYPE
};

struct MartInfo
{
    void (*callback)(void);
    const struct MenuAction *menuActions;
    u16 (*listItemCallback)(u16);
    void const* listItemData;
    u16 listItemTerminator;
    u16 itemCount;
    u16 minPrice;
    u16 dynamicMartCategory;
    u8 windowId;
    u8 martType;
    bool8 anythingBought;
};

struct ShopData
{
    /*0x0000*/ u16 tilemapBuffers[4][0x400];
    /*0x2000*/ u32 totalCost;
    /*0x2004*/ u16 itemsShowed;
    /*0x2006*/ u16 selectedRow;
    /*0x2008*/ u16 scrollOffset;
    /*0x200A*/ u16 maxQuantity;
    /*0x200B*/ u8 scrollIndicatorsTaskId;
    /*0x200C*/ u8 iconSlot;
    /*0x200D*/ u8 itemSpriteIds[2];
    /*0x2010*/ s16 viewportObjects[OBJECT_EVENTS_COUNT][5];
};

void CreatePokemartMenu(const u16 *);
void CreateDecorationShop1Menu(const u16 *);
void CreateDecorationShop2Menu(const u16 *);
void CreatePokemartMenuWithMinPrice(const u16 *, u16 minPrice);
void CreateDynamicPokemartMenu(const u16 category);
void CB2_ExitSellMenu(void);

#endif // GUARD_SHOP_H
