#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Trapdoor lowers a grounded opponent's Speed on entry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_TRAPDOOR);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRAPDOOR);
    } THEN {
        EXPECT_EQ(
            opponent->statStages[STAT_SPEED],
            DEFAULT_STAT_STAGE - 1
        );
    }
}

SINGLE_BATTLE_TEST("Trapdoor does not lower an airborne opponent's Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_TRAPDOOR);
        }
        OPPONENT(SPECIES_PIDGEOT);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TRAPDOOR);
    } THEN {
        EXPECT_EQ(
            opponent->statStages[STAT_SPEED],
            DEFAULT_STAT_STAGE
        );
    }
}

DOUBLE_BATTLE_TEST("Trapdoor lowers both grounded opponents but not its ally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_TRAPDOOR);
        }
        PLAYER(SPECIES_WYNAUT);

        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_TRAPDOOR);
    } THEN {
        EXPECT_EQ(
            opponentLeft->statStages[STAT_SPEED],
            DEFAULT_STAT_STAGE - 1
        );

        EXPECT_EQ(
            opponentRight->statStages[STAT_SPEED],
            DEFAULT_STAT_STAGE - 1
        );

        EXPECT_EQ(
            playerRight->statStages[STAT_SPEED],
            DEFAULT_STAT_STAGE
        );
    }
}



SINGLE_BATTLE_TEST("Trapdoor activates when TR Flygon switches in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_FLYGON_ROCKET);

        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            SWITCH(player, 1);
        }
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_TRAPDOOR);

        EXPECT_EQ(
            opponent->statStages[STAT_SPEED],
            DEFAULT_STAT_STAGE - 1
        );
    }
}
