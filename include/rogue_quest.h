#ifndef ROGUE_QUEST_H
#define ROGUE_QUEST_H

u8 Rogue_GetQuestCompletionStatus(u16 questId);
bool8 Rogue_IsQuestActive(u16 questId);
bool8 Rogue_IsQuestPinned(u16 questId);

#endif