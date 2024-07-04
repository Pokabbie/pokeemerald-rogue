#include "global.h"
#include "constants/battle.h"
#include "constants/battle_frontier.h"
#include "constants/items.h"
#include "constants/rogue.h"
#include "constants/script_menu.h"

#include "battle_main.h"
#include "battle_message.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "item_menu.h"
#include "main.h"
#include "money.h"
#include "overworld.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "script.h"
#include "script_menu.h"
#include "shop.h"
#include "sound.h"
#include "string_util.h"
#include "strings.h"

#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_assistant.h"
#include "rogue_baked.h"
#include "rogue_campaign.h"
#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_followmon.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_ridemon.h"
#include "rogue_safari.h"
#include "rogue_script.h"
#include "rogue_timeofday.h"
#include "rogue_trainers.h"
#include "rogue_multiplayer.h"
#include "rogue_player_customisation.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_questmenu.h"
#include "rogue_settings.h"

void DoSpecialTrainerBattle(void);

static const u8 sTypeNames[NUMBER_OF_MON_TYPES][10] = // alt version of gTypeNames
{
    [TYPE_NORMAL] = _("Normal"),
    [TYPE_FIGHTING] = _("Fighting"),
    [TYPE_FLYING] = _("Flying"),
    [TYPE_POISON] = _("Poison"),
    [TYPE_GROUND] = _("Ground"),
    [TYPE_ROCK] = _("Rock"),
    [TYPE_BUG] = _("Bug"),
    [TYPE_GHOST] = _("Ghost"),
    [TYPE_STEEL] = _("Steel"),
    [TYPE_MYSTERY] = _("???"),
    [TYPE_FIRE] = _("Fire"),
    [TYPE_WATER] = _("Water"),
    [TYPE_GRASS] = _("Grass"),
    [TYPE_ELECTRIC] = _("Electric"),
    [TYPE_PSYCHIC] = _("Psychic"),
    [TYPE_ICE] = _("Ice"),
    [TYPE_DRAGON] = _("Dragon"),
    [TYPE_DARK] = _("Dark"),
#ifdef ROGUE_EXPANSION
    [TYPE_FAIRY] = _("Fairy"),
#endif
};

static const u8 sStatNamesTable[NUM_STATS][13] = // a;t versopm pf gStatNamesTable
{
    [STAT_HP]      = _("HP"),
    [STAT_ATK]     = _("Attack"),
    [STAT_DEF]     = _("Defence"),
    [STAT_SPEED]   = _("Speed"),
    [STAT_SPATK]   = _("Sp. Attack"),
    [STAT_SPDEF]   = _("Sp. Defence"),
};

static u8 const sText_The[] = _(" the ");
static u8 const sText_TheShiny[] = _(" the shiny ");

bool8 Rogue_CheckPartyHasRoomForMon(void)
{
    if(Rogue_IsRunActive())
    {
        u8 partySize = Rogue_GetMaxPartySize();

        // We don't actually want to shift around the party as that can cause issue in EX
        // Where stuff like megas keep track of exact indicies which mega evolve
        //RemoveAnyFaintedMons(FALSE);

        if (CalculatePlayerPartyCount() >= partySize)
        {
            return FALSE;
        }
    }

    return TRUE;
}

void Rogue_SeedRandomGenerators(void)
{
    SeedRng(gMain.vblankCounter1);
    SeedRng2(gMain.vblankCounter2);
}

u16 GetStartDifficulty(void);

static u8 Calc_RandomTradeLevel(struct Pokemon* mon)
{
    if(gRogueRun.enteredRoomCounter == 0)
    {
        u16 startDifficulty = GetStartDifficulty();

        if(startDifficulty == 0)
            return 7;
        else
            return 5 + GetStartDifficulty() * 10;
    }
    else
        return GetMonData(mon, MON_DATA_LEVEL);
}

void Rogue_RandomisePartyMon(void)
{
    u16 species;
    u32 temp;
    u8 monIdx = gSpecialVar_0x8004;
    u8 targetlevel = Calc_RandomTradeLevel(&gPlayerParty[0]);

    RogueMonQuery_Begin();
    RogueMonQuery_IsSpeciesActive();

    if(Rogue_GetCurrentDifficulty() < ROGUE_GYM_MID_DIFFICULTY)
        RogueMonQuery_IsLegendary(QUERY_FUNC_EXCLUDE);

    RogueMonQuery_TransformIntoEggSpecies();
    RogueMonQuery_TransformIntoEvos(targetlevel, TRUE, TRUE);

    // Remove random entries until we can safely calcualte weights without going over
    while(RogueWeightQuery_IsOverSafeCapacity())
    {
        RogueMiscQuery_FilterByChance(Random(), QUERY_FUNC_INCLUDE, 50, PARTY_SIZE);
    }
    
    if(IsCurseActive(EFFECT_WILD_EGG_SPECIES))
        RogueMonQuery_TransformIntoEggSpecies();

    RogueWeightQuery_Begin();
    {
        // we can have dupes but just not as common
        RogueWeightQuery_FillWeights(20);

        if(monIdx == 255)
        {
            // Entire team
            u8 i;

            IncrementGameStat(GAME_STAT_RANDO_TRADE_PARTY);

            for(i = 0; i < gPlayerPartyCount; ++i)
            {
                IncrementGameStat(GAME_STAT_RANDO_TRADE_TOTAL_PKMN);

                targetlevel = Calc_RandomTradeLevel(&gPlayerParty[i]);
                temp = GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM);

                species = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(Random(), 1);

                ZeroMonData(&gPlayerParty[i]);
                CreateMon(&gPlayerParty[i], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

                SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &temp);

                temp = ITEM_SAFARI_BALL;
                SetMonData(&gPlayerParty[i], MON_DATA_POKEBALL, &temp);

                GetSetPokedexSpeciesFlag(species, IsMonShiny(&gPlayerParty[i]) ? FLAG_SET_CAUGHT_SHINY : FLAG_SET_CAUGHT);
            }
        }
        else
        {
            // Single mon in team

            IncrementGameStat(GAME_STAT_RANDO_TRADE_SINGLE);
            IncrementGameStat(GAME_STAT_RANDO_TRADE_TOTAL_PKMN);

            targetlevel = Calc_RandomTradeLevel(&gPlayerParty[monIdx]);
            temp = GetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM);

            species = RogueWeightQuery_SelectRandomFromWeightsWithUpdate(Random(), 1);

            ZeroMonData(&gPlayerParty[monIdx]);
            CreateMon(&gPlayerParty[monIdx], species, targetlevel, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);

            SetMonData(&gPlayerParty[monIdx], MON_DATA_HELD_ITEM, &temp);

            temp = ITEM_SAFARI_BALL;
            SetMonData(&gPlayerParty[monIdx], MON_DATA_POKEBALL, &temp);

            GetSetPokedexSpeciesFlag(species, IsMonShiny(&gPlayerParty[monIdx]) ? FLAG_SET_CAUGHT_SHINY : FLAG_SET_CAUGHT);
        }
    }
    RogueWeightQuery_End();

    RogueMonQuery_End();
}

void Rogue_AlterMonIVs(void)
{
    const u16 delta = 10;

    u16 statId;
    u16 ivAmount;
    u16 monIdx = gSpecialVar_0x8004;
    u16 statOp = gSpecialVar_0x8005;

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
            {
                ivAmount = GetMonData(&gPlayerParty[i], statId);

                if(statOp == 0)
                {
                    ivAmount += delta;
                    ivAmount = min(31, ivAmount);
                }
                else
                {
                    if(ivAmount < delta)
                        ivAmount = 0;
                    else
                        ivAmount -= delta;
                }

                SetMonData(&gPlayerParty[i], statId, &ivAmount);
                CalculateMonStats(&gPlayerParty[i]);
            }
        }
    }
    else
    {
        // Modify just 1 mon
        for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
        {
            ivAmount = GetMonData(&gPlayerParty[monIdx], statId);

            if(statOp == 0)
            {
                ivAmount += delta;
                ivAmount = min(31, ivAmount);
            }
            else
            {
                if(ivAmount < delta)
                    ivAmount = 0;
                else
                    ivAmount -= delta;
            }

            SetMonData(&gPlayerParty[monIdx], statId, &ivAmount);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
    }
}

void Rogue_ApplyStatusToMon(void)
{
    u32 statusAilment = 0;
    u16 monIdx = gSpecialVar_0x8004;

    switch(gSpecialVar_0x8005)
    {
        case 0:
            statusAilment = STATUS1_POISON;
            break;

        case 1:
            statusAilment = STATUS1_PARALYSIS;
            break;

        case 2:
            statusAilment = STATUS1_SLEEP;
            break;

        case 3:
            statusAilment = STATUS1_FREEZE;
            break;

        case 4:
            statusAilment = STATUS1_BURN;
            break;
    }

    if(monIdx == 255)
    {
        // Entire team
        u8 i;

        for(i = 0; i < gPlayerPartyCount; ++i)
        {
            SetMonData(&gPlayerParty[i], MON_DATA_STATUS, &statusAilment);
        }
    }
    else
    {
        SetMonData(&gPlayerParty[monIdx], MON_DATA_STATUS, &statusAilment);
    }
}

void Rogue_ReleaseMonInSlot(void)
{
    u16 monIdx = gSpecialVar_0x8004;

    if(monIdx < gPlayerPartyCount)
    {
        RemoveMonAtSlot(monIdx, TRUE, TRUE);
    }
}

u16 Rogue_CalcMaxPartySize(void)
{
    return Rogue_GetMaxPartySize();
}

u16 Rogue_GetMonEvoCount(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 species = GetMonData(&gPlayerParty[monIdx], MON_DATA_SPECIES);

    if(species != SPECIES_NONE)
    {
        u16 e;
        struct Evolution evo;
        u8 evoCount = Rogue_GetMaxEvolutionCount(species);
        u16 count = 0;

        for (e = 0; e < evoCount; e++)
        {
            Rogue_ModifyEvolution(species, e, &evo);
            Rogue_ModifyEvolution_ApplyCurses(species, e, &evo);

            if (evo.targetSpecies != SPECIES_NONE)
            {
                ++count;
            }
        }

        return count;
    }

    return 0;
}

void Rogue_GetMonEvoParams(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 evoIdx = gSpecialVar_0x8005;
    u16 species = GetMonData(&gPlayerParty[monIdx], MON_DATA_SPECIES);

    gSpecialVar_0x8006 = 0;
    gSpecialVar_0x8007 = 0;

    if(species != SPECIES_NONE)
    {        
        // evoIdx doesn't mean array idx annoyingly as evos can be toggled/changed
        u16 e;
        struct Evolution evo;
        u8 evoCount = Rogue_GetMaxEvolutionCount(species);
        u16 count = 0;

        for (e = 0; e < evoCount; e++)
        {
            Rogue_ModifyEvolution(species, e, &evo);
            Rogue_ModifyEvolution_ApplyCurses(species, e, &evo);

            if (evo.targetSpecies != SPECIES_NONE)
            {
                if(count++ == evoIdx)
                {
                    gSpecialVar_0x8006 = evo.method;
                    gSpecialVar_0x8007 = evo.param;
                    return;
                }
            }
        }
    }
}

void RogueDebug_FillGenPC(void)
{
#ifdef ROGUE_DEBUG
    u8 i;
    u16 species;
    u16 writeIdx = 0;
    u16 genId = gSpecialVar_0x8004;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        if(SpeciesToGen(species) == genId)
        {
            struct Pokemon mon;
            u16 currIdx = writeIdx++;
            u16 targetBox = currIdx / IN_BOX_COUNT;
            u16 boxIndex = currIdx % IN_BOX_COUNT;

            CreateMon(&mon, species, 95, MAX_PER_STAT_IVS, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);

            SetBoxMonAt(targetBox, boxIndex, &mon.box);
        }
    }

    // Clear a box of space
    for(i = 0; i < IN_BOX_COUNT; ++i)
    {
        u16 currIdx = writeIdx++;
        u16 targetBox = currIdx / IN_BOX_COUNT;
        u16 boxIndex = currIdx % IN_BOX_COUNT;
        ZeroBoxMonAt(targetBox, boxIndex);
    }
#endif
}

void RogueDebug_FillDex(void)
{
#ifdef ROGUE_DEBUG
    u16 species;

    for(species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
    {
        //GetSetPokedexSpeciesFlag(species, FLAG_SET_SEEN);
        //GetSetPokedexSpeciesFlag(species, FLAG_SET_CAUGHT);
        GetSetPokedexSpeciesFlag(species, FLAG_SET_CAUGHT_SHINY);
    }
#endif
}

void RogueDebug_ClearQuests(void)
{
#ifdef ROGUE_DEBUG
    RogueQuest_OnNewGame();
#endif
}

void Debug_RogueQuest_CompleteQuest(u16 questId);

void RogueDebug_CompleteAvaliableQuests(void)
{
#ifdef ROGUE_DEBUG
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_IsQuestUnlocked(i))
        {
            if(!RogueQuest_GetStateFlag(i, QUEST_STATE_HAS_COMPLETE))
                Debug_RogueQuest_CompleteQuest(i);
        }
    }
#endif
}

void RogueDebug_CollectAllQuests(void)
{
#ifdef ROGUE_DEBUG
    u16 i;

    for(i = 0; i < QUEST_ID_COUNT; ++i)
    {
        if(RogueQuest_HasPendingRewards(i))
        {
            RogueQuest_TryCollectRewards(i);
        }
    }
#endif
}

void RogueDebug_StartBattle(void)
{
#ifdef ROGUE_DEBUG
    u16 i;

    for(i = 0; i < PARTY_SIZE; ++i)
    {
        ZeroMonData(&gEnemyParty[i]);
        BoxMonAtToMon(TOTAL_BOXES_COUNT- 1, i, &gEnemyParty[i]);
    }

    CalculateEnemyPartyCount();

    gSpecialVar_0x8004 = SPECIAL_BATTLE_AUTOMATION;
    DoSpecialTrainerBattle();
#endif
}

void RogueDebug_GiveDynamicUniqueMon()
{
#ifdef ROGUE_DEBUG
    struct Pokemon* mon = &gEnemyParty[0];
    u32 customMonId = RogueGift_CreateDynamicMonId(gSpecialVar_0x8004, SPECIES_AIPOM);

    RogueGift_CreateMon(customMonId, mon, SPECIES_AIPOM, STARTER_MON_LEVEL, 0);
    GiveTradedMonToPlayer(mon);
#endif
}

void Rogue_GetDynamicUniqueMonSpecies()
{
    AGB_ASSERT(gSpecialVar_0x8004 < DYNAMIC_UNIQUE_MON_COUNT);

    if(RogueGift_IsDynamicMonSlotEnabled(gSpecialVar_0x8004))
    {
        gSpecialVar_Result = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004)->species;
    }
    else
    {
        gSpecialVar_Result = SPECIES_NONE;
    }
}

static u8 const sText_Timer[] = _("{STR_VAR_1}:{STR_VAR_2} hours");

void Rogue_BufferDynamicUniqueMonCountDown()
{
    u32 countDown = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004)->countDown;
    u32 hours = countDown / 60;
    u32 minutes = countDown % 60;
    
    AGB_ASSERT(gSpecialVar_0x8004 < DYNAMIC_UNIQUE_MON_COUNT);

    ConvertUIntToDecimalStringN(gStringVar1, hours, STR_CONV_MODE_LEFT_ALIGN, 4);
    ConvertUIntToDecimalStringN(gStringVar2, minutes, STR_CONV_MODE_LEADING_ZEROS, 2);

    StringExpandPlaceholders(gStringVar3, sText_Timer);
    gSpecialVar_Result = (countDown != 0);
}

void Rogue_ShowNewQuests()
{
    Rogue_OpenQuestMenu(CB2_ReturnToFieldContinueScript, FALSE);
}

void Rogue_ShowNewMonMasteries()
{
    Rogue_OpenMonMasteryMenu(CB2_ReturnToFieldContinueScript);
}

void Rogue_QuestCollectNextReward()
{
    // 0 - Nothing to collect
    // 1 - Success
    // 2 - Cannot give reward
    u16 questId;

    if(!RogueQuest_IsRewardSequenceActive())
        RogueQuest_BeginRewardSequence();

    for(questId = 0; questId < QUEST_ID_COUNT; ++questId)
    {
        if(RogueQuest_HasPendingRewards(questId))
        {
            if(RogueQuest_TryCollectRewards(questId))
                gSpecialVar_Result = 1;
            else
            {
                gSpecialVar_Result = 2;
                RogueQuest_EndRewardSequence();
            }
            return;
        }
    }

    gSpecialVar_Result = 0;
    RogueQuest_EndRewardSequence();
}

void Rogue_HasAnyNewQuests()
{
    gSpecialVar_Result = RogueQuest_HasPendingNewQuests();
}

void Rogue_HasPendingQuestRewards()
{
    u16 questId;
    gSpecialVar_Result = FALSE;

    for(questId = 0; questId < QUEST_ID_COUNT; ++questId)
    {
        if(RogueQuest_HasPendingRewards(questId))
        {
            gSpecialVar_Result = TRUE;
            return;
        }
    }
}

void Rogue_UnlockChallengeQuests()
{
    FlagSet(FLAG_SYS_CHALLENGES_UNLOCKED);
}

void Rogue_UnlockMasteryQuests()
{
    FlagSet(FLAG_SYS_MASTERIES_UNLOCKED);
}

void Rogue_DetermineItemPickupCount()
{
    u16 itemId = gSpecialVar_0x8001;
    gSpecialVar_0x8002 = Rogue_ModifyItemPickupAmount(itemId, 1);
}

enum BerryTreatBuff
{
    BERRY_BUFF_FRIEND,
    BERRY_BUFF_HP,
    BERRY_BUFF_ATK,
    BERRY_BUFF_DEF,
    BERRY_BUFF_SPD,
    BERRY_BUFF_SPATK,
    BERRY_BUFF_SPDEF,
    BERRY_BUFF_WEAKEN,
};

static u8 BerryItemToTreatBuff(u16 berryItem)
{
    switch(berryItem)
    {
        case ITEM_ORAN_BERRY:
        case ITEM_SITRUS_BERRY:
        case ITEM_POMEG_BERRY:
            return BERRY_BUFF_HP;

        case ITEM_KELPSY_BERRY:
            return BERRY_BUFF_ATK;

        case ITEM_QUALOT_BERRY:
            return BERRY_BUFF_DEF;

        case ITEM_HONDEW_BERRY:
            return BERRY_BUFF_SPATK;

        case ITEM_GREPA_BERRY:
            return BERRY_BUFF_SPDEF;

        case ITEM_TAMATO_BERRY:
        case ITEM_SALAC_BERRY:
            return BERRY_BUFF_SPD;

        case ITEM_LEPPA_BERRY:
            return BERRY_BUFF_WEAKEN;

#ifdef ROGUE_EXPANSION
        case ITEM_LIECHI_BERRY:
            return BERRY_BUFF_ATK;

        case ITEM_GANLON_BERRY:
            return BERRY_BUFF_DEF;

        case ITEM_PETAYA_BERRY:
            return BERRY_BUFF_SPATK;

        case ITEM_APICOT_BERRY:
            return BERRY_BUFF_SPDEF;
#endif
        default:
            return BERRY_BUFF_FRIEND;
    }
}

void Rogue_CheckBerryTreat(void)
{
    u16 itemId = gSpecialVar_ItemId;
    gSpecialVar_Result = BerryItemToTreatBuff(itemId);
}

void Rogue_ApplyBerryTreat(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 itemId = gSpecialVar_ItemId;
    u16 buffAmount = gSpecialVar_0x8005;
    u16 berryBuff = BerryItemToTreatBuff(itemId);

    if(berryBuff == BERRY_BUFF_FRIEND)
    {
        u16 friendship = GetMonData(&gPlayerParty[monIdx], MON_DATA_FRIENDSHIP);

        if(friendship < MAX_FRIENDSHIP)
        {
            gSpecialVar_Result = TRUE;

            friendship += buffAmount * 10;
            if(friendship > MAX_FRIENDSHIP)
                friendship = MAX_FRIENDSHIP;

            SetMonData(&gPlayerParty[monIdx], MON_DATA_FRIENDSHIP, &friendship);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
        else
        {
            gSpecialVar_Result = FALSE;
        }
    }
    else if(berryBuff == BERRY_BUFF_WEAKEN)
    {
        u16 statOffset;

        gSpecialVar_Result = FALSE;

        for(statOffset = 0; statOffset < 6; ++statOffset)
        {
            u16 ivCount = GetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset);

            if(ivCount != 0)
            {
                gSpecialVar_Result = TRUE;

                if(ivCount < buffAmount)
                    ivCount = 0;
                else
                    ivCount -= buffAmount;

                SetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset, &ivCount);
            }
        }

        CalculateMonStats(&gPlayerParty[monIdx]);
    }
    else
    {
        u16 statOffset = berryBuff - 1;
        u16 ivCount = GetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset);

        if(ivCount < 31)
        {
            gSpecialVar_Result = TRUE;

            ivCount += buffAmount;
            if(ivCount > 31)
                ivCount = 31;

            SetMonData(&gPlayerParty[monIdx], MON_DATA_HP_IV + statOffset, &ivCount);
            CalculateMonStats(&gPlayerParty[monIdx]);
        }
        else
        {
            gSpecialVar_Result = FALSE;
        }
    }
}

void Rogue_ChangeMonBall(void)
{
    u16 monIdx = gSpecialVar_0x8004;
    u16 itemId = gSpecialVar_ItemId;

    SetMonData(&gPlayerParty[monIdx], MON_DATA_POKEBALL, &itemId);
}

void Rogue_BufferLabMonName(void)
{
    u16 index = gSpecialVar_0x8002;
    Rogue_CopyLabEncounterMonNickname(index, gStringVar1);
}

void Rogue_GiveLabMon(void)
{
    u16 index = gSpecialVar_0x8002;
    gSpecialVar_Result = Rogue_GiveLabEncounterMon(index);
}

void Rogue_ChooseMiniBossRewardMons(void)
{
    Rogue_SelectMiniBossRewardMons();
}

void Rogue_ClearCharmsAndCurses(void)
{
    Rogue_RemoveCharmsFromBag();
    Rogue_RemoveCursesFromBag();
}

void Rogue_ClearCharms(void)
{
    Rogue_RemoveCharmsFromBag();
}

void Rogue_ClearCurses(void)
{
    Rogue_RemoveCursesFromBag();
}

void Rogue_IsRoamerActive(void)
{
    gSpecialVar_Result = gRogueRun.wildEncounters.roamer.species != SPECIES_NONE;
}

void Rogue_BufferRoamerName(void)
{
    StringCopyN(gStringVar1, RoguePokedex_GetSpeciesName(gRogueRun.wildEncounters.roamer.species), POKEMON_NAME_LENGTH);
}

void Rogue_GetUnlockedCampaignCount(void)
{
    u16 i;
    u16 count = 0;

    for(i = ROGUE_CAMPAIGN_FIRST; i <= ROGUE_CAMPAIGN_LAST; ++i)
    {
        if(gRogueSaveBlock->campaignData[i - ROGUE_CAMPAIGN_FIRST].isUnlocked)
            ++count;
    }

    gSpecialVar_Result = count;
}

void Rogue_GetNextUnlockedCampaign(void)
{
    u16 i = gSpecialVar_0x8004;

    if(i == ROGUE_CAMPAIGN_NONE)
        i = ROGUE_CAMPAIGN_FIRST;
    else
        ++i;

    for(; i <= ROGUE_CAMPAIGN_LAST; ++i)
    {
        if(gRogueSaveBlock->campaignData[i - ROGUE_CAMPAIGN_FIRST].isUnlocked)
        {
            gSpecialVar_0x8004 = i;
            return;
        }
    }

    gSpecialVar_0x8004 = ROGUE_CAMPAIGN_NONE;
}

void Rogue_GetCampaignHighScore(void)
{
    u16 i = VarGet(VAR_ROGUE_DESIRED_CAMPAIGN);

    if(i != ROGUE_CAMPAIGN_NONE)
    {
        if(gRogueSaveBlock->campaignData[i - ROGUE_CAMPAIGN_FIRST].isCompleted)
        {
            gSpecialVar_Result = gRogueSaveBlock->campaignData[i - ROGUE_CAMPAIGN_FIRST].bestScore;
            return;
        }
    }

    gSpecialVar_Result = 0;
    return;
}

void Rogue_BufferCampaignName(void)
{
    StringCopy(&gStringVar1[0], GetCampaignTitle(VarGet(VAR_ROGUE_DESIRED_CAMPAIGN)));
}

static bool8 CheckSpeciesCombo(u16 speciesCheckA, u16 speciesCheckB, u16 speciesTargetA, u16 speciesTargetB)
{
    return (speciesCheckA == speciesTargetA && speciesCheckB == speciesTargetB)
        || (speciesCheckB == speciesTargetA && speciesCheckA == speciesTargetB);
}

static u16 GetSpeciesComboOutput(u16 speciesA, u16 speciesB, bool8 getItem)
{
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_EEVEE, SPECIES_CHARMANDER))
    //    return getItem ? ITEM_PECHA_BERRY : SPECIES_ABSOL;

#ifdef ROGUE_EXPANSION
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_KYUREM, SPECIES_RESHIRAM))
    //    return getItem ? ITEM_DNA_SPLICERS : SPECIES_KYUREM_WHITE;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_KYUREM, SPECIES_ZEKROM))
    //    return getItem ? ITEM_DNA_SPLICERS : SPECIES_KYUREM_BLACK;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_ZYGARDE_COMPLETE, SPECIES_ZYGARDE_COMPLETE))
    //    return getItem ? ITEM_ZYGARDE_CUBE : SPECIES_ZYGARDE_10;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_ZYGARDE_10, SPECIES_ZYGARDE_10))
    //    return getItem ? ITEM_ZYGARDE_CUBE : SPECIES_ZYGARDE;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_ZYGARDE, SPECIES_ZYGARDE))
    //    return getItem ? ITEM_ZYGARDE_CUBE : SPECIES_ZYGARDE_COMPLETE;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_NECROZMA, SPECIES_SOLGALEO))
    //    return getItem ? ITEM_N_SOLARIZER : SPECIES_NECROZMA_DUSK_MANE;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_NECROZMA, SPECIES_LUNALA))
    //    return getItem ? ITEM_N_LUNARIZER : SPECIES_NECROZMA_DAWN_WINGS;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_CALYREX, SPECIES_GLASTRIER))
    //    return getItem ? ITEM_REINS_OF_UNITY : SPECIES_CALYREX_ICE_RIDER;
//
    //if(CheckSpeciesCombo(speciesA, speciesB, SPECIES_CALYREX, SPECIES_SPECTRIER))
    //    return getItem ? ITEM_REINS_OF_UNITY : SPECIES_CALYREX_SHADOW_RIDER;
#endif

    return 0;
}

void Rogue_CheckMonCombo(void)
{
    u16 speciesA = GetMonData(&gPlayerParty[gSpecialVar_0x8003], MON_DATA_SPECIES);
    u16 speciesB = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES);

    gSpecialVar_0x8005 = GetSpeciesComboOutput(speciesA, speciesB, TRUE);
    gSpecialVar_0x8006 = GetSpeciesComboOutput(speciesA, speciesB, FALSE);
}

void Rogue_ApplyMonCombo(void)
{
    // This is broken and we don't want to use it anyway
    AGB_ASSERT(FALSE);

    //u16 speciesA = GetMonData(&gPlayerParty[gSpecialVar_0x8003], MON_DATA_SPECIES);
    //u16 speciesB = GetMonData(&gPlayerParty[gSpecialVar_0x8004], MON_DATA_SPECIES);
    //u16 outputSpecies = GetSpeciesComboOutput(speciesA, speciesB, FALSE);
//
    //if(outputSpecies)
    //{
    //    u8 speciesName[POKEMON_NAME_LENGTH + 1];
    //    StringCopyN(speciesName, RoguePokedex_GetSpeciesName(outputSpecies), ARRAY_COUNT(speciesName));
//
    //    SetMonData(&gPlayerParty[gSpecialVar_0x8003], MON_DATA_SPECIES, &outputSpecies);
    //    SetMonData(&gPlayerParty[gSpecialVar_0x8003], MON_DATA_NICKNAME, speciesName);
    //    RemoveMonAtSlot(gSpecialVar_0x8004, TRUE, TRUE, FALSE);
//
    //    gSpecialVar_Result = TRUE;
    //}
    //else
    //{
    //    gSpecialVar_Result = FALSE;
    //}
}

#ifdef ROGUE_EXPANSION
static u16 const sRotomMovesPerForm[] = 
{
    [SPECIES_ROTOM_HEAT - SPECIES_ROTOM_HEAT] = MOVE_OVERHEAT,
    [SPECIES_ROTOM_WASH - SPECIES_ROTOM_HEAT] = MOVE_HYDRO_PUMP,
    [SPECIES_ROTOM_FROST - SPECIES_ROTOM_HEAT] = MOVE_BLIZZARD,
    [SPECIES_ROTOM_FAN - SPECIES_ROTOM_HEAT] = MOVE_AIR_SLASH,
    [SPECIES_ROTOM_MOW - SPECIES_ROTOM_HEAT] = MOVE_LEAF_STORM,
};

void ShiftMoveSlotExtern(struct Pokemon *mon, u8 slotTo, u8 slotFrom);

static void HandleRotomFormChange(u16 fromSpecies, u16 toSpecies)
{
    u8 i;
    u8 moveToReplace = MAX_MON_MOVES;

    if(fromSpecies != SPECIES_ROTOM)
    {
        for(i = 0; i < MAX_MON_MOVES; ++i)
        {
            u16 move = GetMonData(&gPlayerParty[0], MON_DATA_MOVE1 + i);

            if(move == sRotomMovesPerForm[fromSpecies - SPECIES_ROTOM_HEAT])
            {
                moveToReplace = i;
                break;
            }
        }
    }

    if(moveToReplace != MAX_MON_MOVES)
    {
        // Replace move with appropriate form
        if(toSpecies != SPECIES_ROTOM)
        {
            SetMonMoveSlot(&gPlayerParty[0], sRotomMovesPerForm[toSpecies - SPECIES_ROTOM_HEAT], moveToReplace);
        }
        // Just remove move
        else
        {
            SetMonMoveSlot(&gPlayerParty[0], MOVE_NONE, moveToReplace);
            RemoveMonPPBonus(&gPlayerParty[0], moveToReplace);
            for (i = moveToReplace; i < MAX_MON_MOVES - 1; i++)
                ShiftMoveSlotExtern(&gPlayerParty[0], i, i + 1);
        }
    }
}

void Rogue_TryInteractFormChange(void)
{
    u16 leadSpecies = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES);
    u16 formSpecies = gSpecialVar_0x8005;
    u16 leadBaseSpecies = GET_BASE_SPECIES_ID(leadSpecies);
    u16 formBaseSpecies = GET_BASE_SPECIES_ID(formSpecies);

    gSpecialVar_Result = FALSE;

    if(leadSpecies == formSpecies)
    {
        // Switch from form
        SetMonData(&gPlayerParty[0], MON_DATA_SPECIES, &leadBaseSpecies);

        if(leadBaseSpecies == SPECIES_ROTOM)
            HandleRotomFormChange(leadSpecies, leadBaseSpecies);
    
        gSpecialVar_Result = TRUE;
    }
    else if(leadBaseSpecies == formBaseSpecies)
    {
        SetMonData(&gPlayerParty[0], MON_DATA_SPECIES, &formSpecies);

        if(leadBaseSpecies == SPECIES_ROTOM)
            HandleRotomFormChange(leadSpecies, formSpecies);

        gSpecialVar_Result = TRUE;
    }
}

#else

void Rogue_TryInteractFormChange(void)
{
    gSpecialVar_Result = FALSE;
}

#endif

void Rogue_GetFollowMonSpecies(void)
{
    u16 species;
    bool8 isShiny;
    u8 spawnSlot;
    FollowMon_GetSpeciesFromLastInteracted(&species, &isShiny, &spawnSlot);

    gSpecialVar_0x800A = species;
    gSpecialVar_0x800B = isShiny;
    gSpecialVar_0x8009 = spawnSlot;
}

void Rogue_SetupFollowMonFromParty(void)
{
    FollowMon_SetGraphicsFromParty();
}

void Rogue_TryEnqueueWildBattleMon(void)
{
    u8 spawnSlot = gSpecialVar_0x8009;
    RogueSafari_EnqueueBattleMon(spawnSlot);
}

void Rogue_GetTrainerNum(void)
{
    u16 trainerNum = Rogue_GetTrainerNumFromLastInteracted();
    if(trainerNum != TRAINER_NONE)
    {
        gSpecialVar_0x8004 = trainerNum;
        gSpecialVar_Result = TRUE;
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

void Rogue_PlayStaticTrainerEncounterBGM(void)
{
    u16 trainerNum = VarGet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA);

    struct RogueBattleMusic music;
    Rogue_ModifyBattleMusic(BATTLE_MUSIC_TYPE_TRAINER, trainerNum, &music);

    //PlayBGM();
    PlayNewMapMusic(music.encounterMusic);
    //playbgm(MUS_ENCOUNTER_INTENSE, FALSE)
}

void Rogue_PlayRivalTrainerEncounterBGM(void)
{
    u16 trainerNum = gRogueRun.rivalTrainerNum;

    struct RogueBattleMusic music;
    Rogue_ModifyBattleMusic(BATTLE_MUSIC_TYPE_TRAINER, trainerNum, &music);

    //PlayBGM();
    PlayNewMapMusic(music.encounterMusic);
    //playbgm(MUS_ENCOUNTER_INTENSE, FALSE)
}

void Rogue_PlayTeamBossTrainerEncounterBGM(void)
{
    u16 trainerNum = gRogueRun.teamBossTrainerNum;

    struct RogueBattleMusic music;
    Rogue_ModifyBattleMusic(BATTLE_MUSIC_TYPE_TRAINER, trainerNum, &music);

    //PlayBGM();
    PlayNewMapMusic(music.encounterMusic);
    //playbgm(MUS_ENCOUNTER_INTENSE, FALSE)
}

extern const u8 gPlaceholder_Gym_PreBattleOpenning[];

void Rogue_ShouldSkipTrainerOpenningMsg(void)
{
    // Skip openning text if the message is empty
    const u8* str = Rogue_ModifyFieldMessage(gPlaceholder_Gym_PreBattleOpenning);
    gSpecialVar_Result = (str[0] == 0xFF);
}

u16 Rogue_BufferNextVictoryLapTrainer()
{
    u16 trainerNum = Rogue_ChooseNextBossTrainerForVictoryLap();
    VarSet(VAR_ROGUE_DESIRED_WEATHER, Rogue_GetTrainerWeather(trainerNum));
    return trainerNum;
}

void Rogue_BeginVictoryLap()
{
    u32 i;
    u16* historyBuffer = Rogue_GetVictoryLapHistoryBufferPtr();
    u32 historyBufferSize = Rogue_GetVictoryLapHistoryBufferSize();

    AGB_ASSERT(!Rogue_IsVictoryLapActive());
    AGB_ASSERT(Rogue_GetCurrentDifficulty() == ROGUE_MAX_BOSS_COUNT);

    FlagSet(FLAG_ROGUE_IS_VICTORY_LAP);
    Rogue_SetCurrentDifficulty(ROGUE_MAX_BOSS_COUNT - 1);
    memset(historyBuffer, INVALID_HISTORY_ENTRY, sizeof(u16) * historyBufferSize);
}

void Rogue_EndVictoryLap()
{
    AGB_ASSERT(Rogue_IsVictoryLapActive());

    FlagClear(FLAG_ROGUE_IS_VICTORY_LAP);
    Rogue_SetCurrentDifficulty(ROGUE_MAX_BOSS_COUNT);
}

void Rogue_EnterPartnerMonCapacity()
{
    gSpecialVar_Result = Rogue_GetStartingMonCapacity();
}

void Rogue_SetupFollowParterMonObjectEvent()
{
    FollowMon_ClearCachedPartnerSpecies();
    SetupFollowParterMonObjectEvent();
}

void Rogue_RegisterRideMon()
{
    u16 gfxId = FollowMon_GetMonGraphics(&gPlayerParty[0]);
    VarSet(VAR_ROGUE_REGISTERED_RIDE_MON, gfxId);
}

void Rogue_RunRewardLvls()
{
    gSpecialVar_Result = Rogue_PostRunRewardLvls();
}

void Rogue_RunRewardMoney()
{
    gSpecialVar_Result = Rogue_PostRunRewardMoney();
}

void ReloadWarpSilent()
{
    u8 mapGroup = gSaveBlock1Ptr->location.mapGroup;
    u8 mapNum = gSaveBlock1Ptr->location.mapNum;
    u8 warpId = (u8)-1;
    u16 x = gSaveBlock1Ptr->pos.x;
    u16 y = gSaveBlock1Ptr->pos.y;

    StoreInitialPlayerAvatarStateForReloadWarp();
    SetWarpDestination(mapGroup, mapNum, warpId, x, y);
    DoDiveWarp();
}

void ReloadSafeWarp()
{
    u8 mapGroup = gSaveBlock1Ptr->location.mapGroup;
    u8 mapNum = gSaveBlock1Ptr->location.mapNum;

    StoreInitialPlayerAvatarStateForReloadWarp();
    SetWarpDestination(mapGroup, mapNum, WARP_ID_MAP_START, 0, 0);
    DoTeleportTileWarp();
    //DoDiveWarp();
}

void Rogue_SetTimeAndSeason()
{
    u8 tod = min(gSpecialVar_0x8004, TIME_PRESET_COUNT);
    u8 season = min(gSpecialVar_0x8005, SEASON_COUNT);

    if(tod != TIME_PRESET_COUNT || season != SEASON_COUNT)
    {
        RogueToD_SetTimePreset(tod, season);
        gSpecialVar_Result = TRUE;
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

void Popup_CannotTakeItem()
{
    u16 itemId = gSpecialVar_0x8004;
    u16 quantity = gSpecialVar_0x8005;

    Rogue_PushPopup_CannotTakeItem(itemId, quantity);
}

void Popup_NewBadgeGet()
{
    Rogue_PushPopup_NewBadgeGet(Rogue_GetCurrentDifficulty() - 1);
}

u16 Rogue_GetBagCapacityUpgradeLevel()
{
    return gSaveBlock1Ptr->bagCapacityUpgrades;
}

void Rogue_IncreaseBagCapacityUpgradeLevel()
{
    gSpecialVar_Result = FALSE;

    if(gSaveBlock1Ptr->bagCapacityUpgrades < ITEM_BAG_MAX_CAPACITY_UPGRADE)
    {
        ++gSaveBlock1Ptr->bagCapacityUpgrades;
        Rogue_PushPopup_UpgradeBagCapacity();
        gSpecialVar_Result = TRUE;
        ShrinkBagItems();
    }
}

void Rogue_BufferBagUpgradeCost()
{
    u32 cost = Rogue_CalcBagUpgradeCost();
    ConvertUIntToDecimalStringN(gStringVar2, cost, STR_CONV_MODE_LEFT_ALIGN, 7);
}

void Rogue_CheckBagUpgradeCost()
{
    u32 cost = Rogue_CalcBagUpgradeCost();
    if(IsEnoughMoney(&gSaveBlock1Ptr->money, cost))
        gSpecialVar_Result = TRUE;
    else
        gSpecialVar_Result = FALSE;
}

void Rogue_RemoveBagUpgradeCost()
{
    u32 cost = Rogue_CalcBagUpgradeCost();
    RemoveMoney(&gSaveBlock1Ptr->money, cost);
}

void Rogue_SeedRng()
{
    SeedRng(gMain.vblankCounter1);
    SeedRng2(gMain.vblankCounter2);
}

void Rogue_CheckHubConnectionDir()
{
    u8 dir;
    u8 checkArea = gSpecialVar_0x8004;
    u8 currentArea = RogueHub_GetAreaFromCurrentMap();

    for(dir = HUB_AREA_CONN_SOUTH; dir < HUB_AREA_CONN_COUNT; ++dir)
    {
        if(RogueHub_FindAreaInDir(currentArea, dir) == checkArea)
        {
            gSpecialVar_Result = dir;
            return;
        }
    }

    gSpecialVar_Result = HUB_AREA_CONN_COUNT;
}

void Rogue_AssignDefaultRegion()
{
    u32 flags = RoguePlayer_GetOutfitTrainerFlags();
    bool8 anySet = FALSE;

    if(flags & TRAINER_FLAG_REGION_KANTO)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, TRUE);
        anySet = TRUE;
    }

    if(flags & TRAINER_FLAG_REGION_JOHTO)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, TRUE);
        anySet = TRUE;
    }

    if(flags & TRAINER_FLAG_REGION_HOENN)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, TRUE);
        anySet = TRUE;
    }

#ifdef ROGUE_EXPANSION
    if(flags & TRAINER_FLAG_REGION_SINNOH)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH, TRUE);
        anySet = TRUE;
    }

    if(flags & TRAINER_FLAG_REGION_UNOVA)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA, TRUE);
        anySet = TRUE;
    }

    if(flags & TRAINER_FLAG_REGION_KALOS)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS, TRUE);
        anySet = TRUE;
    }

    if(flags & TRAINER_FLAG_REGION_ALOLA)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA, TRUE);
        anySet = TRUE;
    }

    if(flags & TRAINER_FLAG_REGION_GALAR)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR, TRUE);
        anySet = TRUE;
    }

    if(flags & TRAINER_FLAG_REGION_PALDEA)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA, TRUE);
        anySet = TRUE;
    }
#endif

    // Fallback to Kanto for "nostalgia" I guess?
    // (It doesn't matter too much, just need it to not break)
    if(!anySet)
    {
        Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, TRUE);
    }

#ifdef ROGUE_EXPANSION
    switch (VarGet(VAR_ROGUE_INITIAL_DEX_SELECTION))
    {
    case 0:
        RoguePokedex_SetDexVariant(POKEDEX_VARIANT_ROGUE_MODERN);
        break;
    
    default:
        RoguePokedex_SetDexVariant(POKEDEX_VARIANT_ROGUE_CLASSICPLUS);
        break;
    }
#else
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_DEFAULT);
#endif
}

void Rogue_IsFinalQuestActive()
{
    gSpecialVar_Result = Rogue_UseFinalQuestEffects() != 0;
}

void Rogue_IsNearHoneyTree()
{
    gSpecialVar_Result = (Rogue_IsRunActive() && gRogueAdvPath.currentRoomType == ADVPATH_ROOM_HONEY_TREE);
}

void Rogue_TryScatterPokeblockNearHoneyTree()
{
    if(Rogue_IsRunActive() && gRogueAdvPath.currentRoomType == ADVPATH_ROOM_HONEY_TREE)
    {
        gSpecialVar_Result = Rogue_TryAddHoneyTreePokeblock(VarGet(VAR_ROGUE_ACTIVE_POKEBLOCK));
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

void Rogue_ScatterPokeblockItem()
{
    if(VarGet(VAR_ROGUE_ACTIVE_POKEBLOCK) == ITEM_POKEBLOCK_SHINY)
    {
        // For the shiny pokeblock always scatter it
        gSpecialVar_Result = TRUE;
    }
    else
    {
        u8 type = ItemId_GetSecondaryId(VarGet(VAR_ROGUE_ACTIVE_POKEBLOCK));
        gSpecialVar_Result = Rogue_RerollSingleWildSpecies(type);
    }
}

void Rogue_IsValidPokeblockBerry()
{
    gSpecialVar_Result = (Rogue_BerryToPokeblock(gSpecialVar_ItemId) != ITEM_NONE);
}

void Rogue_IsValidPieCrust()
{
    u16 crustItem = gSpecialVar_ItemId;
    u8 type = ItemId_GetSecondaryId(crustItem);

    // Only type pokeblock can be used for crust
    gSpecialVar_Result = IS_STANDARD_TYPE(type);
}

void Rogue_IsValidPieFilling()
{
    // Only stat specific pokeblock can be used for filling (technically we can also use a shiny pokeblock by a separate path too)
    u16 fillingItem = gSpecialVar_ItemId;
    gSpecialVar_Result = (fillingItem >= ITEM_POKEBLOCK_HP && fillingItem <= ITEM_POKEBLOCK_SPDEF);
}

void Rogue_CanMakeShinyPieFor()
{
    u16 species = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES);
    gSpecialVar_Result = GetSetPokedexSpeciesFlag(species, FLAG_GET_CAUGHT_SHINY);
}

static bool8 WillMonLikePieInternal(u16 crustItem, u16 fillingItem, struct Pokemon* mon)
{
    u8 type = ItemId_GetSecondaryId(crustItem);
    u16 species = GetMonData(mon, MON_DATA_SPECIES);

    if(IS_STANDARD_TYPE(type))
    {
        if(!(RoguePokedex_GetSpeciesType(species, 0) == type || RoguePokedex_GetSpeciesType(species, 1) == type))
        {
            return FALSE;
        }
    }

    if(fillingItem == ITEM_POKEBLOCK_SHINY)
    {
        if(IsMonShiny(mon))
            return FALSE;
    }
    else
    {
        type = ItemId_GetSecondaryId(fillingItem);

        if(IS_STANDARD_TYPE(type))
        {
            if(!(RoguePokedex_GetSpeciesType(species, 0) == type || RoguePokedex_GetSpeciesType(species, 1) == type))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

void Rogue_WillMonLikePie()
{
    u16 crustItem = gSpecialVar_0x8008;
    u16 fillingItem = gSpecialVar_0x8009;

    gSpecialVar_Result = WillMonLikePieInternal(crustItem, fillingItem, &gPlayerParty[0]);
}

void Rogue_FeedMonPie()
{
    u32 temp;
    u16 crustItem = gSpecialVar_0x8008;
    u16 fillingItem = gSpecialVar_0x8009;
    u16 pieSize = gSpecialVar_0x800A;
    bool8 likesPie = WillMonLikePieInternal(crustItem, fillingItem, &gPlayerParty[0]);

    AGB_ASSERT(pieSize <= PIE_SIZE_LARGE);

    if(fillingItem == ITEM_POKEBLOCK_SHINY)
    {
        temp = likesPie ? 1 : 0;
        SetMonData(&gPlayerParty[0], MON_DATA_IS_SHINY, &temp);
        Rogue_PushPopup_MonShinyChange(0, likesPie);
    }
    else
    {
        u16 stat = fillingItem - ITEM_POKEBLOCK_HP;
        temp = GetMonData(&gPlayerParty[0], MON_DATA_HP_IV + stat);

        if(likesPie)
        {
            switch (pieSize)
            {
            case PIE_SIZE_SMALL:
                temp = min(temp + 1, 31);
                break;

            case PIE_SIZE_MEDIUM:
                temp = min(temp + 5, 31);
                break;

            case PIE_SIZE_LARGE:
                temp = min(temp + 10, 31);
                break;
            }
        }
        else
        {
            switch (pieSize)
            {
            case PIE_SIZE_SMALL:
                temp = temp <= 1 ? 0 : (temp - 1);
                break;

            case PIE_SIZE_MEDIUM:
                temp = temp <= 5 ? 0 : (temp - 5);
                break;

            case PIE_SIZE_LARGE:
                temp = temp <= 10 ? 0 : (temp - 10);
                break;
            }
        }

        SetMonData(&gPlayerParty[0], MON_DATA_HP_IV + stat, &temp);
        Rogue_PushPopup_MonStatChange(0, likesPie);
    }

    CalculateMonStats(&gPlayerParty[0]);
}

static bool32 CanSpeciesLearnMove(u16 species, u16 move)
{
    u32 i;

    for (i = 0; gRoguePokemonProfiles[species].levelUpMoves[i].move != MOVE_NONE; i++)
    {
        if(gRoguePokemonProfiles[species].levelUpMoves[i].move == move)
            return TRUE;
    }

    for (i = 0; gRoguePokemonProfiles[species].tutorMoves[i] != MOVE_NONE; i++)
    {
        if(gRoguePokemonProfiles[species].tutorMoves[i] == move)
            return TRUE;
    }

    return FALSE;
}

static bool8 TryChangeMonGenderBySpecies()
{
#ifdef ROGUE_EXPANSION
    u16 startSpecies = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES);
    u16 newSpecies = startSpecies;

    switch (startSpecies)
    {
    case SPECIES_MEOWSTIC_MALE:
        newSpecies = SPECIES_MEOWSTIC_FEMALE;
        break;
    case SPECIES_MEOWSTIC_FEMALE:
        newSpecies = SPECIES_MEOWSTIC_MALE;
        break;
    
    case SPECIES_INDEEDEE_MALE:
        newSpecies = SPECIES_INDEEDEE_FEMALE;
        break;
    case SPECIES_INDEEDEE_FEMALE:
        newSpecies = SPECIES_INDEEDEE_MALE;
        break;
    
    case SPECIES_BASCULEGION_MALE:
        newSpecies = SPECIES_BASCULEGION_FEMALE;
        break;
    case SPECIES_BASCULEGION_FEMALE:
        newSpecies = SPECIES_BASCULEGION_MALE;
        break;
    
    case SPECIES_OINKOLOGNE_MALE:
        newSpecies = SPECIES_OINKOLOGNE_FEMALE;
        break;
    case SPECIES_OINKOLOGNE_FEMALE:
        newSpecies = SPECIES_OINKOLOGNE_MALE;
        break;

    }

    if(startSpecies != newSpecies)
    {
        SetMonData(&gPlayerParty[0], MON_DATA_SPECIES, &newSpecies);

        if(newSpecies == SPECIES_MEOWSTIC_MALE || newSpecies == SPECIES_MEOWSTIC_FEMALE)
        {
            u32 i, j;

            // Remove illegal moves
            for(i = 0; i < MAX_MON_MOVES;)
            {
                u16 moveId = GetMonData(&gPlayerParty[0], MON_DATA_MOVE1 + i);
                if(moveId != MOVE_NONE && !CanSpeciesLearnMove(newSpecies, moveId))
                {
                    SetMonMoveSlot(&gPlayerParty[0], MOVE_NONE, i);
                    RemoveMonPPBonus(&gPlayerParty[0], i);

                    // Shift all moves up
                    for (j = i; j < MAX_MON_MOVES - 1; j++)
                        ShiftMoveSlotExtern(&gPlayerParty[0], j, j + 1);
                    continue;
                }

                ++i;
            }
        }

        return TRUE;
    }
#endif

    return FALSE;
}

void Rogue_SwapMonGender()
{
    if(TryChangeMonGenderBySpecies())
    {
        Rogue_PushPopup_MonGenderChange(0, GetMonGender(&gPlayerParty[0]));
    }
    else
    {
        u8 gender;
        u8 startGender = GetMonGender(&gPlayerParty[0]);
        u32 genderFlag = GetMonData(&gPlayerParty[0], MON_DATA_GENDER_FLAG);

        genderFlag = !genderFlag;

        SetMonData(&gPlayerParty[0], MON_DATA_GENDER_FLAG, &genderFlag);
        
        gender = GetMonGender(&gPlayerParty[0]);

        if(startGender != gender)
        {
            Rogue_PushPopup_MonGenderChange(0, gender);
        }
    }
}

void Rogue_CanPlantBerries()
{
    gSpecialVar_Result = !Rogue_IsRunActive() || gRogueAdvPath.currentRoomType == ADVPATH_ROOM_RESTSTOP;
}

void Rogue_CheckDaycareCount()
{
    u8 i = 0;
    u8 count = 0;
    struct BoxPokemon* mon;

    for(i = 0; i < DAYCARE_SLOT_COUNT; ++i)
    {
        mon = Rogue_GetDaycareBoxMon(i);
        if(GetBoxMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE)
            ++count;
    }

    gSpecialVar_Result = count;
}

void Rogue_IsDaycareSlotEmpty()
{
    u8 daycareSlot = gSpecialVar_0x8005;
    struct BoxPokemon* mon = Rogue_GetDaycareBoxMon(daycareSlot);
    gSpecialVar_Result = (GetBoxMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE);
}

void Rogue_SwapDaycareMon()
{
    u16 partySlot = gSpecialVar_0x8004;
    u8 daycareSlot = gSpecialVar_0x8005;
    Rogue_SwapMonInDaycare(&gPlayerParty[partySlot], daycareSlot);

    // Resetup followmon
    if(partySlot == 0)
        SetupFollowParterMonObjectEvent();
}

void Rogue_TransformIntoValidDaycareEgg()
{
    u16 eggSpecies = Rogue_GetEggSpecies(gSpecialVar_Result);

    if(!RoguePokedex_IsSpeciesLegendary(eggSpecies))
        gSpecialVar_Result = eggSpecies;
    else
        gSpecialVar_Result = SPECIES_NONE;
}

void Rogue_SetupDaycareSpeciesGraphics()
{
    u32 i;
    u32 maxSlots = Rogue_GetCurrentDaycareSlotCount();

    for(i = 0; i < DAYCARE_SLOT_COUNT; ++i)
    {
        struct BoxPokemon* mon = Rogue_GetDaycareBoxMon(i);

        if(i < maxSlots && GetBoxMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE)
        {
            // FLAG_HIDE_SPECIES_0, FLAG_HIDE_SPECIES_1, FLAG_HIDE_SPECIES_1
            FlagClear(FLAG_TEMP_5 + i);
            FollowMon_SetGraphicsRaw(i, FollowMon_GetBoxMonGraphics(mon));
        }
        else
        {
            // FLAG_HIDE_SPECIES_0, FLAG_HIDE_SPECIES_1, FLAG_HIDE_SPECIES_1
            FlagSet(FLAG_TEMP_5 + i);
        }
    }
}

void Rogue_HealAlivePlayerParty()
{
    u8 i, j;
    u8 ppBonuses;
    u8 arg[4];

    CalculatePlayerPartyCount();

    // restore HP.
    for(i = 0; i < gPlayerPartyCount; i++)
    {
        u16 currHp = GetMonData(&gPlayerParty[i], MON_DATA_HP);
        if(currHp != 0)
        {
            u16 maxHP = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
            arg[0] = maxHP;
            arg[1] = maxHP >> 8;
            SetMonData(&gPlayerParty[i], MON_DATA_HP, arg);
            ppBonuses = GetMonData(&gPlayerParty[i], MON_DATA_PP_BONUSES);

            // restore PP.
            for(j = 0; j < MAX_MON_MOVES; j++)
            {
                arg[0] = CalculatePPWithBonus(GetMonData(&gPlayerParty[i], MON_DATA_MOVE1 + j), ppBonuses, j);
                SetMonData(&gPlayerParty[i], MON_DATA_PP1 + j, arg);
            }

            // since status is u32, the four 0 assignments here are probably for safety to prevent undefined data from reaching SetMonData.
            arg[0] = 0;
            arg[1] = 0;
            arg[2] = 0;
            arg[3] = 0;
            SetMonData(&gPlayerParty[i], MON_DATA_STATUS, arg);
        }
    }
}

void Rogue_OnHealWithNurse()
{
    if(IsHealingFlaskEnabled())
    {
        VarSet(VAR_ROGUE_FLASK_HEALS_USED, 0);
        Rogue_PushPopup_FlaskRefilled();
    }

    Rogue_RefillFlightCharges(TRUE);
}

#define VAR_CATCH_CONTEST_TYPE VAR_TEMP_2
#define VAR_CATCH_CONTEST_STAT VAR_TEMP_3

void Rogue_SelectCatchingContestMode()
{
    u8 type = Random() % NUMBER_OF_MON_TYPES;
    u8 stat = Random() % NUM_STATS;

    while(!IS_STANDARD_TYPE(type))
    {
        type = Random() % NUMBER_OF_MON_TYPES;
    }

    VarSet(VAR_CATCH_CONTEST_TYPE, type);
    VarSet(VAR_CATCH_CONTEST_STAT, stat);
}

void Rogue_BufferContestMode()
{
    u8 type = VarGet(VAR_CATCH_CONTEST_TYPE);
    u8 stat = VarGet(VAR_CATCH_CONTEST_STAT);

    StringCopy(gStringVar1, sTypeNames[type]);
    StringCopy(gStringVar2, sStatNamesTable[stat]);
}

void Rogue_CatchingContestBegin()
{
    u8 type = VarGet(VAR_CATCH_CONTEST_TYPE);
    u8 stat = VarGet(VAR_CATCH_CONTEST_STAT);
    Rogue_BeginCatchingContest(type, stat);
}

void Rogue_CatchingContestEnd()
{
    u16 caughtSpecies;
    u16 winningSpecies;
    bool8 didWin;

    Rogue_EndCatchingContest();
    Rogue_GetCatchingContestResults(&caughtSpecies, &didWin, &winningSpecies);

    gSpecialVar_Result = didWin;
    gSpecialVar_0x8000 = caughtSpecies != SPECIES_NONE;
    StringCopy_Nickname(gStringVar1, RoguePokedex_GetSpeciesName(caughtSpecies));
    StringCopy_Nickname(gStringVar2, RoguePokedex_GetSpeciesName(winningSpecies));
}

void Rogue_GiveCatchingContestMon()
{
    GiveMonToPlayer(&gEnemyParty[0]);
}

#undef VAR_CATCH_CONTEST_TYPE
#undef VAR_CATCH_CONTEST_STAT

void Rogue_CanOverLevel()
{
    gSpecialVar_Result = Rogue_GetConfigToggle(CONFIG_TOGGLE_OVER_LVL);
}

void Rogue_AnyLegendsInSafari()
{
    u8 i;

    gSpecialVar_Result = FALSE;

    for(i = ROGUE_SAFARI_LEGENDS_START_INDEX; i <ROGUE_SAFARI_TOTAL_MONS; ++i)
    {
        if(gRogueSaveBlock->safariMons[i].species != SPECIES_NONE)
        {
            gSpecialVar_Result = TRUE;
            return;
        }
    }
}

static bool8 WillSpeciesLikePokeblockInternal(u16 pokeblockItem, u16 species)
{
    u8 type = ItemId_GetSecondaryId(pokeblockItem);

    if(IS_STANDARD_TYPE(type))
    {
        if(!(RoguePokedex_GetSpeciesType(species, 0) == type || RoguePokedex_GetSpeciesType(species, 1) == type))
        {
            return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

void Rogue_CheckSafariMonLikesPokeblock()
{
    u8 safariIndex = gSpecialVar_0x8008;
    gSpecialVar_Result = WillSpeciesLikePokeblockInternal(gSpecialVar_ItemId, gRogueSaveBlock->safariMons[safariIndex].species);
}

void Rogue_AppendMultichoicePokeblockItems()
{
    u16 i;

    // Insert everything into query so we can display in alphabetical order
    RogueItemQuery_Begin();
    RogueItemQuery_Reset(QUERY_FUNC_EXCLUDE);

    for(i = FIRST_ITEM_POKEBLOCK; i <= LAST_ITEM_POKEBLOCK; ++i)
    {
        if(ItemId_GetDescription(i) != NULL)
        {
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, i);
        }
    }

    {
        u16 const* itemsList;
        RogueListQuery_Begin();

        itemsList = RogueListQuery_CollapseItems(ITEM_SORT_MODE_NAME, FALSE);

        for(;*itemsList != ITEM_NONE; ++itemsList)
        {
            u16 itemId = *itemsList;
            u8 type = ItemId_GetSecondaryId(itemId);

            if(IS_STANDARD_TYPE(type))
                ScriptMenu_ScrollingMultichoiceDynamicAppendOption(gTypeNames[type], itemId - FIRST_ITEM_POKEBLOCK);
            else
                ScriptMenu_ScrollingMultichoiceDynamicAppendOption(ItemId_GetName(itemId), itemId - FIRST_ITEM_POKEBLOCK);
        }

        RogueListQuery_End();
    }
    RogueItemQuery_End();

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(gText_Exit, MULTI_B_PRESSED);
}

void Rogue_AppendMultichoiceBerriesForPokeblock()
{
    u16 i;
    u16 targetPokeblock = gSpecialVar_0x8004;

    // Insert everything into query so we can display in alphabetical order
    RogueItemQuery_Begin();
    RogueItemQuery_Reset(QUERY_FUNC_EXCLUDE);

    for(i = FIRST_BERRY_INDEX; i <= LAST_BERRY_INDEX; ++i)
    {
        if(ItemId_GetDescription(i) != NULL && Rogue_BerryToPokeblock(i) == targetPokeblock)
        {
            RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, i);
        }
    }

    {
        u16 const* itemsList;
        RogueListQuery_Begin();

        itemsList = RogueListQuery_CollapseItems(ITEM_SORT_MODE_NAME, FALSE);

        for(;*itemsList != ITEM_NONE; ++itemsList)
        {
            u16 itemId = *itemsList;
            ScriptMenu_ScrollingMultichoiceDynamicAppendOption(ItemId_GetName(itemId), 0);
        }

        RogueListQuery_End();
    }
    RogueItemQuery_End();

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(gText_Exit, MULTI_B_PRESSED);
}

void Rogue_EnqueueSafariBattle()
{
    u8 safariIndex = gSpecialVar_0x8008;
    RogueSafari_EnqueueBattleMonByIndex(safariIndex);

    gSpecialVar_Result = gRogueSaveBlock->safariMons[safariIndex].species;
}

void Rogue_BufferSafariMonInfo()
{
    u8 safariIndex = gSpecialVar_0x8008;
    u8 const* speciesName = RoguePokedex_GetSpeciesName(gRogueSaveBlock->safariMons[safariIndex].species);

    StringCopy_Nickname(gStringVar1, gRogueSaveBlock->safariMons[safariIndex].nickname);

    if(gRogueSaveBlock->safariMons[safariIndex].shinyFlag || StringCompareN(gStringVar1, speciesName, POKEMON_NAME_LENGTH) != 0)
    {
        if(gRogueSaveBlock->safariMons[safariIndex].shinyFlag)
            StringAppend(gStringVar1, sText_TheShiny);
        else
            StringAppend(gStringVar1, sText_The);

        StringAppend(gStringVar1, speciesName);
    }
}

// Multiplayer scripts
//

void Rogue_IsMultiplayerActive(void)
{
    gSpecialVar_Result = RogueMP_IsActive();
}

void Rogue_IsMultiplayerHost(void)
{
    gSpecialVar_Result = RogueMP_IsHost();
}

void Rogue_IsMultiplayerClient(void)
{
    gSpecialVar_Result = RogueMP_IsClient();
}

void Rogue_HostMultiplayer()
{
    RogueMP_OpenHost();
    RogueMP_WaitForConnection();
}

void Rogue_JoinMultiplayer()
{
    RogueMP_OpenClient();
    RogueMP_WaitForConnection();
}

void Rogue_CloseMultiplayer()
{
    RogueMP_Close();
    //RogueMP_WaitForConnection();
}

void Rogue_IsRogueAssistantConnected()
{
    gSpecialVar_Result = Rogue_IsAssistantConnected();
}

void Rogue_IsMultiplayerAdventureJoinable(void)
{
    gSpecialVar_Result = FALSE;

    if(RogueMP_IsClient())
    {
        AGB_ASSERT(gRogueMultiplayer != NULL);
        gSpecialVar_Result = gRogueMultiplayer->gameState.adventure.isRunActive;
    }
}

void Rogue_MultiplayerRequestMon()
{
    u8 slot = gSpecialVar_0x8004;
    RogueMP_Cmd_RequestPartyMon(slot, slot);
}

void Rogue_MultiplayerRequestParty()
{
    RogueMP_Cmd_RequestPartyMon(0, PARTY_SIZE - 1);
}

void Rogue_MultiplayerTalkToPlayer()
{
    RogueMP_Cmd_RequestTalkToPlayer();
}

void Rogue_WaitForRemoteResponse()
{
    RogueMP_WaitForOutgoingCommand(FALSE);
}

void Rogue_WaitForRemoteResponseCancellable()
{
    RogueMP_WaitForOutgoingCommand(TRUE);
}

void Rogue_WaitForRemoteInput()
{
    RogueMP_WaitForIncomingCommand(FALSE);
}

void Rogue_WaitForRemoteInputCancellable()
{
    RogueMP_WaitForIncomingCommand(TRUE);
}

void Rogue_WaitForPlayerStatus()
{
    RogueMP_WaitPlayerStatusSync(FALSE);
}

void Rogue_WaitForPlayerStatusCancellable()
{
    RogueMP_WaitPlayerStatusSync(TRUE);
}

void Rogue_WaitForNextPlayerStatus()
{
    RogueMP_WaitUpdatedPlayerStatus(FALSE);
}

void Rogue_WaitForNextPlayerStatusCancellable()
{
    RogueMP_WaitUpdatedPlayerStatus(TRUE);
}

void Rogue_PrepareForTrade()
{
    u16 playerMonSlot = gSpecialVar_0x800A;
    u16 enemyMonSlot = gSpecialVar_0x800B;

    gSpecialVar_0x8005 = playerMonSlot;
    if(enemyMonSlot != 0)
    {
        CopyMon(&gEnemyParty[0], &gEnemyParty[enemyMonSlot], sizeof(struct Pokemon));
    }
}

#define VAR_WAGER_PARAM0 VAR_TEMP_1
#define VAR_WAGER_PARAM1 VAR_TEMP_2

void Rogue_BattleSim_WagerItem()
{
    // TODO - Choose special items 
    // e.g.
    // -a mega stone that a party mon can use
    // -rare candies
    // -master balls
    // -quick balls / ultra balls for vanilla
    // -expensive held items?
    u16 itemId;
    u16 amount;
    RAND_TYPE startSeed = gRngRogueValue;

    RogueItemQuery_Begin();
    RogueItemQuery_IsItemActive();

    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_ITEMS);
    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_MEDICINE);
    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_BERRIES);
    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_POKEBLOCK);
    RogueItemQuery_IsStoredInPocket(QUERY_FUNC_EXCLUDE, POCKET_KEY_ITEMS);

    RogueItemQuery_InPriceRange(QUERY_FUNC_INCLUDE, 2500, 50000);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_RARE_CANDY);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_MASTER_BALL);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_PREMIER_BALL);

#ifdef ROGUE_EXPANSION
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_ABILITY_CAPSULE);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_ABILITY_PATCH);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_QUICK_BALL);
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_DUSK_BALL);
#else
    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, ITEM_ULTRA_BALL);
    RogueMiscQuery_EditRange(QUERY_FUNC_INCLUDE, ITEM_HM01, ITEM_HM08);
#endif

    itemId = RogueMiscQuery_SelectRandomElement(RogueRandom());

    RogueItemQuery_End();

    if(itemId == ITEM_MASTER_BALL || (itemId >= ITEM_TM01 && itemId <= ITEM_HM08) || (ItemId_GetPocket(itemId) == POCKET_STONES))
    {
        amount = 1;
    }
    else
    {
        u32 targetAmount = ItemId_GetPrice(ITEM_RARE_CANDY) * 10;
        amount = targetAmount / ItemId_GetPrice(itemId);
    }

    VarSet(VAR_WAGER_PARAM0, itemId);
    VarSet(VAR_WAGER_PARAM1, amount);
    gRngRogueValue = startSeed;
}

void Rogue_BattleSim_HandleItemWager()
{
    u16 wagerItem = VarGet(VAR_WAGER_PARAM0);
    u16 wagerAmount = VarGet(VAR_WAGER_PARAM1);

    // won wager
    if(gSpecialVar_Result == TRUE)
    {
        if(AddBagItem(wagerItem, wagerAmount))
            Rogue_PushPopup_AddItem(wagerItem, wagerAmount);
        else
            Rogue_PushPopup_CannotTakeItem(wagerItem, wagerAmount);
    }
    // lost wager
    else
    {
        // Attempt to match the value of the wager
        u32 p, pocket, i;
        s32 remainingMoney = min(ItemId_GetPrice(wagerItem) * wagerAmount, ItemId_GetPrice(ITEM_RARE_CANDY) * 10);

        // Populate query with all valid items we can remove
        RogueItemQuery_Begin();
        RogueItemQuery_Reset(QUERY_FUNC_EXCLUDE);

        for(p = 0; p < POCKETS_COUNT; ++p)
        {
            pocket = p + 1; // conver to POCKET_ variant

            // Ignore these pockets
            if(pocket == POCKET_KEY_ITEMS)
                continue;

            for(i = 0; i < gBagPockets[p].capacity; ++i)
            {
                u16 itemId = BagGetItemIdByPocketPosition(pocket, i);
                if(ItemId_GetPrice(itemId) > 50)
                {
                    RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, itemId);
                }
            }
        }

        // Attempt to match item cost but at most take 5 item stacks
        for(i = 0; i < 5 && remainingMoney > 0 && RogueMiscQuery_AnyActiveElements(); ++i)
        {
            u16 itemId = RogueMiscQuery_SelectRandomElement(Random());
            u16 count = GetItemCountInBag(itemId);
            s32 stackPrice = ItemId_GetPrice(itemId) * count;

            RogueMiscQuery_EditElement(QUERY_FUNC_EXCLUDE, itemId);
            if(RemoveBagItem(itemId, count))
            {
                Rogue_PushPopup_LostItem(itemId, count);
                remainingMoney -= stackPrice;
            }
            ++i;
        }

        RogueItemQuery_End();
    }
}

void Rogue_BattleSim_HandleItemIVs()
{
    u32 ivAmount;
    u32 statId;
    u32 delta = 10;
    u16 slot = VarGet(VAR_WAGER_PARAM0);

    for(statId = MON_DATA_HP_IV; statId <= MON_DATA_SPDEF_IV; ++statId)
    {
        ivAmount = GetMonData(&gPlayerParty[slot], statId);

        if(gSpecialVar_Result) // won wager: add IVs
        {
            ivAmount += delta;
            ivAmount = min(31, ivAmount);
        }
        else // lost wager: Remove IVs
        {
            if(ivAmount < delta)
                ivAmount = 0;
            else
                ivAmount -= delta;
        }

        SetMonData(&gPlayerParty[slot], statId, &ivAmount);
        CalculateMonStats(&gPlayerParty[slot]);
    }

    Rogue_PushPopup_MonStatChange(slot, gSpecialVar_Result);
}

void Rogue_BattleSim_HandleItemMoney()
{
    // won wager
    if(gSpecialVar_Result == TRUE)
    {
        if(Rogue_GetCurrentDifficulty() >= ROGUE_ELITE_START_DIFFICULTY)
        {
            AddMoney(&gSaveBlock1Ptr->money, 15000);
            Rogue_PushPopup_AddMoney(15000);
        }
        else if(Rogue_GetCurrentDifficulty() >= ROGUE_GYM_MID_DIFFICULTY)
        {
            AddMoney(&gSaveBlock1Ptr->money, 10000);
            Rogue_PushPopup_AddMoney(10000);
        }
        else
        {
            AddMoney(&gSaveBlock1Ptr->money, 5000);
            Rogue_PushPopup_AddMoney(5000);
        }
    }
    // lost wager
    else
    {
        // take half of money
        u32 money = GetMoney(&gSaveBlock1Ptr->money) / 2;

        RemoveMoney(&gSaveBlock1Ptr->money, money);
        Rogue_PushPopup_LostMoney(money);
    }
}

#undef VAR_WAGER_PARAM0
#undef VAR_WAGER_PARAM1

void Rogue_FixPartyMonDetails()
{
    Rogue_CorrectMonDetails(gPlayerParty, gPlayerPartyCount);
}

void Rogue_IsValidAdventureToRemember()
{
    gSpecialVar_Result = gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_MOST_RECENT].isValid;
}

void Rogue_IsValidAdventureToReplay()
{
    gSpecialVar_Result = FlagGet(FLAG_ROGUE_MET_PEONIA) && gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED].isValid;
}

void Rogue_RememberAdventure()
{
    memcpy(&gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED], &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_MOST_RECENT], sizeof(struct AdventureReplay));
}

void Rogue_ShouldNursePromptConfigLabSettingsChange()
{
    u32 winStreak = GetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK);
    u32 lossStreak = GetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK);

    gSpecialVar_Result = FALSE;

    if(winStreak == 5 || winStreak == 15 || winStreak == 50)
    {
        gSpecialVar_0x8004 = 1;
        gSpecialVar_Result = TRUE;
    }
    else if(lossStreak == 5 || lossStreak == 15 || lossStreak == 50)
    {
        gSpecialVar_0x8004 = 0;
        gSpecialVar_Result = TRUE;
    }
}

bool8 Rogue_SafeSmartCheckInternal()
{
#if defined(ROGUE_FEATURE_SAFTEY_CHECKS) && ROGUE_FEATURE_SAFTEY_CHECKS == 1
    int i;
    u8 const otName[PLAYER_NAME_LENGTH + 1] = _("SMARTY");

    for (i = 0; otName[i] != 0xFF; i++)
    {
        if (otName[i] != gSaveBlock2Ptr->playerName[i])
            return FALSE;
    }
#endif
    return TRUE;
}

void Rogue_SafeSmartCheck()
{
    gSpecialVar_Result = Rogue_SafeSmartCheckInternal();
}

void Rogue_CanActivatePikinEasterEgg()
{
    gSpecialVar_Result = FALSE;

    if(!FlagGet(FLAG_ROGUE_UNLOCKED_PIKIN_EASTER_EGG))
    {
        u32 customMonId = RogueGift_GetCustomMonId(&gPlayerParty[0]);

        if(customMonId == CUSTOM_MON_ABBIE_MAREEP)
        {
            gSpecialVar_Result = TRUE;
        }
    }
}