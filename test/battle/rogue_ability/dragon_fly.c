#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dragon Fly increases Bug-type move damage", s16 damage)
{
    u32 move;
    u16 ability;

    PARAMETRIZE { move = MOVE_TACKLE; ability = ABILITY_LEVITATE; }
    PARAMETRIZE { move = MOVE_TACKLE; ability = ABILITY_DRAGON_FLY; }
    PARAMETRIZE { move = MOVE_X_SCISSOR; ability = ABILITY_LEVITATE; }
    PARAMETRIZE { move = MOVE_X_SCISSOR; ability = ABILITY_DRAGON_FLY; }
    PARAMETRIZE { move = MOVE_BUG_BUZZ; ability = ABILITY_LEVITATE; }
    PARAMETRIZE { move = MOVE_BUG_BUZZ; ability = ABILITY_DRAGON_FLY; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_BUG);
        ASSUME(gBattleMoves[MOVE_X_SCISSOR].type == TYPE_BUG);
        ASSUME(gBattleMoves[MOVE_BUG_BUZZ].type == TYPE_BUG);
        ASSUME(gBattleMoves[MOVE_X_SCISSOR].split == SPLIT_PHYSICAL);
        ASSUME(gBattleMoves[MOVE_BUG_BUZZ].split == SPLIT_SPECIAL);
        PLAYER(SPECIES_FLYGON) { Ability(ability); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage); // Tackle should be unaffected
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage); // X-Scissor should be affected
        EXPECT_MUL_EQ(results[4].damage, Q_4_12(1.5), results[5].damage); // Bug Buzz should be affected
    }
}

SINGLE_BATTLE_TEST("Dragon Fly increases Dragon-type move damage", s16 damage)
{
    u32 move;
    u16 ability;

    PARAMETRIZE { move = MOVE_TACKLE; ability = ABILITY_LEVITATE; }
    PARAMETRIZE { move = MOVE_TACKLE; ability = ABILITY_DRAGON_FLY; }
    PARAMETRIZE { move = MOVE_DRAGON_CLAW; ability = ABILITY_LEVITATE; }
    PARAMETRIZE { move = MOVE_DRAGON_CLAW; ability = ABILITY_DRAGON_FLY; }
    PARAMETRIZE { move = MOVE_DRAGON_PULSE; ability = ABILITY_LEVITATE; }
    PARAMETRIZE { move = MOVE_DRAGON_PULSE; ability = ABILITY_DRAGON_FLY; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_DRAGON);
        ASSUME(gBattleMoves[MOVE_DRAGON_CLAW].type == TYPE_DRAGON);
        ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].type == TYPE_DRAGON);
        ASSUME(gBattleMoves[MOVE_DRAGON_CLAW].split == SPLIT_PHYSICAL);
        ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].split == SPLIT_SPECIAL);
        PLAYER(SPECIES_SPINDA) { Ability(ability); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage); // Tackle should be unaffected
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage); // Dragon Claw should be affected
        EXPECT_MUL_EQ(results[4].damage, Q_4_12(1.5), results[5].damage); // Dragon Pulse should be affected
    }
}