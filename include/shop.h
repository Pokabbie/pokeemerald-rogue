#ifndef GUARD_SHOP_H
#define GUARD_SHOP_H

extern struct ItemSlot gMartPurchaseHistory[3];

void CreatePokemartMenu(const u16 *);
void CreateDecorationShop1Menu(const u16 *);
void CreateDecorationShop2Menu(const u16 *);
void CreatePokemartMenuWithMinPrice(const u16 *, u16 minPrice);
void CreateDynamicPokemartMenu(const u16 category);
void CreateCustomPokemartMenu(const u16 * buffer, u16 currency);
void CB2_ExitSellMenu(void);

#endif // GUARD_SHOP_H
