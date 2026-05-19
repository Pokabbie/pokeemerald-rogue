#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("(ROGUE) Make It Rain doesn't lock up the game")
{
    GIVEN {
        PLAYER(SPECIES_UMBREON);
        OPPONENT(SPECIES_GHOLDENGO);
    } WHEN {
        TURN { MOVE(player, MOVE_CHARM); MOVE(opponent, MOVE_MAKE_IT_RAIN); }
        TURN { MOVE(player, MOVE_CHARM); MOVE(opponent, MOVE_MAKE_IT_RAIN); }
    } SCENE {
        MESSAGE("Umbreon used Charm!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARM, player);
        MESSAGE("Foe Gholdengo used Make It Rain!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARM, player);
        MESSAGE("Umbreon used Charm!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARM, player);
        MESSAGE("Foe Gholdengo used Make It Rain!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARM, player);
    }
}