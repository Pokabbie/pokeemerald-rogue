#ifndef QUEST_MENU_H
#define QUEST_MENU_H

typedef void (*RogueQuestMenuCallback)(void);

bool8 Rogue_IsQuestMenuOverviewActive(void);

void Rogue_OpenQuestMenu(RogueQuestMenuCallback callback);

#endif