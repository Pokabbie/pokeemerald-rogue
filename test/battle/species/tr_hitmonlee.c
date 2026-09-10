#include "global.h"
#include "pokemon.h"
#include "test/battle.h"
#include "rogue_team_rocket.h"

SINGLE_BATTLE_TEST("TR Hitmonlee has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_HITMONLEE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].baseHP, 50);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].baseAttack, 135);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].baseDefense, 60);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].baseSpeed, 120);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].baseSpAttack, 40);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].baseSpDefense, 115);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].types[0], TYPE_ELECTRIC);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].types[1], TYPE_FIGHTING);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].abilities[0], ABILITY_STATIC_FIELD);
        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].abilities[2], ABILITY_STATIC_FIELD);

        EXPECT_EQ(gSpeciesInfo[SPECIES_HITMONLEE_ROCKET].natDexNum, NATIONAL_DEX_HITMONLEE);

        EXPECT_EQ(player->type1, TYPE_ELECTRIC);
        EXPECT_EQ(player->type2, TYPE_FIGHTING);
        EXPECT_EQ(player->ability, ABILITY_STATIC_FIELD);
    }
}

SINGLE_BATTLE_TEST("TR Hitmonlee maps to Hitmonlee in Team Rocket framework")
{
    GIVEN {
        PLAYER(SPECIES_HITMONLEE_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_HITMONLEE_ROCKET));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HITMONLEE_ROCKET),
            SPECIES_HITMONLEE
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_HITMONLEE));

        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_HITMONLEE),
            SPECIES_HITMONLEE
        );
    }
}
