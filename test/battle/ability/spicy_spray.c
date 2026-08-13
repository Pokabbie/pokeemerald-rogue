#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Spicy Spray burns the attacker")
{
    u16 move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SCRATCH].makesContact);
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SCOVILLAIN) { Ability(ABILITY_SPICY_SPRAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, move); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SPICY_SPRAY);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_BRN, player);
        STATUS_ICON(player, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Spicy Spray burns the attacker even if the attacker is behind a Substitute")
{
    u16 move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SCRATCH].makesContact);
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SCOVILLAIN) { Ability(ABILITY_SPICY_SPRAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SUBSTITUTE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, move); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SPICY_SPRAY);
        STATUS_ICON(player, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Spicy Spray does not burn the attacker if the defender is behind a Substitute")
{
    u16 move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SCRATCH].makesContact);
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SCOVILLAIN) { Ability(ABILITY_SPICY_SPRAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, move); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_SPICY_SPRAY);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_BRN, player);
            STATUS_ICON(player, burn: TRUE);
        }
    }
}

SINGLE_BATTLE_TEST("Spicy Spray burns the attacker even if the attacker has Sheer Force")
{
    u16 move;
    PARAMETRIZE { move = MOVE_CRUNCH; }
    PARAMETRIZE { move = MOVE_ICE_BEAM; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_CRUNCH].makesContact);
        ASSUME(!gBattleMoves[MOVE_ICE_BEAM].makesContact);
        PLAYER(SPECIES_FERALIGATR) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_SCOVILLAIN) { Ability(ABILITY_SPICY_SPRAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, move); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SPICY_SPRAY);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_BRN, player);
        STATUS_ICON(player, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Spicy Spray burns the attacker even if the defender faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SCOVILLAIN) { Ability(ABILITY_SPICY_SPRAY); HP(1); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SPICY_SPRAY);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_BRN, player);
        STATUS_ICON(player, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Spicy Spray burns the attacker even if the defender behind a Substitute takes damage")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SCOVILLAIN) { Ability(ABILITY_SPICY_SPRAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SPICY_SPRAY);
        STATUS_ICON(player, burn: TRUE);
    }
}
