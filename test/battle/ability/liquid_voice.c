#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].power > 0);
}

SINGLE_BATTLE_TEST("Liquid voice turns a sound move into a Water-type move")
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_LIQUID_VOICE; }

    GIVEN {
        PLAYER(SPECIES_TYPHLOSION);
        OPPONENT(SPECIES_PRIMARINA) { Ability(ability); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HYPER_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, opponent);
        if(ability == ABILITY_LIQUID_VOICE)
            MESSAGE("It's super effective!");
        else 
            NOT MESSAGE("It's super effective!");
    }
}
