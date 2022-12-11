#ifndef ROGUE_CAMPAIGN_H
#define ROGUE_CAMPAIGN_H

const u8* GetCampaignTitle(u16 campaignId);

void Rogue_ResetCampaignAfter(u16 count);

u16 Rogue_GetActiveCampaign(void);
bool8 Rogue_IsCampaignActive(void);

bool8 Rogue_TryUpdateDesiredCampaign(u16 word0, u16 word1);

u16 Rogue_PreActivateDesiredCampaign(void);
u16 Rogue_PostActivateDesiredCampaign(void);

u16 Rogue_DeactivateActiveCampaign(void);

bool8 Rogue_CheckCampaignBansItem(u16 item);

bool8 Rogue_IsActiveCampaignScored(void);
bool8 Rogue_IsActiveCampaignLowScoreGood(void);
u16 Rogue_GetCampaignScore(void);
u16 Rogue_GetCampaignRunId(void);

void Rogue_CampaignNotify_StatIncrement(u8 statIndex);
void Rogue_CampaignNotify_OnMonFainted(void);

#endif