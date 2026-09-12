#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dirty Tactics lowers Defense and Sp. Def but not Accuracy")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU_ROCKET) {
            Ability(ABILITY_DIRTY_TACTICS);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DIRTY_TACTICS);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Dirty Tactics lowers both opponents but not allies")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU_ROCKET) {
            Ability(ABILITY_DIRTY_TACTICS);
        }
        PLAYER(SPECIES_WOBBUFFET);

        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_DIRTY_TACTICS);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_ACC], DEFAULT_STAT_STAGE);

        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_ACC], DEFAULT_STAT_STAGE);

        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}


SINGLE_BATTLE_TEST("Dirty Tactics activates after switching in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_RAICHU_ROCKET) {
            Ability(ABILITY_DIRTY_TACTICS);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            SWITCH(player, 1);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DIRTY_TACTICS);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Dirty Tactics does not lower stats through Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_RAICHU_ROCKET) {
            Ability(ABILITY_DIRTY_TACTICS);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            MOVE(opponent, MOVE_SUBSTITUTE);
        }
        TURN {
            SWITCH(player, 1);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ABILITY_POPUP(player, ABILITY_DIRTY_TACTICS);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_SUBSTITUTE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}
