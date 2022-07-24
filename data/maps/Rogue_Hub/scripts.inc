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
	setobjectxy 2, 22, 6
	return


RogueHub_OnMapLoad::
	call RogueHub_HideSafari
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
	goto_if_eq RogueHub_ReturnAdventure_13
RogueHub_ReturnAdventure_12:
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
	goto RogueHub_ReturnAdventure_6

RogueHub_ReturnAdventure_10:
	msgbox RogueHub_ReturnAdventure_Text_5, MSGBOX_DEFAULT
	goto RogueHub_ReturnAdventure_9

RogueHub_ReturnAdventure_13:
	call Common_EventScript_SaveGame
	goto RogueHub_ReturnAdventure_12


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


RogueHub_HideSafari::
	setmetatile 9, 3, 0x10C, TRUE
	setmetatile 10, 3, 0x10C, TRUE
	setmetatile 11, 3, 0x10C, TRUE
	setmetatile 9, 4, 0x10C, TRUE
	setmetatile 10, 4, 0x10C, TRUE
	setmetatile 11, 4, 0x10C, TRUE
	setmetatile 9, 5, 0x10C, TRUE
	setmetatile 10, 5, 0x10C, TRUE
	setmetatile 11, 5, 0x10C, TRUE
	setmetatile 9, 6, 0x10C, TRUE
	setmetatile 10, 6, 0x10C, TRUE
	setmetatile 11, 6, 0x10C, TRUE
	setmetatile 9, 7, 0x10C, TRUE
	setmetatile 10, 7, 0x10C, TRUE
	setmetatile 11, 7, 0x10C, TRUE
	setmetatile 8, 2, 0x075, TRUE
	setmetatile 8, 3, 0x075, TRUE
	setmetatile 8, 4, 0x075, TRUE
	setmetatile 8, 5, 0x075, TRUE
	setmetatile 8, 6, 0x075, TRUE
	setmetatile 8, 7, 0x075, TRUE
	setmetatile 12, 2, 0x073, TRUE
	setmetatile 12, 3, 0x073, TRUE
	setmetatile 12, 4, 0x073, TRUE
	setmetatile 12, 5, 0x073, TRUE
	setmetatile 12, 6, 0x073, TRUE
	setmetatile 12, 7, 0x073, TRUE
	setmetatile 8, 8, 0x002, FALSE
	setmetatile 9, 8, 0x002, FALSE
	setmetatile 10, 8, 0x002, FALSE
	setmetatile 11, 8, 0x002, FALSE
	setmetatile 12, 8, 0x002, FALSE
	setmetatile 0, 2, 0x001, TRUE
	setmetatile 1, 2, 0x001, TRUE
	setmetatile 2, 2, 0x001, TRUE
	setmetatile 3, 2, 0x001, TRUE
	setmetatile 4, 2, 0x001, TRUE
	setmetatile 5, 2, 0x001, TRUE
	setmetatile 6, 2, 0x001, TRUE
	setmetatile 7, 2, 0x001, TRUE
	setmetatile 0, 3, 0x001, TRUE
	setmetatile 1, 3, 0x001, TRUE
	setmetatile 2, 3, 0x001, TRUE
	setmetatile 3, 3, 0x001, TRUE
	setmetatile 4, 3, 0x001, TRUE
	setmetatile 5, 3, 0x001, TRUE
	setmetatile 6, 3, 0x001, TRUE
	setmetatile 7, 3, 0x001, TRUE
	setmetatile 0, 4, 0x001, TRUE
	setmetatile 1, 4, 0x001, TRUE
	setmetatile 2, 4, 0x001, TRUE
	setmetatile 3, 4, 0x001, TRUE
	setmetatile 4, 4, 0x001, TRUE
	setmetatile 5, 4, 0x001, TRUE
	setmetatile 6, 4, 0x001, TRUE
	setmetatile 7, 4, 0x001, TRUE
	setmetatile 0, 5, 0x001, TRUE
	setmetatile 1, 5, 0x001, TRUE
	setmetatile 2, 5, 0x001, TRUE
	setmetatile 3, 5, 0x001, TRUE
	setmetatile 4, 5, 0x001, TRUE
	setmetatile 5, 5, 0x001, TRUE
	setmetatile 6, 5, 0x001, TRUE
	setmetatile 7, 5, 0x001, TRUE
	setmetatile 0, 6, 0x001, TRUE
	setmetatile 1, 6, 0x001, TRUE
	setmetatile 2, 6, 0x001, TRUE
	setmetatile 3, 6, 0x001, TRUE
	setmetatile 4, 6, 0x001, TRUE
	setmetatile 5, 6, 0x001, TRUE
	setmetatile 6, 6, 0x001, TRUE
	setmetatile 7, 6, 0x001, TRUE
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

RogueHub_Sign_AdventureSign_Text_0:
	.string "Adventures await beyond this point...\p"
	.string "Furthest Progress:\n"
	.string "Gained {STR_VAR_1} BADGES...$"

RogueHub_Sign_ProfLab_Text_0:
	.string "PROF. BIRCH's LAB$"

RogueHub_Sign_ConfigLab_Text_0:
	.string "CONFIG LAB$"
