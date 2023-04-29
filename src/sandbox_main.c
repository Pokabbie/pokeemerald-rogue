#include "global.h"
#include "constants/battle.h"

#include "battle.h"
#include "battle_setup.h"

#include "sandbox_main.h"

static bool8 IsKeyTrainerBattle()
{
    //gTrainerBattleOpponent_A
    return FALSE;
}


bool8 Sandbox_UseFastBattleAnims()
{
    // Force slow anims for significant battles
    if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && IsKeyTrainerBattle())
        return FALSE;

    // Force slow anims for legendaries
    if((gBattleTypeFlags & BATTLE_TYPE_LEGENDARY) != 0)
        return FALSE;

    return TRUE;
}

u16 Sandbox_ModifyBattleWaitTime(u16 waitTime, bool8 awaitingMessage)
{
    if(Sandbox_UseFastBattleAnims())
    {
        return awaitingMessage ? 8 : 0;
    }
    else
    {
        // TODO - Go default speed for champ fight

        if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && IsKeyTrainerBattle())
            // Still run faster and default game because it's way too slow :(
            return waitTime / 2;
        else
            // Go faster, but not quite gym leader slow
            return waitTime / 4;
    }
}

s16 Sandbox_ModifyBattleSlideAnim(s16 speed)
{
    if(Sandbox_UseFastBattleAnims())
    {
        if(speed < 0)
            return speed * 2 - 1;
        else
            return speed * 2 + 1;
    }

    return speed;
}