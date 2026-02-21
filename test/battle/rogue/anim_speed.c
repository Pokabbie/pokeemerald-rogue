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
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, player);
        MESSAGE("Foe Chansey used Acrobatics!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, opponent);
    }
    FINALLY
    {
        gSaveBlock2Ptr->optionsTrainerBattleScene = OPTIONS_BATTLE_SCENE_1X;
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
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
        MESSAGE("Foe Chansey used Psychic!");
        //ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponent);
    }
    FINALLY
    {
        gSaveBlock2Ptr->optionsTrainerBattleScene = OPTIONS_BATTLE_SCENE_1X;
    }
}
