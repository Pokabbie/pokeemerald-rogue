Rogue_Encounter_Graveyard_MapScripts::
	.byte 0


Rogue_Graveyard_EnterTrigger::
	lock
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, Graveyard_CameraInitialFocus
	waitmovement 0
	msgbox Rogue_Graveyard_EnterTrigger_Text_0, MSGBOX_DEFAULT
	applymovement OBJ_EVENT_ID_CAMERA, Graveyard_CameraInitialUnfocus
	waitmovement 0
	special RemoveCameraObject
	setvar VAR_TEMP_0, 1
	release
	return


Rogue_Graveyard_SelectOption_Internal::
	lockall
	bufferitemname STR_VAR_1, VAR_0x8001
	msgbox Rogue_Graveyard_SelectOption_Internal_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Graveyard_SelectOption_Internal_2
Rogue_Graveyard_SelectOption_Internal_1:
	releaseall
	return

Rogue_Graveyard_SelectOption_Internal_2:
	finditem VAR_0x8001, 1
	playse SE_THUNDER
	removeobject 1, MAP_ROGUE_ENCOUNTER_GRAVEYARD
	removeobject 2, MAP_ROGUE_ENCOUNTER_GRAVEYARD
	removeobject 3, MAP_ROGUE_ENCOUNTER_GRAVEYARD
	special DoSealedChamberShakingEffect_Long
	applymovement 4, Graveyard_Devil_Eyes
	msgbox Rogue_Graveyard_SelectOption_Internal_Text_1, MSGBOX_DEFAULT
	waitmovement 0
	giveitem VAR_ROGUE_ITEM10, 1
	applymovement 4, Graveyard_Devil_Leave
	waitmovement 0
	removeobject 4, MAP_ROGUE_ENCOUNTER_GRAVEYARD
	completequest QUEST_DevilDeal
	goto Rogue_Graveyard_SelectOption_Internal_1


Rogue_Graveyard_SelectOption0::
	copyvar VAR_0x8001, VAR_ROGUE_ITEM0
	call Rogue_Graveyard_SelectOption_Internal
	return


Rogue_Graveyard_SelectOption1::
	copyvar VAR_0x8001, VAR_ROGUE_ITEM1
	call Rogue_Graveyard_SelectOption_Internal
	return


Rogue_Graveyard_SelectOption2::
	copyvar VAR_0x8001, VAR_ROGUE_ITEM2
	call Rogue_Graveyard_SelectOption_Internal
	return


Graveyard_CameraInitialFocus:
	walk_up
	walk_up
	walk_up
	step_end

Graveyard_CameraInitialUnfocus:
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

Graveyard_Devil_Eyes:
	face_left
	step_end

Graveyard_Devil_Leave:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

Rogue_Graveyard_EnterTrigger_Text_0:
	.string "???: One... choice...$"

Rogue_Graveyard_SelectOption_Internal_Text_0:
	.string "Choose {STR_VAR_1}?$"

Rogue_Graveyard_SelectOption_Internal_Text_1:
	.string "???: The price this time shall be...$"
