#ifndef ROGUE_CHARMS__H
#define ROGUE_CHARMS__H

enum RogueCharmEffects
{
    EFFECT_SHOP_PRICE,
    EFFECT_FLINCH_CHANCE,
    EFFECT_CRIT_CHANCE,
    EFFECT_SHED_SKIN_CHANCE,
};

bool8 IsCharmActive(u8 effectType);
bool8 IsCurseActive(u8 effectType);

u16 GetCharmValue(u8 effectType);
u16 GetCurseValue(u8 effectType);

void Rogue_SelectCharmItems(u16* outBuffer, u16 count);
void Rogue_SelectCurseItems(u16* outBuffer, u16 count);

#endif