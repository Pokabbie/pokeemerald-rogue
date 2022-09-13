#include "global.h"

#include "rogue.h"
#include "rogue_quest.h"

EWRAM_DATA struct RogueQuestState gRogueQuestStates[ROGUE_QUEST_COUNT];

u8 Rogue_GetQuestCompletionStatus(u16 questId)
{
    return gRogueQuestStates[questId].completionLevel;
}

bool8 Rogue_IsQuestActive(u16 questId)
{
    return gRogueQuestStates[questId].isActive;
}

bool8 Rogue_IsQuestPinned(u16 questId)
{
    return gRogueQuestStates[questId].isPinned;
}