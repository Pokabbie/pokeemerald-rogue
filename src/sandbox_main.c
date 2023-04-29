#include "global.h"
#include "constants/battle.h"
#include "constants/trainers.h"

#include "battle.h"
#include "battle_setup.h"
#include "data.h"

#include "sandbox_main.h"

static bool8 IsKeyTrainerBattle()
{
    u8 trainerClass = gTrainers[gTrainerBattleOpponent_A].trainerClass;

    switch (trainerClass)
    {
    case TRAINER_CLASS_AQUA_ADMIN:
    case TRAINER_CLASS_AQUA_LEADER:
    case TRAINER_CLASS_ELITE_FOUR:
    case TRAINER_CLASS_LEADER:
    case TRAINER_CLASS_CHAMPION:
    case TRAINER_CLASS_MAGMA_ADMIN:
    case TRAINER_CLASS_MAGMA_LEADER:
    case TRAINER_CLASS_RIVAL:

    case TRAINER_CLASS_SALON_MAIDEN:
    case TRAINER_CLASS_DOME_ACE:
    case TRAINER_CLASS_PALACE_MAVEN:
    case TRAINER_CLASS_ARENA_TYCOON:
    case TRAINER_CLASS_FACTORY_HEAD:
    case TRAINER_CLASS_PIKE_QUEEN:
    case TRAINER_CLASS_PYRAMID_KING:
        return TRUE;
    }

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
        if((gBattleTypeFlags & BATTLE_TYPE_TRAINER) != 0 && IsKeyTrainerBattle())
        {
            u8 trainerClass = gTrainers[gTrainerBattleOpponent_A].trainerClass;

            // Champ fight at engine default
            if(trainerClass == TRAINER_CLASS_CHAMPION)
                return waitTime;

            // Still run faster and default game because it's way too slow :(
            return waitTime / 2;
        }
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