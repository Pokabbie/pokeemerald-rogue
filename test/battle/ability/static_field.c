#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Static Field paralyzes attackers after physical or special damage")
{
    u32 move;

    PARAMETRIZE { move = MOVE_EARTHQUAKE; }
    PARAMETRIZE { move = MOVE_SWIFT; }

    GIVEN {
        ASSUME(IS_MOVE_PHYSICAL(MOVE_EARTHQUAKE));
        ASSUME(IS_MOVE_SPECIAL(MOVE_SWIFT));
        ASSUME(!gBattleMoves[MOVE_EARTHQUAKE].makesContact);
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);

        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_HITMONLEE) {
            Ability(ABILITY_STATIC_FIELD);
        }
    } WHEN {
        TURN {
            MOVE(player, move);
        }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_STATIC_FIELD);
        ANIMATION(
            ANIM_TYPE_STATUS,
            B_ANIM_STATUS_PRZ,
            player
        );
        STATUS_ICON(player, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Static Field does not activate from status moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_HITMONLEE) {
            Ability(ABILITY_STATIC_FIELD);
        }
    } WHEN {
        TURN {
            MOVE(player, MOVE_GROWL);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(
                opponent,
                ABILITY_STATIC_FIELD
            );
            STATUS_ICON(player, paralysis: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Static Field respects paralysis immunity")
{
    GIVEN {
        PLAYER(SPECIES_PIKACHU);
        OPPONENT(SPECIES_HITMONLEE) {
            Ability(ABILITY_STATIC_FIELD);
        }
    } WHEN {
        TURN {
            MOVE(player, MOVE_TACKLE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(
                opponent,
                ABILITY_STATIC_FIELD
            );
            STATUS_ICON(player, paralysis: TRUE);
        }
    }
}
