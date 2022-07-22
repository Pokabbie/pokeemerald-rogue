Rogue_Hub_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RogueHub_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_LITTLEROOT_INTRO_STATE, 0, RogueHub_Intro0
	.2byte 0


RogueHub_OnTransition::
	compare VAR_LITTLEROOT_INTRO_STATE, 2
	goto_if_le RogueHub_OnTransition_1
	return

RogueHub_OnTransition_1:
	setobjectxyperm 2, 22, 6
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
	lockall
	faceplayer
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_le RogueHub_SpeakScott_2
RogueHub_SpeakScott_1:
	releaseall
	end

RogueHub_SpeakScott_2:
	msgbox RogueHub_SpeakScott_Text_0
	goto RogueHub_SpeakScott_1


RogueHub_EventScript_ExitSign::
	buffernumberstring STR_VAR_1, VAR_ROGUE_FURTHEST_DIFFICULTY
	msgbox RogueHub_EventScript_ExitSign_Text_0, MSGBOX_SIGN
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

RogueHub_EventScript_ExitSign_Text_0:
	.string "The journey begins beyond this point..\p"
	.string "Furthest Progress defeated\n"
	.string "{STR_VAR_1} bosses..$"
