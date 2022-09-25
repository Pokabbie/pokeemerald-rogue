Rogue_Hub_Bakery_MapScripts::
	.byte 0


RogueHub_Bakery_Intro::
	setvar VAR_0x8004, 0
	special BufferMonNickname
	msgbox RogueHub_Bakery_Intro_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_Intro_2
RogueHub_Bakery_Intro_1:
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_Intro_4
	return

RogueHub_Bakery_Intro_2:
	msgbox RogueHub_Bakery_Intro_Text_1
	fadescreen FADE_TO_BLACK
	special Bag_ChooseBerry
	waitstate
	compare VAR_ITEM_ID, 0
	goto_if_eq RogueHub_Bakery_Intro_6
	showmoneybox 0, 0
	buffernumberstring STR_VAR_1, VAR_0x8005
	buffernumberstring STR_VAR_3, VAR_0x8006
	msgbox RogueHub_Bakery_Intro_Text_2
	checkitem VAR_ITEM_ID, VAR_0x8005
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_Intro_10
	checkmoney VAR_0x8006
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_Intro_16
RogueHub_Bakery_Intro_9:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_Intro_14
RogueHub_Bakery_Intro_13:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_Intro_19
RogueHub_Bakery_Intro_18:
	closemessage
	hidemoneybox
	goto RogueHub_Bakery_Intro_1

RogueHub_Bakery_Intro_4:
	msgbox RogueHub_Bakery_Intro_Text_15
	return

RogueHub_Bakery_Intro_6:
	setvar VAR_RESULT, NO
	goto RogueHub_Bakery_Intro_1

RogueHub_Bakery_Intro_10:
	msgbox RogueHub_Bakery_Intro_Text_3
	goto RogueHub_Bakery_Intro_9

RogueHub_Bakery_Intro_14:
	msgbox RogueHub_Bakery_Intro_Text_5, MSGBOX_YESNO
	goto RogueHub_Bakery_Intro_13

RogueHub_Bakery_Intro_16:
	msgbox RogueHub_Bakery_Intro_Text_4
	goto RogueHub_Bakery_Intro_9

RogueHub_Bakery_Intro_19:
	removemoney VAR_0x8006
	removeitem VAR_ITEM_ID, VAR_0x8005
	playse SE_SHOP
	waitse
	updatemoneybox
	msgbox RogueHub_Bakery_Intro_Text_6
	call Common_EventScript_OutOfCenterPartyHeal
	special Rogue_ApplyBerryTreat
	special BufferMonNickname
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_Intro_22
RogueHub_Bakery_Intro_21:
	msgbox RogueHub_Bakery_Intro_Text_14
	setvar VAR_RESULT, YES
	goto RogueHub_Bakery_Intro_18

RogueHub_Bakery_Intro_22:
	special Rogue_CheckBerryTreat
	switch VAR_RESULT
	case 0, RogueHub_Bakery_Intro_25
	case 1, RogueHub_Bakery_Intro_26
	case 2, RogueHub_Bakery_Intro_27
	case 3, RogueHub_Bakery_Intro_28
	case 4, RogueHub_Bakery_Intro_29
	case 5, RogueHub_Bakery_Intro_30
	case 6, RogueHub_Bakery_Intro_31
	goto RogueHub_Bakery_Intro_21

RogueHub_Bakery_Intro_25:
	msgbox RogueHub_Bakery_Intro_Text_7
	goto RogueHub_Bakery_Intro_21

RogueHub_Bakery_Intro_26:
	msgbox RogueHub_Bakery_Intro_Text_8
	goto RogueHub_Bakery_Intro_21

RogueHub_Bakery_Intro_27:
	msgbox RogueHub_Bakery_Intro_Text_9
	goto RogueHub_Bakery_Intro_21

RogueHub_Bakery_Intro_28:
	msgbox RogueHub_Bakery_Intro_Text_10
	goto RogueHub_Bakery_Intro_21

RogueHub_Bakery_Intro_29:
	msgbox RogueHub_Bakery_Intro_Text_11
	goto RogueHub_Bakery_Intro_21

RogueHub_Bakery_Intro_30:
	msgbox RogueHub_Bakery_Intro_Text_12
	goto RogueHub_Bakery_Intro_21

RogueHub_Bakery_Intro_31:
	msgbox RogueHub_Bakery_Intro_Text_13
	goto RogueHub_Bakery_Intro_21


RogueHub_Bakery_SpeakChefSlice::
	lock
	faceplayer
	bufferstring STR_VAR_2, RogueHub_Bakery_SpeakChefSlice_Text_0
	bufferstring STR_VAR_3, RogueHub_Bakery_SpeakChefSlice_Text_1
	setvar VAR_0x8005, 4
	setvar VAR_0x8006, 2000
	setvar VAR_0x8004, 2
	call RogueHub_Bakery_Intro
	release
	return


RogueHub_Bakery_SpeakChefFull::
	lock
	faceplayer
	bufferstring STR_VAR_2, RogueHub_Bakery_SpeakChefFull_Text_0
	bufferstring STR_VAR_3, RogueHub_Bakery_SpeakChefFull_Text_1
	setvar VAR_0x8005, 8
	setvar VAR_0x8006, 4000
	setvar VAR_0x8004, 4
	call RogueHub_Bakery_Intro
	release
	return


RogueHub_Bakery_SpeakBerryCheck::
	lock
	faceplayer
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_0
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakBerryCheck_2
RogueHub_Bakery_SpeakBerryCheck_1:
	release
	return

RogueHub_Bakery_SpeakBerryCheck_2:
	fadescreen FADE_TO_BLACK
	special Bag_ChooseBerry
	waitstate
	compare VAR_ITEM_ID, 0
	goto_if_ne RogueHub_Bakery_SpeakBerryCheck_4
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_4:
	special Rogue_CheckBerryTreat
	bufferitemname STR_VAR_1, VAR_ITEM_ID
	switch VAR_RESULT
	case 0, RogueHub_Bakery_SpeakBerryCheck_7
	case 1, RogueHub_Bakery_SpeakBerryCheck_8
	case 2, RogueHub_Bakery_SpeakBerryCheck_9
	case 3, RogueHub_Bakery_SpeakBerryCheck_10
	case 4, RogueHub_Bakery_SpeakBerryCheck_11
	case 5, RogueHub_Bakery_SpeakBerryCheck_12
	case 6, RogueHub_Bakery_SpeakBerryCheck_13
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_7:
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_2
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_8:
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_3
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_9:
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_4
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_10:
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_5
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_11:
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_6
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_12:
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_7
	goto RogueHub_Bakery_SpeakBerryCheck_1

RogueHub_Bakery_SpeakBerryCheck_13:
	msgbox RogueHub_Bakery_SpeakBerryCheck_Text_8
	goto RogueHub_Bakery_SpeakBerryCheck_1


RogueHub_Bakery_SpeakIVEVInspector::
	lock
	faceplayer
	msgbox RogueHub_Bakery_SpeakIVEVInspector_Text_0
	release
	return


RogueHub_Bakery_Intro_Text_0:
	.string "Welcome to the Bakery!\n"
	.string "I specialize in {STR_VAR_2}s, which\l"
	.string "can{STR_VAR_3} increase your\l"
	.string "POKéMON's stats.\p"
	.string "Would you like me to bake a\n"
	.string "{STR_VAR_2} for both you and\l"
	.string "{STR_VAR_1} to share?$"

RogueHub_Bakery_Intro_Text_1:
	.string "Which type of berry should I used?$"

RogueHub_Bakery_Intro_Text_2:
	.string "For that I'll need {STR_VAR_1}\n"
	.string "berries.\p"
	.string "And it will cost ¥{STR_VAR_3}.$"

RogueHub_Bakery_Intro_Text_3:
	.string "Oh.\n"
	.string "You don't appear to have enough\l"
	.string "berries.$"

RogueHub_Bakery_Intro_Text_4:
	.string "Oh.\n"
	.string "You don't appear to have money.$"

RogueHub_Bakery_Intro_Text_5:
	.string "Are you happy for me to take them off\n"
	.string "your hands, so I can start baking?$"

RogueHub_Bakery_Intro_Text_6:
	.string "Coming right up!$"

RogueHub_Bakery_Intro_Text_7:
	.string "{STR_VAR_1} got more friendly.$"

RogueHub_Bakery_Intro_Text_8:
	.string "{STR_VAR_1}'s HP improved.$"

RogueHub_Bakery_Intro_Text_9:
	.string "{STR_VAR_1}'s Atk improved.$"

RogueHub_Bakery_Intro_Text_10:
	.string "{STR_VAR_1}'s Def improved.$"

RogueHub_Bakery_Intro_Text_11:
	.string "{STR_VAR_1}'s SpAtk improved.$"

RogueHub_Bakery_Intro_Text_12:
	.string "{STR_VAR_1}'s SpDef improved.$"

RogueHub_Bakery_Intro_Text_13:
	.string "{STR_VAR_1}'s Speed improved.$"

RogueHub_Bakery_Intro_Text_14:
	.string "I hope you and {STR_VAR_1} enjoyed\n"
	.string "it!\p"
	.string "Hope to see you again!$"

RogueHub_Bakery_Intro_Text_15:
	.string "You're of course welcome back, if you\n"
	.string "change your mind.$"

RogueHub_Bakery_SpeakChefSlice_Text_0:
	.string "Cupcake$"

RogueHub_Bakery_SpeakChefSlice_Text_1:
	.string " slightly$"

RogueHub_Bakery_SpeakChefFull_Text_0:
	.string "Pie$"

RogueHub_Bakery_SpeakChefFull_Text_1:
	.string "$"

RogueHub_Bakery_SpeakBerryCheck_Text_0:
	.string "Did you know that different berries\n"
	.string "change different POKéMON stats when\l"
	.string "once they've been baked.$"

RogueHub_Bakery_SpeakBerryCheck_Text_1:
	.string "Do you want me to tell you which stat a\n"
	.string "berry will change?$"

RogueHub_Bakery_SpeakBerryCheck_Text_2:
	.string "{STR_VAR_1} will make a POKéMON more\n"
	.string "friendly.$"

RogueHub_Bakery_SpeakBerryCheck_Text_3:
	.string "{STR_VAR_1} will improve a POKéMON's\n"
	.string "HP.$"

RogueHub_Bakery_SpeakBerryCheck_Text_4:
	.string "{STR_VAR_1} will improve a POKéMON's\n"
	.string "Atk.$"

RogueHub_Bakery_SpeakBerryCheck_Text_5:
	.string "{STR_VAR_1} will improve a POKéMON's\n"
	.string "Def.$"

RogueHub_Bakery_SpeakBerryCheck_Text_6:
	.string "{STR_VAR_1} will improve a POKéMON's\n"
	.string "SpAtk.$"

RogueHub_Bakery_SpeakBerryCheck_Text_7:
	.string "{STR_VAR_1} will improve a POKéMON's\n"
	.string "SpDef.$"

RogueHub_Bakery_SpeakBerryCheck_Text_8:
	.string "{STR_VAR_1} will improve a POKéMON's\n"
	.string "Speed.$"

RogueHub_Bakery_SpeakIVEVInspector_Text_0:
	.string "Did you know you can check a POKéMON's\n"
	.string "IVs and EVs by pressing A on the\l"
	.string "POKéMON summary screen?\p"
	.string "Modern technology sure is great huh!$"
