#include "global.h"
#include "constants/abilities.h"
#include "constants/game_stat.h"
#include "constants/items.h"
#include "constants/region_map_sections.h"
#include "constants/songs.h"

#include "battle.h"
#include "event_data.h"
#include "data.h"
#include "item.h"
#include "item_menu.h"
#include "malloc.h"
#include "money.h"
#include "pokedex.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_player_customisation.h"
#include "rogue_pokedex.h"
#include "rogue_quest.h"
#include "rogue_settings.h"
#include "rogue_popup.h"

// new quests

struct RogueQuestRewardOutput
{
    u32 moneyCount;
    u16 buildSuppliesCount;
    u16 failedRewardItem;
    u16 failedRewardCount;
};

static EWRAM_DATA struct RogueQuestRewardOutput* sRogueQuestRewardOutput = NULL;

static bool8 QuestCondition_Always(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_DifficultyGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_DifficultyLessThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_IsStandardRunActive(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_HasCompletedQuestAND(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_HasCompletedQuestOR(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsType(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyOnlyContainsType(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsLegendary(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsOnlyLegendaries(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsOnlyShinys(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsOnlyStarters(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsOnlyUniqueTypes(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsInitialPartner(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsSpecies(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyContainsAllSpecies(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PartyMaxBSTLessThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_CurrentlyInMap(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_CurrentlyInRoomType(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_AreOnlyTheseTrainersActive(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_IsPokedexRegion(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_IsPokedexVariant(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_CanUnlockFinalQuest(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_HasBuiltAllAreas(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_IsFinalQuestConditionMet(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PokedexEntryCountGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PokedexShinyEntryCountGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_InAdventureEncounterType(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_TotalMoneySpentGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_PlayerMoneyGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_RandomanWasUsed(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_RandomanWasActive(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_LastRandomanWasFullParty(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_LastItemWasAny(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_BagContainsItemsOR(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_FlagGet(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_VarGetEqual(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_VarGetLessThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_VarGetGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_RunTimerLessThanMins(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_IsConfigRangeEqualToAny(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_SpeciesMasteryComplete(u16 questId, struct RogueQuestTrigger const* trigger);
static bool8 QuestCondition_CurrentEvilTeamIs(u16 questId, struct RogueQuestTrigger const* trigger);

static bool8 IsQuestSurpressed(u16 questId);
static bool8 CanSurpressedQuestActivate(u16 questId);
static void FailQuest(u16 questId);

bool8 PartyContainsBaseSpecies(struct Pokemon *party, u8 partyCount, u16 species);

#define COMPOUND_STRING(str) (const u8[]) _(str)

#include "data/rogue/quests.h"

#undef COMPOUND_STRING

// ensure we are serializing the exact correct amount
STATIC_ASSERT(QUEST_SAVE_COUNT == QUEST_ID_COUNT, saveQuestCountMissmatch);

//static u8* RogueQuest_GetExtraState(size_t size)
//{
//    // todo
//    AGB_ASSERT(FALSE);
//    return NULL;
//}

static struct RogueQuestEntry const* RogueQuest_GetEntry(u16 questId)
{
    AGB_ASSERT(questId < QUEST_ID_COUNT);
    if(questId < ARRAY_COUNT(sQuestEntries))
        return &sQuestEntries[questId];
    else
        return NULL;
}

static struct RogueQuestState* RogueQuest_GetState(u16 questId)
{
    AGB_ASSERT(questId < QUEST_ID_COUNT);
    if(questId < QUEST_ID_COUNT)
        return &gRogueSaveBlock->questStates[questId];

    return NULL;
}

u8 const* RogueQuest_GetTitle(u16 questId)
{
    struct RogueQuestEntry const* entry = RogueQuest_GetEntry(questId);
    AGB_ASSERT(questId < QUEST_ID_COUNT);
    return entry->title;
}

u8 const* RogueQuest_GetDesc(u16 questId)
{
    struct RogueQuestEntry const* entry = RogueQuest_GetEntry(questId);
    AGB_ASSERT(questId < QUEST_ID_COUNT);
    return entry->desc;
}

bool8 RogueQuest_GetConstFlag(u16 questId, u32 flag)
{
    struct RogueQuestEntry const* entry = RogueQuest_GetEntry(questId);
    AGB_ASSERT(questId < QUEST_ID_COUNT);
    return (entry->flags & flag) != 0;
}

u16 RogueQuest_GetOrderedQuest(u16 index, bool8 alphabetical)
{
    if(alphabetical)
    {
        AGB_ASSERT(index < ARRAY_COUNT(sQuestAlphabeticalOrder));
        return sQuestAlphabeticalOrder[index];
    }
    else
    {

        AGB_ASSERT(index < ARRAY_COUNT(sQuestDisplayOrder));
        return sQuestDisplayOrder[index];
    }
}

bool8 RogueQuest_GetStateFlag(u16 questId, u32 flag)
{
    struct RogueQuestState* questState = RogueQuest_GetState(questId);
    return (questState->stateFlags & flag) != 0;
}

void RogueQuest_SetStateFlag(u16 questId, u32 flag, bool8 state)
{
    struct RogueQuestState* questState = RogueQuest_GetState(questId);

    if(state)
        questState->stateFlags |= flag;
    else
        questState->stateFlags &= ~flag;
}

struct RogueQuestReward const* RogueQuest_GetReward(u16 questId, u16 i)
{
    struct RogueQuestEntry const* entry = RogueQuest_GetEntry(questId);
    AGB_ASSERT(questId < QUEST_ID_COUNT);
    AGB_ASSERT(i < entry->rewardCount);
    return &entry->rewards[i];
}

u16 RogueQuest_GetRewardCount(u16 questId)
{
    struct RogueQuestEntry const* entry = RogueQuest_GetEntry(questId);
    AGB_ASSERT(questId < QUEST_ID_COUNT);
    return entry->rewardCount;
}

u8 RogueQuest_GetHighestCompleteDifficulty(u16 questId)
{
    if(RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
    {
        struct RogueQuestState* questState = RogueQuest_GetState(questId);
        return questState->highestCompleteDifficulty;
    }

    return DIFFICULTY_LEVEL_NONE;
}

static bool8 CanActivateQuest(u16 questId)
{
    if(!RogueQuest_IsQuestUnlocked(questId))
        return FALSE;

    // Cannot start quests we have rewards for
    if(RogueQuest_GetStateFlag(questId, QUEST_STATE_PENDING_REWARDS))
        return FALSE;

    // Masteries still work in the background, but challenges don't
    if(IsQuestSurpressed(questId) && !CanSurpressedQuestActivate(questId))
        return FALSE;

    // Challenges can be run again at a higher difficulty
    if(RogueQuest_GetConstFlag(questId, QUEST_CONST_IS_CHALLENGE))
    {
        if(Rogue_ShouldDisableChallengeQuests())
            return FALSE;

        if(RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
        {
            u8 difficultyLevel = Rogue_GetDifficultyRewardLevel();
            struct RogueQuestState* questState = RogueQuest_GetState(questId);

            if(questState->highestCompleteDifficulty != DIFFICULTY_LEVEL_NONE && difficultyLevel <= questState->highestCompleteDifficulty)
                return FALSE;
        }
    }
    else // QUEST_CONST_IS_MAIN_QUEST || QUEST_CONST_IS_MON_MASTERY
    {
        if(Rogue_ShouldDisableMainQuests())
            return FALSE;

        // Can't repeat main quests
        if(RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
            return FALSE;
    }

    return TRUE;
}

// Surpressed quests are quests which can still activate and be completed, but all UI mentioned are hidden
// i.e. you can technically complete masteries without having them unlocked
static bool8 IsQuestSurpressed(u16 questId)
{
    if(RogueQuest_GetConstFlag(questId, QUEST_CONST_IS_CHALLENGE))
    {
        if(!RogueQuest_HasUnlockedChallenges())
            return TRUE;
    }

    if(RogueQuest_GetConstFlag(questId, QUEST_CONST_IS_MON_MASTERY))
    {
        if(!RogueQuest_HasUnlockedMonMasteries())
            return TRUE;
    }

    return FALSE;
}

static bool8 CanSurpressedQuestActivate(u16 questId)
{
    if(RogueQuest_GetConstFlag(questId, QUEST_CONST_IS_MON_MASTERY))
        return TRUE;

    return FALSE;
}

bool8 RogueQuest_IsQuestUnlocked(u16 questId)
{
    return RogueQuest_GetStateFlag(questId, QUEST_STATE_UNLOCKED);
}

bool8 RogueQuest_IsQuestVisible(u16 questId)
{
    return RogueQuest_IsQuestUnlocked(questId) && !IsQuestSurpressed(questId);
}

bool8 RogueQuest_TryUnlockQuest(u16 questId)
{
    if(!RogueQuest_IsQuestUnlocked(questId))
    {
        RogueQuest_SetStateFlag(questId, QUEST_STATE_UNLOCKED, TRUE);
        RogueQuest_SetStateFlag(questId, QUEST_STATE_NEW_UNLOCK, TRUE);

        // Activate quest now if we can/should (Assuming we only ever call this from within the hub)
        if(RogueQuest_GetConstFlag(questId, QUEST_CONST_ACTIVE_IN_HUB))
        {
            RogueQuest_SetStateFlag(questId, QUEST_STATE_ACTIVE, TRUE);
        }

        return TRUE;
    }

    return FALSE;
}

bool8 RogueQuest_HasPendingNewQuests()
{
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_IsQuestUnlocked(i) && !IsQuestSurpressed(i) && RogueQuest_GetStateFlag(i, QUEST_STATE_NEW_UNLOCK))
            return TRUE;
    }

    return FALSE;
}

void RogueQuest_ClearNewUnlockQuests()
{
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_IsQuestUnlocked(i) && !IsQuestSurpressed(i))
            RogueQuest_SetStateFlag(i, QUEST_STATE_NEW_UNLOCK, FALSE);
    }
}

bool8 RogueQuest_HasCollectedRewards(u16 questId)
{
    if(RogueQuest_IsQuestUnlocked(questId) && !IsQuestSurpressed(questId))
    {
        if(RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE) && !RogueQuest_GetStateFlag(questId, QUEST_STATE_PENDING_REWARDS))
            return TRUE;
    }

    return FALSE;
}

bool8 RogueQuest_HasPendingRewards(u16 questId)
{
    if(RogueQuest_IsQuestUnlocked(questId) && !IsQuestSurpressed(questId))
    {
        if(RogueQuest_GetStateFlag(questId, QUEST_STATE_PENDING_REWARDS))
            return TRUE;
    }

    return FALSE;
}

bool8 RogueQuest_HasAnyPendingRewards()
{
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_HasPendingRewards(i))
            return TRUE;
    }

    return FALSE;
}

static bool8 GiveRewardInternal(struct RogueQuestReward const* rewardInfo)
{
    bool8 state = TRUE;
    bool8 mutePopups = FALSE;

    if(rewardInfo->customPopup != NULL)
    {
        mutePopups = TRUE;
        Rogue_PushPopup_CustomPopup(rewardInfo->customPopup);
    }

    switch (rewardInfo->type)
    {
    case QUEST_REWARD_POKEMON:
        {
            struct Pokemon* mon = &gEnemyParty[0];
            u32 temp = 0;
            bool8 isCustom = FALSE;

            if(rewardInfo->perType.pokemon.customMonId != CUSTOM_MON_NONE)
            {
                isCustom = TRUE;
                RogueGift_CreateMon(rewardInfo->perType.pokemon.customMonId, mon, rewardInfo->perType.pokemon.species, STARTER_MON_LEVEL, USE_RANDOM_IVS);
            }
            else
            {
                ZeroMonData(mon);
                CreateMon(mon, rewardInfo->perType.pokemon.species, STARTER_MON_LEVEL, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

                temp = METLOC_FATEFUL_ENCOUNTER;
                SetMonData(mon, MON_DATA_MET_LOCATION, &temp);
            }

            // Update nickname
            if(rewardInfo->perType.pokemon.nickname != NULL)
            {
                SetMonData(mon, MON_DATA_NICKNAME, rewardInfo->perType.pokemon.nickname);
            }

            // Set shiny state
            if(rewardInfo->perType.pokemon.isShiny)
            {
                temp = 1;
                SetMonData(mon, MON_DATA_IS_SHINY, &temp);
            }

            // Give mon
            if(isCustom)
                GiveTradedMonToPlayer(mon);
            else
                GiveMonToPlayer(mon);

            // Set pokedex flag
            GetSetPokedexSpeciesFlag(rewardInfo->perType.pokemon.species, rewardInfo->perType.pokemon.isShiny ? FLAG_SET_CAUGHT_SHINY : FLAG_SET_CAUGHT);

            if(!mutePopups)
                Rogue_PushPopup_AddPokemon(rewardInfo->perType.pokemon.species, isCustom, rewardInfo->perType.pokemon.isShiny);
        }
        break;

    case QUEST_REWARD_ITEM:
        state = AddBagItem(rewardInfo->perType.item.item, rewardInfo->perType.item.count);

        if(state)
        {
            if(sRogueQuestRewardOutput && rewardInfo->perType.item.item == ITEM_BUILDING_SUPPLIES)
                sRogueQuestRewardOutput->buildSuppliesCount += rewardInfo->perType.item.count;
            else if(!mutePopups)
                Rogue_PushPopup_AddItem(rewardInfo->perType.item.item, rewardInfo->perType.item.count);
        }
        else
        {
            if(sRogueQuestRewardOutput)
            {
                sRogueQuestRewardOutput->failedRewardItem = rewardInfo->perType.item.item;
                sRogueQuestRewardOutput->failedRewardCount = rewardInfo->perType.item.count;
            }
        }
        break;

    case QUEST_REWARD_SHOP_ITEM:
        if(!mutePopups)
            Rogue_PushPopup_UnlockedShopItem(rewardInfo->perType.shopItem.item);
        break;

    case QUEST_REWARD_MONEY:
        AddMoney(&gSaveBlock1Ptr->money, rewardInfo->perType.money.amount);

        if(sRogueQuestRewardOutput)
            sRogueQuestRewardOutput->moneyCount += rewardInfo->perType.money.amount;
        else if(!mutePopups)
            Rogue_PushPopup_AddMoney(rewardInfo->perType.money.amount);

        break;

    case QUEST_REWARD_QUEST_UNLOCK:
        RogueQuest_TryUnlockQuest(rewardInfo->perType.questUnlock.questId);
        break;

    case QUEST_REWARD_FLAG:
        FlagSet(rewardInfo->perType.flag.flagId);
        break;

    case QUEST_REWARD_HUB_UPGRADE:
        RogueHub_SetUpgrade(rewardInfo->perType.hubUpgrade.upgradeId, TRUE);
        AGB_ASSERT(mutePopups); // force custom popups for now, as this is already an edge case anyway
        break;

    case QUEST_REWARD_DECOR:
        if(!mutePopups)
            Rogue_PushPopup_UnlockedDecor(rewardInfo->perType.decor.decorId);
        break;

    case QUEST_REWARD_DECOR_VARIANT:
        if(!mutePopups)
            Rogue_PushPopup_UnlockedDecorVariant(rewardInfo->perType.decorVariant.decorVariantId);
        break;

    case QUEST_REWARD_OUTFIT_UNLOCK:
        RoguePlayer_ActivateOutfitUnlock(rewardInfo->perType.outfitUnlock.outfitUnlockId);
        AGB_ASSERT(mutePopups); // force custom popups for now, as this is already an edge case anyway
        break;

    default:
        AGB_ASSERT(FALSE);
        break;
    }

    return state;
}

static void RemoveRewardInternal(struct RogueQuestReward const* rewardInfo)
{
    switch (rewardInfo->type)
    {
    case QUEST_REWARD_ITEM:
        RemoveBagItem(rewardInfo->perType.item.item, rewardInfo->perType.item.count);

        if(sRogueQuestRewardOutput && rewardInfo->perType.item.item == ITEM_BUILDING_SUPPLIES)
            sRogueQuestRewardOutput->buildSuppliesCount -= rewardInfo->perType.item.count;
        break;

    case QUEST_REWARD_MONEY:
        RemoveMoney(&gSaveBlock1Ptr->money, rewardInfo->perType.money.amount);

        if(sRogueQuestRewardOutput)
            sRogueQuestRewardOutput->moneyCount -= rewardInfo->perType.money.amount;
        break;
    
    default:
        // Cannot refund this type
        AGB_ASSERT(FALSE);
        break;
    }
}

static bool8 IsHighPriorityReward(struct RogueQuestReward const* rewardInfo)
{
    // High priority rewards can fail and be refunded
    switch (rewardInfo->type)
    {
    case QUEST_REWARD_ITEM:
        return TRUE;
    }

    return FALSE;
}

static bool8 ShouldSkipQuestReward(struct RogueQuestReward const* rewardInfo, u8 minRewardDifficulty, u8 maxRewardDifficulty)
{
    // Skip if outside of reward range
    return !(rewardInfo->requiredDifficulty >= minRewardDifficulty && rewardInfo->requiredDifficulty <= maxRewardDifficulty);
}

bool8 RogueQuest_TryCollectRewards(u16 questId)
{
    u16 i;
    bool8 state = TRUE;
    struct RogueQuestReward const* rewardInfo;
    struct RogueQuestState* questState = RogueQuest_GetState(questId);
    u16 rewardCount = RogueQuest_GetRewardCount(questId);
    u8 minRewardDifficulty = questState->highestCollectedRewardDifficulty;
    u8 maxRewardDifficulty = questState->highestCompleteDifficulty;

    if(minRewardDifficulty == DIFFICULTY_LEVEL_NONE)
    {
        minRewardDifficulty = DIFFICULTY_LEVEL_EASY;
    }
    else
    {
        // We've already collected rewards for the highestCollectedRewardDifficulty so don't give them again
        minRewardDifficulty++;
        AGB_ASSERT(minRewardDifficulty <= maxRewardDifficulty);
    }
    

    AGB_ASSERT(RogueQuest_HasPendingRewards(questId));
    AGB_ASSERT(minRewardDifficulty < DIFFICULTY_PRESET_COUNT);
    AGB_ASSERT(maxRewardDifficulty < DIFFICULTY_PRESET_COUNT);

    // Give high pri rewards
    for(i = 0; i < rewardCount; ++i)
    {
        rewardInfo = RogueQuest_GetReward(questId, i);

        if(ShouldSkipQuestReward(rewardInfo, minRewardDifficulty, maxRewardDifficulty))
            continue;

        if(IsHighPriorityReward(rewardInfo))
        {
            state = GiveRewardInternal(rewardInfo);
            if(!state)
                break;
        }
    }

    if(!state)
    {
        // Failed to give items so refund all we had previously given
        rewardCount = i;
        for(i = 0; i < rewardCount; ++i)
        {
            rewardInfo = RogueQuest_GetReward(questId, i);

            if(ShouldSkipQuestReward(rewardInfo, minRewardDifficulty, maxRewardDifficulty))
                continue;

            if(IsHighPriorityReward(rewardInfo))
                RemoveRewardInternal(rewardInfo);
        }
    }
    else
    {
        // Give all remaining low pri rewards
        for(i = 0; i < rewardCount; ++i)
        {
            rewardInfo = RogueQuest_GetReward(questId, i);

            if(ShouldSkipQuestReward(rewardInfo, minRewardDifficulty, maxRewardDifficulty))
                continue;

            if(!IsHighPriorityReward(rewardInfo))
            {
                // We should never be able to fail to give a high pri reward
                state = GiveRewardInternal(rewardInfo);
                AGB_ASSERT(state);
            }
        }

        // Clear pending rewards
        RogueQuest_SetStateFlag(questId, QUEST_STATE_PENDING_REWARDS, FALSE);

        questState->highestCollectedRewardDifficulty = questState->highestCompleteDifficulty;
        return TRUE;
    }

    return FALSE;
}

bool8 RogueQuest_IsRewardSequenceActive()
{
    return sRogueQuestRewardOutput != NULL;
}

void RogueQuest_BeginRewardSequence()
{
    AGB_ASSERT(sRogueQuestRewardOutput == NULL);
    sRogueQuestRewardOutput = AllocZeroed(sizeof(struct RogueQuestRewardOutput));
}

void RogueQuest_EndRewardSequence()
{
    AGB_ASSERT(sRogueQuestRewardOutput != NULL);

    if(sRogueQuestRewardOutput->buildSuppliesCount)
        Rogue_PushPopup_AddItem(ITEM_BUILDING_SUPPLIES, sRogueQuestRewardOutput->buildSuppliesCount);

    if(sRogueQuestRewardOutput->moneyCount)
        Rogue_PushPopup_AddMoney(sRogueQuestRewardOutput->moneyCount);

    if(sRogueQuestRewardOutput->failedRewardItem != ITEM_NONE)
        Rogue_PushPopup_CannotTakeItem(sRogueQuestRewardOutput->failedRewardItem, sRogueQuestRewardOutput->failedRewardCount);

    Free(sRogueQuestRewardOutput);
    sRogueQuestRewardOutput = NULL;
}

void RogueQuest_ActivateQuestsFor(u32 flags)
{
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        bool8 desiredState = RogueQuest_GetConstFlag(i, flags) && CanActivateQuest(i);

        if(RogueQuest_GetStateFlag(i, QUEST_STATE_ACTIVE) != desiredState)
        {
            RogueQuest_SetStateFlag(i, QUEST_STATE_ACTIVE, desiredState);
            // TODO - Trigger for state on activate?
        }
    }
}

static bool8 CheckRequirementCondition(u32 value, u32 conditionValue, u32 condition)
{
    switch (condition)
    {
    case QUEST_REQUIREMENT_OPERATION_EQUAL:
        return value == conditionValue;
    case QUEST_REQUIREMENT_OPERATION_NOT_EQUAL:
        return value != conditionValue;
    case QUEST_REQUIREMENT_OPERATION_GREATER_THAN:
        return value > conditionValue;
    case QUEST_REQUIREMENT_OPERATION_LESS_THAN:
        return value < conditionValue;
    case QUEST_REQUIREMENT_OPERATION_GREATER_THAN_EQUAL:
        return value >= conditionValue;
    case QUEST_REQUIREMENT_OPERATION_LESS_THAN_EQUAL:
        return value <= conditionValue;
    }

    AGB_ASSERT(FALSE);
    return FALSE;
}

static bool8 PassesRequirement(struct RogueQuestRequirement const* requirement)
{
    switch (requirement->type)
    {
    case QUEST_REQUIREMENT_TYPE_ITEM:
        return CheckRequirementCondition(
            GetItemCountInBag(requirement->perType.item.itemId), 
            requirement->perType.item.count, 
            requirement->perType.item.operation
        );

    case QUEST_REQUIREMENT_TYPE_FLAG:
        return !(FlagGet(requirement->perType.flag.flag) != requirement->perType.flag.state);

    case QUEST_REQUIREMENT_TYPE_CONFIG_TOGGLE:
        return !(Rogue_GetConfigToggle(requirement->perType.configToggle.toggle) != requirement->perType.configToggle.state);
        
    case QUEST_REQUIREMENT_TYPE_CONFIG_RANGE:
        return CheckRequirementCondition(
            Rogue_GetConfigRange(requirement->perType.configRange.range), 
            requirement->perType.configRange.value, 
            requirement->perType.configRange.operation
        );
    }

    AGB_ASSERT(FALSE);
    return FALSE;
}

void RogueQuest_CheckQuestRequirements()
{
    u16 i;
    u16 questId;

    for(questId = 0; questId < QUEST_ID_COUNT; ++questId)
    {
        if(RogueQuest_IsQuestActive(questId))
        {
            if(sQuestEntries[questId].requirements != NULL && sQuestEntries[questId].requirementCount != 0)
            {
                for(i = 0; i < sQuestEntries[questId].requirementCount; ++i)
                {
                    if(!PassesRequirement(&sQuestEntries[questId].requirements[i]))
                    {
                        FailQuest(questId);
                        break;
                    }
                }
            }
        }
    }
}

bool8 RogueQuest_IsQuestActive(u16 questId)
{
    return RogueQuest_IsQuestUnlocked(questId) && RogueQuest_GetStateFlag(questId, QUEST_STATE_ACTIVE);
}

u16 RogueQuest_GetQuestCompletePercFor(u32 constFlag)
{
    u16 i;
    u16 complete = 0;
    u16 total = 0;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_GetConstFlag(i, constFlag))
        {
            ++total;

            if(RogueQuest_GetStateFlag(i, QUEST_STATE_HAS_COMPLETE))
            {
                ++complete;
            }
        }
    }

    return (complete * 100) / total;
}

u16 RogueQuest_GetQuestCompletePercAtDifficultyFor(u32 constFlag, u8 difficultyLevel)
{
    u16 i;
    u16 complete = 0;
    u16 total = 0;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_GetConstFlag(i, constFlag))
        {
            ++total;

            if(RogueQuest_GetStateFlag(i, QUEST_STATE_HAS_COMPLETE))
            {
                if(RogueQuest_GetState(i)->highestCompleteDifficulty >= difficultyLevel)
                    ++complete;
            }
        }
    }

    return (complete * 100) / total;
}

void RogueQuest_GetQuestCountsFor(u32 constFlag, u16* activeCount, u16* inactiveCount)
{
    u16 i;
    u16 active = 0;
    u16 inactive = 0;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_GetConstFlag(i, constFlag))
        {
            if(RogueQuest_IsQuestActive(i))
            {
                active++;
            }
            else if(RogueQuest_GetStateFlag(i, QUEST_STATE_UNLOCKED))
            {
                inactive++;
            }
        }
    }

    *activeCount = active;
    *inactiveCount = inactive;
}

u16 RogueQuest_GetQuestTotalCountFor(u32 constFlag, bool8 includeLocked)
{
    u16 i;
    u16 total = 0;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_GetConstFlag(i, constFlag))
        {
            if(includeLocked || RogueQuest_GetStateFlag(i, QUEST_STATE_UNLOCKED))
                total++;
        }
    }

    return total;
}

u16 RogueQuest_GetDisplayCompletePerc()
{
    u32 constFlags = QUEST_CONST_IS_MAIN_QUEST;
    u16 maxValue = 100;

    if(RogueQuest_HasUnlockedChallenges())
        constFlags |= QUEST_CONST_IS_CHALLENGE;
    else
        maxValue = 99;

    if(RogueQuest_HasUnlockedMonMasteries())
        constFlags |= QUEST_CONST_IS_MON_MASTERY;
    else
        maxValue = 99;

    return min(maxValue, RogueQuest_GetQuestCompletePercFor(constFlags));

    //u16 questCompletion = RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_MAIN_QUEST);
//
    //if(questCompletion == 100)
    //{
    //    // Reach 100% total
    //    return questCompletion + RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_CHALLENGE) + RogueQuest_GetQuestCompletePercFor(QUEST_CONST_IS_MON_MASTERY);
    //}
//
    //return questCompletion;
}

static void EnsureUnlockedDefaultQuests()
{
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_GetConstFlag(i, QUEST_CONST_UNLOCKED_BY_DEFAULT))
            RogueQuest_TryUnlockQuest(i);
    }
}

void RogueQuest_OnNewGame()
{
    memset(gRogueSaveBlock->questStates, 0, sizeof(gRogueSaveBlock->questStates));
    EnsureUnlockedDefaultQuests();
}

void RogueQuest_OnLoadGame()
{
    EnsureUnlockedDefaultQuests();
}

static void CompleteQuest(u16 questId)
{
    u8 currentDifficulty = Rogue_GetDifficultyRewardLevel();
    struct RogueQuestState* questState = RogueQuest_GetState(questId);

    questState->highestCompleteDifficulty = currentDifficulty;
    if(!RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
    {
        questState->highestCollectedRewardDifficulty = DIFFICULTY_LEVEL_NONE;
        RogueQuest_SetStateFlag(questId, QUEST_STATE_PENDING_REWARDS, TRUE);
    }
    else if(questState->highestCollectedRewardDifficulty == DIFFICULTY_LEVEL_NONE)
    {
        // Still have rewards to collect
        RogueQuest_SetStateFlag(questId, QUEST_STATE_PENDING_REWARDS, TRUE);
    }
    else
    {
        // Only set pending rewards if we actually do have new rewards for this difficulty
        u32 i;
        u32 rewardCount = RogueQuest_GetRewardCount(questId);
        struct RogueQuestReward const* rewardInfo;

        for(i = 0; i < rewardCount; ++i)
        {
            rewardInfo = RogueQuest_GetReward(questId, i);

            if(rewardInfo->requiredDifficulty > questState->highestCollectedRewardDifficulty && rewardInfo->requiredDifficulty <= questState->highestCompleteDifficulty)
            {
                RogueQuest_SetStateFlag(questId, QUEST_STATE_PENDING_REWARDS, TRUE);
                break;
            }
        }
    }

    RogueQuest_SetStateFlag(questId, QUEST_STATE_ACTIVE, FALSE);
    RogueQuest_SetStateFlag(questId, QUEST_STATE_HAS_COMPLETE, TRUE);

    if(!IsQuestSurpressed(questId))
        Rogue_PushPopup_QuestComplete(questId);
}

void Debug_RogueQuest_CompleteQuest(u16 questId)
{
#ifdef ROGUE_DEBUG
    CompleteQuest(questId);
#endif
}

static void FailQuest(u16 questId)
{
    RogueQuest_SetStateFlag(questId, QUEST_STATE_ACTIVE, FALSE);

    if(RogueQuest_GetStateFlag(questId, QUEST_STATE_PINNED))
        Rogue_PushPopup_QuestFail(questId);
}

static void ExecuteQuestTriggers(u16 questId, u32 triggerFlag)
{
    u16 i;
    struct RogueQuestTrigger const* trigger;

    for(i = 0; i < sQuestEntries[questId].triggerCount; ++i)
    {
        trigger = &sQuestEntries[questId].triggers[i];

        AGB_ASSERT(trigger->callback != NULL);

        if(((trigger->flags & triggerFlag) != 0) && trigger->callback != NULL)
        {
            bool8 condition = trigger->callback(questId, trigger);
            u8 status = condition != FALSE ? trigger->passState : trigger->failState;

            switch (status)
            {
            case QUEST_STATUS_PENDING:
                // Do nothing
                break;

            case QUEST_STATUS_SUCCESS:
                CompleteQuest(questId);
                return;
                break;

            case QUEST_STATUS_FAIL:
                FailQuest(questId);
                return;
                break;

            case QUEST_STATUS_BREAK:
                // Don't execute any more callbacks for this quest here
                return;
                break;
            
            default:
                AGB_ASSERT(FALSE);
                break;
            }
        }
    }
}

void RogueQuest_OnTrigger(u32 triggerFlag)
{
    u16 i;

    // Don't track any quests during victory lap
    if(Rogue_IsVictoryLapActive())
        return;

    // Execute quest callback for any active quests which are listening for this trigger
    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if((sQuestEntries[i].triggerFlags & triggerFlag) != 0)
        {
            if(RogueQuest_GetStateFlag(i, QUEST_STATE_ACTIVE))
                ExecuteQuestTriggers(i, triggerFlag);
        }
    }
}

bool8 RogueQuest_HasUnlockedChallenges()
{
    return FlagGet(FLAG_SYS_CHALLENGES_UNLOCKED);
}

bool8 RogueQuest_HasUnlockedMonMasteries()
{
    return FlagGet(FLAG_SYS_MASTERIES_UNLOCKED);
}

extern const u16 gRogueBake_FinalEvoSpecies[];
extern const u16 gRogueBake_EggSpecies[];

static u32 GetMonMasteryIndex(u16 species)
{
    u32 i;
    u16 eggSpecies;

#ifdef ROGUE_EXPANSION
    if(species == SPECIES_DARMANITAN_GALARIAN_ZEN_MODE)
        species = SPECIES_DARMANITAN_GALARIAN;

    if(!gRogueSpeciesInfo[species].isAlolanForm && !gRogueSpeciesInfo[species].isGalarianForm && !gRogueSpeciesInfo[species].isHisuianForm && !gRogueSpeciesInfo[species].isPaldeanForm)
    {
        species = GET_BASE_SPECIES_ID(species);
    }

    if(gRogueSpeciesInfo[species].baseHP == 0)
        return SPECIES_EGG;
#else
    if(species >= SPECIES_OLD_UNOWN_B && species <= SPECIES_OLD_UNOWN_Z)
        return SPECIES_EGG;
#endif

    eggSpecies = Rogue_GetEggSpecies(species);

    for(i = 0; i < SPECIES_EGG_EVO_STAGE_COUNT; ++i)
    {
        if(gRogueBake_EggSpecies[i] == eggSpecies)
            return i;
    }

#ifdef ROGUE_EXPANSION
    // Failed to get this species so try grab the base egg species
    species = eggSpecies;
    eggSpecies = GET_BASE_SPECIES_ID(eggSpecies);

    if(eggSpecies != species)
    {
        for(i = 0; i < SPECIES_EGG_EVO_STAGE_COUNT; ++i)
        {
            if(gRogueBake_EggSpecies[i] == eggSpecies)
                return i;
        }
    }

#endif

    // TODO - Need to decide what to do here
    AGB_ASSERT(FALSE);
    return SPECIES_EGG;
}

bool8 RogueQuest_GetMonMasteryFlag(u16 species)
{
    u32 idx = GetMonMasteryIndex(species);
    if(idx != SPECIES_EGG)
    {
        u32 offset = idx / 8;
        u8 bit = idx % 8;
        u8 bitMask = 1 << bit;

        AGB_ASSERT(offset < ARRAY_COUNT(gRogueSaveBlock->monMasteryFlags));
        return (gRogueSaveBlock->monMasteryFlags[offset] & bitMask) != 0;
    }

    return FALSE;
}

void RogueQuest_SetMonMasteryFlag(u16 species)
{
    u32 idx = GetMonMasteryIndex(species);
    if(idx != SPECIES_EGG)
    {
        u32 offset = idx / 8;
        u8 bit = idx % 8;
        u8 bitMask = 1 << bit;

        AGB_ASSERT(offset < ARRAY_COUNT(gRogueSaveBlock->monMasteryFlags));

        gRogueSaveBlock->monMasteryFlags[offset] |= bitMask;
    }
}

void RogueQuest_SetMonMasteryFlagFromParty()
{
    u32 i;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        if(species != SPECIES_NONE)
            RogueQuest_SetMonMasteryFlag(species);
    }
}

u32 RogueQuest_GetMonMasteryTotalPerc()
{
    u32 i;
    u32 complete = 0;

    for(i = 0; i < MON_MASTERY_TOTAL_COUNT; ++i)
    {
        u32 offset = i / 8;
        u8 bit = i % 8;
        u8 bitMask = 1 << bit;

        if((gRogueSaveBlock->monMasteryFlags[offset] & bitMask) != 0)
            ++complete;
    }

    return (complete * 100) / MON_MASTERY_TOTAL_COUNT;
}

void RogueDebug_ClearMonMasteries()
{
#ifdef ROGUE_DEBUG
    memset(gRogueSaveBlock->monMasteryFlags, 0, sizeof(gRogueSaveBlock->monMasteryFlags));
#endif
}

void RogueDebug_FillMonMasteries()
{
#ifdef ROGUE_DEBUG
    memset(gRogueSaveBlock->monMasteryFlags, 255, sizeof(gRogueSaveBlock->monMasteryFlags));
#endif
}

// QuestCondition
//

#define ASSERT_PARAM_COUNT(count) AGB_ASSERT(trigger->paramCount == count)

static bool8 QuestCondition_Always(u16 questId, struct RogueQuestTrigger const* trigger)
{
    return TRUE;
}

static bool8 QuestCondition_DifficultyGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 threshold = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return Rogue_GetCurrentDifficulty() > threshold;
}

static bool8 QuestCondition_DifficultyLessThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 threshold = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return Rogue_GetCurrentDifficulty() < threshold;
}

static bool8 QuestCondition_IsStandardRunActive(u16 questId, struct RogueQuestTrigger const* trigger)
{
    // TODO
    return TRUE;
}

static bool8 UNUSED QuestCondition_HasCompletedQuestAND(u16 triggerQuestId, struct RogueQuestTrigger const* trigger)
{
    u16 i, questId;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        questId = trigger->params[i];
        if(!RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
            return FALSE;
    }

    return TRUE;
}

static bool8 QuestCondition_HasCompletedQuestOR(u16 triggerQuestId, struct RogueQuestTrigger const* trigger)
{
    u16 i, questId;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        questId = trigger->params[i];
        if(RogueQuest_GetStateFlag(questId, QUEST_STATE_HAS_COMPLETE))
            return TRUE;
    }

    return FALSE;
}

static bool8 UNUSED QuestCondition_PartyContainsType(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u8 i;
    u16 species, targetType;

    ASSERT_PARAM_COUNT(1);
    targetType = trigger->params[0];

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(RoguePokedex_GetSpeciesType(species, 0) == targetType || RoguePokedex_GetSpeciesType(species, 1) == targetType)
        {
            return TRUE;
        }
    }

    return FALSE;
}

static bool8 QuestCondition_PartyOnlyContainsType(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u8 i;
    u16 species, targetType;

    ASSERT_PARAM_COUNT(1);
    targetType = trigger->params[0];

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(RoguePokedex_GetSpeciesType(species, 0) != targetType && RoguePokedex_GetSpeciesType(species, 1) != targetType)
        {
            return FALSE;
        }
    }

    return TRUE;
}

static bool8 QuestCondition_PartyContainsLegendary(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u8 i;
    u16 species;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(RoguePokedex_IsSpeciesLegendary(species))
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_PartyContainsOnlyLegendaries(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u8 i;
    u16 species;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(!RoguePokedex_IsSpeciesLegendary(species))
            return FALSE;
    }

    return TRUE;
}

static bool8 QuestCondition_PartyContainsOnlyShinys(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u8 i;
    u16 species;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        if(!IsMonShiny(&gPlayerParty[i]))
            return FALSE;
    }

    return TRUE;
}

static bool8 IsStarterSpecies(u16 species)
{
    species = Rogue_GetEggSpecies(species);

    switch (species)
    {

    case SPECIES_PICHU:
    case SPECIES_EEVEE:

    case SPECIES_BULBASAUR:
    case SPECIES_SQUIRTLE:
    case SPECIES_CHARMANDER:

    case SPECIES_CHIKORITA:
    case SPECIES_TOTODILE:
    case SPECIES_CYNDAQUIL:

    case SPECIES_TREECKO:
    case SPECIES_MUDKIP:
    case SPECIES_TORCHIC:

#ifdef ROGUE_EXPANSION
    case SPECIES_TURTWIG:
    case SPECIES_PIPLUP:
    case SPECIES_CHIMCHAR:

    case SPECIES_SNIVY:
    case SPECIES_OSHAWOTT:
    case SPECIES_TEPIG:

    case SPECIES_CHESPIN:
    case SPECIES_FROAKIE:
    case SPECIES_FENNEKIN:

    case SPECIES_ROWLET:
    case SPECIES_LITTEN:
    case SPECIES_POPPLIO:

    case SPECIES_GROOKEY:
    case SPECIES_SCORBUNNY:
    case SPECIES_SOBBLE:

    case SPECIES_SPRIGATITO:
    case SPECIES_FUECOCO:
    case SPECIES_QUAXLY:
#endif
        return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_PartyContainsOnlyStarters(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u8 i;
    u16 species;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(!IsStarterSpecies(species))
            return FALSE;
    }

    return TRUE;
}

static bool8 QuestCondition_PartyContainsOnlyUniqueTypes(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u8 i;
    u32 partyTypeFlags = 0;
    u32 checkTypeFlags;
    u16 species;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        checkTypeFlags = 0;
        Rogue_AppendSpeciesTypeFlags(species, &checkTypeFlags);

        if(partyTypeFlags & checkTypeFlags)
            return FALSE;

        partyTypeFlags |= checkTypeFlags;
    }

    return TRUE;
}

static bool8 QuestCondition_PartyContainsInitialPartner(u16 questId, struct RogueQuestTrigger const* trigger)
{
    return Rogue_IsPartnerMonInTeam();
}

static bool8 QuestCondition_PartyContainsSpecies(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;
    u16 species;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        species = trigger->params[i];

        if(PartyContainsBaseSpecies(gPlayerParty, gPlayerPartyCount, species))
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_PartyContainsAllSpecies(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;
    u16 species;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        species = trigger->params[i];

        if(!PartyContainsBaseSpecies(gPlayerParty, gPlayerPartyCount, species))
            return FALSE;
    }

    return TRUE;
}

static bool8 QuestCondition_PartyMaxBSTLessThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;
    u16 species;
    u16 bstValue = trigger->params[0];
    ASSERT_PARAM_COUNT(1);

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(RoguePokedex_GetSpeciesBST(species) >= bstValue)
            return FALSE;
    }

    return TRUE;
}


static bool8 CheckSingleTrainerConfigValid(u32 toggleToCheck, u32 currentToggle)
{
    if(toggleToCheck == currentToggle)
        return Rogue_GetConfigToggle(currentToggle) == TRUE;
    else
        return Rogue_GetConfigToggle(currentToggle) == FALSE;
}

bool8 CheckOnlyTheseTrainersEnabled(u32 toggleToCheck)
{
    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_ROGUE))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_KANTO))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_JOHTO))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_HOENN))
        return FALSE;

#ifdef ROGUE_EXPANSION
    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_SINNOH))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_UNOVA))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_KALOS))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_ALOLA))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_GALAR))
        return FALSE;

    if(!CheckSingleTrainerConfigValid(toggleToCheck, CONFIG_TOGGLE_TRAINER_PALDEA))
        return FALSE;
#endif
    return TRUE;
}

static bool8 QuestCondition_AreOnlyTheseTrainersActive(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 trainerConfigToggle = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return CheckOnlyTheseTrainersEnabled(trainerConfigToggle);
}

static bool8 QuestCondition_IsPokedexRegion(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 region = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return RoguePokedex_GetDexRegion() == region;
}

static bool8 UNUSED QuestCondition_IsPokedexVariant(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 variant = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return RoguePokedex_GetDexVariant() == variant;
}

static bool8 QuestCondition_CurrentlyInMap(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 mapId = trigger->params[0];
    u16 mapGroup = (mapId >> 8); // equiv to MAP_GROUP
    u16 mapNum = (mapId & 0xFF); // equiv to MAP_NUM

    ASSERT_PARAM_COUNT(1);
    return gSaveBlock1Ptr->location.mapNum == mapNum || gSaveBlock1Ptr->location.mapGroup == mapGroup;
}

static bool8 QuestCondition_CurrentlyInRoomType(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 roomtType = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return gRogueAdvPath.currentRoomType == roomtType;
}

static bool8 QuestCondition_CanUnlockFinalQuest(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        // Check all other main quests except these 2 have been completed
        if(i == QUEST_ID_ONE_LAST_QUEST || i == QUEST_ID_THE_FINAL_RUN)
            continue;

        if(RogueQuest_GetConstFlag(i, QUEST_CONST_IS_MAIN_QUEST))
        {
            if(!(RogueQuest_IsQuestUnlocked(i) && RogueQuest_HasCollectedRewards(i)))
                return FALSE;
        }
    }

    return TRUE;
}

static bool8 QuestCondition_HasBuiltAllAreas(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(!RogueHub_HasAreaBuilt(i))
            return FALSE;
    }

    return TRUE;
}

static bool8 QuestCondition_IsFinalQuestConditionMet(u16 questId, struct RogueQuestTrigger const* trigger)
{
    return Rogue_UseFinalQuestEffects();
}

static bool8 QuestCondition_PokedexEntryCountGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 count = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return RoguePokedex_CountNationalCaughtMons(FLAG_GET_CAUGHT) > count;
}

static bool8 QuestCondition_PokedexShinyEntryCountGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 count = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return RoguePokedex_CountNationalCaughtMons(FLAG_GET_CAUGHT_SHINY) > count;
}

static bool8 QuestCondition_InAdventureEncounterType(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;
    u16 encounterType;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        encounterType = trigger->params[i];

        if(gRogueAdvPath.currentRoomType == encounterType)
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_TotalMoneySpentGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 count = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return Rogue_GetTotalSpentOnActiveMap() > count;
}

static bool8 QuestCondition_PlayerMoneyGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 count = trigger->params[0];
    ASSERT_PARAM_COUNT(1);
    return GetMoney(&gSaveBlock1Ptr->money) > count;
}

static bool8 QuestCondition_RandomanWasUsed(u16 questId, struct RogueQuestTrigger const* trigger)
{
    ASSERT_PARAM_COUNT(0);
    return FlagGet(FLAG_ROGUE_RANDOM_TRADE_WAS_ACTIVE) && FlagGet(FLAG_ROGUE_RANDOM_TRADE_DISABLED);
}

static bool8 QuestCondition_RandomanWasActive(u16 questId, struct RogueQuestTrigger const* trigger)
{
    ASSERT_PARAM_COUNT(0);
    return !!FlagGet(FLAG_ROGUE_RANDOM_TRADE_WAS_ACTIVE);
}

static bool8 QuestCondition_LastRandomanWasFullParty(u16 questId, struct RogueQuestTrigger const* trigger)
{
    ASSERT_PARAM_COUNT(0);
    return !!FlagGet(FLAG_ROGUE_RANDOM_TRADE_WAS_FULL_PARTY);
}

static bool8 QuestCondition_LastItemWasAny(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        if(trigger->params[i] == gSpecialVar_ItemId)
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_BagContainsItemsOR(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        if(CheckBagHasItem(trigger->params[i], 1))
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_FlagGet(u16 questId, struct RogueQuestTrigger const* trigger)
{
    ASSERT_PARAM_COUNT(1);
    return FlagGet(trigger->params[0]);
}

static bool8 QuestCondition_VarGetEqual(u16 questId, struct RogueQuestTrigger const* trigger)
{
    ASSERT_PARAM_COUNT(2);
    return VarGet(trigger->params[0]) == trigger->params[1];
}

static bool8 QuestCondition_VarGetLessThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    ASSERT_PARAM_COUNT(2);
    return VarGet(trigger->params[0]) < trigger->params[1];
}

static bool8 QuestCondition_VarGetGreaterThan(u16 questId, struct RogueQuestTrigger const* trigger)
{
    ASSERT_PARAM_COUNT(2);
    return VarGet(trigger->params[0]) > trigger->params[1];
}

static bool8 QuestCondition_RunTimerLessThanMins(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u32 totalMinutes = gSaveBlock2Ptr->playTimeHours * 60 + gSaveBlock2Ptr->playTimeMinutes;
    ASSERT_PARAM_COUNT(1);
    return totalMinutes < trigger->params[0];
}

static bool8 QuestCondition_IsConfigRangeEqualToAny(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;
    u16 configRange = trigger->params[0];

    AGB_ASSERT(trigger->paramCount > 1);

    for(i = 1; i < trigger->paramCount; ++i)
    {
        if(Rogue_GetConfigRange(configRange) == trigger->params[i])
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_SpeciesMasteryComplete(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;
    u16 species;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        species = trigger->params[i];

        if(RogueQuest_GetMonMasteryFlag(species))
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestCondition_CurrentEvilTeamIs(u16 questId, struct RogueQuestTrigger const* trigger)
{
    u16 i;

    for(i = 0; i < trigger->paramCount; ++i)
    {
        if(gRogueRun.teamEncounterNum ==  trigger->params[i])
            return TRUE;
    }

    return FALSE;
}