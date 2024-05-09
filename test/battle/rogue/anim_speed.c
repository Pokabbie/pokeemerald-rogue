#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("(ROGUE) Background fade doesn't stall: Acrobatics")
{
    u8 opt;

    for(opt = 0; opt < OPTIONS_BATTLE_SCENE_COUNT; ++opt)
    {
        PARAMETRIZE { gSaveBlock2Ptr->optionsTrainerBattleScene = i; }
    }
    GIVEN {
        PLAYER(SPECIES_CHANSEY);
        OPPONENT(SPECIES_CHANSEY);
    } WHEN {
        TURN { MOVE(player, MOVE_ACROBATICS); MOVE(opponent, MOVE_ACROBATICS); }
    } SCENE {
        MESSAGE("Chansey used Acrobatics!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, player);
        MESSAGE("Foe Chansey used Acrobatics!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, opponent);

        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
        //MESSAGE("Foe Chansey's Sp.Def fell!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponent);
        //MESSAGE("Chansey's Sp.Def fell!");
        //MESSAGE("Foe Wobbuffet's accuracy fell!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    }
}

SINGLE_BATTLE_TEST("(ROGUE) Background fade doesn't stall: Psychic")
{
    u8 opt;

    for(opt = 0; opt < OPTIONS_BATTLE_SCENE_COUNT; ++opt)
    {
        PARAMETRIZE { gSaveBlock2Ptr->optionsTrainerBattleScene = i; }
    }
    GIVEN {
        PLAYER(SPECIES_CHANSEY);
        OPPONENT(SPECIES_CHANSEY);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        MESSAGE("Chansey used Psychic!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
        MESSAGE("Foe Chansey used Psychic!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponent);

        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
        //MESSAGE("Foe Chansey's Sp.Def fell!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponent);
        //MESSAGE("Chansey's Sp.Def fell!");
        //MESSAGE("Foe Wobbuffet's accuracy fell!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    }
}

//{
//    bool32 tera;
//    PARAMETRIZE { tera = FALSE; }
//    PARAMETRIZE { tera = TRUE; }
//    GIVEN {
//        PLAYER(SPECIES_BULBASAUR) { TeraType(TYPE_NORMAL); }
//        OPPONENT(SPECIES_WOBBUFFET);
//    } WHEN {
//        TURN { MOVE(player, MOVE_VINE_WHIP, tera: tera); }
//        TURN { MOVE(player, MOVE_SLUDGE_BOMB); }
//    } SCENE {
//        MESSAGE("Bulbasaur used Vine Whip!");
//    } FINALLY {
//        EXPECT_EQ(results[0].damage1, results[1].damage1);
//        EXPECT_EQ(results[0].damage2, results[1].damage2);
//    }
//}