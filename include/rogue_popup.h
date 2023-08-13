#ifndef QUEST_POPUP_H
#define QUEST_POPUP_H

void Rogue_PushPopup(u8 msgType, u16 param);
void Rogue_ClearPopupQueue(void);
void Rogue_UpdatePopups(bool8 inOverworld, bool8 inputEnabled);

void Rogue_PushPartyMoveLearnPopup(u8 slotId);
void Rogue_PushPartyEvoPopup(u8 slotId);

#endif //QUEST_POPUP_H
