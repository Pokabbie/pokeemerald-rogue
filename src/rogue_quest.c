#include "global.h"

#include "rogue.h"
#include "rogue_quest.h"

extern EWRAM_DATA struct RogueQuestData gRogueQuestData;

typedef void (*QuestCallback)(u16 questId, struct RogueQuestState* state);

void ResetQuestState(u16 startQuestId)
{
    u16 i;

    // Reset the state for any new quests
    for(i = startQuestId; i < QUEST_COUNT; ++i)
    {
        memset(&gRogueQuestData.questStates[i], 0, sizeof(struct RogueQuestState));
    }
}

bool8 GetQuestState(u16 questId, struct RogueQuestState* outState)
{
    if(questId < QUEST_COUNT)
    {
        memcpy(outState, &gRogueQuestData.questStates[questId], sizeof(struct RogueQuestState));
        return TRUE;
    }

    return FALSE;
}

void SetQuestState(u16 questId, struct RogueQuestState* state)
{
    if(questId < QUEST_COUNT)
    {
        memcpy(&gRogueQuestData.questStates[questId], state, sizeof(struct RogueQuestState));
    }
}

static void MarkQuestAsComplete(u16 questId)
{
    struct RogueQuestState* state = &gRogueQuestData.questStates[questId];

    // TODO - handle global & repeatable quests
    if(!state->isCompleted)
    {
        state->isCompleted = TRUE;
        state->hasPendingRewards = TRUE;
    }
}

static void ForEachQuest(QuestCallback callback)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = 0; i < QUEST_COUNT; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        callback(i, state);
    }
}

static void ForEachActiveQuest(QuestCallback callback)
{
    u16 i;
    struct RogueQuestState* state;

    for(i = 0; i < QUEST_COUNT; ++i)
    {
        state = &gRogueQuestData.questStates[i];
        if(state->isValid && !state->isCompleted)
        {
            callback(i, state);
        }
    }
}

//u8 isUnlocked : 1;
//u8 isCompleted : 1;
//u8 isValid : 1;
//u8 isPinned : 1;
//u8 hasPendingRewards : 1;

static void ActivateQuestIfNeeded(u16 questId, struct RogueQuestState* state)
{
    // TODO - handle global & repeatable quests
    if(!state->isCompleted)
    {
        state->isValid = TRUE;
    }
}

static void DeactivateQuestIfNeeded(u16 questId, struct RogueQuestState* state)
{
    // TODO - handle global & repeatable quests
    if(state->isValid)
    {
        state->isValid = FALSE;
    }
}

void QuestNotify_BeginAdventure(void)
{
    ForEachQuest(ActivateQuestIfNeeded);
}

void QuestNotify_EndAdventure(void)
{
    ForEachQuest(DeactivateQuestIfNeeded);
}