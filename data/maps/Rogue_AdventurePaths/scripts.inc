Rogue_AdventurePaths_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_AdventurePaths_OnLoad
	.byte 0


Rogue_AdventurePaths_OnLoad::
	special SpawnCameraObject
	applymovement OBJ_EVENT_ID_CAMERA, AdventurePaths_OffsetCamera
	call Rogue_AdventurePaths_RedrawMap
	return


AdventurePaths_OffsetCamera:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Rogue_AdventurePaths_RedrawMap::
	special RogueAdv_ApplyAdventureMetatiles
	special RogueAdv_UpdateObjectGFX
	return


Rogue_AdventurePaths_InteractNodeInternal::
	lock
	special RogueAdv_GetNodeParams
	switch VAR_0x8002
	case 6, Rogue_AdventurePaths_InteractNodeInternal_3
	case 2, Rogue_AdventurePaths_InteractNodeInternal_4
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_4
Rogue_AdventurePaths_InteractNodeInternal_1:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_5, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_AdventurePaths_InteractNodeInternal_7
Rogue_AdventurePaths_InteractNodeInternal_6:
	release
	return

Rogue_AdventurePaths_InteractNodeInternal_3:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_0
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_4:
	switch VAR_0x8003
	case 0, Rogue_AdventurePaths_InteractNodeInternal_10
	case 1, Rogue_AdventurePaths_InteractNodeInternal_11
	case 2, Rogue_AdventurePaths_InteractNodeInternal_12
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_4
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_7:
	special RogueAdv_ExecuteNodeAction
	goto Rogue_AdventurePaths_InteractNodeInternal_6

Rogue_AdventurePaths_InteractNodeInternal_10:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_1
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_11:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_2
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_12:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_3
	goto Rogue_AdventurePaths_InteractNodeInternal_1


Rogue_AdventurePaths_InteractNode0::
	setvar VAR_0x8001, 0
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode1::
	setvar VAR_0x8001, 1
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode2::
	setvar VAR_0x8001, 2
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode3::
	setvar VAR_0x8001, 3
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode4::
	setvar VAR_0x8001, 4
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode5::
	setvar VAR_0x8001, 5
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode6::
	setvar VAR_0x8001, 6
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode7::
	setvar VAR_0x8001, 7
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode8::
	setvar VAR_0x8001, 8
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode9::
	setvar VAR_0x8001, 9
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode10::
	setvar VAR_0x8001, 10
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode11::
	setvar VAR_0x8001, 11
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode12::
	setvar VAR_0x8001, 12
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode13::
	setvar VAR_0x8001, 13
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode14::
	setvar VAR_0x8001, 14
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode15::
	setvar VAR_0x8001, 15
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNodeInternal_Text_0:
	.string "A might trainer!$"

Rogue_AdventurePaths_InteractNodeInternal_Text_1:
	.string "It's a healing Rest Stop.$"

Rogue_AdventurePaths_InteractNodeInternal_Text_2:
	.string "It's a Shop Rest Stop.$"

Rogue_AdventurePaths_InteractNodeInternal_Text_3:
	.string "It's a Battle Prep. Rest Stop.$"

Rogue_AdventurePaths_InteractNodeInternal_Text_4:
	.string "??$"

Rogue_AdventurePaths_InteractNodeInternal_Text_5:
	.string "Enter this encounter?$"
