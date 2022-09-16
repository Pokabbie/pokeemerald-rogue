Rogue_Hub_AdventureEnterance_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_Adventure_RedrawMap
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Hub_AdventureEnterance_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Hub_AdventureEnterance_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_0, 0, Rogue_Adventure_WarpState0
	.2byte 0


Rogue_Adventure_WarpState0::
	goto_if_set FLAG_ROGUE_LEFT_ON_RUN, Rogue_Adventure_WarpState0_2
	goto_if_set FLAG_ROGUE_MET_POKABBIE, Rogue_Adventure_WarpState0_6
Rogue_Adventure_WarpState0_5:
	compare VAR_LITTLEROOT_INTRO_STATE, 4
	goto_if_eq Rogue_Adventure_WarpState0_4
Rogue_Adventure_WarpState0_1:
	setvar VAR_TEMP_0, 1
	return

Rogue_Adventure_WarpState0_2:
	call Rogue_ReturnAdventureCommon
	goto Rogue_Adventure_WarpState0_1

Rogue_Adventure_WarpState0_3:
	call Rogue_Adventure_ScottGift
	compare VAR_LITTLEROOT_INTRO_STATE, 4
	goto_if_eq Rogue_Adventure_WarpState0_10
	goto Rogue_Adventure_WarpState0_1

Rogue_Adventure_WarpState0_4:
	call Rogue_Adventure_ScottIntro
	setvar VAR_LITTLEROOT_INTRO_STATE, 5
	goto Rogue_Adventure_WarpState0_1

Rogue_Adventure_WarpState0_6:
	goto_if_unset FLAG_ROGUE_SCOTT_GIFTED_EXTRAS, Rogue_Adventure_WarpState0_3
	goto Rogue_Adventure_WarpState0_5

Rogue_Adventure_WarpState0_10:
	setvar VAR_LITTLEROOT_INTRO_STATE, 5
	goto Rogue_Adventure_WarpState0_1


Rogue_Adventure_RedrawMap::
	goto_if_set FLAG_ROGUE_SCOTT_GIFTED_EXTRAS, Rogue_Adventure_RedrawMap_1
	goto_if_set FLAG_ROGUE_MET_POKABBIE, Rogue_Adventure_RedrawMap_5
Rogue_Adventure_RedrawMap_2:
	setflag FLAG_ROGUE_TRAINER0
	fillmetatile 9, 0, 17, 8, 0x001, TRUE
	return

Rogue_Adventure_RedrawMap_1:
	clearflag FLAG_ROGUE_TRAINER0
	return

Rogue_Adventure_RedrawMap_5:
	goto_if_unset FLAG_ROGUE_SCOTT_GIFTED_EXTRAS, Rogue_Adventure_RedrawMap_1
	goto Rogue_Adventure_RedrawMap_2


Rogue_Adventure_ScottIntro::
	lockall
	applymovement 1, Rogue_Adventure_ScottIntroMovement0
	waitmovement 0
	msgbox Rogue_Adventure_ScottIntro_Text_0
	releaseall
	return


Rogue_Adventure_ScottGift::
	lockall
	setflag FLAG_ROGUE_SCOTT_GIFTED_EXTRAS
	applymovement 1, Rogue_Adventure_ScottIntroMovement0
	waitmovement 0
	msgbox Rogue_Adventure_ScottGift_Text_0, MSGBOX_DEFAULT
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, Rogue_Adventure_ScottGift_2
Rogue_Adventure_ScottGift_1:
	releaseall
	return

Rogue_Adventure_ScottGift_2:
	msgbox Rogue_Adventure_ScottGift_Text_1, MSGBOX_DEFAULT
	giveitem 695
	giveitem 696
	giveitem 699
	goto Rogue_Adventure_ScottGift_1


Rogue_Adventure_SpeakScott::
	lock
	faceplayer
	compare VAR_LITTLEROOT_INTRO_STATE, 5
	goto_if_le Rogue_Adventure_SpeakScott_2
	nextquestreward
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Adventure_SpeakScott_5
	msgbox Rogue_Adventure_SpeakScott_Text_4
Rogue_Adventure_SpeakScott_1:
	release
	return

Rogue_Adventure_SpeakScott_2:
	msgbox Rogue_Adventure_SpeakScott_Text_0
	goto Rogue_Adventure_SpeakScott_1

Rogue_Adventure_SpeakScott_5:
	msgbox Rogue_Adventure_SpeakScott_Text_1
Rogue_Adventure_SpeakScott_9:
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Adventure_SpeakScott_10
	msgbox Rogue_Adventure_SpeakScott_Text_3
	goto Rogue_Adventure_SpeakScott_1

Rogue_Adventure_SpeakScott_10:
	playse SE_SHOP
	msgbox Rogue_Adventure_SpeakScott_Text_2
	waitse
	nextquestreward
	goto Rogue_Adventure_SpeakScott_9


Rogue_Adventure_ScottIntroMovement0:
	face_down
	emote_exclamation_mark
	delay_16
	walk_left
	walk_left
	walk_down
	walk_down
	step_end

Rogue_Adventure_StatsSign::
	buffernumberstring STR_VAR_1, VAR_ROGUE_FURTHEST_DIFFICULTY
	msgbox Rogue_Adventure_StatsSign_Text_0, MSGBOX_SIGN
	return


Rogue_Adventure_StartStandard::
	call Rogue_ResetAdventureFlags
	lock
	faceplayer
	msgbox Rogue_Adventure_StartStandard_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Adventure_StartStandard_2
	msgbox Rogue_Adventure_StartStandard_Text_1, MSGBOX_DEFAULT
Rogue_Adventure_StartStandard_1:
	release
	return

Rogue_Adventure_StartStandard_2:
	call Rogue_ResetAdventureFlags
	call Rogue_StartAdventureCommon
	goto Rogue_Adventure_StartStandard_1


Rogue_Adventure_StartDoubles::
	call Rogue_ResetAdventureFlags
	lock
	faceplayer
	message Rogue_Adventure_StartDoubles_Text_0
	waitmessage
	multichoice 20, 6, MULTI_YESNOINFO, NO
	switch VAR_RESULT
	case 0, Rogue_Adventure_StartDoubles_3
	case 1, Rogue_Adventure_StartDoubles_4
	case 2, Rogue_Adventure_StartDoubles_5
	case MULTI_B_PRESSED, Rogue_Adventure_StartDoubles_6
Rogue_Adventure_StartDoubles_1:
	release
	return

Rogue_Adventure_StartDoubles_3:
	call Rogue_ResetAdventureFlags
	setflag FLAG_ROGUE_DOUBLE_BATTLES
	call Rogue_StartAdventureCommon
	goto Rogue_Adventure_StartDoubles_1

Rogue_Adventure_StartDoubles_4:
	msgbox Rogue_Adventure_StartStandard_Text_1, MSGBOX_DEFAULT
	goto Rogue_Adventure_StartDoubles_1

Rogue_Adventure_StartDoubles_5:
	msgbox Rogue_Adventure_StartDoubles_Text_1, MSGBOX_DEFAULT
	goto Rogue_Adventure_StartDoubles_1

Rogue_Adventure_StartDoubles_6:
	msgbox Rogue_Adventure_StartStandard_Text_1, MSGBOX_DEFAULT
	goto Rogue_Adventure_StartDoubles_1


Rogue_Adventure_StartGauntlet::
	call Rogue_ResetAdventureFlags
	lock
	faceplayer
	message Rogue_Adventure_StartGauntlet_Text_0
	waitmessage
	multichoice 20, 6, MULTI_YESNOINFO, NO
	switch VAR_RESULT
	case 0, Rogue_Adventure_StartGauntlet_3
	case 1, Rogue_Adventure_StartGauntlet_4
	case 2, Rogue_Adventure_StartGauntlet_5
	case MULTI_B_PRESSED, Rogue_Adventure_StartGauntlet_6
Rogue_Adventure_StartGauntlet_1:
	release
	return

Rogue_Adventure_StartGauntlet_3:
	call Rogue_ResetAdventureFlags
	setflag FLAG_ROGUE_GAUNTLET_MODE
	msgbox Rogue_Adventure_StartGauntlet_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Adventure_StartGauntlet_8
Rogue_Adventure_StartGauntlet_7:
	call Rogue_StartAdventureCommon
	goto Rogue_Adventure_StartGauntlet_1

Rogue_Adventure_StartGauntlet_4:
	msgbox Rogue_Adventure_StartStandard_Text_1, MSGBOX_DEFAULT
	goto Rogue_Adventure_StartGauntlet_1

Rogue_Adventure_StartGauntlet_5:
	msgbox Rogue_Adventure_StartGauntlet_Text_2, MSGBOX_DEFAULT
	goto Rogue_Adventure_StartGauntlet_1

Rogue_Adventure_StartGauntlet_6:
	msgbox Rogue_Adventure_StartStandard_Text_1, MSGBOX_DEFAULT
	goto Rogue_Adventure_StartGauntlet_1

Rogue_Adventure_StartGauntlet_8:
	setflag FLAG_ROGUE_DOUBLE_BATTLES
	goto Rogue_Adventure_StartGauntlet_7


Rogue_ResetAdventureFlags::
	clearflag FLAG_ROGUE_DOUBLE_BATTLES
	clearflag FLAG_ROGUE_GAUNTLET_MODE
	return


Rogue_StartAdventureCommon::
	getpartysize
	goto_if_set FLAG_ROGUE_DOUBLE_BATTLES, Rogue_StartAdventureCommon_2
	compare VAR_RESULT, 1
	goto_if_gt Rogue_StartAdventureCommon_10
	setvar VAR_RESULT, YES
Rogue_StartAdventureCommon_1:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_StartAdventureCommon_5
	return

Rogue_StartAdventureCommon_2:
	compare VAR_RESULT, 2
	goto_if_gt Rogue_StartAdventureCommon_7
	setvar VAR_RESULT, YES
	goto Rogue_StartAdventureCommon_1

Rogue_StartAdventureCommon_5:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	specialvar VAR_RESULT, GetLeadMonLevel
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_StartAdventureCommon_15
Rogue_StartAdventureCommon_13:
	compare VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	goto_if_ne Rogue_StartAdventureCommon_19
Rogue_StartAdventureCommon_18:
	call Common_EventScript_SaveGame
	compare VAR_RESULT, YES
	goto_if_eq Rogue_StartAdventureCommon_37
	msgbox Rogue_StartAdventureCommon_Text_5, MSGBOX_DEFAULT
	return

Rogue_StartAdventureCommon_7:
	msgbox Rogue_StartAdventureCommon_Text_0, MSGBOX_DEFAULT
	setvar VAR_RESULT, NO
	goto Rogue_StartAdventureCommon_1

Rogue_StartAdventureCommon_10:
	msgbox Rogue_StartAdventureCommon_Text_1, MSGBOX_DEFAULT
	setvar VAR_RESULT, NO
	goto Rogue_StartAdventureCommon_1

Rogue_StartAdventureCommon_14:
	compare VAR_RESULT, 85
	goto_if_gt Rogue_StartAdventureCommon_21
	compare VAR_RESULT, 75
	goto_if_gt Rogue_StartAdventureCommon_22
	compare VAR_RESULT, 65
	goto_if_gt Rogue_StartAdventureCommon_23
	compare VAR_RESULT, 55
	goto_if_gt Rogue_StartAdventureCommon_24
	compare VAR_RESULT, 45
	goto_if_gt Rogue_StartAdventureCommon_25
	compare VAR_RESULT, 35
	goto_if_gt Rogue_StartAdventureCommon_26
	compare VAR_RESULT, 25
	goto_if_gt Rogue_StartAdventureCommon_27
	compare VAR_RESULT, 15
	goto_if_gt Rogue_StartAdventureCommon_28
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_15:
	goto_if_unset FLAG_ROGUE_GAUNTLET_MODE, Rogue_StartAdventureCommon_14
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_19:
	msgbox Rogue_StartAdventureCommon_Text_2, MSGBOX_DEFAULT
	buffernumberstring STR_VAR_1, VAR_ROGUE_SKIP_TO_DIFFICULTY
	msgbox Rogue_StartAdventureCommon_Text_3, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq Rogue_StartAdventureCommon_40
	goto Rogue_StartAdventureCommon_18

Rogue_StartAdventureCommon_21:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 8
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_22:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 7
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_23:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 6
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_24:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 5
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_25:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 4
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_26:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 3
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_27:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 2
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_28:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 1
	goto Rogue_StartAdventureCommon_13

Rogue_StartAdventureCommon_37:
	msgbox Rogue_StartAdventureCommon_Text_4, MSGBOX_DEFAULT
	compare VAR_LITTLEROOT_INTRO_STATE, 5
	goto_if_eq Rogue_StartAdventureCommon_43
	setvar VAR_LITTLEROOT_INTRO_STATE, 7
Rogue_StartAdventureCommon_42:
	setflag FLAG_ROGUE_LEFT_ON_RUN
	setrespawn HEAL_LOCATION_ROGUE_HUB
	warp MAP_ROGUE_HUB_TRANSITION, 0
	return

Rogue_StartAdventureCommon_40:
	setvar VAR_ROGUE_SKIP_TO_DIFFICULTY, 0
	goto Rogue_StartAdventureCommon_18

Rogue_StartAdventureCommon_43:
	setvar VAR_LITTLEROOT_INTRO_STATE, 6
	goto Rogue_StartAdventureCommon_42


Rogue_ReturnAdventureCommon::
	lockall
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	msgbox Rogue_ReturnAdventureCommon_Text_0, MSGBOX_DEFAULT
	compare VAR_LITTLEROOT_INTRO_STATE, 6
	goto_if_eq Rogue_ReturnAdventureCommon_2
Rogue_ReturnAdventureCommon_1:
	buffernumberstring STR_VAR_1, VAR_ROGUE_REWARD_MONEY
	buffernumberstring STR_VAR_2, VAR_ROGUE_REWARD_CANDY
	message Rogue_ReturnAdventureCommon_Text_4
	playfanfare MUS_OBTAIN_ITEM
	waitmessage
	waitfanfare
	compare VAR_ROGUE_REWARD_CANDY, 0
	goto_if_ne Rogue_ReturnAdventureCommon_5
Rogue_ReturnAdventureCommon_4:
	compare VAR_LITTLEROOT_INTRO_STATE, 6
	goto_if_eq Rogue_ReturnAdventureCommon_8
Rogue_ReturnAdventureCommon_7:
	clearflag FLAG_ROGUE_LEFT_ON_RUN
	setvar VAR_LITTLEROOT_INTRO_STATE, 7
	msgbox Rogue_ReturnAdventureCommon_Text_8, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ReturnAdventureCommon_14
Rogue_ReturnAdventureCommon_13:
	msgbox Rogue_ReturnAdventureCommon_Text_9, MSGBOX_DEFAULT
	releaseall
	return

Rogue_ReturnAdventureCommon_2:
	msgbox Rogue_ReturnAdventureCommon_Text_1, MSGBOX_DEFAULT
	buffernumberstring STR_VAR_1, VAR_ROGUE_CURRENT_ROOM_IDX
	buffernumberstring STR_VAR_2, VAR_ROGUE_DIFFICULTY
	msgbox Rogue_ReturnAdventureCommon_Text_2, MSGBOX_DEFAULT
	msgbox Rogue_ReturnAdventureCommon_Text_3, MSGBOX_DEFAULT
	goto Rogue_ReturnAdventureCommon_1

Rogue_ReturnAdventureCommon_5:
	msgbox Rogue_ReturnAdventureCommon_Text_5, MSGBOX_DEFAULT
	giveitem ITEM_RARE_CANDY, VAR_ROGUE_REWARD_CANDY
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 1
	goto_if_ge Rogue_ReturnAdventureCommon_11
Rogue_ReturnAdventureCommon_10:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 2
	goto_if_ge Rogue_ReturnAdventureCommon_17
Rogue_ReturnAdventureCommon_16:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 3
	goto_if_ge Rogue_ReturnAdventureCommon_22
Rogue_ReturnAdventureCommon_21:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 4
	goto_if_ge Rogue_ReturnAdventureCommon_27
Rogue_ReturnAdventureCommon_26:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 5
	goto_if_ge Rogue_ReturnAdventureCommon_32
Rogue_ReturnAdventureCommon_31:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 6
	goto_if_ge Rogue_ReturnAdventureCommon_37
Rogue_ReturnAdventureCommon_36:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 7
	goto_if_ge Rogue_ReturnAdventureCommon_42
Rogue_ReturnAdventureCommon_41:
	compare VAR_ROGUE_FURTHEST_DIFFICULTY, 8
	goto_if_ge Rogue_ReturnAdventureCommon_47
Rogue_ReturnAdventureCommon_46:
	compare VAR_LITTLEROOT_INTRO_STATE, 6
	goto_if_eq Rogue_ReturnAdventureCommon_51
	goto Rogue_ReturnAdventureCommon_4

Rogue_ReturnAdventureCommon_8:
	msgbox Rogue_ReturnAdventureCommon_Text_7, MSGBOX_DEFAULT
	goto Rogue_ReturnAdventureCommon_7

Rogue_ReturnAdventureCommon_11:
	checkitem ITEM_HM01
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_19
	goto Rogue_ReturnAdventureCommon_10

Rogue_ReturnAdventureCommon_14:
	call Common_EventScript_SaveGame
	goto Rogue_ReturnAdventureCommon_13

Rogue_ReturnAdventureCommon_17:
	checkitem ITEM_HM05_FLASH
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_24
	goto Rogue_ReturnAdventureCommon_16

Rogue_ReturnAdventureCommon_19:
	giveitem ITEM_HM01
	goto Rogue_ReturnAdventureCommon_10

Rogue_ReturnAdventureCommon_22:
	checkitem ITEM_HM06_ROCK_SMASH
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_29
	goto Rogue_ReturnAdventureCommon_21

Rogue_ReturnAdventureCommon_24:
	giveitem ITEM_HM05_FLASH
	goto Rogue_ReturnAdventureCommon_16

Rogue_ReturnAdventureCommon_27:
	checkitem ITEM_HM04_STRENGTH
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_34
	goto Rogue_ReturnAdventureCommon_26

Rogue_ReturnAdventureCommon_29:
	giveitem ITEM_HM06_ROCK_SMASH
	goto Rogue_ReturnAdventureCommon_21

Rogue_ReturnAdventureCommon_32:
	checkitem ITEM_HM08_DIVE
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_39
	goto Rogue_ReturnAdventureCommon_31

Rogue_ReturnAdventureCommon_34:
	giveitem ITEM_HM04_STRENGTH
	goto Rogue_ReturnAdventureCommon_26

Rogue_ReturnAdventureCommon_37:
	checkitem ITEM_HM02_FLY
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_44
	goto Rogue_ReturnAdventureCommon_36

Rogue_ReturnAdventureCommon_39:
	giveitem ITEM_HM08_DIVE
	goto Rogue_ReturnAdventureCommon_31

Rogue_ReturnAdventureCommon_42:
	checkitem ITEM_HM07_WATERFALL
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_49
	goto Rogue_ReturnAdventureCommon_41

Rogue_ReturnAdventureCommon_44:
	giveitem ITEM_HM02_FLY
	goto Rogue_ReturnAdventureCommon_36

Rogue_ReturnAdventureCommon_47:
	checkitem ITEM_HM03_SURF
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ReturnAdventureCommon_53
	goto Rogue_ReturnAdventureCommon_46

Rogue_ReturnAdventureCommon_49:
	giveitem ITEM_HM07_WATERFALL
	goto Rogue_ReturnAdventureCommon_41

Rogue_ReturnAdventureCommon_51:
	msgbox Rogue_ReturnAdventureCommon_Text_6, MSGBOX_DEFAULT
	goto Rogue_ReturnAdventureCommon_4

Rogue_ReturnAdventureCommon_53:
	giveitem ITEM_HM03_SURF
	goto Rogue_ReturnAdventureCommon_46


Rogue_Adventure_ScottIntro_Text_0:
	.string "SCOTT: Hey {PLAYER}!\p"
	.string "Looks like you got yourself a partner\n"
	.string "POKéMON, so you're all ready to go on an\l"
	.string "adventure!\p"
	.string "Just go chat to the nurse over there\n"
	.string "and you can start exploring!$"

Rogue_Adventure_ScottGift_Text_0:
	.string "SCOTT: Oh hey {PLAYER}!\n"
	.string "POKABBIE has gone and added some\l"
	.string "extra modes here for you to try!$"

Rogue_Adventure_ScottGift_Text_1:
	.string "She's actually left me a bunch of items\n"
	.string "to give to you too!$"

Rogue_Adventure_SpeakScott_Text_0:
	.string "SCOTT: Hey {PLAYER}!\p"
	.string "Just go chat to the nurse over there\n"
	.string "and you can start exploring!$"

Rogue_Adventure_SpeakScott_Text_1:
	.string "SCOTT: Hey {PLAYER}!\p"
	.string "I have some Quest Rewards for you!$"

Rogue_Adventure_SpeakScott_Text_2:
	.string "{STR_VAR_3}$"

Rogue_Adventure_SpeakScott_Text_3:
	.string "And that's all for the moment!\p"
	.string "Good luck with your next Adventure\n"
	.string "champ!$"

Rogue_Adventure_SpeakScott_Text_4:
	.string "SCOTT: Hey {PLAYER}!\p"
	.string "Be sure to come and chat with me once\n"
	.string "you complete Quests so I can give you\l"
	.string "your rewards!$"

Rogue_Adventure_StatsSign_Text_0:
	.string "Furthest Progress:\n"
	.string "Gained {STR_VAR_1} BADGES...\p"
	.string "A note for the dev..\p"
	.string "-Run stats to come in a future update!$"

Rogue_Adventure_StartStandard_Text_0:
	.string "Would you like to go on an adventure?$"

Rogue_Adventure_StartStandard_Text_1:
	.string "Feel free to speak with me again, if you\n"
	.string "change your mind.$"

Rogue_Adventure_StartDoubles_Text_0:
	.string "Would you like to go on a DOUBLEs\n"
	.string "adventure?$"

Rogue_Adventure_StartDoubles_Text_1:
	.string "The adventure uses the same settings\n"
	.string "as the standard mode, however all\l"
	.string "TRAINER battles will forced to be\l"
	.string "DOUBLE battles.$"

Rogue_Adventure_StartGauntlet_Text_0:
	.string "Would you like to go on a GAUNTLET\n"
	.string "adventure?$"

Rogue_Adventure_StartGauntlet_Text_1:
	.string "Would you like to enable DOUBLE mode\n"
	.string "for this adventure too?$"

Rogue_Adventure_StartGauntlet_Text_2:
	.string "In GAUNTLET adventures, you will get\n"
	.string "given a few routes to capture POKéMON\l"
	.string "and collect items.\p"
	.string "After that you will fight every BOSS\n"
	.string "back to back with no chance to get any\l"
	.string "more POKéMON or items.\p"
	.string "This is a high stakes mode intended for\n"
	.string "faster more difficult battles.$"

Rogue_StartAdventureCommon_Text_0:
	.string "Looks like you have too many POKéMON.\p"
	.string "You can only take in 2.\n"
	.string "You should use the PC to deposit some\l"
	.string "before I can let you on an adventure.$"

Rogue_StartAdventureCommon_Text_1:
	.string "Looks like you have too many POKéMON.\p"
	.string "You can only take in 1.\n"
	.string "You should use the PC to deposit some\l"
	.string "before I can let you on an adventure.$"

Rogue_StartAdventureCommon_Text_2:
	.string "You've leveled up your PARTNER enough\n"
	.string "to skip ahead!$"

Rogue_StartAdventureCommon_Text_3:
	.string "Would you like to skip ahead and start\n"
	.string "at BADGE {STR_VAR_1}?$"

Rogue_StartAdventureCommon_Text_4:
	.string "Good luck {PLAYER}!$"

Rogue_StartAdventureCommon_Text_5:
	.string "OK. Come speak to me once you're ready.$"

Rogue_ReturnAdventureCommon_Text_0:
	.string "Welcome back {PLAYER}!$"

Rogue_ReturnAdventureCommon_Text_1:
	.string "Whenever you return from an adventure\n"
	.string "you earn rewards based on how well you\l"
	.string "did.$"

Rogue_ReturnAdventureCommon_Text_2:
	.string "Lets have a look now...\p"
	.string "You managed to complete{PAUSE 15}.{PAUSE 15}.{PAUSE 15}.\n"
	.string "{STR_VAR_1} routes...{PAUSE 15}\p"
	.string "and you recieved {STR_VAR_2}\n"
	.string "BADGES...{PAUSE 15}$"

Rogue_ReturnAdventureCommon_Text_3:
	.string "So hmm...\n"
	.string "Your rewards should be{PAUSE 15}.{PAUSE 15}.{PAUSE 15}.$"

Rogue_ReturnAdventureCommon_Text_4:
	.string "You recieve ¥{STR_VAR_1}$"

Rogue_ReturnAdventureCommon_Text_5:
	.string "and for the BADGE's...$"

Rogue_ReturnAdventureCommon_Text_6:
	.string "By levelling up your PARTNER POKéMON,\n"
	.string "you'll get the option to skip over\l"
	.string "eariler GYM LEADERs.\p"
	.string "Although it does take a lot of levels to\n"
	.string "do that...$"

Rogue_ReturnAdventureCommon_Text_7:
	.string "Since you're back from your first\n"
	.string "adventure, you should have a look\l"
	.string "round the HUB!\p"
	.string "You tend to get fans as you adventure\n"
	.string "more and some of them may want to move\l"
	.string "into the HUB and setup some useful\l"
	.string "facilities.$"

Rogue_ReturnAdventureCommon_Text_8:
	.string "Would you like to SAVE your progress?$"

Rogue_ReturnAdventureCommon_Text_9:
	.string "Come speak to me once you're ready for\n"
	.string "your next adventure.$"
