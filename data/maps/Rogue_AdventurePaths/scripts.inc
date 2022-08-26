Rogue_AdventurePaths_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_AdventurePaths_RedrawMap
	.byte 0


Rogue_AdventurePaths_RedrawMap::
	special RogueAdv_ApplyAdventureMetatiles
	special RogueAdv_UpdateObjectGFX
	return


Rogue_AdventurePaths_InteractNodeInternal::
	lock
	special RogueAdv_BufferNodeMessage
	msgbox Rogue_AdventurePaths_InteractNodeInternal_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_AdventurePaths_InteractNodeInternal_2
Rogue_AdventurePaths_InteractNodeInternal_1:
	release
	return

Rogue_AdventurePaths_InteractNodeInternal_2:
	special RogueAdv_ExecuteNodeAction
	goto Rogue_AdventurePaths_InteractNodeInternal_1


Rogue_AdventurePaths_InteractNode0::
	setvar VAR_0x8000, 0
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode1::
	setvar VAR_0x8000, 1
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode2::
	setvar VAR_0x8000, 2
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode3::
	setvar VAR_0x8000, 3
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode4::
	setvar VAR_0x8000, 4
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode5::
	setvar VAR_0x8000, 5
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode6::
	setvar VAR_0x8000, 6
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode7::
	setvar VAR_0x8000, 7
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode8::
	setvar VAR_0x8000, 8
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode9::
	setvar VAR_0x8000, 9
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode10::
	setvar VAR_0x8000, 10
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode11::
	setvar VAR_0x8000, 11
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode12::
	setvar VAR_0x8000, 12
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode13::
	setvar VAR_0x8000, 13
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode14::
	setvar VAR_0x8000, 14
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNode15::
	setvar VAR_0x8000, 15
	call Rogue_AdventurePaths_InteractNodeInternal
	return


Rogue_AdventurePaths_InteractNodeInternal_Text_0:
	.string "{STR_VAR_1}$"
