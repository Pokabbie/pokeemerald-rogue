#pragma once
#include "gba/gba.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/items.h"

struct Evolution
{
    u16 method;
    u16 param;
    u16 targetSpecies;
};

void memcpy(void* dst, void* src, size_t size);