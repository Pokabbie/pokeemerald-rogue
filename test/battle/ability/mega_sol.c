#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mega Sol multiplies the power of Fire-type moves by 1.5x", s16 damage)
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);

    u16 ability;
    PARAMETRIZE { ability = ABILITY_OVERGROW;}
    PARAMETRIZE { ability = ABILITY_MEGA_SOL;}
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Ability(ability);}
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mega Sol halves the power of the user's Water-type moves", s16 damage)
{

    u16 ability;
    PARAMETRIZE { ability = ABILITY_OVERGROW;}
    PARAMETRIZE { ability = ABILITY_MEGA_SOL;}

    GIVEN {
        ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
        PLAYER(SPECIES_MEGANIUM) { Ability(ability);}
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mega Sol Weather Ball doubles its power and turns to a Fire-type move if user has Mega Sol", s16 damage)
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_OVERGROW;}
    PARAMETRIZE { ability = ABILITY_MEGA_SOL;}

    GIVEN {
        ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
        PLAYER(SPECIES_MEGANIUM) { Ability(ability);}
        OPPONENT(SPECIES_PINSIR){HP(9999); MaxHP(9999);}
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(6.0), results[1].damage); // double base power + type effectiveness + sun 50% boost
    }
}

SINGLE_BATTLE_TEST("Mega Sol Synthesis recovers 2/3 of the user's max HP if user has Mega Sol (Gen3+)")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SYNTHESIS].effect == EFFECT_SYNTHESIS);
        PLAYER(SPECIES_MEGANIUM) { HP(1); MaxHP(300); Ability(ABILITY_MEGA_SOL);  }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SYNTHESIS); }
    } SCENE {
        HP_BAR(player, damage: -(300 / 1.5));
    }
}

SINGLE_BATTLE_TEST("Mega Sol ignores Sandstorm's solarbeam power reduction, and its rock defense boost", s16 damage)
{
    u16 ability;
    KNOWN_FAILING; 
    PARAMETRIZE { ability = ABILITY_OVERGROW;}
    PARAMETRIZE { ability = ABILITY_MEGA_SOL;}

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SOLARBEAM].effect == EFFECT_SOLAR_BEAM);
        ASSUME(gBattleMoves[MOVE_SOLARBEAM].type == TYPE_GRASS);
        PLAYER(SPECIES_MEGANIUM) { Ability(ability);}
        OPPONENT(SPECIES_BASTIODON) { Ability(ABILITY_SAND_STREAM);}
    } WHEN {
        TURN {}
        TURN { MOVE(player, MOVE_SOLAR_BEAM); }
        if (ability == ABILITY_OVERGROW) {
            TURN { SKIP_TURN(player); }
        }
    } SCENE {
        HP_BAR(player); // checking sandstorm occurred
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOLAR_BEAM, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mega Sol doesn't trigger the foe's Leaf Guard", s16 damage)
{
    u16 move;
    PARAMETRIZE { move = MOVE_CELEBRATE;}
    PARAMETRIZE { move = MOVE_SUNNY_DAY;}

    GIVEN {
        ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
        PLAYER(SPECIES_MEGANIUM) { Ability(ABILITY_MEGA_SOL);}
        OPPONENT(SPECIES_LEAFEON) { Ability(ABILITY_LEAF_GUARD);}
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, MOVE_WILL_O_WISP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        if (move == MOVE_CELEBRATE) {
	    ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
            STATUS_ICON(opponent, STATUS1_BURN);
        }
        else {
	    NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
            ABILITY_POPUP(opponent, ABILITY_LEAF_GUARD);
            MESSAGE("It doesn't affect Foe Leafeon…");
            NOT STATUS_ICON(opponent, STATUS1_BURN);
        }
    }
}


SINGLE_BATTLE_TEST("Mega Sol ignores Cloud Nine", s16 damage)
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);

    u16 ability;
    PARAMETRIZE { ability = ABILITY_OVERGROW;}
    PARAMETRIZE { ability = ABILITY_MEGA_SOL;}
    GIVEN {
        PLAYER(SPECIES_MEGANIUM) { Ability(ability);}
        OPPONENT(SPECIES_WOBBUFFET)  { Ability(ABILITY_CLOUD_NINE); HP(9999); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mega Sol Solar Beam does not need a charging turn if user has Mega Sol")
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_MEGA_SOL; }
    PARAMETRIZE { ability = ABILITY_OVERGROW; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SOLARBEAM].effect == EFFECT_SOLAR_BEAM);
        ASSUME(gBattleMoves[MOVE_SOLARBEAM].type == TYPE_GRASS);
        PLAYER(SPECIES_MEGANIUM) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SOLAR_BEAM); }
        if (ability == ABILITY_OVERGROW) {
            TURN { SKIP_TURN(player); }
        }
    } SCENE {
        if (ability == ABILITY_OVERGROW) {
            MESSAGE("Meganium used Solar Beam!");
	    NOT HP_BAR(opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
	else  {
            MESSAGE("Meganium used Solar Beam!");
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SOLAR_BEAM, player);
            HP_BAR(opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Mega Sol Growth increases Attack and Sp. Atk by 2 stages under Mega Sol (Gen 5+)")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_GROWTH].effect == EFFECT_GROWTH);
        PLAYER(SPECIES_MEGANIUM) { Ability(ABILITY_MEGA_SOL); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GROWTH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWTH, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Mega Sol ignores Sand Veil")
{
    PASSES_RANDOMLY(5, 5, RNG_ACCURACY);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POUND].accuracy == 100);
        PLAYER(SPECIES_SANDSHREW) { Ability(ABILITY_SAND_VEIL); }
        OPPONENT(SPECIES_MEGANIUM) { Ability(ABILITY_MEGA_SOL); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SANDSTORM); }
        TURN { MOVE(opponent, MOVE_POUND); }
    } SCENE {
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Mega Sol ignores Snow Cloak")
{
    PASSES_RANDOMLY(5, 5, RNG_ACCURACY);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POUND].accuracy == 100);
        PLAYER(SPECIES_GLACEON) { Ability(ABILITY_SNOW_CLOAK); }
        OPPONENT(SPECIES_MEGANIUM) { Ability(ABILITY_MEGA_SOL); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAIL); }
        TURN {  MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player);
    }
}
