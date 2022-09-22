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


Rogue_Debug_AddMoney::
	lock
	addmoney 10000
	msgbox Rogue_Debug_AddMoney_Text_0, MSGBOX_DEFAULT
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

Rogue_Debug_AddMoney_Text_0:
	.string "Money added.$"
