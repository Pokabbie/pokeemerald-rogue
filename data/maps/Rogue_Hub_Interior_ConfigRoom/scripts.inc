Rogue_Hub_Interior_ConfigRoom_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_ConfigRoom_RedrawMap
	.byte 0


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
	setmetatile 7, 6, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_1:
	special DrawWholeMapView
	return

Rogue_ConfigRoom_RedrawMap_2:
	setmetatile 7, 6, METATILE_BikeShop_Button_Green, FALSE
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
