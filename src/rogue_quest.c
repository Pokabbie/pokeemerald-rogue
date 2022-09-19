#include "global.h"
#include "constants/items.h"

#include "battle.h"
#include "event_data.h"
#include "item.h"
#include "money.h"
#include "pokedex.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_quest.h"

extern const u8 gText_QuestRewardGive[];
extern const u8 gText_QuestRewardGiveMoney[];
extern const u8 gText_QuestLogStatusIncomplete[];

extern EWRAM_DATA struct RogueQuestData gRogueQuestData;

static EWRAM_DATA u8 sRewardQuest = 0;
static EWRAM_DATA u8 sRewardParam = 0;
static EWRAM_DATA u8 sPreviousRouteType = 0;

typedef void (*QuestCallback)(u16 questId, struct RogueQuestState* state);

static void UnlockFollowingQuests(u16 questId);

void ResetQuestState(u16 saveVersion)
{
    u16 i;

    if(saveVersion == 0)
    {
        // Reset the state for any new quests
        for(i = 0; i < QUEST_CAPACITY; ++i)
        {
            memset(&gRogueQuestData.questStates[i], 0, sizeof(struct RogueQuestState));
        }

        // These quests must always be unlocked
        for(i = QUEST_FirstAdventure; i <= QUEST_Champion; ++i)
        {
            TryUnlockQuest(i);
        }
    }
}

bool8 AnyNewQuests(void)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = 0; i < QUEST_CAPACITY; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isUnlocked && state->hasNewMarker)
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool8 AnyQuestRewardsPending(void)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = 0; i < QUEST_CAPACITY; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isUnlocked && state->hasPendingRewards)
        {
            return TRUE;
        }
    }

    return FALSE;
}

u16 GetCompletedQuestCount(void)
{
    u16 i;
    struct RogueQuestState* state;
    u16 count = 0;

    for(i = 0; i < QUEST_CAPACITY; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isUnlocked && state->isCompleted)
            ++count;
    }

    return count;
}

u16 GetUnlockedQuestCount(void)
{
    u16 i;
    struct RogueQuestState* state;
    u16 count = 0;

    for(i = 0; i < QUEST_CAPACITY; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isUnlocked)
            ++count;
    }

    return count;
}

bool8 GetQuestState(u16 questId, struct RogueQuestState* outState)
{
    if(questId < QUEST_CAPACITY)
    {
        memcpy(outState, &gRogueQuestData.questStates[questId], sizeof(struct RogueQuestState));
        return outState->isUnlocked;
    }

    return FALSE;
}

void SetQuestState(u16 questId, struct RogueQuestState* state)
{
    if(questId < QUEST_CAPACITY)
    {
        memcpy(&gRogueQuestData.questStates[questId], state, sizeof(struct RogueQuestState));
    }
}

bool8 IsQuestRepeatable(u16 questId)
{
    return (gRogueQuests[questId].flags & QUEST_FLAGS_REPEATABLE) != 0;
}

bool8 IsQuestCollected(u16 questId)
{
    struct RogueQuestState state;
    if (GetQuestState(questId, &state))
    {
        return state.isCompleted && !state.hasPendingRewards;
    }

    return FALSE;
}

bool8 IsQuestGloballyTracked(u16 questId)
{
    return (gRogueQuests[questId].flags & QUEST_FLAGS_GLOBALALLY_TRACKED) != 0;
}

bool8 IsQuestActive(u16 questId)
{
    struct RogueQuestState state;
    if (GetQuestState(questId, &state))
    {
        return state.isValid;
    }

    return FALSE;
}

bool8 DoesQuestHaveUnlocks(u16 questId)
{
    return gRogueQuests[questId].unlockedQuests[0] != QUEST_NONE;
}


static struct RogueQuestReward const* GetCurrentRewardTarget()
{
    return &gRogueQuests[sRewardQuest].rewards[sRewardParam];
}

static bool8 QueueTargetRewardQuest()
{
    u16 i;
    struct RogueQuestState* state;
    for(i = 0; i < QUEST_CAPACITY; ++i)
    {
        state = &gRogueQuestData.questStates[i];

        if(state->hasPendingRewards)
        {
            if(sRewardQuest != i)
            {
                sRewardQuest = i;
                sRewardParam = 0;
                UnlockFollowingQuests(sRewardQuest);
                // TODO - Check to see if we have enough space for all of these items
            }
            else
            {
                ++sRewardParam;
            }
            return TRUE;
        }
    }

    sRewardQuest = QUEST_NONE;
    sRewardParam = 0;
    return FALSE;
}

static bool8 QueueNextReward()
{
    while(QueueTargetRewardQuest())
    {
        struct RogueQuestReward const* reward = GetCurrentRewardTarget();

        if(sRewardParam >= QUEST_MAX_REWARD_COUNT || reward->type == QUEST_REWARD_NONE)
        {
            // We've cleared out this quest's rewards
            gRogueQuestData.questStates[sRewardQuest].hasPendingRewards = FALSE;

            sRewardQuest = QUEST_NONE;
            sRewardParam = 0;
        }
        else
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 GiveAndGetNextAnnouncedReward()
{
    while(QueueNextReward())
    {
        struct RogueQuestReward const* reward = GetCurrentRewardTarget();
        bool8 shouldAnnounce = reward->giveText != NULL;

        // Actually give the reward here
        switch(reward->type)
        {
            case QUEST_REWARD_SET_FLAG:
                FlagSet(reward->params[0]);
                break;

            case QUEST_REWARD_CLEAR_FLAG:
                FlagClear(reward->params[0]);
                break;

            case QUEST_REWARD_GIVE_ITEM:
                if(!AddBagItem(reward->params[0], reward->params[1]))
                {
                    AddPCItem(reward->params[0], reward->params[1]);
                }
                shouldAnnounce = TRUE;
                break;

            case QUEST_REWARD_GIVE_MONEY:
                AddMoney(&gSaveBlock1Ptr->money, reward->params[0]);
                shouldAnnounce = TRUE;
                break;

            //case QUEST_REWARD_CUSTOM_TEXT:
            //    break;
        }
        
        if(shouldAnnounce)
        {
            return TRUE;
        }
    }

    return FALSE;
}


bool8 GiveNextRewardAndFormat(u8* str)
{
    if(GiveAndGetNextAnnouncedReward())
    {
        struct RogueQuestReward const* reward = GetCurrentRewardTarget();

        if(reward->giveText)
        {
            StringCopy(str, reward->giveText);
        }
        else
        {
            switch(reward->type)
            {
                case QUEST_REWARD_GIVE_ITEM:
                    CopyItemNameHandlePlural(reward->params[0], gStringVar1, reward->params[1]);
                    StringExpandPlaceholders(str, gText_QuestRewardGive);
                    break;

                case QUEST_REWARD_GIVE_MONEY:
                    ConvertUIntToDecimalStringN(gStringVar1, reward->params[0], STR_CONV_MODE_LEFT_ALIGN, 7);
                    StringExpandPlaceholders(str, gText_QuestRewardGiveMoney);
                    break;
                
                default:
                    // Just return an obviously broken message
                    StringCopy(str, gText_QuestLogStatusIncomplete);
                    break;
            }
        }
        return TRUE;
    }

    return FALSE;
}

bool8 TryUnlockQuest(u16 questId)
{
    struct RogueQuestState* state = &gRogueQuestData.questStates[questId];

    if(!state->isUnlocked)
    {
        state->isUnlocked = TRUE;
        state->isValid = FALSE;
        state->isCompleted = FALSE;
        state->hasNewMarker = TRUE;
        return TRUE;
    }

    return FALSE;
}

bool8 TryMarkQuestAsComplete(u16 questId)
{
    struct RogueQuestState* state = &gRogueQuestData.questStates[questId];

    if(state->isValid)
    {
        state->isValid = FALSE;

        if(!state->isCompleted)
        {
            // First time finishing
            state->isCompleted = TRUE;
            state->hasPendingRewards = TRUE;
        }
        else if(IsQuestRepeatable(questId))
        {
            // Has already completed this once before so has rewards pending
            state->hasPendingRewards = TRUE;
        }

        return TRUE;
    }

    return FALSE;
}

bool8 TryDeactivateQuest(u16 questId)
{
    struct RogueQuestState* state = &gRogueQuestData.questStates[questId];

    if(state->isValid)
    {
        state->isValid = FALSE;
        return TRUE;
    }

    return FALSE;
}

static void UnlockFollowingQuests(u16 questId)
{
    u8 i;

    for(i = 0; i < QUEST_MAX_FOLLOWING_QUESTS; ++i)
    {
        if(gRogueQuests[questId].unlockedQuests[i] == QUEST_NONE)
            break;

        TryUnlockQuest(gRogueQuests[questId].unlockedQuests[i]);
    }
}

static void ForEachUnlockedQuest(QuestCallback callback)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = 0; i < QUEST_CAPACITY; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isUnlocked)
        {
            callback(i, state);
        }
    }
}

static void ForEachActiveQuest(QuestCallback callback)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = 0; i < QUEST_CAPACITY; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isValid && !state->isCompleted)
        {
            callback(i, state);
        }
    }
}

static void ActivateQuestIfNeeded(u16 questId, struct RogueQuestState* state)
{
    if(state->isUnlocked)
    {
        if(IsQuestRepeatable(questId))
        {
            // Don't reactivate quests if we have rewards pending
            if(!state->hasPendingRewards)
                state->isValid = TRUE;
        }
        else if(!state->isCompleted)
        {
            state->isValid = TRUE;
        }
    }
}

static void DeactivateQuestIfNeeded(u16 questId, struct RogueQuestState* state)
{
    if(state->isValid && !IsQuestGloballyTracked(questId))
    {
        state->isValid = FALSE;
    }
}

// External callbacks
//

static void UpdateChaosChampion(bool8 enteringPotentialEncounter)
{
    struct RogueQuestState state;

    if(IsQuestActive(QUEST_ChaosChampion) && GetQuestState(QUEST_ChaosChampion, &state))
    {
        bool8 isRandomanDisabled = FlagGet(FLAG_ROGUE_RANDOM_TRADE_DISABLED);

        if(enteringPotentialEncounter)
        {
            state.data.half = isRandomanDisabled ? 0 : 1;
            SetQuestState(QUEST_ChaosChampion, &state);
        }
        else if(state.data.half)
        {
            if(!isRandomanDisabled)
            {
                // Randoman was still active i.e. we didn't use him
                TryDeactivateQuest(QUEST_ChaosChampion);
            }
            else
            {
                state.data.half = 0;
                SetQuestState(QUEST_ChaosChampion, &state);
            }
        }
    }
}

void QuestNotify_BeginAdventure(void)
{
    sPreviousRouteType = 0;

    ForEachUnlockedQuest(ActivateQuestIfNeeded);

    // Handle skip difficulty
    if(gRogueRun.currentDifficulty > 0)
    {
        TryDeactivateQuest(QUEST_GymChallenge);
        TryDeactivateQuest(QUEST_GymMaster);
        TryDeactivateQuest(QUEST_NoFainting2);
        TryDeactivateQuest(QUEST_NoFainting3);
    }

    if(gRogueRun.currentDifficulty > 4)
    {
        TryDeactivateQuest(QUEST_NoFainting1);
    }

    if(gRogueRun.currentDifficulty > 8)
    {
        // Can't technically happen atm
        TryDeactivateQuest(QUEST_EliteMaster);
    }
    
    UpdateChaosChampion(TRUE);
}

static void OnEndBattle(void)
{
    struct RogueQuestState state;

    if(IsQuestActive(QUEST_NoFainting1) && GetQuestState(QUEST_NoFainting1, &state))
    {
        if(Rogue_IsPartnerMonInTeam() == FALSE)
        {
            state.isValid = FALSE;
            SetQuestState(QUEST_NoFainting1, &state);
        }
    }
}

void QuestNotify_EndAdventure(void)
{
    TryMarkQuestAsComplete(QUEST_FirstAdventure);

    ForEachUnlockedQuest(DeactivateQuestIfNeeded);
}

void QuestNotify_OnWildBattleEnd(void)
{
    if(gBattleOutcome == B_OUTCOME_CAUGHT)
    {
        if(IsQuestActive(QUEST_Collector1))
        {
            u16 caughtCount = GetNationalPokedexCount(FLAG_GET_CAUGHT);
            if(caughtCount >= 15)
                TryMarkQuestAsComplete(QUEST_Collector1);
        }
    }

    OnEndBattle();
}

bool8 IsSpeciesLegendary(u16 species);

void QuestNotify_OnTrainerBattleEnd(bool8 isBossTrainer)
{
    u8 i;

    if(isBossTrainer)
    {
        u16 relativeDifficulty = gRogueRun.currentDifficulty - VarGet(VAR_ROGUE_SKIP_TO_DIFFICULTY);

        switch(gRogueRun.currentDifficulty)
        {
            case 1:
                TryMarkQuestAsComplete(QUEST_Gym1);
                break;
            case 2:
                TryMarkQuestAsComplete(QUEST_Gym2);
                break;
            case 3:
                TryMarkQuestAsComplete(QUEST_Gym3);
                break;
            case 4:
                TryMarkQuestAsComplete(QUEST_Gym4);
                break;
            case 5:
                TryMarkQuestAsComplete(QUEST_Gym5);
                break;
            case 6:
                TryMarkQuestAsComplete(QUEST_Gym6);
                break;
            case 7:
                TryMarkQuestAsComplete(QUEST_Gym7);
                break;
            case 8: // Just beat last Gym
                TryMarkQuestAsComplete(QUEST_Gym8);
                TryMarkQuestAsComplete(QUEST_NoFainting2);
                break;

            case 12: // Just beat last E4
                if(IsQuestActive(QUEST_Collector2))
                {
                    for(i = 0; i < PARTY_SIZE; ++i)
                    {
                        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
                        if(IsSpeciesLegendary(species))
                        {
                            TryMarkQuestAsComplete(QUEST_Collector2);
                            break;
                        }
                    }
                }
                break;

            case 14: // Just beat final champ
                TryMarkQuestAsComplete(QUEST_Champion);
                TryMarkQuestAsComplete(QUEST_NoFainting3);
                TryMarkQuestAsComplete(QUEST_ChaosChampion);
                break;
        }

        if(gRogueRun.currentDifficulty >= 4)
            TryMarkQuestAsComplete(QUEST_GymChallenge);

        if(gRogueRun.currentDifficulty >= 8)
            TryMarkQuestAsComplete(QUEST_GymMaster);

        if(gRogueRun.currentDifficulty >= 12)
            TryMarkQuestAsComplete(QUEST_EliteMaster);

        if(relativeDifficulty == 4)
        {
            if(IsQuestActive(QUEST_NoFainting1))
                TryMarkQuestAsComplete(QUEST_NoFainting1);
        }
    }
    
    OnEndBattle();
}

void QuestNotify_OnMonFainted()
{
    TryDeactivateQuest(QUEST_NoFainting2);
    TryDeactivateQuest(QUEST_NoFainting3);
}

void QuestNotify_OnWarp(struct WarpData* warp)
{
    if(Rogue_IsRunActive())
    {
        struct RogueQuestState state;

        // Warped into
        switch(gRogueAdvPath.currentRoomType)
        {
            case ADVPATH_ROOM_ROUTE:
                if(gRogueAdvPath.currentRoomParams.perType.route.difficulty == 2)
                {
                    if(IsQuestActive(QUEST_Bike1) && GetQuestState(QUEST_Bike1, &state))
                    {
                        state.data.byte[0] = gSaveBlock2Ptr->playTimeHours;
                        state.data.byte[1] = gSaveBlock2Ptr->playTimeMinutes;
                        SetQuestState(QUEST_Bike1, &state);
                    }

                    if(gRogueRun.currentDifficulty >= 8)
                    {
                        if(IsQuestActive(QUEST_Bike2) && GetQuestState(QUEST_Bike2, &state))
                        {
                            state.data.byte[0] = gSaveBlock2Ptr->playTimeHours;
                            state.data.byte[1] = gSaveBlock2Ptr->playTimeMinutes;
                            SetQuestState(QUEST_Bike2, &state);
                        }
                    }
                }
                break;

            case ADVPATH_ROOM_RESTSTOP:
                UpdateChaosChampion(TRUE);
                break;
        }

        // Warped out of
        switch(sPreviousRouteType)
        {
            case ADVPATH_ROOM_ROUTE:
                if(gRogueAdvPath.currentRoomParams.perType.route.difficulty == 2)
                {
                    if(IsQuestActive(QUEST_Bike1) && GetQuestState(QUEST_Bike1, &state))
                    {
                        u16 startTime = ((u16)state.data.byte[0]) * 60 + ((u16)state.data.byte[1]);
                        u16 exitTime = ((u16)gSaveBlock2Ptr->playTimeHours) * 60 + ((u16)gSaveBlock2Ptr->playTimeMinutes);

                        if((exitTime - startTime) < 120)
                            TryMarkQuestAsComplete(QUEST_Bike1);
                    }

                    if(gRogueRun.currentDifficulty >= 8)
                    {
                        if(IsQuestActive(QUEST_Bike2) && GetQuestState(QUEST_Bike2, &state))
                        {
                            u16 startTime = ((u16)state.data.byte[0]) * 60 + ((u16)state.data.byte[1]);
                            u16 exitTime = ((u16)gSaveBlock2Ptr->playTimeHours) * 60 + ((u16)gSaveBlock2Ptr->playTimeMinutes);

                            if((exitTime - startTime) < 60)
                                TryMarkQuestAsComplete(QUEST_Bike2);
                        }
                    }
                }
                break;
        }

        if(gRogueAdvPath.currentRoomType != ADVPATH_ROOM_RESTSTOP)
        {
            UpdateChaosChampion(FALSE);
        }

        sPreviousRouteType = gRogueAdvPath.currentRoomType;
    }
}

void QuestNotify_OnAddMoney(u32 amount)
{

}

void QuestNotify_OnRemoveMoney(u32 amount)
{
    if(Rogue_IsRunActive())
    {
        struct RogueQuestState state;

        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_RESTSTOP)
        {
            if(IsQuestActive(QUEST_ShoppingSpree) && GetQuestState(QUEST_ShoppingSpree, &state))
            {
                state.data.half += amount;
                SetQuestState(QUEST_ShoppingSpree, &state);

                if(state.data.half >= 20000)
                    TryMarkQuestAsComplete(QUEST_ShoppingSpree);
            }
        }
    }
}