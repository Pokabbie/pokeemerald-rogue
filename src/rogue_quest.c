#include "global.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_quest.h"

extern EWRAM_DATA struct RogueQuestData gRogueQuestData;

typedef void (*QuestCallback)(u16 questId, struct RogueQuestState* state);

static void UnlockQuest(u16 questId);
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
        UnlockQuest(i);
    }
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

static void UnlockQuest(u16 questId)
{
    struct RogueQuestState* state = &gRogueQuestData.questStates[questId];

    if(!state->isUnlocked)
    {
        state->isUnlocked = TRUE;
        state->isValid = FALSE;
        state->isCompleted = FALSE;
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
            UnlockFollowingQuests(questId);
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
    switch(questId)
    {
        case QUEST_GymMaster:
            UnlockQuest(QUEST_Electric_Master);
            UnlockQuest(QUEST_Electric_Champion);
            break;
    }
}

static void ForEachQuest(QuestCallback callback)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = QUEST_FIRST; i < QUEST_COUNT; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        callback(i, state);
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
    ForEachQuest(ActivateQuestIfNeeded);

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

    ForEachQuest(DeactivateQuestIfNeeded);
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
            TryMarkQuestAsComplete(QUEST_GymChallenge);

        if(gRogueRun.currentDifficulty >= 12)
            TryMarkQuestAsComplete(QUEST_EliteMaster);

        if(gRogueRun.currentDifficulty >= 14)
            TryMarkQuestAsComplete(QUEST_Champion);
    }
}