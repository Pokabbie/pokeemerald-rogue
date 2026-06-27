#ifndef BAKE_HELPERS_EX_H
#define BAKE_HELPERS_EX_H

#define MON_COORDS_SIZE(width, height)(DIV_ROUND_UP(width, 8) << 4 | DIV_ROUND_UP(height, 8))
//#define GET_MON_COORDS_WIDTH(size)((size >> 4) * 8)
//#define GET_MON_COORDS_HEIGHT(size)((size & 0xF) * 8)
//#define TRAINER_PARTY_IVS(hp, atk, def, speed, spatk, spdef) (hp | (atk << 5) | (def << 10) | (speed << 15) | (spatk << 20) | (spdef << 25))
//#define TRAINER_PARTY_EVS(hp, atk, def, speed, spatk, spdef) ((const u8[6]){hp,atk,def,spatk,spdef,speed})
//#define TRAINER_PARTY_NATURE(nature) (nature+1)

//#include "gba/gba.h"
//#include "constants/global.h"
//#include "constants/rogue.h"
//
//#ifdef ROGUE_EXPANSION
#include "config.h"
#include "config/pokemon.h"
#include "constants/cries.h"
//#include "constants/pokedex.h"
#include "constants/form_change_types.h"
#include "constants/battle.h"
//#include "constants/maps.h"
#include "pokemon_animation.h"
//#endif

bool32 IsSpeciesEnabled(u16 species);
u16 SanitizeSpeciesId(u16 species);

const u16* GetSpeciesFormTable(u16 species);
u16 GetFormSpeciesId(u16 speciesId, u8 formId);

#endif // BAKE_HELPERS_EX_H