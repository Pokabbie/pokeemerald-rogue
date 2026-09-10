#include "global.h"
#include "pokemon.h"
#include "test/battle.h"
#include "rogue_team_rocket.h"

SINGLE_BATTLE_TEST("TR Arbok has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].baseHP, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].baseAttack, 105);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].baseDefense, 69);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].baseSpeed, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].baseSpAttack, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].baseSpDefense, 79);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].types[0], TYPE_POISON);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].abilities[0], ABILITY_NEUROTOXIN);
        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].abilities[2], ABILITY_NEUROTOXIN);

        EXPECT_EQ(gSpeciesInfo[SPECIES_ARBOK_ROCKET].natDexNum, NATIONAL_DEX_ARBOK);

        EXPECT_EQ(player->type1, TYPE_POISON);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_NEUROTOXIN);
    }
}

SINGLE_BATTLE_TEST("TR Arbok maps to Arbok in Team Rocket framework")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_ARBOK_ROCKET));
        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ARBOK_ROCKET),
            SPECIES_ARBOK
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_ARBOK));
        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_ARBOK),
            SPECIES_ARBOK
        );
    }
}
