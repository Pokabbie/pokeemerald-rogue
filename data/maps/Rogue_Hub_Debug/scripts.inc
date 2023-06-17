Rogue_Hub_Debug_MapScripts::
	.byte 0


Rogue_Debug_FillPCWithGen::
	lock
	message Rogue_Debug_FillPCWithGen_Text_0
	waitmessage
	checkitem ITEM_QUEST_LOG
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Debug_FillPCWithGen_2
Rogue_Debug_FillPCWithGen_1:
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_Debug_FillPCWithGen_5
	multichoice 20, 6, MULTI_GENERATIONS_UPTO3, NO
Rogue_Debug_FillPCWithGen_4:
	switch VAR_RESULT
	case 0, Rogue_Debug_FillPCWithGen_10
	case 1, Rogue_Debug_FillPCWithGen_10
	case 2, Rogue_Debug_FillPCWithGen_10
	case 3, Rogue_Debug_FillPCWithGen_10
	case 4, Rogue_Debug_FillPCWithGen_10
	case 5, Rogue_Debug_FillPCWithGen_10
	case 6, Rogue_Debug_FillPCWithGen_10
	case 7, Rogue_Debug_FillPCWithGen_10
	case MULTI_B_PRESSED, Rogue_Debug_FillPCWithGen_11
Rogue_Debug_FillPCWithGen_8:
	release
	return

Rogue_Debug_FillPCWithGen_2:
	giveitem ITEM_QUEST_LOG
	goto Rogue_Debug_FillPCWithGen_1

Rogue_Debug_FillPCWithGen_5:
	multichoice 20, 0, MULTI_GENERATIONS_UPTO8, NO
	goto Rogue_Debug_FillPCWithGen_4

Rogue_Debug_FillPCWithGen_10:
	waitse
	playse SE_PIN
	addvar VAR_RESULT, 1
	copyvar VAR_0x8004, VAR_RESULT
	special RogueDebug_FillGenPC
	msgbox Rogue_Debug_FillPCWithGen_Text_1, MSGBOX_DEFAULT
	goto Rogue_Debug_FillPCWithGen_8

Rogue_Debug_FillPCWithGen_11:
	msgbox Rogue_Debug_FillPCWithGen_Text_2, MSGBOX_DEFAULT
	goto Rogue_Debug_FillPCWithGen_8


Rogue_Debug_ClearQuests::
	lock
	msgbox Rogue_Debug_ClearQuests_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Debug_ClearQuests_2
Rogue_Debug_ClearQuests_1:
	release
	return

Rogue_Debug_ClearQuests_2:
	special RogueDebug_ClearQuests
	goto Rogue_Debug_ClearQuests_1


Rogue_Debug_CompleteAvaliableQuests::
	lock
	msgbox Rogue_Debug_CompleteAvaliableQuests_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Debug_CompleteAvaliableQuests_2
Rogue_Debug_CompleteAvaliableQuests_1:
	release
	return

Rogue_Debug_CompleteAvaliableQuests_2:
	special RogueDebug_CompleteAvaliableQuests
	goto Rogue_Debug_CompleteAvaliableQuests_1


Rogue_Debug_CollectAllQuests::
	lock
	msgbox Rogue_Debug_CollectAllQuests_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Debug_CollectAllQuests_2
Rogue_Debug_CollectAllQuests_1:
	release
	return

Rogue_Debug_CollectAllQuests_2:
	special RogueDebug_CollectAllQuests
	goto Rogue_Debug_CollectAllQuests_1


Rogue_Debug_AddMoney::
	lock
	addmoney 10000
	msgbox Rogue_Debug_AddMoney_Text_0, MSGBOX_DEFAULT
	checkitem ITEM_HEALING_FLASK
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Debug_AddMoney_2
Rogue_Debug_AddMoney_1:
	giveitem ITEM_STAR_PIECE, 999
	giveitem ITEM_RARE_CANDY, 999
	giveitem ITEM_ESCAPE_ROPE, 999
	release
	return

Rogue_Debug_AddMoney_2:
	giveitem ITEM_HEALING_FLASK, 1
	goto Rogue_Debug_AddMoney_1


Rogue_Debug_TrainerBattleSingle::
	lock
	clearflag FLAG_ROGUE_DOUBLE_BATTLES
	special HealPlayerParty
	special RogueDebug_StartBattle
	release
	return


Rogue_Debug_TrainerBattleDouble::
	lock
	setflag FLAG_ROGUE_DOUBLE_BATTLES
	special HealPlayerParty
	special RogueDebug_StartBattle
	release
	return


Rogue_Debug_DisplayDifficultyConfigMenu::
	lock
	msgbox Rogue_Debug_DisplayDifficultyConfigMenu_Text_0
	special Special_ViewDifficultyConfigMenu
	waitstate
	release
	return


Rogue_Debug_DisplayPokedex::
	lock
	msgbox Rogue_Debug_DisplayPokedex_Text_0
	special Special_ViewRoguePokedex
	waitstate
	release
	return


Rogue_Debug_FillPCWithGen_Text_0:
	.string "Fill PC with GEN?$"

Rogue_Debug_FillPCWithGen_Text_1:
	.string "Done.$"

Rogue_Debug_FillPCWithGen_Text_2:
	.string "No worries!\n"
	.string "Come chat to me if you ever change your\l"
	.string "mind.$"

Rogue_Debug_ClearQuests_Text_0:
	.string "ClearQuests$"

Rogue_Debug_CompleteAvaliableQuests_Text_0:
	.string "CompleteAvaliableQuests$"

Rogue_Debug_CollectAllQuests_Text_0:
	.string "CollectAllQuests$"

Rogue_Debug_AddMoney_Text_0:
	.string "Lots of stuff.$"

Rogue_Debug_DisplayDifficultyConfigMenu_Text_0:
	.string "Difficulty menu$"

Rogue_Debug_DisplayPokedex_Text_0:
	.string "Rogue Pokedex$"
