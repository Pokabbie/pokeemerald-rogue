Rogue_Hub_Interior_ConfigRoom_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_ConfigRoom_RedrawMap
	.byte 0


Rogue_ConfigRoom_TalkLanette::
	lock
	faceplayer
	msgbox Rogue_ConfigRoom_TalkLanette_Text_0, MSGBOX_DEFAULT
	release
	return


Rogue_ConfigRoom_TalkAssistant::
	lock
	faceplayer
	msgbox Rogue_ConfigRoom_TalkAssistant_Text_0, MSGBOX_DEFAULT
	call Rogue_ConfigRoom_TalkAssistantInternal
	release
	return


Rogue_ConfigRoom_TalkAssistantInternal::
	goto_if_set FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_TalkAssistantInternal_1
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_TalkAssistantInternal_6
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_WalkLeft
	applymovement 1, ConfigRoom_LookLeft
	waitmovement 0
	return

Rogue_ConfigRoom_TalkAssistantInternal_1:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_TalkAssistantInternal_4
	return

Rogue_ConfigRoom_TalkAssistantInternal_4:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_1, MSGBOX_DEFAULT
	clearflag FLAG_SET_SEED_ENABLED
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_WalkLeft
	applymovement 1, ConfigRoom_LookLeft
	waitmovement 0
	return

Rogue_ConfigRoom_TalkAssistantInternal_6:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_3, MSGBOX_DEFAULT
	setflag FLAG_SET_SEED_ENABLED
	return


Rogue_ConfigRoom_WalkAboveAssistant::
	lockall
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_WalkAboveAssistant_2
Rogue_ConfigRoom_WalkAboveAssistant_1:
	releaseall
	return

Rogue_ConfigRoom_WalkAboveAssistant_2:
	lockall
	applymovement 1, ConfigRoom_LookUp
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_LookDown
	waitmovement 0
	call Rogue_ConfigRoom_TalkAssistantInternal
	goto Rogue_ConfigRoom_WalkAboveAssistant_1


Rogue_ConfigRoom_WalkBelowAssistant::
	lockall
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_WalkBelowAssistant_2
Rogue_ConfigRoom_WalkBelowAssistant_1:
	releaseall
	return

Rogue_ConfigRoom_WalkBelowAssistant_2:
	applymovement 1, ConfigRoom_LookDown
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_LookUp
	waitmovement 0
	call Rogue_ConfigRoom_TalkAssistantInternal
	goto Rogue_ConfigRoom_WalkBelowAssistant_1


ConfigRoom_LookUp:
	face_up
	step_end

ConfigRoom_LookDown:
	face_down
	step_end

ConfigRoom_LookLeft:
	face_left
	step_end

ConfigRoom_WalkLeft:
	walk_left
	step_end

Rogue_ConfigRoom_PreButtonInternal::
	lockall
	compare VAR_0x8000, YES
	goto_if_eq Rogue_ConfigRoom_PreButtonInternal_2
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreButtonInternal_Text_1
	bufferstring STR_VAR_3, Rogue_ConfigRoom_PreButtonInternal_Text_0
Rogue_ConfigRoom_PreButtonInternal_1:
	msgbox Rogue_ConfigRoom_PreButtonInternal_Text_2, MSGBOX_DEFAULT
	msgbox Rogue_ConfigRoom_PreButtonInternal_Text_3, MSGBOX_YESNO
	return

Rogue_ConfigRoom_PreButtonInternal_2:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreButtonInternal_Text_0
	bufferstring STR_VAR_3, Rogue_ConfigRoom_PreButtonInternal_Text_1
	goto Rogue_ConfigRoom_PreButtonInternal_1


Rogue_ConfigRoom_PostButtonInternal::
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_PostButtonInternal_2
Rogue_ConfigRoom_PostButtonInternal_1:
	releaseall
	return

Rogue_ConfigRoom_PostButtonInternal_2:
	waitse
	playse SE_PIN
	call Rogue_ConfigRoom_RedrawMap
	msgbox Rogue_ConfigRoom_PostButtonInternal_Text_0, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_PostButtonInternal_1


Rogue_ConfigRoom_RedrawMap::
	goto_if_set FLAG_ROGUE_EXP_ALL, Rogue_ConfigRoom_RedrawMap_2
	setmetatile 1, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_1:
	special DrawWholeMapView
	return

Rogue_ConfigRoom_RedrawMap_2:
	setmetatile 1, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_1


Rogue_ConfigRoom_Button_ExpAll::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_ExpAll_Text_0
	goto_if_set FLAG_ROGUE_EXP_ALL, Rogue_ConfigRoom_Button_ExpAll_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_ExpAll_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_ExpAll_6
Rogue_ConfigRoom_Button_ExpAll_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_ExpAll_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_ExpAll_1

Rogue_ConfigRoom_Button_ExpAll_6:
	goto_if_set FLAG_ROGUE_EXP_ALL, Rogue_ConfigRoom_Button_ExpAll_8
	setflag FLAG_ROGUE_EXP_ALL
	goto Rogue_ConfigRoom_Button_ExpAll_5

Rogue_ConfigRoom_Button_ExpAll_8:
	clearflag FLAG_ROGUE_EXP_ALL
	goto Rogue_ConfigRoom_Button_ExpAll_5


Rogue_ConfigRoom_TalkLanette_Text_0:
	.string "LANETTE: Hi {PLAYER}!\p"
	.string "This is the CONFIG LAB!\p"
	.string "In here you can change settings for\n"
	.string "the next time you go on an adventure.\p"
	.string "Just step on the switches to change\n"
	.string "the settings.$"

Rogue_ConfigRoom_TalkAssistant_Text_0:
	.string "ASSISTANT: The settings back here are\n"
	.string "for SEEDED mode!\p"
	.string "In SEEDED mode you will always\n"
	.string "encounter the same ROUTES, TRAINERS\l"
	.string "and POKéMON$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_0:
	.string "ASSISTANT: Do you want to disable\n"
	.string "SEEDED mode?$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_1:
	.string "ASSISTANT: OK.\n"
	.string "I'll disable SEEDED mode$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_2:
	.string "ASSISTANT: Do you want to enable\n"
	.string "SEEDED mode?$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_3:
	.string "ASSISTANT: OK.\n"
	.string "I'll enable SEEDED mode$"

Rogue_ConfigRoom_PreButtonInternal_Text_0:
	.string "ON$"

Rogue_ConfigRoom_PreButtonInternal_Text_1:
	.string "OFF$"

Rogue_ConfigRoom_PreButtonInternal_Text_2:
	.string "{STR_VAR_1} is set to\n"
	.string "{STR_VAR_2}$"

Rogue_ConfigRoom_PreButtonInternal_Text_3:
	.string "Flip the switch to {STR_VAR_3}?$"

Rogue_ConfigRoom_PostButtonInternal_Text_0:
	.string "{STR_VAR_1} set to {STR_VAR_3}$"

Rogue_ConfigRoom_Button_ExpAll_Text_0:
	.string "EXP. ALL$"
