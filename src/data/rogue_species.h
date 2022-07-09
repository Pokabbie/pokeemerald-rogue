static const u16 sEncounters_Test0[] = {
    SPECIES_YANMA, SPECIES_TREECKO
};

static const u16 sEncounters_Test1[] = {
    SPECIES_TORCHIC, SPECIES_MUDKIP
};

const struct SpeciesTable gRogueSpeciesTable[] = 
{
    {
        .wildSpeciesCount = ARRAY_COUNT(sEncounters_Test0),
        .wildSpecies = sEncounters_Test0,
        .trainerSpeciesCount = ARRAY_COUNT(sEncounters_Test1),
        .trainerSpecies = sEncounters_Test1
    },
};