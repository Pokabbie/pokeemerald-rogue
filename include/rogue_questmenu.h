#ifndef QUEST_MENU_H
#define QUEST_MENU_H

typedef void (*RogueQuestMenuCallback)(void);

void Rogue_OpenQuestMenu(RogueQuestMenuCallback callback);

#endif