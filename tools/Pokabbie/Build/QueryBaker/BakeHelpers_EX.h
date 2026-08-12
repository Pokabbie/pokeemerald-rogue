#ifndef BAKE_HELPERS_EX_H
#define BAKE_HELPERS_EX_H

#define MON_COORDS_SIZE(width, height)(DIV_ROUND_UP(width, 8) << 4 | DIV_ROUND_UP(height, 8))

#include "config.h"
#include "config/pokemon.h"
#include "constants/battle.h"
#include "constants/battle_script_commands.h"
#include "constants/cries.h"
#include "constants/form_change_types.h"
#include "constants/hold_effects.h"
#include "constants/z_move_effects.h"
#include "battle_dynamax.h"
#include "pokemon_animation.h"

bool32 IsSpeciesEnabled(u16 species);
u16 SanitizeSpeciesId(u16 species);

const u16* GetSpeciesFormTable(u16 species);
u16 GetFormSpeciesId(u16 speciesId, u8 formId);

#endif // BAKE_HELPERS_EX_H