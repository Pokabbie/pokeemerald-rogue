#include "global.h"
#include "constants/game_stat.h"
#include "constants/items.h"

#include "event_data.h"
#include "item.h"
#include "pokemon.h"

#include "rogue_campaign.h"
#include "rogue_controller.h"

static void Campaign_LowBst_RecalculateScore(void);
static u16 Campaign_LowBst_ScoreFromSpecies(u16 species);

u16 Rogue_GetActiveCampaign(void)
{
    u16 desiredCampaign;

    if(!Rogue_IsRunActive() || VarGet(VAR_ROGUE_SKIP_TO_DIFFICULTY) != 0)
        return ROGUE_CAMPAIGN_NONE;

    desiredCampaign = VarGet(VAR_ROGUE_DESIRED_CAMPAIGN);

    switch (desiredCampaign)
    {
    case ROGUE_CAMPAIGN_LOW_BST:
        if(FlagGet(FLAG_ROGUE_RAINBOW_MODE) || FlagGet(FLAG_ROGUE_GAUNTLET_MODE))
            return ROGUE_CAMPAIGN_NONE;
        break;

    default:
        break;
    }

    return desiredCampaign;
}

bool8 Rogue_IsCampaignActive(void)
{
    return Rogue_GetActiveCampaign() != ROGUE_CAMPAIGN_NONE;
}

bool8 Rogue_TryUpdateDesiredCampaign(u16 word0, u16 word1)
{
    if(word0 == 5160 && word1 == 6705) // SMALL HOLIDAY
    {
        VarSet(VAR_ROGUE_DESIRED_CAMPAIGN, ROGUE_CAMPAIGN_LOW_BST);
        return TRUE;
    }

    if(word0 == 9843 && word1 == 6699) // REFLECT ADVENTURE
    {
        VarSet(VAR_ROGUE_DESIRED_CAMPAIGN, ROGUE_CAMPAIGN_CLASSIC);
        return TRUE;
    }

    VarSet(VAR_ROGUE_DESIRED_CAMPAIGN, ROGUE_CAMPAIGN_NONE);
    return FALSE;
}

u16 Rogue_PreActivateDesiredCampaign(void)
{
    switch (Rogue_GetActiveCampaign())
    {
    case ROGUE_CAMPAIGN_LOW_BST:
        Rogue_ResetConfigHubSettings();

        FlagSet(FLAG_ROGUE_FORCE_BASIC_BAG);

        // Expansion Room settings
#ifdef ROGUE_EXPANSION
        VarSet(VAR_ROGUE_ENABLED_GEN_LIMIT, 8);
#else
        VarSet(VAR_ROGUE_ENABLED_GEN_LIMIT, 3);
#endif
        VarSet(VAR_ROGUE_REGION_DEX_LIMIT, 0);

        FlagSet(FLAG_ROGUE_HOENN_BOSSES);
        FlagSet(FLAG_ROGUE_KANTO_BOSSES);
        FlagSet(FLAG_ROGUE_JOHTO_BOSSES);
        break;
    }
}

u16 Rogue_PostActivateDesiredCampaign(void)
{
    switch (Rogue_GetActiveCampaign())
    {
    case ROGUE_CAMPAIGN_LOW_BST:
        {
            u16 i;
            gRogueRun.campaignData.lowBst.scoreSpecies = SPECIES_NONE;
            Campaign_LowBst_RecalculateScore();

            for(i = 0; i < PARTY_SIZE; ++i)
                ZeroMonData(&gPlayerParty[i]);
        
            // Force sunkern start
            CreateMon(&gPlayerParty[0], SPECIES_SUNKERN, 15, USE_RANDOM_IVS, 0, 0, OT_ID_PLAYER_ID, 0);
            CalculatePlayerPartyCount();

            AddBagItem(ITEM_EVERSTONE_CURSE, 1);
            AddBagItem(ITEM_LINK_CABLE, 10);
            
            Campaign_LowBst_RecalculateScore();
        }
        break;
    }
}

bool8 Rogue_IsActiveCampaignScored(void)
{
    switch (Rogue_GetActiveCampaign())
    {
    case ROGUE_CAMPAIGN_LOW_BST:
        return TRUE;
    }

    return FALSE;
}

u16 Rogue_GetCampaignScore(void)
{
    switch (Rogue_GetActiveCampaign())
    {
    case ROGUE_CAMPAIGN_LOW_BST:
        return Campaign_LowBst_ScoreFromSpecies(gRogueRun.campaignData.lowBst.scoreSpecies);
    }

    return 0;
}

u16 Rogue_GetCampaignRunId(void)
{
    // Some basic verification for screenshots, do bitwise XOR on this and score and then bitflip
    u16 trainerId = (gSaveBlock2Ptr->playerTrainerId[0]) | (gSaveBlock2Ptr->playerTrainerId[1] << 8);
    u16 score = Rogue_GetCampaignScore();

    return ~(trainerId ^ score);
}

bool8 Rogue_CheckCampaignBansItem(u16 item)
{
    switch (Rogue_GetActiveCampaign())
    {
    case ROGUE_CAMPAIGN_LOW_BST:
        {
            if(item == ITEM_TM06_TOXIC)
                return TRUE;
        }
        break;
    }

    return FALSE;
}

void Rogue_CampaignNotify_StatIncrement(u8 statIndex)
{
    if(!Rogue_IsCampaignActive())
        return;

    switch (statIndex)
    {
    case GAME_STAT_TOTAL_BATTLES:
        if(Rogue_GetActiveCampaign() == ROGUE_CAMPAIGN_LOW_BST)
            Campaign_LowBst_RecalculateScore();
        break;
    
    default:
        break;
    } 
}

static void Campaign_LowBst_RecalculateScore(void)
{
    u16 i;
    u16 tempSpecies;
    u16 tempScore;
    u16 currentSpecies =  gRogueRun.campaignData.lowBst.scoreSpecies;
    u16 currentScore = Campaign_LowBst_ScoreFromSpecies(currentSpecies);

    // Get lowest BST
    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        tempSpecies = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if(tempSpecies != SPECIES_NONE)
        {
            tempScore = Campaign_LowBst_ScoreFromSpecies(tempSpecies);

            if(tempScore < currentScore || currentScore == 0)
            {
                currentSpecies = tempSpecies;
                currentScore = tempScore;
            }
        }
    }

    gRogueRun.campaignData.lowBst.scoreSpecies = currentSpecies;
}

static u16 Campaign_LowBst_ScoreFromSpecies(u16 species)
{
    if(species == SPECIES_NONE)
        return 0;

    return gBaseStats[species].baseHP +
        gBaseStats[species].baseAttack +
        gBaseStats[species].baseDefense +
        gBaseStats[species].baseSpeed +
        gBaseStats[species].baseSpAttack +
        gBaseStats[species].baseSpDefense;
}