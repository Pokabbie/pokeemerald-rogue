Rogue_Hub_Bakery_MapScripts::
	.byte 0


RogueHub_Bakery_SpeakChef_ApplyTreat::
	special Rogue_ApplyBerryTreat
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChef_ApplyTreat_1
	special BufferMonNickname
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_9
	return

RogueHub_Bakery_SpeakChef_ApplyTreat_1:
	playse SE_SHOP
	waitse
	updatemoneybox
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_0
	fadescreen FADE_TO_BLACK
	playfanfare MUS_RG_HEAL
	waitfanfare
	fadescreen FADE_FROM_BLACK
	special BufferMonNickname
	special Rogue_CheckBerryTreat
	switch VAR_RESULT
	case 0, RogueHub_Bakery_SpeakChef_ApplyTreat_6
	case 1, RogueHub_Bakery_SpeakChef_ApplyTreat_7
	case 2, RogueHub_Bakery_SpeakChef_ApplyTreat_8
	case 3, RogueHub_Bakery_SpeakChef_ApplyTreat_9
	case 5, RogueHub_Bakery_SpeakChef_ApplyTreat_10
	case 6, RogueHub_Bakery_SpeakChef_ApplyTreat_11
	case 4, RogueHub_Bakery_SpeakChef_ApplyTreat_12
RogueHub_Bakery_SpeakChef_ApplyTreat_4:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_8
	return

RogueHub_Bakery_SpeakChef_ApplyTreat_6:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_1
	goto RogueHub_Bakery_SpeakChef_ApplyTreat_4

RogueHub_Bakery_SpeakChef_ApplyTreat_7:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_2
	goto RogueHub_Bakery_SpeakChef_ApplyTreat_4

RogueHub_Bakery_SpeakChef_ApplyTreat_8:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_3
	goto RogueHub_Bakery_SpeakChef_ApplyTreat_4

RogueHub_Bakery_SpeakChef_ApplyTreat_9:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_4
	goto RogueHub_Bakery_SpeakChef_ApplyTreat_4

RogueHub_Bakery_SpeakChef_ApplyTreat_10:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_5
	goto RogueHub_Bakery_SpeakChef_ApplyTreat_4

RogueHub_Bakery_SpeakChef_ApplyTreat_11:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_6
	goto RogueHub_Bakery_SpeakChef_ApplyTreat_4

RogueHub_Bakery_SpeakChef_ApplyTreat_12:
	msgbox RogueHub_Bakery_SpeakChef_ApplyTreat_Text_7
	goto RogueHub_Bakery_SpeakChef_ApplyTreat_4


RogueHub_Bakery_SpeakChefSlice::
	lock
	faceplayer
	setvar VAR_0x8004, 0
	special BufferMonNickname
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_2
RogueHub_Bakery_SpeakChefSlice_1:
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_5
RogueHub_Bakery_SpeakChefSlice_4:
	release
	return

RogueHub_Bakery_SpeakChefSlice_2:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_1
	fadescreen FADE_TO_BLACK
	special Bag_ChooseBerry
	waitstate
	compare VAR_ITEM_ID, 0
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_7
	showmoneybox 0, 0
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_2
	checkitem VAR_ITEM_ID, 3
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_11
	checkmoney 500
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_17
RogueHub_Bakery_SpeakChefSlice_10:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_15
RogueHub_Bakery_SpeakChefSlice_14:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_20
RogueHub_Bakery_SpeakChefSlice_19:
	closemessage
	hidemoneybox
	goto RogueHub_Bakery_SpeakChefSlice_1

RogueHub_Bakery_SpeakChefSlice_5:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_6
	goto RogueHub_Bakery_SpeakChefSlice_4

RogueHub_Bakery_SpeakChefSlice_7:
	setvar VAR_RESULT, NO
	goto RogueHub_Bakery_SpeakChefSlice_1

RogueHub_Bakery_SpeakChefSlice_11:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_3
	goto RogueHub_Bakery_SpeakChefSlice_10

RogueHub_Bakery_SpeakChefSlice_15:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_5, MSGBOX_YESNO
	goto RogueHub_Bakery_SpeakChefSlice_14

RogueHub_Bakery_SpeakChefSlice_17:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_4
	goto RogueHub_Bakery_SpeakChefSlice_10

RogueHub_Bakery_SpeakChefSlice_20:
	setvar VAR_0x8005, 1
	call RogueHub_Bakery_SpeakChef_ApplyTreat
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefSlice_22
	goto RogueHub_Bakery_SpeakChefSlice_19

RogueHub_Bakery_SpeakChefSlice_22:
	removemoney 500
	removeitem VAR_ITEM_ID, 3
	goto RogueHub_Bakery_SpeakChefSlice_19


RogueHub_Bakery_SpeakChefFull::
	lock
	faceplayer
	setvar VAR_0x8004, 0
	special BufferMonNickname
	msgbox RogueHub_Bakery_SpeakChefFull_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefFull_2
RogueHub_Bakery_SpeakChefFull_1:
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_SpeakChefFull_5
RogueHub_Bakery_SpeakChefFull_4:
	release
	return

RogueHub_Bakery_SpeakChefFull_2:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_1
	fadescreen FADE_TO_BLACK
	special Bag_ChooseBerry
	waitstate
	compare VAR_ITEM_ID, 0
	goto_if_eq RogueHub_Bakery_SpeakChefFull_7
	showmoneybox 0, 0
	msgbox RogueHub_Bakery_SpeakChefFull_Text_1
	checkitem VAR_ITEM_ID, 15
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_SpeakChefFull_11
	checkmoney 2500
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_Bakery_SpeakChefFull_17
RogueHub_Bakery_SpeakChefFull_10:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefFull_15
RogueHub_Bakery_SpeakChefFull_14:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefFull_20
RogueHub_Bakery_SpeakChefFull_19:
	closemessage
	hidemoneybox
	goto RogueHub_Bakery_SpeakChefFull_1

RogueHub_Bakery_SpeakChefFull_5:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_6
	goto RogueHub_Bakery_SpeakChefFull_4

RogueHub_Bakery_SpeakChefFull_7:
	setvar VAR_RESULT, NO
	goto RogueHub_Bakery_SpeakChefFull_1

RogueHub_Bakery_SpeakChefFull_11:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_3
	goto RogueHub_Bakery_SpeakChefFull_10

RogueHub_Bakery_SpeakChefFull_15:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_5, MSGBOX_YESNO
	goto RogueHub_Bakery_SpeakChefFull_14

RogueHub_Bakery_SpeakChefFull_17:
	msgbox RogueHub_Bakery_SpeakChefSlice_Text_4
	goto RogueHub_Bakery_SpeakChefFull_10

RogueHub_Bakery_SpeakChefFull_20:
	setvar VAR_0x8005, 5
	call RogueHub_Bakery_SpeakChef_ApplyTreat
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_Bakery_SpeakChefFull_22
	goto RogueHub_Bakery_SpeakChefFull_19

RogueHub_Bakery_SpeakChefFull_22:
	removemoney 2500
	removeitem VAR_ITEM_ID, 15
	goto RogueHub_Bakery_SpeakChefFull_19


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
	case 5, RogueHub_Bakery_SpeakBerryCheck_11
	case 6, RogueHub_Bakery_SpeakBerryCheck_12
	case 4, RogueHub_Bakery_SpeakBerryCheck_13
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


RogueHub_Bakery_SpeakChef_ApplyTreat_Text_0:
	.string "Coming right up!$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_1:
	.string "{STR_VAR_1} got more friendly.$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_2:
	.string "{STR_VAR_1}'s HP improved.$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_3:
	.string "{STR_VAR_1}'s Atk improved.$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_4:
	.string "{STR_VAR_1}'s Def improved.$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_5:
	.string "{STR_VAR_1}'s SpAtk improved.$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_6:
	.string "{STR_VAR_1}'s SpDef improved.$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_7:
	.string "{STR_VAR_1}'s Speed improved.$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_8:
	.string "I hope you and {STR_VAR_1} enjoyed\n"
	.string "it!\p"
	.string "Hope to see you again!$"

RogueHub_Bakery_SpeakChef_ApplyTreat_Text_9:
	.string "Oh.\p"
	.string "{STR_VAR_1} looks too full to eat\n"
	.string "this.$"

RogueHub_Bakery_SpeakChefSlice_Text_0:
	.string "Welcome to the Bakery!\n"
	.string "I specialize in Cupcakes, which can\l"
	.string "slightly increase your POKéMON's\l"
	.string "stats.\p"
	.string "Would you like me to bake a Cupcake for\n"
	.string "both you and {STR_VAR_1} to share?$"

RogueHub_Bakery_SpeakChefSlice_Text_1:
	.string "Which type of berry should I used?$"

RogueHub_Bakery_SpeakChefSlice_Text_2:
	.string "For that I'll need 3 berries.\p"
	.string "And it will cost ¥500.$"

RogueHub_Bakery_SpeakChefSlice_Text_3:
	.string "Oh.\n"
	.string "You don't appear to have enough\l"
	.string "berries.$"

RogueHub_Bakery_SpeakChefSlice_Text_4:
	.string "Oh.\n"
	.string "You don't appear to have money.$"

RogueHub_Bakery_SpeakChefSlice_Text_5:
	.string "Are you happy for me to take them off\n"
	.string "your hands, so I can start baking?$"

RogueHub_Bakery_SpeakChefSlice_Text_6:
	.string "You're of course welcome back, if you\n"
	.string "change your mind.$"

RogueHub_Bakery_SpeakChefFull_Text_0:
	.string "Welcome to the Bakery!\n"
	.string "I specialize in Pies, which can increase\l"
	.string "your POKéMON's stats.\p"
	.string "Would you like me to bake a Pie for both\n"
	.string "you and {STR_VAR_1} to share?$"

RogueHub_Bakery_SpeakChefFull_Text_1:
	.string "For that I'll need 15 berries.\p"
	.string "And it will cost ¥2500.$"

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
