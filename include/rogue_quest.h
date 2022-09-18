#ifndef QUEST_H
#define QUEST_H

void ResetQuestState(u16 startQuestId);
bool8 AnyNewQuests(void);

bool8 GetQuestState(u16 questId, struct RogueQuestState* outState);
void SetQuestState(u16 questId, struct RogueQuestState* state);

bool8 IsQuestRepeatable(u16 questId);
bool8 IsQuestGloballyTracked(u16 questId);
bool8 DoesQuestHaveUnlocks(u16 questId);

bool8 GiveNextRewardAndFormat(u8* str);

void QuestNotify_BeginAdventure(void);
void QuestNotify_EndAdventure(void);

void QuestNotify_OnWildBattleEnd(void);
void QuestNotify_OnTrainerBattleEnd(bool8 isBossTrainer);

#endif