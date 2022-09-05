Rogue_Encounter_RestStop_MapScripts::
	.byte 0


Rogue_Encounter_RestStop_Heal::
	lock
	faceplayer
	msgbox Rogue_Encounter_RestStop_Heal_Text_0, MSGBOX_DEFAULT
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox Rogue_Encounter_RestStop_Heal_Text_1, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_RandomMan::
	lock
	applymovement 1, Rogue_RestStop_RandomMan_Spin
	waitmovement 0
	faceplayer
	msgbox Rogue_Encounter_RestStop_RandomMan_Text_0, MSGBOX_DEFAULT
	showmoneybox 0, 0
	msgbox Rogue_Encounter_RestStop_RandomMan_Text_1, MSGBOX_DEFAULT
	message Rogue_Encounter_RestStop_RandomMan_Text_2
	waitmessage
	multichoice 17, 6, MULTI_RANDOM_SWAP_OPTIONS, NO
	hidemoneybox
	switch VAR_RESULT
	case 0, Rogue_Encounter_RestStop_RandomMan_3
	case 1, Rogue_Encounter_RestStop_RandomMan_4
	case MULTI_B_PRESSED, Rogue_Encounter_RestStop_RandomMan_5
Rogue_Encounter_RestStop_RandomMan_1:
	release
	return

Rogue_Encounter_RestStop_RandomMan_3:
	checkmoney 1000
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_RestStop_RandomMan_6
	msgbox Rogue_Encounter_RestStop_RandomMan_Text_5, MSGBOX_DEFAULT
	goto Rogue_Encounter_RestStop_RandomMan_1

Rogue_Encounter_RestStop_RandomMan_4:
	setvar VAR_0x8004, 255
	fadescreen FADE_TO_BLACK
	playfanfare MUS_MOVE_DELETED
	special Rogue_RandomisePartyMon
	waitfanfare
	fadescreen FADE_FROM_BLACK
	msgbox Rogue_Encounter_RestStop_RandomMan_Text_4
	applymovement 1, Rogue_RestStop_RandomMan_RunAway
	waitmovement 0
	setflag FLAG_ROGUE_RANDOM_TRADE_DISABLED
	setflag FLAG_ROGUE_RANDOM_TRADE_USED
	removeobject 1
	goto Rogue_Encounter_RestStop_RandomMan_1

Rogue_Encounter_RestStop_RandomMan_5:
	msgbox Rogue_Encounter_RestStop_RandomMan_Text_3, MSGBOX_DEFAULT
	goto Rogue_Encounter_RestStop_RandomMan_1

Rogue_Encounter_RestStop_RandomMan_6:
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Encounter_RestStop_RandomMan_9
	removemoney 1000
	updatemoneybox
	fadescreen FADE_TO_BLACK
	playfanfare MUS_MOVE_DELETED
	special Rogue_RandomisePartyMon
	waitfanfare
	fadescreen FADE_FROM_BLACK
	msgbox Rogue_Encounter_RestStop_RandomMan_Text_4
	applymovement 1, Rogue_RestStop_RandomMan_RunAway
	waitmovement 0
	setflag FLAG_ROGUE_RANDOM_TRADE_DISABLED
	setflag FLAG_ROGUE_RANDOM_TRADE_USED
	removeobject 1
	goto Rogue_Encounter_RestStop_RandomMan_1

Rogue_Encounter_RestStop_RandomMan_9:
	msgbox Rogue_Encounter_RestStop_RandomMan_Text_3, MSGBOX_DEFAULT
	goto Rogue_Encounter_RestStop_RandomMan_1


Rogue_RestStop_RandomMan_Spin:
	face_right
	delay_1
	face_up
	delay_1
	face_left
	delay_1
	face_down
	delay_1
	face_right
	delay_1
	face_up
	delay_1
	face_left
	delay_1
	face_down
	delay_1
	jump_in_place_down
	step_end

Rogue_RestStop_RandomMan_RunAway:
	jump_2_right
	jump_2_right
	jump_2_right
	jump_2_right
	step_end

Rogue_Encounter_RestStop_Heal_Text_0:
	.string "You look tired.\n"
	.string "Let me patch you up!$"

Rogue_Encounter_RestStop_Heal_Text_1:
	.string "Take care!$"

Rogue_Encounter_RestStop_RandomMan_Text_0:
	.string "WAHEY!\p"
	.string "You looking to add a bit of randomness\n"
	.string "to you life?\p"
	.string "Tell you what...$"

Rogue_Encounter_RestStop_RandomMan_Text_1:
	.string "For ¥1000 I'll trade you one POKéMON.\n"
	.string "OR!\p"
	.string "I'll trade ALL your POKéMON for free?$"

Rogue_Encounter_RestStop_RandomMan_Text_2:
	.string "What do you say?$"

Rogue_Encounter_RestStop_RandomMan_Text_3:
	.string "Blagh!\n"
	.string "How boring...$"

Rogue_Encounter_RestStop_RandomMan_Text_4:
	.string "Ke ke ke!\p"
	.string "Enjoy!$"

Rogue_Encounter_RestStop_RandomMan_Text_5:
	.string "Blagh!\n"
	.string "How boring...\p"
	.string "Come back with some more money.$"
