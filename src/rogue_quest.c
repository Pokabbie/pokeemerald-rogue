#include "global.h"
#include "constants/items.h"

#include "event_data.h"
#include "item.h"
#include "money.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_quest.h"

extern const u8 gText_QuestRewardGive[];
extern const u8 gText_QuestRewardGiveMoney[];
extern const u8 gText_QuestLogTitleStatusIncomplete[];

extern EWRAM_DATA struct RogueQuestData gRogueQuestData;

static EWRAM_DATA u8 sRewardQuest = 0;
static EWRAM_DATA u8 sRewardParam = 0;

typedef void (*QuestCallback)(u16 questId, struct RogueQuestState* state);

static void TryUnlockQuest(u16 questId);
static void TryMarkQuestAsComplete(u16 questId);
static void UnlockFollowingQuests(u16 questId);

void ResetQuestState(u16 startQuestId)
{
    u16 i;

    // Reset the state for any new quests
    for(i = startQuestId; i < QUEST_COUNT; ++i)
    {
        memset(&gRogueQuestData.questStates[i], 0, sizeof(struct RogueQuestState));
    }

    // These quests must always be unlocked
    for(i = QUEST_FirstAdventure; i <= QUEST_Champion; ++i)
    {
        TryUnlockQuest(i);
    }
}

bool8 AnyNewQuests(void)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isUnlocked && state->hasNewMarker)
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool8 GetQuestState(u16 questId, struct RogueQuestState* outState)
{
    if(questId < QUEST_COUNT)
    {
        memcpy(outState, &gRogueQuestData.questStates[questId], sizeof(struct RogueQuestState));
        return outState->isUnlocked;
    }

    return FALSE;
}

void SetQuestState(u16 questId, struct RogueQuestState* state)
{
    if(questId >= QUEST_FIRST && questId < QUEST_COUNT)
    {
        memcpy(&gRogueQuestData.questStates[questId], state, sizeof(struct RogueQuestState));
    }
}

bool8 IsQuestRepeatable(u16 questId)
{
    return (gRogueQuests[questId].flags & QUEST_FLAGS_REPEATABLE) != 0;
}

bool8 IsQuestGloballyTracked(u16 questId)
{
    return (gRogueQuests[questId].flags & QUEST_FLAGS_GLOBALALLY_TRACKED) != 0;
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
    for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
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
                    StringCopy(str, gText_QuestLogTitleStatusIncomplete);
                    break;
            }
        }
        return TRUE;
    }

    return FALSE;
}

static void TryUnlockQuest(u16 questId)
{
    struct RogueQuestState* state = &gRogueQuestData.questStates[questId];

    if(!state->isUnlocked)
    {
        state->isUnlocked = TRUE;
        state->isValid = FALSE;
        state->isCompleted = FALSE;
        state->hasNewMarker = TRUE;
    }
}
static void TryMarkQuestAsComplete(u16 questId)
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
    }
}

static void UnlockFollowingQuests(u16 questId)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(gRogueQuests[questId].unlockedQuests); ++i)
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

    for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
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

    for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
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

static void DeactivateQuest(u16 questId)
{
    DeactivateQuestIfNeeded(questId, &gRogueQuestData.questStates[questId]);
}

void QuestNotify_BeginAdventure(void)
{
    ForEachUnlockedQuest(ActivateQuestIfNeeded);

    // Handle skip difficulty
    if(gRogueRun.currentDifficulty > 0)
    {
        DeactivateQuest(QUEST_GymChallenge);
        DeactivateQuest(QUEST_GymMaster);
    }

    if(gRogueRun.currentDifficulty > 8)
    {
        // Can't technically happen atm
        DeactivateQuest(QUEST_EliteMaster);
    }
}

void QuestNotify_EndAdventure(void)
{
    TryMarkQuestAsComplete(QUEST_FirstAdventure);

    ForEachUnlockedQuest(DeactivateQuestIfNeeded);
}

void QuestNotify_OnWildBattleEnd(void)
{

}

void QuestNotify_OnTrainerBattleEnd(bool8 isBossTrainer)
{
    if(isBossTrainer)
    {
        if(gRogueRun.currentDifficulty >= 4)
            TryMarkQuestAsComplete(QUEST_GymChallenge);

        if(gRogueRun.currentDifficulty >= 8)
            TryMarkQuestAsComplete(QUEST_GymMaster);

        if(gRogueRun.currentDifficulty >= 12)
            TryMarkQuestAsComplete(QUEST_EliteMaster);

        if(gRogueRun.currentDifficulty >= 14)
            TryMarkQuestAsComplete(QUEST_Champion);
    }
}