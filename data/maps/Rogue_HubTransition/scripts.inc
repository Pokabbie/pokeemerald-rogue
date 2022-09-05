Rogue_HubTransition_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerNorth
	.2byte 0


Rogue_HubTransition_RandomMan::
	lock
	applymovement 1, Rogue_RestStop_RandomMan_Spin
	waitmovement 0
	faceplayer
	msgbox Rogue_HubTransition_RandomMan_Text_0, MSGBOX_DEFAULT
	msgbox Rogue_HubTransition_RandomMan_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_HubTransition_RandomMan_2
	msgbox Rogue_HubTransition_RandomMan_Text_3, MSGBOX_DEFAULT
Rogue_HubTransition_RandomMan_1:
	release
	return

Rogue_HubTransition_RandomMan_2:
	setvar VAR_0x8004, 255
	fadescreen FADE_TO_BLACK
	playfanfare MUS_MOVE_DELETED
	special Rogue_RandomisePartyMon
	waitfanfare
	fadescreen FADE_FROM_BLACK
	msgbox Rogue_HubTransition_RandomMan_Text_2
	applymovement 1, Rogue_RestStop_RandomMan_RunAway
	waitmovement 0
	setflag FLAG_ROGUE_RANDOM_TRADE_DISABLED
	setflag FLAG_ROGUE_RANDOM_TRADE_USED
	removeobject 1
	goto Rogue_HubTransition_RandomMan_1


Rogue_HubTransition_RandomMan_Text_0:
	.string "WAHEY!\p"
	.string "You looking to add a bit of randomness\n"
	.string "to you life?$"

Rogue_HubTransition_RandomMan_Text_1:
	.string "You wanna trade me your POKéMON?$"

Rogue_HubTransition_RandomMan_Text_2:
	.string "Ke ke ke!\p"
	.string "Enjoy!$"

Rogue_HubTransition_RandomMan_Text_3:
	.string "Blagh!\n"
	.string "How boring...$"
