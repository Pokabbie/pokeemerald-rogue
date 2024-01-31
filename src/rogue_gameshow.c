#include "global.h"
#include "easy_chat.h"
#include "event_data.h"
#include "random.h"
#include "string_util.h"

#include "rogue_gameshow.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"

#define VAR_CURRENT_ROUND VAR_TEMP_0

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
    CopyEasyChatWord(gStringVar4, gSaveBlock1Ptr->dewfordTrends[2].words[0]);

    gSpecialVar_Result = StringCompareN(RoguePokedex_GetSpeciesName(gameShow->recentSpecies), gStringVar4, POKEMON_NAME_LENGTH) == 0;
}