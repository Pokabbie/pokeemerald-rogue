#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Neurotoxin lowers Speed when the user poisons a foe")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) {
            Ability(ABILITY_NEUROTOXIN);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            MOVE(player, MOVE_TOXIC);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NEUROTOXIN);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_TOXIC_POISON);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Neurotoxin does not activate when poisoning fails")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) {
            Ability(ABILITY_NEUROTOXIN);
        }
        OPPONENT(SPECIES_MAGNEMITE);
    } WHEN {
        TURN {
            MOVE(player, MOVE_TOXIC);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, 0);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Neurotoxin only lowers Speed when poison is newly inflicted")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) {
            Ability(ABILITY_NEUROTOXIN);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            MOVE(player, MOVE_TOXIC);
        }
        TURN {
            MOVE(player, MOVE_TOXIC);
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_TOXIC_POISON);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Neurotoxin respects abilities that prevent Speed loss")
{
    GIVEN {
        PLAYER(SPECIES_ARBOK) {
            Ability(ABILITY_NEUROTOXIN);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Ability(ABILITY_CLEAR_BODY);
        }
    } WHEN {
        TURN {
            MOVE(player, MOVE_TOXIC);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NEUROTOXIN);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_TOXIC_POISON);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
