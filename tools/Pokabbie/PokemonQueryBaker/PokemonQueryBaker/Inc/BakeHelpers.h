#pragma once
#include "gba/gba.h"
#include "constants/global.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/region_map_sections.h"
#include "constants/map_groups.h"
#include "constants/rogue.h"

struct Evolution
{
    u16 method;
    u16 param;
    u16 targetSpecies;
};

void memcpy(void* dst, void* src, size_t size);