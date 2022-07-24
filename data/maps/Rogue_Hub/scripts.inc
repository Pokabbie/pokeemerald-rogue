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
RogueHub_OnMapLoad_5:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 2
	goto_if_lt RogueHub_OnMapLoad_9
	clearflag FLAG_ROGUE_TRAINER1
RogueHub_OnMapLoad_8:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 3
	goto_if_lt RogueHub_OnMapLoad_13
	clearflag FLAG_ROGUE_TRAINER2
RogueHub_OnMapLoad_12:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 4
	goto_if_lt RogueHub_OnMapLoad_17
	clearflag FLAG_ROGUE_TRAINER3
RogueHub_OnMapLoad_16:
	fillmetatile 31, 21, 37, 26, 0x001, FALSE
	fillmetatile 41, 20, 48, 25, 0x001, FALSE
	return

RogueHub_OnMapLoad_2:
	fillmetatile 31, 9, 40, 16, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER0
	goto RogueHub_OnMapLoad_1

RogueHub_OnMapLoad_6:
	fillmetatile 1, 3, 14, 10, 0x001, FALSE
	fillmetatile 1, 11, 12, 11, 0x001, FALSE
	fillmetatile 1, 12, 6, 12, 0x001, FALSE
	goto RogueHub_OnMapLoad_5

RogueHub_OnMapLoad_9:
	fillmetatile 22, 12, 27, 16, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER1
	goto RogueHub_OnMapLoad_8

RogueHub_OnMapLoad_13:
	fillmetatile 42, 14, 48, 19, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER2
	goto RogueHub_OnMapLoad_12

RogueHub_OnMapLoad_17:
	fillmetatile 13, 27, 27, 36, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER3
	goto RogueHub_OnMapLoad_16


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
	goto_if_gt RogueHub_SpeakScott_36
	setvar VAR_RESULT, YES
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_11:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	specialvar VAR_RESULT, GetLeadMonLevel
	compare VAR_RESULT, 80
	goto_if_gt RogueHub_SpeakScott_17
	compare VAR_RESULT, 70
	goto_if_gt RogueHub_SpeakScott_18
	compare VAR_RESULT, 60
	goto_if_gt RogueHub_SpeakScott_19
	compare VAR_RESULT, 50
	goto_if_gt RogueHub_SpeakScott_20
	compare VAR_RESULT, 40
	goto_if_gt RogueHub_SpeakScott_21
	compare VAR_RESULT, 30
	goto_if_gt RogueHub_SpeakScott_22
	compare VAR_RESULT, 20
	goto_if_gt RogueHub_SpeakScott_23
	compare VAR_RESULT, 10
	goto_if_gt RogueHub_SpeakScott_24
RogueHub_SpeakScott_16:
	compare VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	goto_if_ne RogueHub_SpeakScott_40
RogueHub_SpeakScott_39:
	msgbox RogueHub_SpeakScott_Text_6, MSGBOX_DEFAULT
	setvar VAR_TEMP_0, 1
	compare VAR_LITTLEROOT_INTRO_STATE, 4
	goto_if_eq RogueHub_SpeakScott_43
	setvar VAR_LITTLEROOT_INTRO_STATE, 6
RogueHub_SpeakScott_42:
	setflag FLAG_ROGUE_LEFT_ON_RUN
	warp MAP_ROGUE_HUB_TRANSITION, 0
	goto RogueHub_SpeakScott_1

RogueHub_SpeakScott_13:
	compare VAR_RESULT, 2
	goto_if_gt RogueHub_SpeakScott_33
	setvar VAR_RESULT, YES
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_17:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 8
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_18:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 7
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_19:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 6
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_20:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 5
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_21:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 4
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_22:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 3
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_23:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 2
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_24:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 1
	goto RogueHub_SpeakScott_16

RogueHub_SpeakScott_33:
	msgbox RogueHub_SpeakScott_Text_3, MSGBOX_YESNO
	setvar VAR_RESULT, NO
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_36:
	msgbox RogueHub_SpeakScott_Text_3, MSGBOX_YESNO
	setvar VAR_RESULT, NO
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_40:
	msgbox RogueHub_SpeakScott_Text_4, MSGBOX_DEFAULT
	buffernumberstring STR_VAR_1, VAR_ROGUE_SKIP_TO_DIFFICULTY
	msgbox RogueHub_SpeakScott_Text_5, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_SpeakScott_46
	goto RogueHub_SpeakScott_39

RogueHub_SpeakScott_43:
	setvar VAR_LITTLEROOT_INTRO_STATE, 5
	goto RogueHub_SpeakScott_42

RogueHub_SpeakScott_46:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	goto RogueHub_SpeakScott_39


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


RogueHub_SpeakFisherman::
	lock
	faceplayer
	checkitem ITEM_SUPER_ROD
	compare VAR_RESULT, NO
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
	msgbox RogueHub_SpeakBank_Text_0
	release
	return


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


RogueHub_Sign_Bank::
	msgbox RogueHub_Sign_Bank_Text_0, MSGBOX_SIGN
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
	.string "SCOTT: You've leveled up your PARTNER\n"
	.string "quite a bit there!$"

RogueHub_SpeakScott_Text_5:
	.string "Would you like to skip ahead and start\n"
	.string "at BADGE {STR_VAR_1}?$"

RogueHub_SpeakScott_Text_6:
	.string "SCOTT: Good luck CHAMP!$"

RogueHub_SpeakBikeShop_Text_0:
	.string "Do you want to use the {STR_VAR_1} instead?$"

RogueHub_SpeakBikeShop_Text_1:
	.string "Finally!\p"
	.string "You must be this {PLAYER} I've been\n"
	.string "told about.\p"
	.string "You seem a bit slower than I expected...\p"
	.string "I know!\n"
	.string "I have a couple old BIKE's I've been\l"
	.string "looked to get rid of!\p"
	.string "You can have them!$"

RogueHub_SpeakBikeShop_Text_2:
	.string "On second thought...\n"
	.string "You look like you're struggling to carry\l"
	.string "that much.\p"
	.string "Tell you what. You take that one and if\n"
	.string "you ever want to swap it out for the\l"
	.string "other one, let me know$"

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
	.string "TODO$"

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

RogueHub_Sign_Bank_Text_0:
	.string "BANK$"
