#include "global.h"
#include "pokemon.h"
#include "test/battle.h"
#include "rogue_team_rocket.h"

SINGLE_BATTLE_TEST("TR Flygon has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_FLYGON_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_FLYGON_ROCKET].baseHP, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FLYGON_ROCKET].baseAttack, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FLYGON_ROCKET].baseDefense, 80);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FLYGON_ROCKET].baseSpeed, 110);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FLYGON_ROCKET].baseSpAttack, 70);
        EXPECT_EQ(gSpeciesInfo[SPECIES_FLYGON_ROCKET].baseSpDefense, 80);

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_FLYGON_ROCKET].types[0],
            TYPE_BUG
        );

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_FLYGON_ROCKET].types[1],
            TYPE_DRAGON
        );

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_FLYGON_ROCKET].abilities[0],
            ABILITY_TRAPDOOR
        );

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_FLYGON_ROCKET].abilities[2],
            ABILITY_TRAPDOOR
        );

        EXPECT_EQ(
            gSpeciesInfo[SPECIES_FLYGON_ROCKET].natDexNum,
            NATIONAL_DEX_FLYGON
        );

        EXPECT_EQ(player->type1, TYPE_BUG);
        EXPECT_EQ(player->type2, TYPE_DRAGON);
        EXPECT_EQ(player->ability, ABILITY_TRAPDOOR);
    }
}

SINGLE_BATTLE_TEST("TR Flygon maps to Flygon in Team Rocket framework")
{
    GIVEN {
        PLAYER(SPECIES_FLYGON_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_FLYGON_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(
                SPECIES_FLYGON_ROCKET
            ),
            SPECIES_FLYGON
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_FLYGON));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_FLYGON),
            SPECIES_FLYGON
        );
    }
}
