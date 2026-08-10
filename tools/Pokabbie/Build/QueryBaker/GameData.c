#include "BakeHelpers.h"

#ifdef ROGUE_EXPANSION
const union AnimCmd sAnim_GeneralFrame0[] =
{
};

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

bool32 IsSpeciesEnabled(u16 species)
{
    return gRogueSpeciesInfo[species].baseHP > 0 || species == SPECIES_EGG;
}

u16 SanitizeSpeciesId(u16 species)
{
    if (species > NUM_SPECIES || !IsSpeciesEnabled(species))
        return SPECIES_NONE;
    else
        return species;
}

#ifdef ROGUE_EXPANSION
const u16* GetSpeciesFormTable(u16 species)
{
    const u16* formTable = gRogueSpeciesInfo[SanitizeSpeciesId(species)].formSpeciesIdTable;
    if (formTable == NULL)
        return gRogueSpeciesInfo[SPECIES_NONE].formSpeciesIdTable;
    return formTable;
}

u16 GetFormSpeciesId(u16 speciesId, u8 formId)
{
    if (GetSpeciesFormTable(speciesId) != NULL)
        return GetSpeciesFormTable(speciesId)[formId];
    else
        return speciesId;
}
#endif