#include "global.h"
#include "pokemon.h"
#include "test/battle.h"
#include "rogue_team_rocket.h"

SINGLE_BATTLE_TEST("TR Machamp has intended species data")
{
    GIVEN {
        PLAYER(SPECIES_MACHAMP_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].baseHP, 90);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].baseAttack, 145);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].baseDefense, 85);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].baseSpeed, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].baseSpAttack, 65);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].baseSpDefense, 95);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].types[0], TYPE_FIGHTING);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].types[1], TYPE_DARK);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].abilities[0], ABILITY_NO_RULES);
        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].abilities[2], ABILITY_NO_RULES);

        EXPECT_EQ(gSpeciesInfo[SPECIES_MACHAMP_ROCKET].natDexNum, NATIONAL_DEX_MACHAMP);

        EXPECT_EQ(player->type1, TYPE_FIGHTING);
        EXPECT_EQ(player->type2, TYPE_DARK);
        EXPECT_EQ(player->ability, ABILITY_NO_RULES);
    }
}

SINGLE_BATTLE_TEST("TR Machamp maps to Machamp in Team Rocket framework")
{
    GIVEN {
        PLAYER(SPECIES_MACHAMP_ROCKET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT(RogueTeamRocket_IsVariant(SPECIES_MACHAMP_ROCKET));
        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MACHAMP_ROCKET),
            SPECIES_MACHAMP
        );

        EXPECT(!RogueTeamRocket_IsVariant(SPECIES_MACHAMP));
        EXPECT_EQ(
            RogueTeamRocket_GetBaseSpecies(SPECIES_MACHAMP),
            SPECIES_MACHAMP
        );
    }
}
