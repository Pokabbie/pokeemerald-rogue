Rogue_AdventurePaths_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Rogue_AdventurePaths_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_AdventurePaths_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_AdventurePaths_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_0, 0, Rogue_AdventurePaths_PreviewCutscene
	.2byte 0


Rogue_AdventurePaths_OnLoad::
	special RogueAdv_ApplyAdventureMetatiles
	return


Rogue_AdventurePaths_PreviewCutscene::
	setvar VAR_TEMP_0, 1
	return


AdventurePaths_OffsetCamera:
	walk_right
	walk_right
	walk_right
	walk_right
	step_end

Rogue_AdventurePaths_InteractNode::
	lock
	special RogueAdv_GetNodeParams
	switch VAR_0x8005
	case ADVPATH_ROOM_BOSS, Rogue_AdventurePaths_InteractNode_3
	case ADVPATH_ROOM_MINIBOSS, Rogue_AdventurePaths_InteractNode_4
	case ADVPATH_ROOM_WILD_DEN, Rogue_AdventurePaths_InteractNode_5
	case ADVPATH_ROOM_GAMESHOW, Rogue_AdventurePaths_InteractNode_6
	case ADVPATH_ROOM_DARK_DEAL, Rogue_AdventurePaths_InteractNode_7
	case ADVPATH_ROOM_LAB, Rogue_AdventurePaths_InteractNode_8
	case ADVPATH_ROOM_RESTSTOP, Rogue_AdventurePaths_InteractNode_9
	case ADVPATH_ROOM_ROUTE, Rogue_AdventurePaths_InteractNode_10
	case ADVPATH_ROOM_LEGENDARY, Rogue_AdventurePaths_InteractNode_11
	msgbox Rogue_AdventurePaths_InteractNode_Text_9, MSGBOX_YESNO
Rogue_AdventurePaths_InteractNode_1:
	compare VAR_RESULT, YES
	goto_if_eq Rogue_AdventurePaths_InteractNode_14
Rogue_AdventurePaths_InteractNode_13:
	release
	return

Rogue_AdventurePaths_InteractNode_3:
	msgbox Rogue_AdventurePaths_InteractNode_Text_0, MSGBOX_DEFAULT
	setvar VAR_RESULT, YES
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_4:
	msgbox Rogue_AdventurePaths_InteractNode_Text_1, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_5:
	msgbox Rogue_AdventurePaths_InteractNode_Text_2, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_6:
	msgbox Rogue_AdventurePaths_InteractNode_Text_3, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_7:
	msgbox Rogue_AdventurePaths_InteractNode_Text_4, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_8:
	msgbox Rogue_AdventurePaths_InteractNode_Text_5, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_9:
	switch VAR_0x8006
	case 0, Rogue_AdventurePaths_InteractNode_17
	case 1, Rogue_AdventurePaths_InteractNode_18
	case 2, Rogue_AdventurePaths_InteractNode_19
	msgbox Rogue_AdventurePaths_InteractNode_Text_9, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_10:
	switch VAR_0x8006
	case 0, Rogue_AdventurePaths_InteractNode_22
	case 1, Rogue_AdventurePaths_InteractNode_23
	case 2, Rogue_AdventurePaths_InteractNode_24
	msgbox Rogue_AdventurePaths_InteractNode_Text_9, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_11:
	msgbox Rogue_AdventurePaths_InteractNode_Text_13, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_14:
	special RogueAdv_ExecuteNodeAction
	goto Rogue_AdventurePaths_InteractNode_13

Rogue_AdventurePaths_InteractNode_17:
	msgbox Rogue_AdventurePaths_InteractNode_Text_6, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_18:
	msgbox Rogue_AdventurePaths_InteractNode_Text_7, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_19:
	msgbox Rogue_AdventurePaths_InteractNode_Text_8, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_22:
	msgbox Rogue_AdventurePaths_InteractNode_Text_10, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_23:
	msgbox Rogue_AdventurePaths_InteractNode_Text_11, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1

Rogue_AdventurePaths_InteractNode_24:
	msgbox Rogue_AdventurePaths_InteractNode_Text_12, MSGBOX_YESNO
	goto Rogue_AdventurePaths_InteractNode_1


Rogue_AdventurePaths_InteractNode_Text_0:
	.string "It's a Mighty Trainer!$"

Rogue_AdventurePaths_InteractNode_Text_1:
	.string "It's a Strong Trainer!\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_2:
	.string "It's the home of a Strong POKéMON!\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_3:
	.string "It's a... Game Show?\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_4:
	.string "It's a Dark Deal...?\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_5:
	.string "It's a POKéMON LAB.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_6:
	.string "It's a Full Rest Stop.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_7:
	.string "It's a Mart Rest Stop.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_8:
	.string "It's a Battle Prep. Rest Stop.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_9:
	.string "??\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_10:
	.string "It's a CALM {STR_VAR_1} route.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_11:
	.string "It's an AVERAGE {STR_VAR_1} route.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_12:
	.string "It's an TOUGH {STR_VAR_1} route.\n"
	.string "Enter this encounter?$"

Rogue_AdventurePaths_InteractNode_Text_13:
	.string "It's a Powerful POKéMON!\n"
	.string "Enter this encounter?$"
