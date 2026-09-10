#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("No Rules boosts punching moves by 30 percent", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_NO_RULES; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_ICE_PUNCH].punchingMove);
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ability);
            Attack(300);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Defense(100);
            MaxHP(1000);
            HP(1000);
        }
    } WHEN {
        TURN {
            MOVE(player, MOVE_ICE_PUNCH,
                 WITH_RNG(RNG_DAMAGE_MODIFIER, 15));
        }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(
            results[0].damage,
            Q_4_12(1.3),
            results[1].damage
        );
    }
}

SINGLE_BATTLE_TEST("No Rules punching moves ignore positive Defense stages", s16 damage)
{
    u16 setupMove;

    PARAMETRIZE { setupMove = MOVE_SWORDS_DANCE; }
    PARAMETRIZE { setupMove = MOVE_IRON_DEFENSE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_DRAIN_PUNCH].punchingMove);
        PLAYER(SPECIES_MACHAMP) { Ability(ABILITY_NO_RULES); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_DRAIN_PUNCH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("No Rules does not ignore Defense boosts for non-punching moves", s16 damage)
{
    u16 setupMove;

    PARAMETRIZE { setupMove = MOVE_SWORDS_DANCE; }
    PARAMETRIZE { setupMove = MOVE_IRON_DEFENSE; }

    GIVEN {
        ASSUME(!gBattleMoves[MOVE_KNOCK_OFF].punchingMove);
        PLAYER(SPECIES_MACHAMP) { Ability(ABILITY_NO_RULES); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_KNOCK_OFF); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}
