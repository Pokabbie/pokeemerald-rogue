#ifndef QUEST_H
#define QUEST_H

void ResetQuestState(u16 saveVersion);
bool8 AnyNewQuests(void);
bool8 AnyQuestRewardsPending(void);

u16 GetCompletedQuestCount(void);
u16 GetUnlockedQuestCount(void);

bool8 GetQuestState(u16 questId, struct RogueQuestState* outState);
void SetQuestState(u16 questId, struct RogueQuestState* state);

bool8 IsQuestRepeatable(u16 questId);
bool8 IsQuestCollected(u16 questId);
bool8 IsQuestGloballyTracked(u16 questId);
bool8 IsQuestActive(u16 questId);
bool8 DoesQuestHaveUnlocks(u16 questId);

bool8 GiveNextRewardAndFormat(u8* str);
bool8 TryUnlockQuest(u16 questId);
bool8 TryMarkQuestAsComplete(u16 questId);
bool8 TryDeactivateQuest(u16 questId);

void QuestNotify_BeginAdventure(void);
void QuestNotify_EndAdventure(void);

void QuestNotify_OnWildBattleEnd(void);
void QuestNotify_OnTrainerBattleEnd(bool8 isBossTrainer);
void QuestNotify_OnMonFainted(void);

void QuestNotify_OnWarp(struct WarpData* warp);
void QuestNotify_OnAddMoney(u32 amount);
void QuestNotify_OnRemoveMoney(u32 amount);

#endif