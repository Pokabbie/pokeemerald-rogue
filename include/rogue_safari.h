#ifndef ROGUE_SAFARI_H
#define ROGUE_SAFARI_H

void RogueSafari_PushMon(struct Pokemon* mon);
void RogueSafari_PushBoxMon(struct BoxPokemon* mon);
void RogueSafari_PushLowPriorityMon(struct Pokemon* mon);

void RogueSafari_ResetSpawns();

void RogueSafari_CopyFromSafariMon(struct RogueSafariMon* fromMon, struct BoxPokemon* toMon);
void RogueSafari_CopyToSafariMon(struct BoxPokemon* fromMon, struct RogueSafariMon* toMon);

struct RogueSafariMon* RogueSafari_ChooseSafariMonForSlot(u8 slot);
void RogueSafari_RemoveMonFromSlot(u8 slot);

struct RogueSafariMon* RogueSafari_GetSafariMonAt(u8 index);
void RogueSafari_ClearSafariMonAtIdx(u8 index);

void RogueSafari_EnqueueBattleMon(u8 slot);
void RogueSafari_EnqueueBattleMonByIndex(u8 index);
u8 RogueSafari_GetPendingBattleMonIdx();
struct RogueSafariMon* RogueSafari_GetPendingBattleMon();

u16 RogueSafari_GetActivePokeballType();
void RogueSafari_SetActivePokeballType(u16 itemId);

void RogueSafari_CompactEmptyEntries();

#endif