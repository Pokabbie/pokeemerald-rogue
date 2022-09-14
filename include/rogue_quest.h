#ifndef QUEST_H
#define QUEST_H

void ResetQuestState(u16 startQuestId);

bool8 GetQuestState(u16 questId, struct RogueQuestState* outState);
void SetQuestState(u16 questId, struct RogueQuestState* state);

void QuestNotify_BeginAdventure(void);
void QuestNotify_EndAdventure(void);

#endif