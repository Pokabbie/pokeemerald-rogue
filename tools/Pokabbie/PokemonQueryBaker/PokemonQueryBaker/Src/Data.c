#include "BakeHelpers.h"
#ifdef ROGUE_EXPANSION
const union AnimCmd sAnim_GeneralFrame0[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END,
};

#include "../Src/data/graphics/pokemon.h"
#include "../Src/data/pokemon_graphics/front_pic_anims.h"
#include "../Src/data/pokemon/form_change_tables.h"
#include "../Src/data/pokemon/form_change_table_pointers.h"
#include "../Src/data/pokemon/form_species_tables.h"
#include "data/pokemon/species_info.h"
#else
#include "data/pokemon/evolution.h"
#include "data/pokemon/base_stats.h"
#include "data/graphics/pokemon.h"
#endif
#include "data/rogue_pokedex.h"