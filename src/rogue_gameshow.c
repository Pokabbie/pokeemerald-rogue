#include "global.h"
#include "constants/item.h"
#include "easy_chat.h"
#include "event_data.h"
#include "item.h"
#include "random.h"
#include "script_menu.h"
#include "string_util.h"

#include "rogue_baked.h"
#include "rogue_gameshow.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"

#define VAR_CURRENT_ROUND           VAR_TEMP_0
#define VAR_CURRENT_REWARD_COUNTER  VAR_TEMP_2
#define VAR_CURRENT_REWARD_ITEM     VAR_TEMP_3
#define VAR_CURRENT_REWARD_COUNT    VAR_TEMP_4

#define VAR_TRIVIA_ANSWER           VAR_TEMP_A

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

static void BeginGameshowSpeciesQuery()
{
    RogueMonQuery_Begin();
    RogueMonQuery_IsSpeciesActive();
    RogueMonQuery_IsBaseSpeciesInCurrentDex(QUERY_FUNC_INCLUDE);
    //RogueMonQuery_IsSeenInPokedex(QUERY_FUNC_INCLUDE);
}

void GameShow_SelectRandomSpecies()
{
    struct RogueGameShow* gameShow = Rogue_GetGameShow();

    BeginGameshowSpeciesQuery();
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

void GameShow_AppendSpeciesMultichoiceOptions()
{
    u8 i;
    struct RogueGameShow* gameShow = Rogue_GetGameShow();
    u16 currentRound = VarGet(VAR_CURRENT_ROUND);
    u8 optionCount = 3 + currentRound;
    u8 correctAnswerIndex = Random() % optionCount;

    BeginGameshowSpeciesQuery();

    RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, gameShow->recentSpecies);

    for(i = 0; i < optionCount; ++i)
    {
        if(!RogueMiscQuery_AnyActiveElements())
            break;

        if(i == correctAnswerIndex)
        {
            ScriptMenu_ScrollingMultichoiceDynamicAppendOption(RoguePokedex_GetSpeciesName(gameShow->recentSpecies), TRUE);
        }
        else
        {
            u16 species = RogueMiscQuery_SelectRandomElement(Random());
            ScriptMenu_ScrollingMultichoiceDynamicAppendOption(RoguePokedex_GetSpeciesName(species), FALSE);
            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, species);

        }
    }

    if(i < correctAnswerIndex)
    {
        // Ran out of mons, so put it on the end
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(RoguePokedex_GetSpeciesName(gameShow->recentSpecies), TRUE);
    }

    RogueMonQuery_End();
}

void GameShow_SelectRewardItem()
{
    u8 i;
    u16 itemId;
    u16 amount;
    u32 targetPrice = 2000 + VarGet(VAR_CURRENT_REWARD_COUNTER) * 2000;
    RAND_TYPE startSeed = gRngRogueValue;

    RogueItemQuery_Begin();
    RogueItemQuery_IsItemActive();

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

void GameShow_IsTeamTriviaRoundValid()
{
    // We should only do this if we have enough questions
    gSpecialVar_Result = FALSE; // TODO - (gPlayerPartyCount >= 4);
}

enum
{
    TRIVIA_QUESTION_HIGHEST_HP,
    TRIVIA_QUESTION_HIGHEST_ATK,
    TRIVIA_QUESTION_HIGHEST_DEF,
    TRIVIA_QUESTION_HIGHEST_SPEED,
    TRIVIA_QUESTION_HIGHEST_SPATK,
    TRIVIA_QUESTION_HIGHEST_SPDEF,
};

static void BufferTriviaQuestion(u8 question)
{
    u8* const stringBuffers[] = 
    {
        gStringVar1,
        gStringVar2,
        gStringVar3,
    };

    switch (question)
    {
    case TRIVIA_QUESTION_HIGHEST_HP:
    case TRIVIA_QUESTION_HIGHEST_ATK:
    case TRIVIA_QUESTION_HIGHEST_DEF:
    case TRIVIA_QUESTION_HIGHEST_SPEED:
    case TRIVIA_QUESTION_HIGHEST_SPATK:
    case TRIVIA_QUESTION_HIGHEST_SPDEF:
        {
            u8 i;
            u8 stats[NUM_STATS];
            u8 statNum = (question - TRIVIA_QUESTION_HIGHEST_HP);

            u8 highestStat = 0;
            u8 slot = 0;

            for(i = 0; i < gPlayerPartyCount; ++i)
            {
                u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
                RoguePokedex_GetSpeciesStatArray(species, stats, NUM_STATS);

                if(stats[statNum] > highestStat)
                {
                    highestStat = stats[statNum];
                    slot = i;
                }
            }

            // Buffer answer
            {
                u8 answerOpt = RogueRandom() % 3;
                VarSet(VAR_TRIVIA_ANSWER, answerOpt);
                
                StringCopy_Nickname(stringBuffers[answerOpt], RoguePokedex_GetSpeciesName(GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES)));

                for(i = 0; i < 3; ++i)
                {
                    if(i != answerOpt)
                    {
                        u8 rand = slot;
                        while(rand == slot)
                        {
                            rand = RogueRandom() % gPlayerPartyCount;
                            StringCopy_Nickname(stringBuffers[i], RoguePokedex_GetSpeciesName(GetMonData(&gPlayerParty[rand], MON_DATA_SPECIES)));
                        }
                    }
                }
            }
        }
        break;
    
    default:
        AGB_ASSERT(FALSE);
        break;
    }
}

void GameShow_NextTriviaQuestion()
{
    u8 i;
    RAND_TYPE startSeed = gRngRogueValue;

    for(i = 0; i < VarGet(VAR_CURRENT_ROUND);++i)
        RogueRandom();

    switch (VarGet(VAR_CURRENT_ROUND))
    {
    case 1:
        {
            const u8 questionOptions[] = 
            {
                TRIVIA_QUESTION_HIGHEST_HP,
                TRIVIA_QUESTION_HIGHEST_ATK,
                TRIVIA_QUESTION_HIGHEST_DEF,
                TRIVIA_QUESTION_HIGHEST_SPEED,
                TRIVIA_QUESTION_HIGHEST_SPATK,
                TRIVIA_QUESTION_HIGHEST_SPDEF,
            };
            BufferTriviaQuestion(questionOptions[RogueRandom() % ARRAY_COUNT(questionOptions)]);
        }
        break;

    case 2:
        {
            const u8 questionOptions[] = 
            {
                TRIVIA_QUESTION_HIGHEST_HP,
                TRIVIA_QUESTION_HIGHEST_ATK,
                TRIVIA_QUESTION_HIGHEST_DEF,
                TRIVIA_QUESTION_HIGHEST_SPEED,
                TRIVIA_QUESTION_HIGHEST_SPATK,
                TRIVIA_QUESTION_HIGHEST_SPDEF,
            };
            BufferTriviaQuestion(questionOptions[RogueRandom() % ARRAY_COUNT(questionOptions)]);
        }
        break;

    case 3:
        {
            const u8 questionOptions[] = 
            {
                TRIVIA_QUESTION_HIGHEST_HP,
                TRIVIA_QUESTION_HIGHEST_ATK,
                TRIVIA_QUESTION_HIGHEST_DEF,
                TRIVIA_QUESTION_HIGHEST_SPEED,
                TRIVIA_QUESTION_HIGHEST_SPATK,
                TRIVIA_QUESTION_HIGHEST_SPDEF,
            };
            BufferTriviaQuestion(questionOptions[RogueRandom() % ARRAY_COUNT(questionOptions)]);
        }
        break;

    case 4:
        {
            const u8 questionOptions[] = 
            {
                TRIVIA_QUESTION_HIGHEST_HP,
                TRIVIA_QUESTION_HIGHEST_ATK,
                TRIVIA_QUESTION_HIGHEST_DEF,
                TRIVIA_QUESTION_HIGHEST_SPEED,
                TRIVIA_QUESTION_HIGHEST_SPATK,
                TRIVIA_QUESTION_HIGHEST_SPDEF,
            };
            BufferTriviaQuestion(questionOptions[RogueRandom() % ARRAY_COUNT(questionOptions)]);
        }
        break;

    case 5:
        {
            const u8 questionOptions[] = 
            {
                TRIVIA_QUESTION_HIGHEST_HP,
                TRIVIA_QUESTION_HIGHEST_ATK,
                TRIVIA_QUESTION_HIGHEST_DEF,
                TRIVIA_QUESTION_HIGHEST_SPEED,
                TRIVIA_QUESTION_HIGHEST_SPATK,
                TRIVIA_QUESTION_HIGHEST_SPDEF,
            };
            BufferTriviaQuestion(questionOptions[RogueRandom() % ARRAY_COUNT(questionOptions)]);
        }
        break;

    case 6:
        {
            const u8 questionOptions[] = 
            {
                TRIVIA_QUESTION_HIGHEST_HP,
                TRIVIA_QUESTION_HIGHEST_ATK,
                TRIVIA_QUESTION_HIGHEST_DEF,
                TRIVIA_QUESTION_HIGHEST_SPEED,
                TRIVIA_QUESTION_HIGHEST_SPATK,
                TRIVIA_QUESTION_HIGHEST_SPDEF,
            };
            BufferTriviaQuestion(questionOptions[RogueRandom() % ARRAY_COUNT(questionOptions)]);
        }
        break;
    
    default:
        AGB_ASSERT(FALSE);
        break;
    }

    gRngRogueValue = startSeed;
}