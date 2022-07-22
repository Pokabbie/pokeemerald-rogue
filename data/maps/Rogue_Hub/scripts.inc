Rogue_Hub_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RogueHub_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 0, RogueHub_Intro0
	.2byte 0


RogueHub_OnTransition::
	compare VAR_LITTLEROOT_INTRO_STATE, 2
	goto_if_le RogueHub_OnTransition_2
RogueHub_OnTransition_1:
	compare VAR_LITTLEROOT_INTRO_STATE, 5
	goto_if_ge RogueHub_OnTransition_4
	return

RogueHub_OnTransition_2:
	setobjectxyperm 2, 22, 6
	goto RogueHub_OnTransition_1

RogueHub_OnTransition_4:
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
	goto_if_gt RogueHub_SpeakScott_19
	setvar VAR_RESULT, YES
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_11:
	msgbox RogueHub_SpeakScott_Text_4, MSGBOX_DEFAULT
	setvar VAR_LITTLEROOT_INTRO_STATE, 5
	warp MAP_ROGUE_HUB_TRANSITION, 0
	goto RogueHub_SpeakScott_1

RogueHub_SpeakScott_13:
	compare VAR_RESULT, 2
	goto_if_gt RogueHub_SpeakScott_16
	setvar VAR_RESULT, YES
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_16:
	msgbox RogueHub_SpeakScott_Text_3, MSGBOX_YESNO
	setvar VAR_RESULT, NO
	goto RogueHub_SpeakScott_8

RogueHub_SpeakScott_19:
	msgbox RogueHub_SpeakScott_Text_3, MSGBOX_YESNO
	setvar VAR_RESULT, NO
	goto RogueHub_SpeakScott_8


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

RogueHub_Intro0_Text_0:
	.string "SCOTT: Well we're here!\p"
	.string "The POKéMON HUB!\p"
	.string "Sorry again for making you sit in the\n"
	.string "back with all the supplies.\p"
	.string "I'm sure you're raring to go, but first\n"
	.string "you should go speak the PROF. first\n"
	.string "I'll be here waiting when you're done.$"

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
	.string "SCOTT: Good luck CHAMP!$"

RogueHub_Sign_AdventureSign_Text_0:
	.string "Adventures await beyond this point...\p"
	.string "Furthest Progress:\n"
	.string "Gained {STR_VAR_1} BADGES...$"

RogueHub_Sign_ProfLab_Text_0:
	.string "PROF. BIRCH's LAB$"

RogueHub_Sign_ConfigLab_Text_0:
	.string "CONFIG LAB$"
