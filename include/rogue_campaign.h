#ifndef ROGUE_CAMPAIGN_H
#define ROGUE_CAMPAIGN_H

u16 Rogue_GetActiveCampaign(void);
bool8 Rogue_IsCampaignActive(void);

bool8 Rogue_TryUpdateDesiredCampaign(u16 word0, u16 word1);

u16 Rogue_PreActivateDesiredCampaign(void);
u16 Rogue_PostActivateDesiredCampaign(void);

bool8 Rogue_CheckCampaignBansItem(u16 item);

bool8 Rogue_IsActiveCampaignScored(void);
u16 Rogue_GetCampaignScore(void);
u16 Rogue_GetCampaignRunId(void);

void Rogue_CampaignNotify_StatIncrement(u8 statIndex);

#endif