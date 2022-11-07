Rogue_HubTransition_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_HubTransition_OnLoad
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerNorth
	.2byte 0

Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_HubTransition_OnEnter
	.2byte 0


Rogue_HubTransition_OnEnter::
	lock
	goto_if_set FLAG_ROGUE_TRAINERS_STRONG_LEGENDARIES, Rogue_HubTransition_OnEnter_2
	goto_if_set FLAG_ROGUE_TRAINERS_WEAK_LEGENDARIES, Rogue_HubTransition_OnEnter_3
Rogue_HubTransition_OnEnter_1:
	release
	setvar VAR_TEMP_1, 1
	end

Rogue_HubTransition_OnEnter_2:
	msgbox Rogue_HubTransition_OnEnter_Text_0
	goto Rogue_HubTransition_OnEnter_1

Rogue_HubTransition_OnEnter_3:
	msgbox Rogue_HubTransition_OnEnter_Text_1
	goto Rogue_HubTransition_OnEnter_1


Rogue_HubTransition_OnLoad::
	questcollected QUEST_DevilDeal
	compare VAR_RESULT, NO
	goto_if_eq Rogue_HubTransition_OnLoad_1
	return

Rogue_HubTransition_OnLoad_1:
	setflag FLAG_TEMP_1
	return


Rogue_HubTransition_RandomMan::
	lock
	applymovement 1, Rogue_RestStop_RandomMan_Spin
	waitmovement 0
	faceplayer
	msgbox Rogue_HubTransition_RandomMan_Text_0, MSGBOX_DEFAULT
	msgbox Rogue_HubTransition_RandomMan_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_HubTransition_RandomMan_2
	msgbox Rogue_HubTransition_RandomMan_Text_6, MSGBOX_DEFAULT
Rogue_HubTransition_RandomMan_1:
	release
	return

Rogue_HubTransition_RandomMan_2:
	goto_if_set FLAG_SET_SEED_ENABLED, Rogue_HubTransition_RandomMan_6
Rogue_HubTransition_RandomMan_5:
	setvar VAR_0x8004, 255
	fadescreen FADE_TO_BLACK
	playfanfare MUS_MOVE_DELETED
	special Rogue_RandomisePartyMon
	waitfanfare
	fadescreen FADE_FROM_BLACK
	msgbox Rogue_HubTransition_RandomMan_Text_5
	applymovement 1, Rogue_RestStop_RandomMan_RunAway
	waitmovement 0
	setflag FLAG_ROGUE_RANDOM_TRADE_DISABLED
	removeobject 1
	goto Rogue_HubTransition_RandomMan_1

Rogue_HubTransition_RandomMan_6:
	msgbox Rogue_HubTransition_RandomMan_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_HubTransition_RandomMan_8
	msgbox Rogue_HubTransition_RandomMan_Text_4, MSGBOX_DEFAULT
	goto Rogue_HubTransition_RandomMan_5

Rogue_HubTransition_RandomMan_8:
	msgbox Rogue_HubTransition_RandomMan_Text_3, MSGBOX_DEFAULT
	special Rogue_SeedRandomGenerators
	goto Rogue_HubTransition_RandomMan_5


Rogue_HubTransition_OnEnter_Text_0:
	.string "Strong Legendary clause activated...$"

Rogue_HubTransition_OnEnter_Text_1:
	.string "Basic Legendary clause activated...$"

Rogue_HubTransition_RandomMan_Text_0:
	.string "WAHEY!\p"
	.string "You looking to add a bit of randomness\n"
	.string "to you life?$"

Rogue_HubTransition_RandomMan_Text_1:
	.string "You wanna trade me your POKéMON?$"

Rogue_HubTransition_RandomMan_Text_2:
	.string "Oh you have SEEDED mode on do ya?\p"
	.string "Did you want this trade to be for a\n"
	.string "SEEDED POKéMON too?$"

Rogue_HubTransition_RandomMan_Text_3:
	.string "Okidoke!\n"
	.string "One SEEDED POKéMON coming up!$"

Rogue_HubTransition_RandomMan_Text_4:
	.string "Okidoke!\n"
	.string "One surpise POKéMON coming up!$"

Rogue_HubTransition_RandomMan_Text_5:
	.string "Ke ke ke!\p"
	.string "Enjoy!$"

Rogue_HubTransition_RandomMan_Text_6:
	.string "Blagh!\n"
	.string "How boring...$"
