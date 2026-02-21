#ifndef ROGUE_QUEST_H
#define ROGUE_QUEST_H

#include "constants/generated/quests.h"

struct RogueQuestTrigger;
struct CustomMonPreset;

typedef bool8 (*QuestFunc)(u16 questId, struct RogueQuestTrigger const* trigger);

enum
{
    QUEST_REWARD_POKEMON,
    QUEST_REWARD_ITEM,
    QUEST_REWARD_SHOP_ITEM,
    QUEST_REWARD_MONEY,
    QUEST_REWARD_QUEST_UNLOCK,
    QUEST_REWARD_FLAG,
    QUEST_REWARD_HUB_UPGRADE,
    QUEST_REWARD_DECOR,
    QUEST_REWARD_DECOR_VARIANT,
    QUEST_REWARD_OUTFIT_UNLOCK,
};

enum
{
    QUEST_REWARD_VISIBLITY_DEFAULT,
    QUEST_REWARD_VISIBLITY_INVISIBLE,
    QUEST_REWARD_VISIBLITY_OBSCURED,
};

struct RogueQuestReward
{
    u8 type;
    u8 visiblity;
    u8 requiredDifficulty;
    struct CustomPopup const* customPopup;
    union
    {
        struct
        {
            u8 const* nickname;
            u16 customMonId;
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
        struct
        {
            u16 flagId;
        } flag;
        struct
        {
            u16 upgradeId;
        } hubUpgrade;
        struct
        {
            u16 decorId;
        } decor;
        struct
        {
            u16 decorVariantId;
        } decorVariant;
        struct
        {
            u16 outfitUnlockId;
        } outfitUnlock;
    } perType;
};

struct RogueQuestTrigger
{
    u16 const* params;
    QuestFunc callback;
    u32 flags;
    u16 paramCount;
    u8 passState;
    u8 failState;
};

enum
{
    QUEST_REQUIREMENT_TYPE_ITEM,
    QUEST_REQUIREMENT_TYPE_FLAG,
    QUEST_REQUIREMENT_TYPE_CONFIG_TOGGLE,
    QUEST_REQUIREMENT_TYPE_CONFIG_RANGE,
};

enum
{
    QUEST_REQUIREMENT_OPERATION_EQUAL,
    QUEST_REQUIREMENT_OPERATION_NOT_EQUAL,
    QUEST_REQUIREMENT_OPERATION_GREATER_THAN,
    QUEST_REQUIREMENT_OPERATION_LESS_THAN,
    QUEST_REQUIREMENT_OPERATION_GREATER_THAN_EQUAL,
    QUEST_REQUIREMENT_OPERATION_LESS_THAN_EQUAL,
};

struct RogueQuestRequirement
{
    u16 type;
    union
    {
        struct
        {
            u16 itemId;
            u8 operation;
            u8 count;
        } item;
        struct
        {
            u16 flag;
            u8 state;
        } flag;
        struct
        {
            u16 toggle;
            u8 state;
        } configToggle;
        struct
        {
            u16 range;
            u8 operation;
            u8 value;
        } configRange;
    } perType;
};

struct RogueQuestEntry
{
    u8 const* title;
    u8 const* desc;
    struct RogueQuestReward const* rewards;
    struct RogueQuestTrigger const* triggers;
    struct RogueQuestRequirement const* requirements;
    u32 flags;
    u32 triggerFlags;
    u16 stateOffset;
    u16 stateSize;
    u16 rewardCount;
    u16 triggerCount;
    u16 requirementCount;
};

u8 const* RogueQuest_GetTitle(u16 questId);
u8 const* RogueQuest_GetDesc(u16 questId);
bool8 RogueQuest_GetConstFlag(u16 questId, u32 flag);

u16 RogueQuest_GetOrderedQuest(u16 index, bool8 alphabetical);

bool8 RogueQuest_GetStateFlag(u16 questId, u32 flag);
void RogueQuest_SetStateFlag(u16 questId, u32 flag, bool8 state);

struct RogueQuestReward const* RogueQuest_GetReward(u16 questId, u16 i);
u16 RogueQuest_GetRewardCount(u16 questId);

u8 RogueQuest_GetHighestCompleteDifficulty(u16 questId);

bool8 RogueQuest_IsQuestUnlocked(u16 questId);
bool8 RogueQuest_TryUnlockQuest(u16 questId);
bool8 RogueQuest_IsQuestVisible(u16 questId);
bool8 RogueQuest_HasPendingNewQuests();
void RogueQuest_ClearNewUnlockQuests();

bool8 RogueQuest_HasCollectedRewards(u16 questId);
bool8 RogueQuest_HasPendingRewards(u16 questId);
bool8 RogueQuest_HasAnyPendingRewards();
bool8 RogueQuest_TryCollectRewards(u16 questId);
bool8 RogueQuest_IsRewardSequenceActive();
void RogueQuest_BeginRewardSequence();
void RogueQuest_EndRewardSequence();

void RogueQuest_ActivateQuestsFor(u32 flags);
bool8 RogueQuest_IsQuestActive(u16 questId);

void RogueQuest_CheckQuestRequirements();

u16 RogueQuest_GetQuestCompletePercFor(u32 constFlag);
u16 RogueQuest_GetQuestCompletePercAtDifficultyFor(u32 constFlag, u8 difficultyLevel);
void RogueQuest_GetQuestCountsFor(u32 constFlag, u16* activeCount, u16* inactiveCount);
u16 RogueQuest_GetQuestTotalCountFor(u32 constFlag, bool8 includeLocked);
u16 RogueQuest_GetDisplayCompletePerc();

void RogueQuest_OnNewGame();
void RogueQuest_OnLoadGame();
void RogueQuest_OnTrigger(u32 trigger);

bool8 RogueQuest_HasUnlockedChallenges();
bool8 RogueQuest_HasUnlockedMonMasteries();

bool8 RogueQuest_GetMonMasteryFlag(u16 species);
void RogueQuest_SetMonMasteryFlag(u16 species);
void RogueQuest_SetMonMasteryFlagFromParty();
u32 RogueQuest_GetMonMasteryTotalPerc();

#endif