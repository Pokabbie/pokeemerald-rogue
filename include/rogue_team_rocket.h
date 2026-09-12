#ifndef GUARD_ROGUE_TEAM_ROCKET_H
#define GUARD_ROGUE_TEAM_ROCKET_H

#include "global.h"
#include "constants/species.h"

#ifdef ROGUE_EXPANSION

struct RogueTeamRocketSpeciesPair
{
    u16 baseSpecies;
    u16 rocketSpecies;
};

static const struct RogueTeamRocketSpeciesPair sRogueTeamRocketSpeciesPairs[] =
{
    { SPECIES_ARBOK, SPECIES_ARBOK_ROCKET },
    { SPECIES_RAICHU, SPECIES_RAICHU_ROCKET },
    { SPECIES_MACHAMP, SPECIES_MACHAMP_ROCKET },
    { SPECIES_HITMONLEE, SPECIES_HITMONLEE_ROCKET },
    { SPECIES_FLYGON, SPECIES_FLYGON_ROCKET },
    { SPECIES_LUXRAY, SPECIES_LUXRAY_ROCKET },
    { SPECIES_MIGHTYENA, SPECIES_MIGHTYENA_ROCKET },
    { SPECIES_SEVIPER, SPECIES_SEVIPER_ROCKET },
    { SPECIES_BANETTE, SPECIES_BANETTE_ROCKET },
    { SPECIES_ABSOL, SPECIES_ABSOL_ROCKET },
    { SPECIES_DONPHAN, SPECIES_DONPHAN_ROCKET },
    { SPECIES_REVAVROOM, SPECIES_REVAVROOM_ROCKET },
    { SPECIES_CERULEDGE, SPECIES_CERULEDGE_ROCKET },
    { SPECIES_PINSIR, SPECIES_PINSIR_ROCKET },
    { SPECIES_CROBAT, SPECIES_CROBAT_ROCKET },
    { SPECIES_PERSIAN, SPECIES_PERSIAN_ROCKET },
    { SPECIES_NIDOKING, SPECIES_NIDOKING_ROCKET },
    { SPECIES_NIDOQUEEN, SPECIES_NIDOQUEEN_ROCKET },
    { SPECIES_WEEZING, SPECIES_WEEZING_ROCKET },
    { SPECIES_HYPNO, SPECIES_HYPNO_ROCKET },
    { SPECIES_ELECTIVIRE, SPECIES_ELECTIVIRE_ROCKET },
    { SPECIES_MAGMORTAR, SPECIES_MAGMORTAR_ROCKET },
    { SPECIES_SCIZOR, SPECIES_SCIZOR_ROCKET },
    { SPECIES_GYARADOS, SPECIES_GYARADOS_ROCKET },
    { SPECIES_AERODACTYL, SPECIES_AERODACTYL_ROCKET },
    { SPECIES_ARIADOS, SPECIES_ARIADOS_ROCKET },
    { SPECIES_XATU, SPECIES_XATU_ROCKET },
    { SPECIES_AMPHAROS, SPECIES_AMPHAROS_ROCKET },
    { SPECIES_SUDOWOODO, SPECIES_SUDOWOODO_ROCKET },
    { SPECIES_HONCHKROW, SPECIES_HONCHKROW_ROCKET },
    { SPECIES_MISMAGIUS, SPECIES_MISMAGIUS_ROCKET },
    { SPECIES_FORRETRESS, SPECIES_FORRETRESS_ROCKET },
    { SPECIES_GLISCOR, SPECIES_GLISCOR_ROCKET },
    { SPECIES_GRANBULL, SPECIES_GRANBULL_ROCKET },
    { SPECIES_HOUNDOOM, SPECIES_HOUNDOOM_ROCKET },
    { SPECIES_SHARPEDO, SPECIES_SHARPEDO_ROCKET },
    { SPECIES_CACTURNE, SPECIES_CACTURNE_ROCKET },
    { SPECIES_DRAPION, SPECIES_DRAPION_ROCKET },
    { SPECIES_TOXICROAK, SPECIES_TOXICROAK_ROCKET },
    { SPECIES_SPIRITOMB, SPECIES_SPIRITOMB_ROCKET },
    { SPECIES_KROOKODILE, SPECIES_KROOKODILE_ROCKET },
    { SPECIES_SCRAFTY, SPECIES_SCRAFTY_ROCKET },
    { SPECIES_CHANDELURE, SPECIES_CHANDELURE_ROCKET },
    { SPECIES_BISHARP, SPECIES_BISHARP_ROCKET },
    { SPECIES_MALAMAR, SPECIES_MALAMAR_ROCKET },
    { SPECIES_SALAZZLE, SPECIES_SALAZZLE_ROCKET },
    { SPECIES_GOLISOPOD, SPECIES_GOLISOPOD_ROCKET },
    { SPECIES_TOXTRICITY_AMPED, SPECIES_TOXTRICITY_ROCKET },
    { SPECIES_GRIMMSNARL, SPECIES_GRIMMSNARL_ROCKET },
    { SPECIES_OBSTAGOON, SPECIES_OBSTAGOON_ROCKET },
    { SPECIES_GRAFAIAI, SPECIES_GRAFAIAI_ROCKET },
    { SPECIES_TYRANITAR, SPECIES_TYRANITAR_ROCKET },
    { SPECIES_DRAGONITE, SPECIES_DRAGONITE_ROCKET },
    { SPECIES_HYDREIGON, SPECIES_HYDREIGON_ROCKET },
    { SPECIES_GARCHOMP, SPECIES_GARCHOMP_ROCKET },
    { SPECIES_YANMEGA, SPECIES_YANMEGA_ROCKET },
    { SPECIES_SLAKING, SPECIES_SLAKING_ROCKET },
    { SPECIES_KANGASKHAN, SPECIES_KANGASKHAN_ROCKET },
};

static inline u32 RogueTeamRocket_GetVariantCount(void)
{
    return sizeof(sRogueTeamRocketSpeciesPairs)
         / sizeof(sRogueTeamRocketSpeciesPairs[0]);
}

static inline u16 RogueTeamRocket_GetBaseSpeciesByIndex(u32 index)
{
    return sRogueTeamRocketSpeciesPairs[index].baseSpecies;
}

static inline u16 RogueTeamRocket_GetVariantSpeciesByIndex(u32 index)
{
    return sRogueTeamRocketSpeciesPairs[index].rocketSpecies;
}

static inline u16 RogueTeamRocket_GetVariantSpecies(u16 species)
{
    u32 i;

    for(i = 0; i < RogueTeamRocket_GetVariantCount(); ++i)
    {
        if(sRogueTeamRocketSpeciesPairs[i].baseSpecies == species)
            return sRogueTeamRocketSpeciesPairs[i].rocketSpecies;
    }

    return SPECIES_NONE;
}

static inline u16 RogueTeamRocket_GetBaseSpecies(u16 species)
{
    u32 i;

    for(i = 0; i < RogueTeamRocket_GetVariantCount(); ++i)
    {
        if(sRogueTeamRocketSpeciesPairs[i].rocketSpecies == species)
            return sRogueTeamRocketSpeciesPairs[i].baseSpecies;
    }

    return species;
}

static inline bool8 RogueTeamRocket_IsVariant(u16 species)
{
    return RogueTeamRocket_GetBaseSpecies(species) != species;
}

#else

static inline u32 RogueTeamRocket_GetVariantCount(void)
{
    return 0;
}

static inline u16 RogueTeamRocket_GetBaseSpeciesByIndex(u32 index)
{
    return SPECIES_NONE;
}

static inline u16 RogueTeamRocket_GetVariantSpeciesByIndex(u32 index)
{
    return SPECIES_NONE;
}

static inline u16 RogueTeamRocket_GetVariantSpecies(u16 species)
{
    return SPECIES_NONE;
}

static inline u16 RogueTeamRocket_GetBaseSpecies(u16 species)
{
    return species;
}

static inline bool8 RogueTeamRocket_IsVariant(u16 species)
{
    return FALSE;
}

#endif // ROGUE_EXPANSION


// Universal Team Rocket conversion.
// Using the Dubious Disc on one of these source species
// produces its corresponding Team Rocket variant.
//
// Kept separate from the normal species evolution tables so
// Rogue's final-evolution and encounter logic is not altered.
static inline u16 RogueTeamRocket_GetEvolutionTarget(u16 species)
{
    switch (species)
    {
    case SPECIES_EKANS:
        return SPECIES_ARBOK_ROCKET;
    case SPECIES_PIKACHU:
        return SPECIES_RAICHU_ROCKET;
    case SPECIES_MACHOKE:
        return SPECIES_MACHAMP_ROCKET;
    case SPECIES_TYROGUE:
        return SPECIES_HITMONLEE_ROCKET;
    case SPECIES_VIBRAVA:
        return SPECIES_FLYGON_ROCKET;
    case SPECIES_LUXIO:
        return SPECIES_LUXRAY_ROCKET;
    case SPECIES_POOCHYENA:
        return SPECIES_MIGHTYENA_ROCKET;
    case SPECIES_SEVIPER:
        return SPECIES_SEVIPER_ROCKET;
    case SPECIES_SHUPPET:
        return SPECIES_BANETTE_ROCKET;
    case SPECIES_ABSOL:
        return SPECIES_ABSOL_ROCKET;
    case SPECIES_PHANPY:
        return SPECIES_DONPHAN_ROCKET;
    case SPECIES_VAROOM:
        return SPECIES_REVAVROOM_ROCKET;
    case SPECIES_CHARCADET:
        return SPECIES_CERULEDGE_ROCKET;
    case SPECIES_PINSIR:
        return SPECIES_PINSIR_ROCKET;
    case SPECIES_GOLBAT:
        return SPECIES_CROBAT_ROCKET;
    case SPECIES_MEOWTH:
        return SPECIES_PERSIAN_ROCKET;
    case SPECIES_NIDORINO:
        return SPECIES_NIDOKING_ROCKET;
    case SPECIES_NIDORINA:
        return SPECIES_NIDOQUEEN_ROCKET;
    case SPECIES_KOFFING:
        return SPECIES_WEEZING_ROCKET;
    case SPECIES_DROWZEE:
        return SPECIES_HYPNO_ROCKET;
    case SPECIES_ELECTABUZZ:
        return SPECIES_ELECTIVIRE_ROCKET;
    case SPECIES_MAGMAR:
        return SPECIES_MAGMORTAR_ROCKET;
    case SPECIES_SCYTHER:
        return SPECIES_SCIZOR_ROCKET;
    case SPECIES_MAGIKARP:
        return SPECIES_GYARADOS_ROCKET;
    case SPECIES_AERODACTYL:
        return SPECIES_AERODACTYL_ROCKET;
    case SPECIES_SPINARAK:
        return SPECIES_ARIADOS_ROCKET;
    case SPECIES_NATU:
        return SPECIES_XATU_ROCKET;
    case SPECIES_FLAAFFY:
        return SPECIES_AMPHAROS_ROCKET;
    case SPECIES_BONSLY:
        return SPECIES_SUDOWOODO_ROCKET;
    case SPECIES_MURKROW:
        return SPECIES_HONCHKROW_ROCKET;
    case SPECIES_MISDREAVUS:
        return SPECIES_MISMAGIUS_ROCKET;
    case SPECIES_PINECO:
        return SPECIES_FORRETRESS_ROCKET;
    case SPECIES_GLIGAR:
        return SPECIES_GLISCOR_ROCKET;
    case SPECIES_SNUBBULL:
        return SPECIES_GRANBULL_ROCKET;
    case SPECIES_HOUNDOUR:
        return SPECIES_HOUNDOOM_ROCKET;

    case SPECIES_CARVANHA:
        return SPECIES_SHARPEDO_ROCKET;
    case SPECIES_CACNEA:
        return SPECIES_CACTURNE_ROCKET;
    case SPECIES_SKORUPI:
        return SPECIES_DRAPION_ROCKET;
    case SPECIES_CROAGUNK:
        return SPECIES_TOXICROAK_ROCKET;
    case SPECIES_SPIRITOMB:
        return SPECIES_SPIRITOMB_ROCKET;
    case SPECIES_KROKOROK:
        return SPECIES_KROOKODILE_ROCKET;
    case SPECIES_SCRAGGY:
        return SPECIES_SCRAFTY_ROCKET;
    case SPECIES_LAMPENT:
        return SPECIES_CHANDELURE_ROCKET;
    case SPECIES_PAWNIARD:
        return SPECIES_BISHARP_ROCKET;
    case SPECIES_INKAY:
        return SPECIES_MALAMAR_ROCKET;
    case SPECIES_SALANDIT:
        return SPECIES_SALAZZLE_ROCKET;
    case SPECIES_WIMPOD:
        return SPECIES_GOLISOPOD_ROCKET;
    case SPECIES_TOXEL:
        return SPECIES_TOXTRICITY_ROCKET;
    case SPECIES_MORGREM:
        return SPECIES_GRIMMSNARL_ROCKET;
    case SPECIES_LINOONE_GALARIAN:
        return SPECIES_OBSTAGOON_ROCKET;
    case SPECIES_SHROODLE:
        return SPECIES_GRAFAIAI_ROCKET;
    case SPECIES_PUPITAR:
        return SPECIES_TYRANITAR_ROCKET;
    case SPECIES_DRAGONAIR:
        return SPECIES_DRAGONITE_ROCKET;
    case SPECIES_ZWEILOUS:
        return SPECIES_HYDREIGON_ROCKET;
    case SPECIES_GABITE:
        return SPECIES_GARCHOMP_ROCKET;

    case SPECIES_YANMA:
        return SPECIES_YANMEGA_ROCKET;
    case SPECIES_VIGOROTH:
        return SPECIES_SLAKING_ROCKET;
    case SPECIES_KANGASKHAN:
        return SPECIES_KANGASKHAN_ROCKET;
    default:
        return SPECIES_NONE;
    }
}

#endif // GUARD_ROGUE_TEAM_ROCKET_H
