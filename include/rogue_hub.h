#ifndef ROGUE_HUB_H
#define ROGUE_HUB_H

void RogueHub_Enter();
void RogueHub_Exit();

bool8 RogueHub_HasUpgrade(u16 upgradeId);
void RogueHub_SetUpgrade(u16 upgradeId, bool8 state);
bool8 RogueHub_HasUpgradeRequirements(u16 upgradeId);

u8 RogueHub_GetAreaFromCurrentMap();
void RogueHub_GetAvaliableUpgrades(u8 area, u8* outUpgrades, u8* outUpgradeCount);

void RogueHub_ApplyMapMetatiles();

void RogueHub_GetAreaUpgradesMultichoice(struct MenuAction* outList, u8* outCount, u8 listCapcity);
void RogueHub_GetUpgradeFromMultichoiceResult();
void RogueHub_ApplyHubUpgrade();

#endif