#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BATON_PASS].effect == EFFECT_BATON_PASS);
}

// This softlocked the game before.
SINGLE_BATTLE_TEST("Baton Pass used after Memento works correctly")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
        OPPONENT(SPECIES_CATERPIE);
    } WHEN {
        TURN { MOVE(player, MOVE_MEMENTO); SEND_OUT(player, 1); MOVE(opponent, MOVE_BATON_PASS); SEND_OUT(opponent, 1); }
    } SCENE {
        MESSAGE("Wobbuffet used Memento!");
        MESSAGE("Wobbuffet fainted!");
        MESSAGE("Foe Wynaut used Baton Pass!");
        MESSAGE("2 sent out Caterpie!");
        MESSAGE("Go! Wobbuffet!");
    }
}