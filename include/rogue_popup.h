#ifndef QUEST_POPUP_H
#define QUEST_POPUP_H

void Rogue_ClearPopupQueue(void);
void Rogue_UpdatePopups(bool8 inOverworld, bool8 inputEnabled);

void Rogue_ForceEnablePopups(bool8 allowAudio);
bool8 Rogue_HasPendingPopups();

void Rogue_PushPopup_NextPartyNotification();
void Rogue_PushPopup_NewMoves(u8 slotId);
void Rogue_PushPopup_NewEvos(u8 slotId);
void Rogue_PushPopup_UnableToEvolve(u8 slotId);

void Rogue_PushPopup_QuestComplete(u16 questId);
void Rogue_PushPopup_QuestFail(u16 questId);
void Rogue_PushPopup_QuestUnlocked(u16 questId);

void Rogue_PushPopup_PokemonChain(u16 species, u16 chainSize);
void Rogue_PushPopup_PokemonChainBroke(u16 species);

void Rogue_PushPopup_WeakPokemonClause(u16 species);
void Rogue_PushPopup_StrongPokemonClause(u16 species);
void Rogue_PushPopup_RoamerPokemonActivated(u16 species);

void Rogue_PushPopup_AddItem(u16 itemId, u16 amount);
void Rogue_PushPopup_AddBerry(u16 itemId, u16 amount);
void Rogue_PushPopup_CannotTakeItem(u16 itemId, u16 amount);

void Rogue_PushPopup_RequipBerrySuccess(u16 itemId);
void Rogue_PushPopup_RequipBerryFail(u16 itemId);

void Rogue_PushPopup_TriggerExtraLife();

void Rogue_PushPopup_UnlockPokedex();
void Rogue_PushPopup_UpgradePokedex();

void Rogue_PushPopup_AssistantConnected();
void Rogue_PushPopup_AssistantDisconnected();

void Rogue_PushPopup_OutfitUnlocked();

#endif //QUEST_POPUP_H
