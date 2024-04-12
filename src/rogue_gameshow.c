#include "global.h"
#include "constants/item.h"
#include "easy_chat.h"
#include "event_data.h"
#include "item.h"
#include "random.h"
#include "string_util.h"

#include "rogue_gameshow.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"

#define VAR_CURRENT_ROUND           VAR_TEMP_0
#define VAR_CURRENT_REWARD_COUNTER  VAR_TEMP_2
#define VAR_CURRENT_REWARD_ITEM     VAR_TEMP_3
#define VAR_CURRENT_REWARD_COUNT    VAR_TEMP_4

// Specials
//
void GameShow_RestartGameShow()
{
    struct RogueGameShow* gameShow = Rogue_GetGameShow();
    memset(gameShow, 0, sizeof(*gameShow));

    VarSet(VAR_CURRENT_ROUND, 1);

    // Reset default answer
    gSaveBlock1Ptr->dewfordTrends[2].words[0] = 0;
}

void GameShow_SelectRandomSpecies()
{
    struct RogueGameShow* gameShow = Rogue_GetGameShow();

    RogueMonQuery_Begin();
    RogueMonQuery_IsSpeciesActive();
    RogueMonQuery_IsBaseSpeciesInCurrentDex(QUERY_FUNC_INCLUDE);
    RogueMonQuery_IsSeenInPokedex(QUERY_FUNC_INCLUDE);

    gameShow->recentSpecies = RogueMiscQuery_SelectRandomElement(Random());

    RogueMonQuery_End();
}

u16 GameShow_BufferSpecies()
{
    struct RogueGameShow* gameShow = Rogue_GetGameShow();
    return gameShow->recentSpecies;
}

void GameShow_CheckResultMatchesSpecies()
{
    struct RogueGameShow* gameShow = Rogue_GetGameShow();
    gSpecialVar_Result = gSpecialVar_Result == gameShow->recentSpecies;
}

void GameShow_SelectRewardItem()
{
    u8 i;
    u16 itemId;
    u16 amount;
    u32 targetPrice = 2000 + VarGet(VAR_CURRENT_REWARD_COUNTER) * 2000;
    RAND_TYPE startSeed = gRngRogueValue;

    RogueItemQuery_Begin();
    RogueItemQuery_Reset(QUERY_FUNC_INCLUDE);

    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_BERRIES);
    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_POKEBLOCK);
    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_KEY_ITEMS);

#if !defined(ROGUE_EXPANSION)
    RogueMiscQuery_EditRange(QUERY_FUNC_EXCLUDE, ITEM_HM01, ITEM_HM08);
#endif

    RogueItemQuery_InPriceRange(QUERY_FUNC_INCLUDE, targetPrice / 5, targetPrice);

    // Cycle RNG
    for(i = 0; i < VarGet(VAR_CURRENT_REWARD_COUNTER);++i)
        RogueRandom();

    itemId = RogueMiscQuery_SelectRandomElement(RogueRandom());

    RogueItemQuery_End();

    if(itemId == ITEM_MASTER_BALL || (itemId >= ITEM_TM01 && itemId <= ITEM_HM08) || (ItemId_GetPocket(itemId) == POCKET_STONES))
    {
        amount = 1;
    }
    else
    {
        amount = targetPrice / ItemId_GetPrice(itemId);
    }

    VarSet(VAR_CURRENT_REWARD_ITEM, itemId);
    VarSet(VAR_CURRENT_REWARD_COUNT, amount);
    gRngRogueValue = startSeed;
}
