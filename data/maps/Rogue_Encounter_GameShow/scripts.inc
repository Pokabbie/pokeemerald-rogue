Rogue_Encounter_GameShow_MapScripts::
	.byte 0


Rogue_Encounter_GameShow_TriggerIntro::
	lockall
	msgbox Rogue_Encounter_GameShow_TriggerIntro_Text_0, MSGBOX_DEFAULT
	applymovement 2, GameShow_Host_Spin
	waitmovement 0
	msgbox Rogue_Encounter_GameShow_TriggerIntro_Text_1, MSGBOX_DEFAULT
	call Rogue_Encounter_GameShow_DropBoxes
	releaseall
	setvar VAR_TEMP_0, 1
	setvar VAR_TEMP_1, 1
	return


Rogue_Encounter_GameShow_RestoreBoxes::
	waitse
	playse SE_BALL_BOUNCE_1
	hideobjectat 3, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	hideobjectat 1, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	setobjectxy 3, 5, 2
	setobjectxy 1, 9, 2
	showobjectat 3, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	showobjectat 1, MAP_ROGUE_ENCOUNTER_GAME_SHOW
	return


Rogue_Encounter_GameShow_DropBoxes::
	waitse
	playse SE_FALL
	waitse
	applymovement 3, GameShow_BoxFall
	applymovement 1, GameShow_BoxFall
	waitmovement 0
	waitse
	playse SE_BRIDGE_WALK
	return


Rogue_Encounter_GameShow_NextRound::
	compare VAR_TEMP_1, 3
	goto_if_ge Rogue_Encounter_GameShow_NextRound_1
	addvar VAR_TEMP_1, 1
	buffernumberstring STR_VAR_1, VAR_TEMP_1
	call Rogue_Encounter_GameShow_RestoreBoxes
	applymovement 2, GameShow_Host_Spin
	waitmovement 0
	msgbox Rogue_Encounter_GameShow_NextRound_Text_0, MSGBOX_DEFAULT
	call Rogue_Encounter_GameShow_DropBoxes
	return

Rogue_Encounter_GameShow_NextRound_1:
	call Rogue_Encounter_GameShow_Finish
	return


Rogue_Encounter_GameShow_Finish::
	call Rogue_Encounter_GameShow_RestoreBoxes
	applymovement 2, GameShow_Host_Spin
	waitmovement 0
	msgbox Rogue_Encounter_GameShow_Finish_Text_0, MSGBOX_DEFAULT
	applymovement 2, GameShow_Host_Exit
	waitmovement 0
	setobjectxyperm 2, 7, 0
	return


Rogue_Encounter_GameShow_ChooseInternal::
	msgbox Rogue_Encounter_GameShow_ChooseInternal_Text_0
	call Rogue_Encounter_GameShow_NextRound
	return


Rogue_Encounter_GameShow_ChooseLeft::
	lock
	turnobject 2, DIR_WEST
	msgbox Rogue_Encounter_GameShow_ChooseLeft_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_GameShow_ChooseLeft_2
Rogue_Encounter_GameShow_ChooseLeft_1:
	release
	return

Rogue_Encounter_GameShow_ChooseLeft_2:
	msgbox Rogue_Encounter_GameShow_ChooseLeft_Text_1, MSGBOX_DEFAULT
	call Rogue_Encounter_GameShow_ChooseInternal
	goto Rogue_Encounter_GameShow_ChooseLeft_1


Rogue_Encounter_GameShow_ChooseRight::
	lock
	turnobject 2, DIR_EAST
	msgbox Rogue_Encounter_GameShow_ChooseRight_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_GameShow_ChooseRight_2
Rogue_Encounter_GameShow_ChooseRight_1:
	release
	return

Rogue_Encounter_GameShow_ChooseRight_2:
	msgbox Rogue_Encounter_GameShow_ChooseRight_Text_1, MSGBOX_DEFAULT
	call Rogue_Encounter_GameShow_ChooseInternal
	goto Rogue_Encounter_GameShow_ChooseRight_1


GameShow_BoxFall:
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	walk_faster_down
	step_end

GameShow_Host_Spin:
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

GameShow_Host_Exit:
	jump_2_up
	jump_2_up
	step_end

Rogue_Encounter_GameShow_TriggerIntro_Text_0:
	.string "WAHEY!$"

Rogue_Encounter_GameShow_TriggerIntro_Text_1:
	.string "Welcome to my ever so lovingly setup\n"
	.string "GAME SHOW!\p"
	.string "This game is simple!\n"
	.string "You'll get 2 choices...\p"
	.string "One might be good...\n"
	.string "One might be bad..!\p"
	.string "Now!\n"
	.string "On to Round 1!$"

Rogue_Encounter_GameShow_NextRound_Text_0:
	.string "On to Round {STR_VAR_1}!$"

Rogue_Encounter_GameShow_Finish_Text_0:
	.string "That's all for now folks!\n"
	.string "Ke ke ke$"

Rogue_Encounter_GameShow_ChooseInternal_Text_0:
	.string "TODO$"

Rogue_Encounter_GameShow_ChooseLeft_Text_0:
	.string "So you pick the LEFT?$"

Rogue_Encounter_GameShow_ChooseLeft_Text_1:
	.string "The LEFT it is!$"

Rogue_Encounter_GameShow_ChooseRight_Text_0:
	.string "So you pick the RIGHT?$"

Rogue_Encounter_GameShow_ChooseRight_Text_1:
	.string "The RIGHT it is!$"
