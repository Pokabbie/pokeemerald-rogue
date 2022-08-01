Rogue_Hub_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RogueHub_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, RogueHub_OnMapLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 0, RogueHub_Intro0
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 5, RogueHub_ReturnAdventure
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 6, RogueHub_ReturnAdventure
	.2byte 0


RogueHub_OnTransition::
	compare VAR_LITTLEROOT_INTRO_STATE, 2
	goto_if_le RogueHub_OnTransition_1
	return

RogueHub_OnTransition_1:
	setobjectxyperm 2, 23, 6
	return


RogueHub_OnMapLoad::
	compare VAR_LITTLEROOT_INTRO_STATE, 4
	goto_if_le RogueHub_OnMapLoad_2
	clearflag FLAG_ROGUE_TRAINER0
RogueHub_OnMapLoad_1:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 1
	goto_if_lt RogueHub_OnMapLoad_6
	clearflag FLAG_ROGUE_TRAINER4
RogueHub_OnMapLoad_5:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 2
	goto_if_lt RogueHub_OnMapLoad_10
	clearflag FLAG_ROGUE_TRAINER1
RogueHub_OnMapLoad_9:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 3
	goto_if_lt RogueHub_OnMapLoad_14
	clearflag FLAG_ROGUE_TRAINER2
RogueHub_OnMapLoad_13:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 4
	goto_if_lt RogueHub_OnMapLoad_17
	clearflag FLAG_ROGUE_TRAINER3
	return

RogueHub_OnMapLoad_2:
	fillmetatile 31, 10, 40, 16, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER0
	goto RogueHub_OnMapLoad_1

RogueHub_OnMapLoad_6:
	fillmetatile 1, 3, 14, 10, 0x001, FALSE
	fillmetatile 1, 11, 12, 11, 0x001, FALSE
	fillmetatile 1, 12, 6, 12, 0x001, FALSE
	fillmetatile 31, 21, 37, 26, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER4
	goto RogueHub_OnMapLoad_5

RogueHub_OnMapLoad_10:
	fillmetatile 22, 12, 27, 16, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER1
	fillmetatile 41, 20, 48, 25, 0x001, FALSE
	goto RogueHub_OnMapLoad_9

RogueHub_OnMapLoad_14:
	fillmetatile 42, 14, 48, 19, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER2
	goto RogueHub_OnMapLoad_13

RogueHub_OnMapLoad_17:
	fillmetatile 13, 27, 27, 36, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER3
	fillmetatile 33, 8, 35, 8, 0x001, FALSE
	return


RogueHub_Intro0::
	lockall
	playse SE_LEDGE
	applymovement OBJ_EVENT_ID_PLAYER, RogueHub_Intro_StepOffTruck
	waitmovement 0
	applymovement 2, RogueHub_Intro_ScottGreetAtTruck
	waitmovement 0
	applymovement OBJ_EVENT_ID_PLAYER, RogueHub_Intro_PlayerGreetAtTruck
	waitmovement 0
	msgbox RogueHub_Intro0_Text_0
	applymovement 2, RogueHub_Intro_ScottWalkToPost
	waitmovement 0
	setobjectxyperm 2, 29, 6
	setvar VAR_LITTLEROOT_INTRO_STATE, 3
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_TRUCK
	releaseall
	end


RogueHub_Intro_StepOffTruck:
	jump_right
	delay_16
	delay_16
	step_end

RogueHub_Intro_PlayerGreetAtTruck:
	face_up
	step_end

RogueHub_Intro_ScottGreetAtTruck:
	walk_left
	walk_up
	walk_right
	walk_right
	walk_right
	walk_down
	step_end

RogueHub_Intro_ScottWalkToPost:
	walk_right
	walk_right
	walk_right
	walk_right
	face_down
	step_end

RogueHub_ReturnAdventure::
	compare VAR_TEMP_0, 1
	goto_if_ne RogueHub_ReturnAdventure_1
	return

RogueHub_ReturnAdventure_1:
	lockall
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	compare VAR_LITTLEROOT_INTRO_STATE, 5
	goto_if_eq RogueHub_ReturnAdventure_4
RogueHub_ReturnAdventure_3:
	buffernumberstring STR_VAR_1, VAR_ROGUE_REWARD_MONEY
	buffernumberstring STR_VAR_2, VAR_ROGUE_REWARD_CANDY
	message RogueHub_ReturnAdventure_Text_3
	playfanfare MUS_OBTAIN_ITEM
	waitmessage
	waitfanfare
	compare VAR_ROGUE_REWARD_CANDY, 0
	goto_if_ne RogueHub_ReturnAdventure_7
RogueHub_ReturnAdventure_6:
	compare VAR_LITTLEROOT_INTRO_STATE, 5
	goto_if_eq RogueHub_ReturnAdventure_10
RogueHub_ReturnAdventure_9:
	clearflag FLAG_ROGUE_LEFT_ON_RUN
	setvar VAR_LITTLEROOT_INTRO_STATE, 7
	msgbox RogueHub_ReturnAdventure_Text_6, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_ReturnAdventure_16
RogueHub_ReturnAdventure_15:
	msgbox RogueHub_ReturnAdventure_Text_7, MSGBOX_DEFAULT
	releaseall
	return

RogueHub_ReturnAdventure_4:
	msgbox RogueHub_ReturnAdventure_Text_0, MSGBOX_DEFAULT
	buffernumberstring STR_VAR_1, VAR_ROGUE_CURRENT_ROOM_IDX
	buffernumberstring STR_VAR_2, VAR_ROGUE_DIFFICULTY
	msgbox RogueHub_ReturnAdventure_Text_1, MSGBOX_DEFAULT
	turnobject 2, DIR_EAST
	msgbox RogueHub_ReturnAdventure_Text_2, MSGBOX_DEFAULT
	turnobject 2, DIR_SOUTH
	goto RogueHub_ReturnAdventure_3

RogueHub_ReturnAdventure_7:
	msgbox RogueHub_ReturnAdventure_Text_4, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY, VAR_ROGUE_REWARD_CANDY
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 1
	goto_if_ge RogueHub_ReturnAdventure_13
RogueHub_ReturnAdventure_12:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 2
	goto_if_ge RogueHub_ReturnAdventure_19
RogueHub_ReturnAdventure_18:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 3
	goto_if_ge RogueHub_ReturnAdventure_24
RogueHub_ReturnAdventure_23:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 4
	goto_if_ge RogueHub_ReturnAdventure_29
RogueHub_ReturnAdventure_28:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 5
	goto_if_ge RogueHub_ReturnAdventure_34
RogueHub_ReturnAdventure_33:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 6
	goto_if_ge RogueHub_ReturnAdventure_39
RogueHub_ReturnAdventure_38:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 7
	goto_if_ge RogueHub_ReturnAdventure_44
RogueHub_ReturnAdventure_43:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 8
	goto_if_ge RogueHub_ReturnAdventure_48
	goto RogueHub_ReturnAdventure_6

RogueHub_ReturnAdventure_10:
	msgbox RogueHub_ReturnAdventure_Text_5, MSGBOX_DEFAULT
	goto RogueHub_ReturnAdventure_9

RogueHub_ReturnAdventure_13:
	checkitem ITEM_HM01
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_21
	goto RogueHub_ReturnAdventure_12

RogueHub_ReturnAdventure_16:
	call Common_EventScript_SaveGame
	goto RogueHub_ReturnAdventure_15

RogueHub_ReturnAdventure_19:
	checkitem ITEM_HM05_FLASH
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_26
	goto RogueHub_ReturnAdventure_18

RogueHub_ReturnAdventure_21:
	giveitem ITEM_HM01
	goto RogueHub_ReturnAdventure_12

RogueHub_ReturnAdventure_24:
	checkitem ITEM_HM06_ROCK_SMASH
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_31
	goto RogueHub_ReturnAdventure_23

RogueHub_ReturnAdventure_26:
	giveitem ITEM_HM05_FLASH
	goto RogueHub_ReturnAdventure_18

RogueHub_ReturnAdventure_29:
	checkitem ITEM_HM04_STRENGTH
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_36
	goto RogueHub_ReturnAdventure_28

RogueHub_ReturnAdventure_31:
	giveitem ITEM_HM06_ROCK_SMASH
	goto RogueHub_ReturnAdventure_23

RogueHub_ReturnAdventure_34:
	checkitem ITEM_HM08_DIVE
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_41
	goto RogueHub_ReturnAdventure_33

RogueHub_ReturnAdventure_36:
	giveitem ITEM_HM04_STRENGTH
	goto RogueHub_ReturnAdventure_28

RogueHub_ReturnAdventure_39:
	checkitem ITEM_HM02_FLY
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_46
	goto RogueHub_ReturnAdventure_38

RogueHub_ReturnAdventure_41:
	giveitem ITEM_HM08_DIVE
	goto RogueHub_ReturnAdventure_33

RogueHub_ReturnAdventure_44:
	checkitem ITEM_HM07_WATERFALL
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_50
	goto RogueHub_ReturnAdventure_43

RogueHub_ReturnAdventure_46:
	giveitem ITEM_HM02_FLY
	goto RogueHub_ReturnAdventure_38

RogueHub_ReturnAdventure_48:
	checkitem ITEM_HM03_SURF
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ReturnAdventure_52
	goto RogueHub_ReturnAdventure_6

RogueHub_ReturnAdventure_50:
	giveitem ITEM_HM07_WATERFALL
	goto RogueHub_ReturnAdventure_43

RogueHub_ReturnAdventure_52:
	giveitem ITEM_HM03_SURF
	goto RogueHub_ReturnAdventure_6


RogueHub_SpeakScott::
	lock
	faceplayer
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_le RogueHub_SpeakScott_2
	compare VAR_LITTLEROOT_INTRO_STATE, 4
	goto_if_eq RogueHub_SpeakScott_6
RogueHub_SpeakScott_5:
	msgbox RogueHub_SpeakScott_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakScott_9
RogueHub_SpeakScott_8:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakScott_11
RogueHub_SpeakScott_1:
	release
	end

RogueHub_SpeakScott_2:
	msgbox RogueHub_SpeakScott_Text_0, MSGBOX_DEFAULT
	goto RogueHub_SpeakScott_1

RogueHub_SpeakScott_6:
	msgbox RogueHub_SpeakScott_Text_1, MSGBOX_DEFAULT
	goto RogueHub_SpeakScott_5

RogueHub_SpeakScott_9:
	getpartysize
	goto_if_set FLAG_ROGUE_DOUBLE_BATTLES, RogueHub_SpeakScott_13
	compare VAR_RESULT, 1
	goto_if_gt RogueHub_SpeakScott_22
	setvar VAR_RESULT, YES
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_11:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	specialvar VAR_RESULT, GetLeadMonLevel
	goto_if_unset FLAG_SET_SEED_ENABLED, RogueHub_SpeakScott_17
RogueHub_SpeakScott_16:
	compare VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	goto_if_ne RogueHub_SpeakScott_26
RogueHub_SpeakScott_25:
	call Common_EventScript_SaveGame
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakScott_44
	msgbox RogueHub_SpeakScott_Text_8, MSGBOX_DEFAULT
	goto RogueHub_SpeakScott_1

RogueHub_SpeakScott_13:
	compare VAR_RESULT, 2
	goto_if_gt RogueHub_SpeakScott_19
	setvar VAR_RESULT, YES
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_17:
	compare VAR_RESULT, 80
	goto_if_gt RogueHub_SpeakScott_28
	compare VAR_RESULT, 70
	goto_if_gt RogueHub_SpeakScott_29
	compare VAR_RESULT, 60
	goto_if_gt RogueHub_SpeakScott_30
	compare VAR_RESULT, 50
	goto_if_gt RogueHub_SpeakScott_31
	compare VAR_RESULT, 40
	goto_if_gt RogueHub_SpeakScott_32
	compare VAR_RESULT, 30
	goto_if_gt RogueHub_SpeakScott_33
	compare VAR_RESULT, 20
	goto_if_gt RogueHub_SpeakScott_34
	compare VAR_RESULT, 10
	goto_if_gt RogueHub_SpeakScott_35
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_19:
	msgbox RogueHub_SpeakScott_Text_3, MSGBOX_DEFAULT
	setvar VAR_RESULT, NO
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_22:
	msgbox RogueHub_SpeakScott_Text_4, MSGBOX_DEFAULT
	setvar VAR_RESULT, NO
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_26:
	msgbox RogueHub_SpeakScott_Text_5, MSGBOX_DEFAULT
	buffernumberstring STR_VAR_1, VAR_ROGUE_SKIP_TO_DIFFICULTY
	msgbox RogueHub_SpeakScott_Text_6, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_SpeakScott_47
	goto RogueHub_SpeakScott_25

RogueHub_SpeakScott_28:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 8
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_29:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 7
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_30:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 6
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_31:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 5
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_32:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 4
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_33:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 3
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_34:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 2
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_35:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 1
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_44:
	msgbox RogueHub_SpeakScott_Text_7, MSGBOX_DEFAULT
	setvar VAR_TEMP_0, 1
	compare VAR_LITTLEROOT_INTRO_STATE, 4
	goto_if_eq RogueHub_SpeakScott_50
	setvar VAR_LITTLEROOT_INTRO_STATE, 6
RogueHub_SpeakScott_49:
	setflag FLAG_ROGUE_LEFT_ON_RUN
	warp MAP_ROGUE_HUB_TRANSITION, 0
	goto RogueHub_SpeakScott_1

RogueHub_SpeakScott_47:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	goto RogueHub_SpeakScott_25

RogueHub_SpeakScott_50:
	setvar VAR_LITTLEROOT_INTRO_STATE, 5
	goto RogueHub_SpeakScott_49


RogueHub_SpeakBikeShop::
	lock
	faceplayer
	checkitem ITEM_ACRO_BIKE
	copyvar VAR_0x8000, VAR_RESULT
	checkitem ITEM_MACH_BIKE
	copyvar VAR_0x8001, VAR_RESULT
	compare VAR_0x8000, YES
	goto_if_eq RogueHub_SpeakBikeShop_2
	compare VAR_0x8001, YES
	goto_if_eq RogueHub_SpeakBikeShop_3
	msgbox RogueHub_SpeakBikeShop_Text_1
	giveitem ITEM_ACRO_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_2
RogueHub_SpeakBikeShop_1:
	release
	return

RogueHub_SpeakBikeShop_2:
	bufferitemname STR_VAR_1, ITEM_MACH_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakBikeShop_7
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_3:
	bufferitemname STR_VAR_1, ITEM_ACRO_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakBikeShop_9
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_7:
	removeitem ITEM_ACRO_BIKE
	giveitem ITEM_MACH_BIKE
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_9:
	removeitem ITEM_MACH_BIKE
	giveitem ITEM_ACRO_BIKE
	goto RogueHub_SpeakBikeShop_1


RogueHub_SpeakSafariWelcome::
	lock
	faceplayer
	msgbox RogueHub_SpeakSafariWelcome_Text_0, MSGBOX_NPC
	release
	return


RogueHub_SpeakFisherman::
	lock
	faceplayer
	checkitem ITEM_SUPER_ROD
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakFisherman_2
	msgbox RogueHub_SpeakFisherman_Text_1, MSGBOX_DEFAULT
	giveitem ITEM_SUPER_ROD
RogueHub_SpeakFisherman_1:
	release
	return

RogueHub_SpeakFisherman_2:
	msgbox RogueHub_SpeakFisherman_Text_0, MSGBOX_DEFAULT
	goto RogueHub_SpeakFisherman_1


RogueHub_SpeakBank::
	lock
	faceplayer
	compare VAR_ROGUE_ADVENTURE_MONEY, 0
	goto_if_eq RogueHub_SpeakBank_2
RogueHub_SpeakBank_1:
	showmoneybox 0, 0
	buffernumberstring STR_VAR_2, VAR_ROGUE_ADVENTURE_MONEY
	msgbox RogueHub_SpeakBank_Text_3, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakBank_5
RogueHub_SpeakBank_4:
	hidemoneybox
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_SpeakBank_8
RogueHub_SpeakBank_7:
	release
	return

RogueHub_SpeakBank_2:
	msgbox RogueHub_SpeakBank_Text_0, MSGBOX_DEFAULT
	msgbox RogueHub_SpeakBank_Text_1, MSGBOX_DEFAULT
	msgbox RogueHub_SpeakBank_Text_2, MSGBOX_DEFAULT
	setvar VAR_ROGUE_ADVENTURE_MONEY, 500
	playse SE_SHOP
	waitse
	goto RogueHub_SpeakBank_1

RogueHub_SpeakBank_5:
	checkmoney 500
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_SpeakBank_10
	removemoney 500
	hidemoneybox
	showmoneybox 0, 0
	addvar VAR_ROGUE_ADVENTURE_MONEY, 500
	buffernumberstring STR_VAR_2, VAR_ROGUE_ADVENTURE_MONEY
	playse SE_SHOP
	waitse
	msgbox RogueHub_SpeakBank_Text_5, MSGBOX_DEFAULT
	goto RogueHub_SpeakBank_4

RogueHub_SpeakBank_8:
	msgbox RogueHub_SpeakBank_Text_6, MSGBOX_DEFAULT
	goto RogueHub_SpeakBank_7

RogueHub_SpeakBank_10:
	msgbox RogueHub_SpeakBank_Text_4, MSGBOX_YESNO
	goto RogueHub_SpeakBank_4


RogueHub_Sign_AdventureSign::
	buffernumberstring STR_VAR_1, VAR_ROGUE_FURTHEST_DIFFICULTY
	msgbox RogueHub_Sign_AdventureSign_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_ProfLab::
	msgbox RogueHub_Sign_ProfLab_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_ConfigLab::
	msgbox RogueHub_Sign_ConfigLab_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_Safari::
	msgbox RogueHub_Sign_Safari_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_Shop::
	msgbox RogueHub_Sign_Shop_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_BikeShop::
	msgbox RogueHub_Sign_BikeShop_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_NameRater::
	msgbox RogueHub_Sign_NameRater_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_Bank::
	msgbox RogueHub_Sign_Bank_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_ClothesShop::
	msgbox RogueHub_Sign_ClothesShop_Text_0, MSGBOX_SIGN
	return


RogueHub_Intro0_Text_0:
	.string "SCOTT: Well we're here!\p"
	.string "The POKéMON HUB!\p"
	.string "Sorry again for making you sit in the\n"
	.string "back with all the supplies.\p"
	.string "I'm sure you're raring to go, but first\n"
	.string "you should go speak the PROF. first\n"
	.string "I'll be here waiting when you're done.$"

RogueHub_ReturnAdventure_Text_0:
	.string "SCOTT: {PLAYER}!\p"
	.string "Welcome back!\p"
	.string "Whenever you return from an adventure\n"
	.string "you earn rewards based on how well you\l"
	.string "did.$"

RogueHub_ReturnAdventure_Text_1:
	.string "Lets have a look now...\p"
	.string "You managed to complete{PAUSE 15}.{PAUSE 15}.{PAUSE 15}.\n"
	.string "{STR_VAR_1} routes...{PAUSE 15}\p"
	.string "and you recieved {STR_VAR_2}\n"
	.string "BADGES...{PAUSE 15}$"

RogueHub_ReturnAdventure_Text_2:
	.string "So erm.\n"
	.string "Your rewards should be{PAUSE 15}.{PAUSE 15}.{PAUSE 15}.$"

RogueHub_ReturnAdventure_Text_3:
	.string "SCOTT: You recieve ¥{STR_VAR_1}$"

RogueHub_ReturnAdventure_Text_4:
	.string "and for the BADGE's...$"

RogueHub_ReturnAdventure_Text_5:
	.string "SCOTT: Good job champ!\p"
	.string "You should have a look round the HUB\n"
	.string "too!\p"
	.string "As you adventure more, people may be\n"
	.string "attracted to the HUB and want to move\l"
	.string "in.\p"
	.string "Some useful buildings might have been\n"
	.string "setup when you return back here.$"

RogueHub_ReturnAdventure_Text_6:
	.string "SCOTT: Would you like to SAVE your\n"
	.string "progress?$"

RogueHub_ReturnAdventure_Text_7:
	.string "SCOTT: Come chat to me once you're\n"
	.string "ready for your next adventure.$"

RogueHub_SpeakScott_Text_0:
	.string "SCOTT: You should go and speak to find\n"
	.string "the PROF. first$"

RogueHub_SpeakScott_Text_1:
	.string "SCOTT: Great!\n"
	.string "You got yourself a PARTNER POKéMON!$"

RogueHub_SpeakScott_Text_2:
	.string "SCOTT: So what'd you say?\n"
	.string "Ready to go on an adventure?$"

RogueHub_SpeakScott_Text_3:
	.string "SCOTT: Looks like you have too many\n"
	.string "POKéMON there bud.\p"
	.string "You can only take in 2.\n"
	.string "You should use the PC in the PROF.'s LAB\l"
	.string "to deposit some before I can let you on\l"
	.string "an adventure.$"

RogueHub_SpeakScott_Text_4:
	.string "SCOTT: Looks like you have too many\n"
	.string "POKéMON there bud.\p"
	.string "You can only take in 1.\n"
	.string "You should use the PC in the PROF.'s LAB\l"
	.string "to deposit some before I can let you on\l"
	.string "an adventure.$"

RogueHub_SpeakScott_Text_5:
	.string "SCOTT: You've leveled up your PARTNER\n"
	.string "quite a bit there!$"

RogueHub_SpeakScott_Text_6:
	.string "Would you like to skip ahead and start\n"
	.string "at BADGE {STR_VAR_1}?$"

RogueHub_SpeakScott_Text_7:
	.string "SCOTT: Good luck CHAMP!$"

RogueHub_SpeakScott_Text_8:
	.string "SCOTT: OK. Come chat to me once you're\n"
	.string "ready.$"

RogueHub_SpeakBikeShop_Text_0:
	.string "Do you want to swap to the\n"
	.string "{STR_VAR_1}?$"

RogueHub_SpeakBikeShop_Text_1:
	.string "Finally!\p"
	.string "You must be this {PLAYER} I've been\n"
	.string "told about.\p"
	.string "You seem a bit slower than I expected...\p"
	.string "I know!\n"
	.string "I have a couple old BIKEs I've been\l"
	.string "looking to get rid of.\p"
	.string "You can have them!$"

RogueHub_SpeakBikeShop_Text_2:
	.string "On second thought...\n"
	.string "You look like you're struggling to carry\l"
	.string "that much.\p"
	.string "Tell you what. You take that one and if\n"
	.string "you ever want to swap it out for the\l"
	.string "other one, let me know$"

RogueHub_SpeakSafariWelcome_Text_0:
	.string "Welcome to the SAFARI ZONE!\p"
	.string "We use the data from your POKéDEX to\n"
	.string "breed POKéMON and let them roam\l"
	.string "freely!\p"
	.string "As a thanks for providing the POKéMON\n"
	.string "data for us, we'll allow you to enter\l"
	.string "and capture a few POKéMON of your\l"
	.string "choosing which you may then use as a\l"
	.string "PARTNER in future adventures!$"

RogueHub_SpeakFisherman_Text_0:
	.string "Hope that SUPER ROD is doing good work\n"
	.string "for you!$"

RogueHub_SpeakFisherman_Text_1:
	.string "Oh you're that TRAINER I keep hearing\n"
	.string "about!\p"
	.string "You know, if you find yourself short on\n"
	.string "WATER POKéMON you can always use a\l"
	.string "FISHING ROD\p"
	.string "What!?\n"
	.string "You don't have one!\p"
	.string "Here, you should take this one!$"

RogueHub_SpeakBank_Text_0:
	.string "Why hello!\n"
	.string "You must be {PLAYER} if I'm not\l"
	.string "mistaken?\p"
	.string "Wonderful!\n"
	.string "You see I'm rather useful when it comes\l"
	.string "to managing money and I was hoping to\l"
	.string "setup an ADVENTURE FUND for yourself.$"

RogueHub_SpeakBank_Text_1:
	.string "What's an ADVENTURE FUND?\n"
	.string "In essence, if you donate your HUB\l"
	.string "MONEY to myself, I will invest it on\l"
	.string "your behalf and then every time you\l"
	.string "head out on another adventure, I will\l"
	.string "give you some extra money to start\l"
	.string "with.\p"
	.string "So rather than having to start with ¥0,\n"
	.string "by donating money to myself, you can\l"
	.string "start with ¥1000, ¥2000, ¥10000!\p"
	.string "The sky is the limit really!\n"
	.string "Or rather I guess the limit would be\l"
	.string "your HUB MONEY.$"

RogueHub_SpeakBank_Text_2:
	.string "Here, let me get you started with a gift.\p"
	.string "I shall add ¥500 to your ADVENTURE FUND\n"
	.string "out of my own pocket!$"

RogueHub_SpeakBank_Text_3:
	.string "Your fund currently has\n"
	.string "¥{STR_VAR_2}.\p"
	.string "Would you like to deposit another ¥500?$"

RogueHub_SpeakBank_Text_4:
	.string "I'm afraid you seem to be lacking the\n"
	.string "required funds currently.$"

RogueHub_SpeakBank_Text_5:
	.string "Thankyou very much!\n"
	.string "Your fund now contains\l"
	.string "¥{STR_VAR_2}.$"

RogueHub_SpeakBank_Text_6:
	.string "I see...\n"
	.string "You know where I am, if you change your\l"
	.string "mind.$"

RogueHub_Sign_AdventureSign_Text_0:
	.string "Adventures await beyond this point...\p"
	.string "Furthest Progress:\n"
	.string "Gained {STR_VAR_1} BADGES...$"

RogueHub_Sign_ProfLab_Text_0:
	.string "PROF. BIRCH's LAB$"

RogueHub_Sign_ConfigLab_Text_0:
	.string "CONFIG LAB$"

RogueHub_Sign_Safari_Text_0:
	.string "SAFARI ZONE$"

RogueHub_Sign_Shop_Text_0:
	.string "OUTDOOR MART$"

RogueHub_Sign_BikeShop_Text_0:
	.string "BIKE SHOP$"

RogueHub_Sign_NameRater_Text_0:
	.string "NAME RATER's HOUSE$"

RogueHub_Sign_Bank_Text_0:
	.string "BANK$"

RogueHub_Sign_ClothesShop_Text_0:
	.string "CLOTHES SHOP$"
