#include "global.h"
#include "constants/battle.h"
#include "constants/battle_ai.h"
#include "constants/trainers.h"

#include "battle.h"
#include "battle_setup.h"
#include "data.h"
#include "graphics.h"

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
    case TRAINER_CLASS_KANTO_CHAMPION:
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
            if(trainerClass == TRAINER_CLASS_CHAMPION || trainerClass == TRAINER_CLASS_KANTO_CHAMPION)
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

#define PLAYER_STYLE(prefix, x, y) if(style1 == x && style2 == y) return prefix ## _ ## x ## _ ## y

const void* Sandbox_ModifyLoadPalette(const void *src)
{
    const u8 gender = gSaveBlock2Ptr->playerGender;
    const u8 style1 = gSaveBlock2Ptr->playerStyle[0];
    const u8 style2 = gSaveBlock2Ptr->playerStyle[1];

    // ObjectEvent palette
    if(gender == 0 && src == gObjectEventPal_Brendan)
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gObjectEventPal_Brendan, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }
    if(gender == 1 && src == gObjectEventPal_May)
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gObjectEventPal_May, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }

    // Trainer
    // front/back pics
    if(gender == 0 && (src == gTrainerPalette_Brendan || src == gTrainerBackPic_Brendan))
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gTrainerPalette_Brendan, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }
    if(gender == 1 && (src == gTrainerPalette_May || src == gTrainerBackPic_May))
    {
        #define PALETTE_FUNC(x, y) PLAYER_STYLE(gTrainerPalette_May, x, y);
        FOREACH_VISUAL_PRESETS(PALETTE_FUNC)
        #undef PALETTE_FUNC
    }

    return src;
}

const void* Sandbox_ModifyLoadCompressedPalette(const void *src)
{
    return Sandbox_ModifyLoadPalette(src);
}

u32 Sandbox_GetTrainerAIFlags(u16 trainerNum)
{
    u32 flags = gTrainers[trainerNum].aiFlags;


#ifdef POKEMON_EXPANSION
    // Every trainer is smart
    flags |= AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_SETUP_FIRST_TURN | AI_FLAG_HP_AWARE;

    // EX only flags
    flags |= AI_FLAG_WILL_SUICIDE | AI_FLAG_SMART_SWITCHING;

    if(gTrainers[trainerNum].doubleBattle)
    {
        flags |= AI_FLAG_HELP_PARTNER;
    }
#else
    // Every trainer is smart
    flags |= AI_SCRIPT_CHECK_BAD_MOVE | AI_SCRIPT_TRY_TO_FAINT | AI_SCRIPT_CHECK_VIABILITY | AI_SCRIPT_SETUP_FIRST_TURN | AI_SCRIPT_HP_AWARE | AI_SCRIPT_TRY_SUNNY_DAY_START;
#endif

    return flags;
}