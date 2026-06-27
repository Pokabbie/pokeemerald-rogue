#include "BakeHelpers.h"

#ifdef ROGUE_EXPANSION
#include "data/graphics/pokemon.h"
#include "data/pokemon_graphics/front_pic_anims.h"
#include "data/pokemon/form_change_tables.h"
#include "data/pokemon/form_change_table_pointers.h"
#include "data/pokemon/form_species_tables.h"
#include "data/pokemon/species_info.h"
#else
#include "data/pokemon/evolution.h"
#include "data/pokemon/base_stats.h"
#include "data/graphics/pokemon.h"
#endif
#include "data/rogue_pokedex.h"

#ifdef ROGUE_EXPANSION
#include "data/rogue/pokemon_expansion_profiles.h"
#include "data/rogue/pokemon_expansion_profiles_revised.h"
#else
#include "data/rogue/pokemon_vanilla_profiles.h"
#include "data/rogue/pokemon_vanilla_profiles_revised.h"
#endif

#include "rogue_baked.c"