#pragma once
#include "gba/gba.h"
#include "constants/global.h"
#include "constants/abilities.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/region_map_sections.h"
#include "constants/map_groups.h"
#include "constants/rogue.h"

#define min(x, y) (x < y ? x : y)
#define max(x, y) (x > y ? x : y)

struct Evolution
{
    u16 method;
    u16 param;
    u16 targetSpecies;
};

struct BaseStats
{
#ifndef ROGUE_EXPANSION
    u8 baseHP;
    u8 baseAttack;
    u8 baseDefense;
    u8 baseSpeed;
    u8 baseSpAttack;
    u8 baseSpDefense;
    u8 type1;
    u8 type2;
    u8 catchRate;
    u8 expYield;
    u16 evYield_HP : 2;
    u16 evYield_Attack : 2;
    u16 evYield_Defense : 2;
    u16 evYield_Speed : 2;
    u16 evYield_SpAttack : 2;
    u16 evYield_SpDefense : 2;
    u16 itemCommon;
    u16 itemRare;
    u8 genderRatio;
    u8 eggCycles;
    u8 friendship;
    u8 growthRate;
    u8 eggGroup1;
    u8 eggGroup2;
    u8 abilities[2];
    u8 safariZoneFleeRate;
    u8 bodyColor : 7;
    u8 noFlip : 1;
#else
    u8 baseHP;
    u8 baseAttack;
    u8 baseDefense;
    u8 baseSpeed;
    u8 baseSpAttack;
    u8 baseSpDefense;
    u8 type1;
    u8 type2;
    u8 catchRate;
    u16 expYield;
    u16 evYield_HP : 2;
    u16 evYield_Attack : 2;
    u16 evYield_Defense : 2;
    u16 evYield_Speed : 2;
    u16 evYield_SpAttack : 2;
    u16 evYield_SpDefense : 2;
    u16 itemCommon;
    u16 itemRare;
    u8 genderRatio;
    u8 eggCycles;
    u8 friendship;
    u8 growthRate;
    u8 eggGroup1;
    u8 eggGroup2;
    u16 abilities[NUM_ABILITY_SLOTS];
    u8 safariZoneFleeRate;
    u8 bodyColor : 7;
    u8 noFlip : 1;
    u8 flags;
#endif
};

void memcpy(void* dst, void* src, size_t size);