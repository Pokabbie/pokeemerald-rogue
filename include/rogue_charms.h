#ifndef ROGUE_CHARMS__H
#define ROGUE_CHARMS__H

enum RogueCharmEffects
{
    EFFECT_SHOP_PRICE,
    EFFECT_FLINCH_CHANCE,
    EFFECT_CRIT_CHANCE,
    EFFECT_SHED_SKIN_CHANCE,
    EFFECT_WILD_IV_RATE,
    EFFECT_CATCH_RATE,

    EFFECT_COUNT,
};

bool8 IsCharmActive(u8 effectType);
bool8 IsCurseActive(u8 effectType);

u16 GetCharmValue(u8 effectType);
u16 GetCurseValue(u8 effectType);

u16 Rogue_NextCharmItem(u16* historyBuffer, u16 historyBufferCount);
u16 Rogue_NextCurseItem(u16* historyBuffer, u16 historyBufferCount);

#endif