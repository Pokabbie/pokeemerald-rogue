#ifndef ROGUE_SAFARI_H
#define ROGUE_SAFARI_H

void RogueSafari_PushMon(struct Pokemon* mon);
void RogueSafari_PushBoxMon(struct BoxPokemon* mon);

void RogueSafari_ResetSpawns();

void RogueSafari_CopyFromSafariMon(struct RogueSafariMon* fromMon, struct BoxPokemon* toMon);
void RogueSafari_CopyToSafariMon(struct BoxPokemon* fromMon, struct RogueSafariMon* toMon);

struct RogueSafariMon* RogueSafari_ChooseNewSafariMon(u8 index);
struct RogueSafariMon* RogueSafari_GetSafariMonAt(u8 index);
void RogueSafari_ClearSafariMonAt(u8 index);

void RogueSafari_EnqueueBattleMon(u8 index);
struct RogueSafariMon* RogueSafari_ConsumePendingBattleMon();

#endif