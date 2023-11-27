#ifndef ROGUE_QUEST_H
#define ROGUE_QUEST_H

#include "constants/generated/quests.h"

typedef u8 (*QuestFunc)(u16 questId, u16 trigger);

enum
{
    QUEST_REWARD_POKEMON,
    QUEST_REWARD_ITEM,
    QUEST_REWARD_SHOP_ITEM,
    QUEST_REWARD_MONEY,
    QUEST_REWARD_QUEST_UNLOCK,
};

struct RogueQuestRewardNEW
{
    u8 type;
    union
    {
        struct
        {
            u16 species;
            u8 isShiny : 1;
        } pokemon;
        struct
        {
            u16 item;
            u16 count;
        } item;
        struct
        {
            u16 item;
        } shopItem;
        struct
        {
            u32 amount;
        } money;
        struct
        {
            u16 questId;
        } questUnlock;
    } perType;
};

struct RogueQuestEntry
{
    u8 const* title;
    u8 const* desc;
    u16 const* triggerParams;
    struct RogueQuestRewardNEW const* rewards;
    QuestFunc triggerFunc;
    u32 flags;
    u32 triggerFlags;
    u16 triggerParamCount;
    u16 stateOffset;
    u16 stateSize;
    u16 rewardCount;
};

u8 const* RogueQuest_GetTitle(u16 questId);
u8 const* RogueQuest_GetDesc(u16 questId);
bool8 RogueQuest_GetConstFlag(u16 questId, u32 flag);

bool8 RogueQuest_GetStateFlag(u16 questId, u32 flag);
void RogueQuest_SetStateFlag(u16 questId, u32 flag, bool8 state);

bool8 RogueQuest_IsQuestUnlocked(u16 questId);
bool8 RogueQuest_TryUnlockQuest(u16 questId);
void RogueQuest_ClearNewUnlockQuests();

void RogueQuest_ActivateQuestsFor(u32 flags);

void RogueQuest_OnNewGame();
void RogueQuest_OnLoadGame();
void RogueQuest_OnTrigger(u16 trigger);


// old
void ResetQuestStateAfter(u16 loadedQuestCapacity);
bool8 AnyNewQuests(void);
bool8 AnyQuestRewardsPending(void);
bool8 AnyNewQuestsPending(void);

u16 GetCompletedQuestCount(void);
u16 GetUnlockedQuestCount(void);
u8 GetCompletedQuestPerc(void);

bool8 GetQuestState(u16 questId, struct OLDRogueQuestState* outState);
void SetQuestState(u16 questId, struct OLDRogueQuestState* state);

bool8 IsQuestRepeatable(u16 questId);
bool8 IsQuestCollected(u16 questId);
bool8 IsQuestGloballyTracked(u16 questId);
bool8 IsQuestActive(u16 questId);
bool8 DoesQuestHaveUnlocks(u16 questId);

bool8 GiveNextRewardAndFormat(u8* str, u8* type);
bool8 TryUnlockQuest(u16 questId);
bool8 TryMarkQuestAsComplete(u16 questId);
bool8 TryDeactivateQuest(u16 questId);
void UnlockFollowingQuests(u16 questId);

void QuestNotify_BeginAdventure(void);
void QuestNotify_EndAdventure(void);

void QuestNotify_OnWildBattleEnd(void);
void QuestNotify_OnTrainerBattleEnd(bool8 isBossTrainer);
void QuestNotify_OnMonFainted(void);

void QuestNotify_OnExitHubTransition(void);
void QuestNotify_OnWarp(struct WarpData* warp);
void QuestNotify_OnAddMoney(u32 amount);
void QuestNotify_OnRemoveMoney(u32 amount);

void QuestNotify_OnAddBagItem(u16 itemId, u16 count);
void QuestNotify_OnRemoveBagItem(u16 itemId, u16 count);

void QuestNotify_OnUseBattleItem(u16 itemId);

void QuestNotify_OnMegaEvolve(u16 species);
void QuestNotify_OnZMoveUsed(u16 move);

void QuestNotify_StatIncrement(u8 statIndex);

#endif