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
	goto_if_unset FLAG_SET_SEED_ENABLED, Rogue_ConfigRoom_TalkAssistant_2
Rogue_ConfigRoom_TalkAssistant_1:
	call Rogue_ConfigRoom_TalkAssistantInternal
	release
	return

Rogue_ConfigRoom_TalkAssistant_2:
	msgbox Rogue_ConfigRoom_TalkAssistant_Text_0, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_TalkAssistant_1


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
	setvar VAR_0x8004, EASY_CHAT_TYPE_TRENDY_PHRASE
	call Common_ShowEasyChatScreen
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_ConfigRoom_TalkAssistantInternal_9
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_5, MSGBOX_DEFAULT
	applymovement OBJ_EVENT_ID_PLAYER, ConfigRoom_WalkLeft
	applymovement 1, ConfigRoom_LookLeft
	waitmovement 0
	return

Rogue_ConfigRoom_TalkAssistantInternal_9:
	msgbox Rogue_ConfigRoom_TalkAssistantInternal_Text_4, MSGBOX_DEFAULT
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

Rogue_ConfigRoom_TalkHiddenEnterance::
	goto_if_unset FLAG_ROGUE_UNCOVERRED_POKABBIE, Rogue_ConfigRoom_TalkHiddenEnterance_1
	return

Rogue_ConfigRoom_TalkHiddenEnterance_1:
	lock
	msgbox Rogue_ConfigRoom_TalkHiddenEnterance_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_TalkHiddenEnterance_4
Rogue_ConfigRoom_TalkHiddenEnterance_3:
	release
	return

Rogue_ConfigRoom_TalkHiddenEnterance_4:
	setflag FLAG_ROGUE_UNCOVERRED_POKABBIE
	waitse
	playse SE_PIN
	call Rogue_ConfigRoom_RedrawMap
	goto Rogue_ConfigRoom_TalkHiddenEnterance_3


Rogue_ConfigRoom_MeetPokabbie::
	lockall
	applymovement 4, ConfigRoom_Pokabbie_NoticePlayer
	waitmovement 0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_0
	applymovement 4, ConfigRoom_Pokabbie_FaceUp
	waitmovement 0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_1
	applymovement 4, ConfigRoom_Pokabbie_FaceDown
	waitmovement 0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_2
	applymovement 4, ConfigRoom_Pokabbie_Spin
	waitmovement 0
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_3
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_4
	msgbox Rogue_ConfigRoom_MeetPokabbie_Text_5
	setflag FLAG_ROGUE_MET_POKABBIE
	setvar VAR_TEMP_1, 1
	releaseall
	return


Rogue_ConfigRoom_TalkPokabbie::
	lockall
	faceplayer
	msgbox Rogue_ConfigRoom_TalkPokabbie_Text_0, MSGBOX_DEFAULT
	applymovement 4, ConfigRoom_Pokabbie_Spin
	waitmovement 0
	releaseall
	return


Rogue_ConfigRoom_TalkDitto::
	lock
	playmoncry SPECIES_DITTO, CRY_MODE_NORMAL
	msgbox Rogue_ConfigRoom_TalkDitto_Text_0, MSGBOX_DEFAULT
	release
	return


ConfigRoom_Pokabbie_FaceDown:
	face_down
	step_end

ConfigRoom_Pokabbie_FaceUp:
	face_up
	step_end

ConfigRoom_Pokabbie_NoticePlayer:
	face_down
	emote_exclamation_mark
	step_end

ConfigRoom_Pokabbie_Spin:
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
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_PreButtonInternal_6
Rogue_ConfigRoom_PreButtonInternal_5:
	return

Rogue_ConfigRoom_PreButtonInternal_2:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreButtonInternal_Text_0
	bufferstring STR_VAR_3, Rogue_ConfigRoom_PreButtonInternal_Text_1
	goto Rogue_ConfigRoom_PreButtonInternal_1

Rogue_ConfigRoom_PreButtonInternal_6:
	waitse
	playse SE_PIN
	goto Rogue_ConfigRoom_PreButtonInternal_5


Rogue_ConfigRoom_PreEAHButtonInternal::
	lockall
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_PreEAHButtonInternal_3
	case 1, Rogue_ConfigRoom_PreEAHButtonInternal_4
	case 2, Rogue_ConfigRoom_PreEAHButtonInternal_5
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreEAHButtonInternal_Text_3
Rogue_ConfigRoom_PreEAHButtonInternal_1:
	message Rogue_ConfigRoom_PreEAHButtonInternal_Text_4
	waitmessage
	multichoice 20, 4, MULTI_EASY_AVERAGE_HARD_INFO, NO
	switch VAR_RESULT
	case 0, Rogue_ConfigRoom_PreEAHButtonInternal_8
	case 1, Rogue_ConfigRoom_PreEAHButtonInternal_9
	case 2, Rogue_ConfigRoom_PreEAHButtonInternal_10
	case 3, Rogue_ConfigRoom_PreEAHButtonInternal_11
	case MULTI_B_PRESSED, Rogue_ConfigRoom_PreEAHButtonInternal_12
	return

Rogue_ConfigRoom_PreEAHButtonInternal_3:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreEAHButtonInternal_Text_0
	goto Rogue_ConfigRoom_PreEAHButtonInternal_1

Rogue_ConfigRoom_PreEAHButtonInternal_4:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreEAHButtonInternal_Text_1
	goto Rogue_ConfigRoom_PreEAHButtonInternal_1

Rogue_ConfigRoom_PreEAHButtonInternal_5:
	bufferstring STR_VAR_2, Rogue_ConfigRoom_PreEAHButtonInternal_Text_2
	goto Rogue_ConfigRoom_PreEAHButtonInternal_1

Rogue_ConfigRoom_PreEAHButtonInternal_8:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 0
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreEAHButtonInternal_9:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 1
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreEAHButtonInternal_10:
	waitse
	playse SE_PIN
	setvar VAR_0x8000, 2
	setvar VAR_RESULT, YES
	return

Rogue_ConfigRoom_PreEAHButtonInternal_11:
	setvar VAR_RESULT, NO
	return

Rogue_ConfigRoom_PreEAHButtonInternal_12:
	setvar VAR_RESULT, YES
	return


Rogue_ConfigRoom_PostButtonInternal::
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_PostButtonInternal_2
Rogue_ConfigRoom_PostButtonInternal_1:
	releaseall
	return

Rogue_ConfigRoom_PostButtonInternal_2:
	call Rogue_ConfigRoom_RedrawMap
	goto Rogue_ConfigRoom_PostButtonInternal_1


Rogue_ConfigRoom_RedrawMap::
	goto_if_set FLAG_ROGUE_UNCOVERRED_POKABBIE, Rogue_ConfigRoom_RedrawMap_2
Rogue_ConfigRoom_RedrawMap_1:
	goto_if_set FLAG_ROGUE_MET_POKABBIE, Rogue_ConfigRoom_RedrawMap_5
	setvar VAR_TEMP_1, 0
Rogue_ConfigRoom_RedrawMap_4:
	goto_if_set FLAG_ROGUE_EXP_ALL, Rogue_ConfigRoom_RedrawMap_9
	setmetatile 1, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_8:
	goto_if_set FLAG_ROGUE_DOUBLE_BATTLES, Rogue_ConfigRoom_RedrawMap_13
	setmetatile 3, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_12:
	goto_if_set FLAG_ROGUE_CAN_OVERLVL, Rogue_ConfigRoom_RedrawMap_17
	setmetatile 5, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_16:
	goto_if_set FLAG_ROGUE_EV_GAIN_ENABLED, Rogue_ConfigRoom_RedrawMap_21
	setmetatile 7, 3, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_20:
	goto_if_set FLAG_ROGUE_HARD_TRAINERS, Rogue_ConfigRoom_RedrawMap_25
	goto_if_set FLAG_ROGUE_EASY_TRAINERS, Rogue_ConfigRoom_RedrawMap_26
	setmetatile 1, 5, METATILE_BikeShop_Button_Green, FALSE
Rogue_ConfigRoom_RedrawMap_24:
	goto_if_set FLAG_ROGUE_HARD_ITEMS, Rogue_ConfigRoom_RedrawMap_31
	goto_if_set FLAG_ROGUE_EASY_ITEMS, Rogue_ConfigRoom_RedrawMap_32
	setmetatile 3, 5, METATILE_BikeShop_Button_Green, FALSE
Rogue_ConfigRoom_RedrawMap_30:
	goto_if_set FLAG_SET_SEED_ITEMS, Rogue_ConfigRoom_RedrawMap_37
	setmetatile 14, 6, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_36:
	goto_if_set FLAG_SET_SEED_WILDMONS, Rogue_ConfigRoom_RedrawMap_41
	setmetatile 16, 6, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_40:
	goto_if_set FLAG_SET_SEED_TRAINERS, Rogue_ConfigRoom_RedrawMap_45
	setmetatile 14, 8, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_44:
	goto_if_set FLAG_SET_SEED_BOSSES, Rogue_ConfigRoom_RedrawMap_49
	setmetatile 16, 8, METATILE_BikeShop_Button_Pressed, FALSE
Rogue_ConfigRoom_RedrawMap_48:
	special DrawWholeMapView
	return

Rogue_ConfigRoom_RedrawMap_2:
	setmetatile 8, 0, 0x25C, TRUE
	setmetatile 8, 1, 0x293, TRUE
	setmetatile 9, 0, 0x26E, TRUE
	setmetatile 9, 1, 0x276, FALSE
	setmetatile 10, 0, 0x25D, TRUE
	setmetatile 10, 1, 0x294, TRUE
	goto Rogue_ConfigRoom_RedrawMap_1

Rogue_ConfigRoom_RedrawMap_5:
	setvar VAR_TEMP_1, 1
	goto Rogue_ConfigRoom_RedrawMap_4

Rogue_ConfigRoom_RedrawMap_9:
	setmetatile 1, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_8

Rogue_ConfigRoom_RedrawMap_13:
	setmetatile 3, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_12

Rogue_ConfigRoom_RedrawMap_17:
	setmetatile 5, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_16

Rogue_ConfigRoom_RedrawMap_21:
	setmetatile 7, 3, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_20

Rogue_ConfigRoom_RedrawMap_25:
	setmetatile 1, 5, 0x23E, FALSE
	goto Rogue_ConfigRoom_RedrawMap_24

Rogue_ConfigRoom_RedrawMap_26:
	setmetatile 1, 5, METATILE_BikeShop_Button_Blue, FALSE
	goto Rogue_ConfigRoom_RedrawMap_24

Rogue_ConfigRoom_RedrawMap_31:
	setmetatile 3, 5, 0x23E, FALSE
	goto Rogue_ConfigRoom_RedrawMap_30

Rogue_ConfigRoom_RedrawMap_32:
	setmetatile 3, 5, METATILE_BikeShop_Button_Blue, FALSE
	goto Rogue_ConfigRoom_RedrawMap_30

Rogue_ConfigRoom_RedrawMap_37:
	setmetatile 14, 6, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_36

Rogue_ConfigRoom_RedrawMap_41:
	setmetatile 16, 6, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_40

Rogue_ConfigRoom_RedrawMap_45:
	setmetatile 14, 8, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_44

Rogue_ConfigRoom_RedrawMap_49:
	setmetatile 16, 8, METATILE_BikeShop_Button_Green, FALSE
	goto Rogue_ConfigRoom_RedrawMap_48


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


Rogue_ConfigRoom_Button_DoubleBattle::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_DoubleBattle_Text_0
	goto_if_set FLAG_ROGUE_DOUBLE_BATTLES, Rogue_ConfigRoom_Button_DoubleBattle_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_DoubleBattle_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_DoubleBattle_6
Rogue_ConfigRoom_Button_DoubleBattle_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_DoubleBattle_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_DoubleBattle_1

Rogue_ConfigRoom_Button_DoubleBattle_6:
	goto_if_set FLAG_ROGUE_DOUBLE_BATTLES, Rogue_ConfigRoom_Button_DoubleBattle_8
	setflag FLAG_ROGUE_DOUBLE_BATTLES
	goto Rogue_ConfigRoom_Button_DoubleBattle_5

Rogue_ConfigRoom_Button_DoubleBattle_8:
	clearflag FLAG_ROGUE_DOUBLE_BATTLES
	goto Rogue_ConfigRoom_Button_DoubleBattle_5


Rogue_ConfigRoom_Button_CanOverLvl::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_CanOverLvl_Text_0
	goto_if_set FLAG_ROGUE_CAN_OVERLVL, Rogue_ConfigRoom_Button_CanOverLvl_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_CanOverLvl_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_CanOverLvl_6
Rogue_ConfigRoom_Button_CanOverLvl_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_CanOverLvl_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_CanOverLvl_1

Rogue_ConfigRoom_Button_CanOverLvl_6:
	goto_if_set FLAG_ROGUE_CAN_OVERLVL, Rogue_ConfigRoom_Button_CanOverLvl_8
	setflag FLAG_ROGUE_CAN_OVERLVL
	goto Rogue_ConfigRoom_Button_CanOverLvl_5

Rogue_ConfigRoom_Button_CanOverLvl_8:
	clearflag FLAG_ROGUE_CAN_OVERLVL
	goto Rogue_ConfigRoom_Button_CanOverLvl_5


Rogue_ConfigRoom_Button_EVGainEnabled::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_EVGainEnabled_Text_0
	goto_if_set FLAG_ROGUE_EV_GAIN_ENABLED, Rogue_ConfigRoom_Button_EVGainEnabled_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_EVGainEnabled_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_EVGainEnabled_6
Rogue_ConfigRoom_Button_EVGainEnabled_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_EVGainEnabled_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_EVGainEnabled_1

Rogue_ConfigRoom_Button_EVGainEnabled_6:
	goto_if_set FLAG_ROGUE_EV_GAIN_ENABLED, Rogue_ConfigRoom_Button_EVGainEnabled_8
	setflag FLAG_ROGUE_EV_GAIN_ENABLED
	goto Rogue_ConfigRoom_Button_EVGainEnabled_5

Rogue_ConfigRoom_Button_EVGainEnabled_8:
	clearflag FLAG_ROGUE_EV_GAIN_ENABLED
	goto Rogue_ConfigRoom_Button_EVGainEnabled_5


Rogue_ConfigRoom_Button_TrainerDifficulty::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_TrainerDifficulty_Text_0
	goto_if_set FLAG_ROGUE_EASY_TRAINERS, Rogue_ConfigRoom_Button_TrainerDifficulty_2
	goto_if_set FLAG_ROGUE_HARD_TRAINERS, Rogue_ConfigRoom_Button_TrainerDifficulty_3
	setvar VAR_0x8000, 1
Rogue_ConfigRoom_Button_TrainerDifficulty_1:
	call Rogue_ConfigRoom_PreEAHButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_TrainerDifficulty_9
	case 1, Rogue_ConfigRoom_Button_TrainerDifficulty_10
	case 2, Rogue_ConfigRoom_Button_TrainerDifficulty_11
Rogue_ConfigRoom_Button_TrainerDifficulty_7:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_TrainerDifficulty_13
Rogue_ConfigRoom_Button_TrainerDifficulty_12:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_TrainerDifficulty_2:
	setvar VAR_0x8000, 0
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_1

Rogue_ConfigRoom_Button_TrainerDifficulty_3:
	setvar VAR_0x8000, 2
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_1

Rogue_ConfigRoom_Button_TrainerDifficulty_9:
	setflag FLAG_ROGUE_EASY_TRAINERS
	clearflag FLAG_ROGUE_HARD_TRAINERS
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_7

Rogue_ConfigRoom_Button_TrainerDifficulty_10:
	clearflag FLAG_ROGUE_EASY_TRAINERS
	clearflag FLAG_ROGUE_HARD_TRAINERS
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_7

Rogue_ConfigRoom_Button_TrainerDifficulty_11:
	clearflag FLAG_ROGUE_EASY_TRAINERS
	setflag FLAG_ROGUE_HARD_TRAINERS
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_7

Rogue_ConfigRoom_Button_TrainerDifficulty_13:
	msgbox Rogue_ConfigRoom_Button_TrainerDifficulty_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_TrainerDifficulty_12


Rogue_ConfigRoom_Button_ItemDifficulty::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_ItemDifficulty_Text_0
	goto_if_set FLAG_ROGUE_EASY_ITEMS, Rogue_ConfigRoom_Button_ItemDifficulty_2
	goto_if_set FLAG_ROGUE_HARD_ITEMS, Rogue_ConfigRoom_Button_ItemDifficulty_3
	setvar VAR_0x8000, 1
Rogue_ConfigRoom_Button_ItemDifficulty_1:
	call Rogue_ConfigRoom_PreEAHButtonInternal
	switch VAR_0x8000
	case 0, Rogue_ConfigRoom_Button_ItemDifficulty_9
	case 1, Rogue_ConfigRoom_Button_ItemDifficulty_10
	case 2, Rogue_ConfigRoom_Button_ItemDifficulty_11
Rogue_ConfigRoom_Button_ItemDifficulty_7:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_ConfigRoom_Button_ItemDifficulty_13
Rogue_ConfigRoom_Button_ItemDifficulty_12:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_ItemDifficulty_2:
	setvar VAR_0x8000, 0
	goto Rogue_ConfigRoom_Button_ItemDifficulty_1

Rogue_ConfigRoom_Button_ItemDifficulty_3:
	setvar VAR_0x8000, 2
	goto Rogue_ConfigRoom_Button_ItemDifficulty_1

Rogue_ConfigRoom_Button_ItemDifficulty_9:
	setflag FLAG_ROGUE_EASY_ITEMS
	clearflag FLAG_ROGUE_HARD_ITEMS
	goto Rogue_ConfigRoom_Button_ItemDifficulty_7

Rogue_ConfigRoom_Button_ItemDifficulty_10:
	clearflag FLAG_ROGUE_EASY_ITEMS
	clearflag FLAG_ROGUE_HARD_ITEMS
	goto Rogue_ConfigRoom_Button_ItemDifficulty_7

Rogue_ConfigRoom_Button_ItemDifficulty_11:
	clearflag FLAG_ROGUE_EASY_ITEMS
	setflag FLAG_ROGUE_HARD_ITEMS
	goto Rogue_ConfigRoom_Button_ItemDifficulty_7

Rogue_ConfigRoom_Button_ItemDifficulty_13:
	msgbox Rogue_ConfigRoom_Button_ItemDifficulty_Text_1, MSGBOX_DEFAULT
	goto Rogue_ConfigRoom_Button_ItemDifficulty_12


Rogue_ConfigRoom_Button_SeedItems::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedItems_Text_0
	goto_if_set FLAG_SET_SEED_ITEMS, Rogue_ConfigRoom_Button_SeedItems_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedItems_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_SeedItems_6
Rogue_ConfigRoom_Button_SeedItems_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedItems_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedItems_1

Rogue_ConfigRoom_Button_SeedItems_6:
	goto_if_set FLAG_SET_SEED_ITEMS, Rogue_ConfigRoom_Button_SeedItems_8
	setflag FLAG_SET_SEED_ITEMS
	goto Rogue_ConfigRoom_Button_SeedItems_5

Rogue_ConfigRoom_Button_SeedItems_8:
	clearflag FLAG_SET_SEED_ITEMS
	goto Rogue_ConfigRoom_Button_SeedItems_5


Rogue_ConfigRoom_Button_SeedWildMons::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedWildMons_Text_0
	goto_if_set FLAG_SET_SEED_WILDMONS, Rogue_ConfigRoom_Button_SeedWildMons_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedWildMons_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_SeedWildMons_6
Rogue_ConfigRoom_Button_SeedWildMons_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedWildMons_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedWildMons_1

Rogue_ConfigRoom_Button_SeedWildMons_6:
	goto_if_set FLAG_SET_SEED_WILDMONS, Rogue_ConfigRoom_Button_SeedWildMons_8
	setflag FLAG_SET_SEED_WILDMONS
	goto Rogue_ConfigRoom_Button_SeedWildMons_5

Rogue_ConfigRoom_Button_SeedWildMons_8:
	clearflag FLAG_SET_SEED_WILDMONS
	goto Rogue_ConfigRoom_Button_SeedWildMons_5


Rogue_ConfigRoom_Button_SeedTrainers::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedTrainers_Text_0
	goto_if_set FLAG_SET_SEED_TRAINERS, Rogue_ConfigRoom_Button_SeedTrainers_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedTrainers_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_SeedTrainers_6
Rogue_ConfigRoom_Button_SeedTrainers_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedTrainers_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedTrainers_1

Rogue_ConfigRoom_Button_SeedTrainers_6:
	goto_if_set FLAG_SET_SEED_TRAINERS, Rogue_ConfigRoom_Button_SeedTrainers_8
	setflag FLAG_SET_SEED_TRAINERS
	goto Rogue_ConfigRoom_Button_SeedTrainers_5

Rogue_ConfigRoom_Button_SeedTrainers_8:
	clearflag FLAG_SET_SEED_TRAINERS
	goto Rogue_ConfigRoom_Button_SeedTrainers_5


Rogue_ConfigRoom_Button_SeedBosses::
	bufferstring STR_VAR_1, Rogue_ConfigRoom_Button_SeedBosses_Text_0
	goto_if_set FLAG_SET_SEED_BOSSES, Rogue_ConfigRoom_Button_SeedBosses_2
	setvar VAR_0x8000, NO
Rogue_ConfigRoom_Button_SeedBosses_1:
	call Rogue_ConfigRoom_PreButtonInternal
	compare VAR_RESULT, YES
	goto_if_eq Rogue_ConfigRoom_Button_SeedBosses_6
Rogue_ConfigRoom_Button_SeedBosses_5:
	call Rogue_ConfigRoom_PostButtonInternal
	return

Rogue_ConfigRoom_Button_SeedBosses_2:
	setvar VAR_0x8000, YES
	goto Rogue_ConfigRoom_Button_SeedBosses_1

Rogue_ConfigRoom_Button_SeedBosses_6:
	goto_if_set FLAG_SET_SEED_BOSSES, Rogue_ConfigRoom_Button_SeedBosses_8
	setflag FLAG_SET_SEED_BOSSES
	goto Rogue_ConfigRoom_Button_SeedBosses_5

Rogue_ConfigRoom_Button_SeedBosses_8:
	clearflag FLAG_SET_SEED_BOSSES
	goto Rogue_ConfigRoom_Button_SeedBosses_5


Rogue_ConfigRoom_TalkLanette_Text_0:
	.string "LANETTE: Hi {PLAYER}!\p"
	.string "This is the CONFIG LAB!\p"
	.string "In here you can change settings for\n"
	.string "the next time you go on an adventure.\p"
	.string "Just step onto a switch to adjust the\n"
	.string "setting.$"

Rogue_ConfigRoom_TalkAssistant_Text_0:
	.string "ASSISTANT: The settings back here are\n"
	.string "for SEEDED mode!\p"
	.string "In SEEDED mode you will always\n"
	.string "encounter the same ROUTES, TRAINERS\l"
	.string "and POKéMON.\p"
	.string "I can enabled SEEDED mode for your\n"
	.string "next adventure, if you'd like?$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_0:
	.string "ASSISTANT: Do you want to disable\n"
	.string "SEEDED mode?$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_1:
	.string "ASSISTANT: OK.\n"
	.string "I'll disable SEEDED mode$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_2:
	.string "ASSISTANT: Do you want to enable\n"
	.string "SEEDED mode for your next adventure?$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_3:
	.string "ASSISTANT: Please select the SEED\n"
	.string "phrases$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_4:
	.string "ASSISTANT: OK.\n"
	.string "I'll enable SEEDED mode using that\l"
	.string "phrase!$"

Rogue_ConfigRoom_TalkAssistantInternal_Text_5:
	.string "ASSISTANT: Oh.\n"
	.string "Speak to me if you ever want to enable\l"
	.string "SEEDED mode$"

Rogue_ConfigRoom_TalkHiddenEnterance_Text_0:
	.string "What's this?\p"
	.string "A hidden switch!\p"
	.string "Press the switch?$"

Rogue_ConfigRoom_MeetPokabbie_Text_0:
	.string "???: What are you doing here!\p"
	.string "You discovered my SECRET LAB already!$"

Rogue_ConfigRoom_MeetPokabbie_Text_1:
	.string "I was gonna wait until you had beat the\n"
	.string "CHAMPION at least...$"

Rogue_ConfigRoom_MeetPokabbie_Text_2:
	.string "Anyway!\p"
	.string "Where I my manners!\n"
	.string "Allow me to introduce myself!$"

Rogue_ConfigRoom_MeetPokabbie_Text_3:
	.string "The name's Abbie!\n"
	.string "But you can call me Pokabbie!$"

Rogue_ConfigRoom_MeetPokabbie_Text_4:
	.string "POKABBIE: I'm the DEV behind EMERALD\n"
	.string "ROGUE!\p"
	.string "Welcome to my LAB!\n"
	.string "Or what I like to dub...\p"
	.string "The EXPANSION ROOM!$"

Rogue_ConfigRoom_MeetPokabbie_Text_5:
	.string "You can use this ROOM to enable\n"
	.string "SPECIAL MODES and although there's\l"
	.string "not too many now, I'll be sure to add\l"
	.string "more in future!\p"
	.string "Feel free to look around!$"

Rogue_ConfigRoom_TalkPokabbie_Text_0:
	.string "POKABBIE: I hope you're enjoying\n"
	.string "EMERALD ROGUE!\p"
	.string "Feel free to enable any SPECIAL MODES\n"
	.string "from this room!\p"
	.string "I'll be sure to add some more in future!$"

Rogue_ConfigRoom_TalkDitto_Text_0:
	.string "A Ditto!\p"
	.string "Why is it here?$"

Rogue_ConfigRoom_PreButtonInternal_Text_0:
	.string "ON$"

Rogue_ConfigRoom_PreButtonInternal_Text_1:
	.string "OFF$"

Rogue_ConfigRoom_PreButtonInternal_Text_2:
	.string "{STR_VAR_1} is set to\n"
	.string "{STR_VAR_2}$"

Rogue_ConfigRoom_PreButtonInternal_Text_3:
	.string "Flip the switch to {STR_VAR_3}?$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_0:
	.string "EASY$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_1:
	.string "AVERAGE$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_2:
	.string "HARD$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_3:
	.string "???$"

Rogue_ConfigRoom_PreEAHButtonInternal_Text_4:
	.string "{STR_VAR_1} is currently\n"
	.string "'{STR_VAR_2}'$"

Rogue_ConfigRoom_Button_ExpAll_Text_0:
	.string "EXP. ALL$"

Rogue_ConfigRoom_Button_DoubleBattle_Text_0:
	.string "DOUBLE BATTLE MODE$"

Rogue_ConfigRoom_Button_CanOverLvl_Text_0:
	.string "ALLOW OVERLEVLING$"

Rogue_ConfigRoom_Button_EVGainEnabled_Text_0:
	.string "ENABLE EVs$"

Rogue_ConfigRoom_Button_TrainerDifficulty_Text_0:
	.string "TRAINER DIFFICULTY$"

Rogue_ConfigRoom_Button_TrainerDifficulty_Text_1:
	.string "Adjust teams/moves from very casual to\n"
	.string "very competitive.\p"
	.string "Average mode will start casual but get\n"
	.string "more competitive as you progress.\p"
	.string "The higher this difficulty the more\n"
	.string "rewards you get when returning from an\l"
	.string "adventure.$"

Rogue_ConfigRoom_Button_ItemDifficulty_Text_0:
	.string "ITEM RATES$"

Rogue_ConfigRoom_Button_ItemDifficulty_Text_1:
	.string "Increases/Decrease the rate at which\n"
	.string "items will appear and money trainers\l"
	.string "give$"

Rogue_ConfigRoom_Button_SeedItems_Text_0:
	.string "SEED ITEMS$"

Rogue_ConfigRoom_Button_SeedWildMons_Text_0:
	.string "SEED WILD POKéMON$"

Rogue_ConfigRoom_Button_SeedTrainers_Text_0:
	.string "SEED TRAINERS$"

Rogue_ConfigRoom_Button_SeedBosses_Text_0:
	.string "SEED BOSSES$"
