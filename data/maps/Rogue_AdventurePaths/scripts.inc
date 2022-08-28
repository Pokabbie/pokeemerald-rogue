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
	case 1, Rogue_AdventurePaths_InteractNodeInternal_5
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_4, MSGBOX_YESNO
Rogue_AdventurePaths_InteractNodeInternal_1:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_AdventurePaths_InteractNodeInternal_8
Rogue_AdventurePaths_InteractNodeInternal_7:
	release
	return

Rogue_AdventurePaths_InteractNodeInternal_3:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_0, MSGBOX_DEFAULT
	setvar VAR_RESULT, YES
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_4:
	switch VAR_0x8003
	case 0, Rogue_AdventurePaths_InteractNodeInternal_11
	case 1, Rogue_AdventurePaths_InteractNodeInternal_12
	case 2, Rogue_AdventurePaths_InteractNodeInternal_13
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_4, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_5:
	switch VAR_0x8003
	case 0, Rogue_AdventurePaths_InteractNodeInternal_16
	case 1, Rogue_AdventurePaths_InteractNodeInternal_17
	case 2, Rogue_AdventurePaths_InteractNodeInternal_18
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_4, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_8:
	special RogueAdv_ExecuteNodeAction
	goto Rogue_AdventurePaths_InteractNodeInternal_7

Rogue_AdventurePaths_InteractNodeInternal_11:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_1, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_12:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_2, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_13:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_3, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_16:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_5, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_17:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_6, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNodeInternal_1

Rogue_AdventurePaths_InteractNodeInternal_18:
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_7, MSGBOX_YESNO
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
	.string "It's a Mighty Trainer!$"

Rogue_AdventurePaths_InteractNodeInternal_Text_1:
	.string "It's a Healing Rest Stop.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNodeInternal_Text_2:
	.string "It's a Mart Rest Stop.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNodeInternal_Text_3:
	.string "It's a Battle Prep. Rest Stop.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNodeInternal_Text_4:
	.string "??\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNodeInternal_Text_5:
	.string "It's a CALM {STR_VAR_1} route.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNodeInternal_Text_6:
	.string "It's an AVERAGE {STR_VAR_1} route.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNodeInternal_Text_7:
	.string "It's an TOUGH {STR_VAR_1} route.\n"
	.string "Enter this encounter?$"
