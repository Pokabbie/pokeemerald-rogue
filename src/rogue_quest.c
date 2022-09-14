#include "global.h"

#include "rogue.h"
#include "rogue_quest.h"

extern EWRAM_DATA struct RogueQuestData gRogueQuestData;


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