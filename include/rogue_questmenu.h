#ifndef ROGUE_QUEST_MENU_H
#define ROGUE_QUEST_MENU_H

typedef void (*RogueQuestMenuCallback)(void);

void Rogue_OpenQuestMenu(RogueQuestMenuCallback callback);

#endif