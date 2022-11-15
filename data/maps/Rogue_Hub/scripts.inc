Rogue_Hub_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, RogueHub_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, RogueHub_OnMapLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Hub_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_0, 0, RogueHub_Intro0
	map_script_2 VAR_TEMP_0, 1, RogueHub_EnterPostTutortial
	.2byte 0


RogueHub_OnTransition::
	compare VAR_LITTLEROOT_INTRO_STATE, 2
	goto_if_le RogueHub_OnTransition_2
RogueHub_OnTransition_1:
	goto_if_set FLAG_IS_CHAMPION, RogueHub_OnTransition_6
RogueHub_OnTransition_5:
	setflag FLAG_TEMP_8
	return

RogueHub_OnTransition_2:
	setobjectxyperm 2, 23, 6
	goto RogueHub_OnTransition_1

RogueHub_OnTransition_4:
	clearflag FLAG_TEMP_8
	return

RogueHub_OnTransition_6:
	goto_if_unset FLAG_ROGUE_UNCOVERRED_POKABBIE, RogueHub_OnTransition_4
	goto RogueHub_OnTransition_5


RogueHub_CheckAnyPlateQuestsCollected::
	goto_if_unset FLAG_ROGUE_EXPANSION_ACTIVE, RogueHub_CheckAnyPlateQuestsCollected_2
	questcollected QUEST_NORMAL_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_5
	questcollected QUEST_FIGHTING_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_8
	questcollected QUEST_FLYING_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_11
	questcollected QUEST_POISON_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_14
	questcollected QUEST_GROUND_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_17
	questcollected QUEST_ROCK_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_20
	questcollected QUEST_BUG_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_23
	questcollected QUEST_GHOST_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_26
	questcollected QUEST_STEEL_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_29
	questcollected QUEST_FIRE_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_32
	questcollected QUEST_WATER_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_35
	questcollected QUEST_GRASS_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_38
	questcollected QUEST_ELECTRIC_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_41
	questcollected QUEST_PSYCHIC_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_44
	questcollected QUEST_ICE_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_47
	questcollected QUEST_DRAGON_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_50
	questcollected QUEST_DARK_Champion
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_CheckAnyPlateQuestsCollected_52
	return

RogueHub_CheckAnyPlateQuestsCollected_2:
	setvar VAR_RESULT, NO
	return

RogueHub_CheckAnyPlateQuestsCollected_5:
	return

RogueHub_CheckAnyPlateQuestsCollected_8:
	return

RogueHub_CheckAnyPlateQuestsCollected_11:
	return

RogueHub_CheckAnyPlateQuestsCollected_14:
	return

RogueHub_CheckAnyPlateQuestsCollected_17:
	return

RogueHub_CheckAnyPlateQuestsCollected_20:
	return

RogueHub_CheckAnyPlateQuestsCollected_23:
	return

RogueHub_CheckAnyPlateQuestsCollected_26:
	return

RogueHub_CheckAnyPlateQuestsCollected_29:
	return

RogueHub_CheckAnyPlateQuestsCollected_32:
	return

RogueHub_CheckAnyPlateQuestsCollected_35:
	return

RogueHub_CheckAnyPlateQuestsCollected_38:
	return

RogueHub_CheckAnyPlateQuestsCollected_41:
	return

RogueHub_CheckAnyPlateQuestsCollected_44:
	return

RogueHub_CheckAnyPlateQuestsCollected_47:
	return

RogueHub_CheckAnyPlateQuestsCollected_50:
	return

RogueHub_CheckAnyPlateQuestsCollected_52:
	return


RogueHub_OnMapLoad::
	goto_if_set FLAG_ROGUE_DEBUG_DISABLED, RogueHub_OnMapLoad_2
RogueHub_OnMapLoad_1:
	questcollected QUEST_FirstAdventure
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_5
	clearflag FLAG_ROGUE_TRAINER0
RogueHub_OnMapLoad_4:
	questcollected QUEST_Collector1
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_9
	clearflag FLAG_ROGUE_TRAINER4
RogueHub_OnMapLoad_8:
	questcollected QUEST_Bike1
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_13
	clearflag FLAG_ROGUE_TRAINER1
RogueHub_OnMapLoad_12:
	questcollected QUEST_NoFainting1
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_17
	clearflag FLAG_ROGUE_TRAINER2
RogueHub_OnMapLoad_16:
	questcollected QUEST_ShoppingSpree
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_21
	clearflag FLAG_ROGUE_TRAINER7
	clearflag FLAG_ROGUE_TRAINER5
	goto_if_set FLAG_ROGUE_RARE_ITEM_MART_DISABLED, RogueHub_OnMapLoad_29
RogueHub_OnMapLoad_28:
	call RogueHub_CheckAnyPlateQuestsCollected
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_35
	clearflag FLAG_ROGUE_TRAINER10
RogueHub_OnMapLoad_20:
	questcollected QUEST_BigSaver
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_25
	clearflag FLAG_ROGUE_TRAINER3
RogueHub_OnMapLoad_24:
	questcollected QUEST_BerryCollector
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_32
	clearflag FLAG_ROGUE_TRAINER8
RogueHub_OnMapLoad_31:
	questcollected QUEST_DenExplorer
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_39
RogueHub_OnMapLoad_38:
	questcollected QUEST_LegendOnly
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_OnMapLoad_43
	clearflag FLAG_ROGUE_TRAINER9
RogueHub_OnMapLoad_42:
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_gt RogueHub_OnMapLoad_46
	return

RogueHub_OnMapLoad_2:
	fillmetatile 4, 20, 4, 20, 0x001, FALSE
	goto RogueHub_OnMapLoad_1

RogueHub_OnMapLoad_5:
	fillmetatile 31, 10, 40, 16, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER0
	fillmetatile 41, 20, 48, 25, 0x001, FALSE
	goto RogueHub_OnMapLoad_4

RogueHub_OnMapLoad_9:
	fillmetatile 1, 3, 14, 10, 0x001, FALSE
	fillmetatile 1, 11, 12, 11, 0x001, FALSE
	fillmetatile 1, 12, 6, 12, 0x001, FALSE
	fillmetatile 31, 21, 37, 26, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER4
	goto RogueHub_OnMapLoad_8

RogueHub_OnMapLoad_13:
	fillmetatile 22, 12, 27, 16, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER1
	goto RogueHub_OnMapLoad_12

RogueHub_OnMapLoad_17:
	fillmetatile 42, 14, 48, 19, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER2
	goto RogueHub_OnMapLoad_16

RogueHub_OnMapLoad_21:
	fillmetatile 22, 8, 24, 8, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER7
	setflag FLAG_ROGUE_TRAINER5
	goto RogueHub_OnMapLoad_20

RogueHub_OnMapLoad_25:
	fillmetatile 13, 27, 27, 36, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER3
	goto RogueHub_OnMapLoad_24

RogueHub_OnMapLoad_29:
	setflag FLAG_ROGUE_TRAINER5
	goto RogueHub_OnMapLoad_28

RogueHub_OnMapLoad_32:
	fillmetatile 10, 40, 16, 41, 0x001, FALSE
	fillmetatile 18, 40, 24, 41, 0x001, FALSE
	fillmetatile 26, 40, 32, 41, 0x001, FALSE
	fillmetatile 34, 40, 40, 41, 0x001, FALSE
	setflag FLAG_ROGUE_TRAINER8
	goto RogueHub_OnMapLoad_31

RogueHub_OnMapLoad_35:
	setflag FLAG_ROGUE_TRAINER10
	goto RogueHub_OnMapLoad_20

RogueHub_OnMapLoad_39:
	fillmetatile 31, 31, 37, 36, 0x001, FALSE
	goto RogueHub_OnMapLoad_38

RogueHub_OnMapLoad_43:
	setflag FLAG_ROGUE_TRAINER9
	goto RogueHub_OnMapLoad_42

RogueHub_OnMapLoad_46:
	setflag FLAG_ROGUE_TRAINER6
	return


RogueHub_Intro0::
	compare VAR_LITTLEROOT_INTRO_STATE, 0
	goto_if_eq RogueHub_Intro0_2
RogueHub_Intro0_1:
	setvar VAR_TEMP_0, 1
	return

RogueHub_Intro0_2:
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
	setobjectxyperm 2, 29, 6
	setvar VAR_LITTLEROOT_INTRO_STATE, 3
	setflag FLAG_HIDE_LITTLEROOT_TOWN_BRENDANS_HOUSE_TRUCK
	call Rogue_QuickSaveVersionUpdateMessage
	releaseall
	goto RogueHub_Intro0_1


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

RogueHub_EnterPostTutortial::
	call RogueHub_IntroPokabbie
	setvar VAR_TEMP_0, 2
	return


RogueHub_SpeakScott::
	lock
	faceplayer
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_le RogueHub_SpeakScott_2
	msgbox RogueHub_SpeakScott_Text_1, MSGBOX_DEFAULT
RogueHub_SpeakScott_1:
	release
	end

RogueHub_SpeakScott_2:
	msgbox RogueHub_SpeakScott_Text_0, MSGBOX_DEFAULT
	goto RogueHub_SpeakScott_1


RogueHub_IntroPokabbie::
	goto_if_set FLAG_IS_CHAMPION, RogueHub_IntroPokabbie_2
	return

RogueHub_IntroPokabbie_1:
	lockall
	showobjectat 14, MAP_ROGUE_HUB
	msgbox RogueHub_IntroPokabbie_Text_0, MSGBOX_NPC
	turnobject OBJ_EVENT_ID_PLAYER, DIR_SOUTH
	applymovement 14, RogueHub_Pokabbie_Enter
	waitmovement 0
	msgbox RogueHub_IntroPokabbie_Text_1, MSGBOX_NPC
	applymovement 14, RogueHub_Pokabbie_Spin
	waitmovement 0
	msgbox RogueHub_IntroPokabbie_Text_2, MSGBOX_NPC
	msgbox RogueHub_IntroPokabbie_Text_3, MSGBOX_NPC
	applymovement 14, RogueHub_Pokabbie_Spin
	waitmovement 0
	applymovement 14, RogueHub_Pokabbie_Exit
	waitmovement 0
	setflag FLAG_ROGUE_UNCOVERRED_POKABBIE
	setflag FLAG_TEMP_8
	removeobject 14
	releaseall
	return

RogueHub_IntroPokabbie_2:
	goto_if_unset FLAG_ROGUE_UNCOVERRED_POKABBIE, RogueHub_IntroPokabbie_1
	return


RogueHub_Pokabbie_Enter:
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	walk_fast_up
	step_end

RogueHub_Pokabbie_Spin:
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
	face_up
	step_end

RogueHub_Pokabbie_Exit:
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	walk_fast_down
	step_end

RogueHub_SpeakBikeShop::
	lock
	faceplayer
	checkitem ITEM_ACRO_BIKE
	copyvar VAR_0x8000, VAR_RESULT
	checkitem ITEM_MACH_BIKE
	copyvar VAR_0x8001, VAR_RESULT
	questcollected QUEST_Bike2
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakBikeShop_2
	compare VAR_0x8000, YES
	goto_if_eq RogueHub_SpeakBikeShop_10
	compare VAR_0x8001, YES
	goto_if_eq RogueHub_SpeakBikeShop_11
	msgbox RogueHub_SpeakBikeShop_Text_3
	giveitem ITEM_ACRO_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_4
RogueHub_SpeakBikeShop_1:
	release
	return

RogueHub_SpeakBikeShop_2:
	compare VAR_0x8000, NO
	goto_if_eq RogueHub_SpeakBikeShop_5
	compare VAR_0x8001, NO
	goto_if_eq RogueHub_SpeakBikeShop_6
	msgbox RogueHub_SpeakBikeShop_Text_1, MSGBOX_DEFAULT
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_5:
	bufferitemname STR_VAR_1, ITEM_ACRO_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_0, MSGBOX_DEFAULT
	giveitem ITEM_ACRO_BIKE
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_6:
	bufferitemname STR_VAR_1, ITEM_MACH_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_0, MSGBOX_DEFAULT
	giveitem ITEM_MACH_BIKE
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_10:
	bufferitemname STR_VAR_1, ITEM_MACH_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakBikeShop_15
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_11:
	bufferitemname STR_VAR_1, ITEM_ACRO_BIKE
	msgbox RogueHub_SpeakBikeShop_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakBikeShop_17
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_15:
	removeitem ITEM_ACRO_BIKE
	giveitem ITEM_MACH_BIKE
	goto RogueHub_SpeakBikeShop_1

RogueHub_SpeakBikeShop_17:
	removeitem ITEM_MACH_BIKE
	giveitem ITEM_ACRO_BIKE
	goto RogueHub_SpeakBikeShop_1


RogueHub_SpeakSafariWelcome::
	lock
	faceplayer
	msgbox RogueHub_SpeakSafariWelcome_Text_0, MSGBOX_NPC
	release
	return


RogueHub_SpeakBank::
	lock
	faceplayer
	compare VAR_ROGUE_ADVENTURE_MONEY, 0
	goto_if_eq RogueHub_SpeakBank_2
RogueHub_SpeakBank_1:
	showmoneybox 0, 0
	buffernumberstring STR_VAR_2, VAR_ROGUE_ADVENTURE_MONEY
	compare VAR_ROGUE_ADVENTURE_MONEY, 25000
	goto_if_ge RogueHub_SpeakBank_5
	message RogueHub_SpeakBank_Text_4
	waitmessage
	multichoice 20, 6, MULTI_YESNOINFO, NO
	switch VAR_RESULT
	case 0, RogueHub_SpeakBank_10
	case 1, RogueHub_SpeakBank_11
	case 2, RogueHub_SpeakBank_12
	case MULTI_B_PRESSED, RogueHub_SpeakBank_13
RogueHub_SpeakBank_8:
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_SpeakBank_14
RogueHub_SpeakBank_4:
	hidemoneybox
	release
	return

RogueHub_SpeakBank_2:
	msgbox RogueHub_SpeakBank_Text_0, MSGBOX_DEFAULT
	msgbox RogueHub_SpeakBank_Text_1, MSGBOX_DEFAULT
	msgbox RogueHub_SpeakBank_Text_2, MSGBOX_DEFAULT
	setvar VAR_ROGUE_ADVENTURE_MONEY, 500
	playse SE_SHOP
	waitse
	goto RogueHub_SpeakBank_1

RogueHub_SpeakBank_5:
	msgbox RogueHub_SpeakBank_Text_3, MSGBOX_DEFAULT
	goto RogueHub_SpeakBank_4

RogueHub_SpeakBank_10:
	setvar VAR_RESULT, YES
	goto RogueHub_SpeakBank_8

RogueHub_SpeakBank_11:
	setvar VAR_RESULT, NO
	msgbox RogueHub_SpeakBank_Text_5, MSGBOX_DEFAULT
	goto RogueHub_SpeakBank_8

RogueHub_SpeakBank_12:
	setvar VAR_RESULT, NO
	msgbox RogueHub_SpeakBank_Text_1, MSGBOX_DEFAULT
	goto RogueHub_SpeakBank_8

RogueHub_SpeakBank_13:
	setvar VAR_RESULT, NO
	msgbox RogueHub_SpeakBank_Text_5, MSGBOX_DEFAULT
	goto RogueHub_SpeakBank_8

RogueHub_SpeakBank_14:
	checkmoney 500
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_SpeakBank_16
	removemoney 500
	updatemoneybox
	addvar VAR_ROGUE_ADVENTURE_MONEY, 500
	buffernumberstring STR_VAR_2, VAR_ROGUE_ADVENTURE_MONEY
	playse SE_SHOP
	waitse
	msgbox RogueHub_SpeakBank_Text_7, MSGBOX_DEFAULT
	goto RogueHub_SpeakBank_4

RogueHub_SpeakBank_16:
	msgbox RogueHub_SpeakBank_Text_6, MSGBOX_YESNO
	goto RogueHub_SpeakBank_4


RogueHub_SpeakBerryMerchant::
	lock
	faceplayer
	goto_if_unset FLAG_ROGUE_MET_BERRY_MERCHANT, RogueHub_SpeakBerryMerchant_2
	message RogueHub_SpeakBerryMerchant_Text_2
	waitmessage
	rogue_dynamicpokemart ROGUE_SHOP_BERRIES
	msgbox RogueHub_SpeakBerryMerchant_Text_3
RogueHub_SpeakBerryMerchant_1:
	release
	return

RogueHub_SpeakBerryMerchant_2:
	msgbox RogueHub_SpeakBerryMerchant_Text_0
	msgbox RogueHub_SpeakBerryMerchant_Text_1
	setflag FLAG_ROGUE_MET_BERRY_MERCHANT
	goto RogueHub_SpeakBerryMerchant_1


RogueHub_Sign_AdventureSign::
	buffernumberstring STR_VAR_1, VAR_ROGUE_FURTHEST_DIFFICULTY
	msgbox RogueHub_Sign_AdventureSign_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_ProfLab::
	msgbox RogueHub_Sign_ProfLab_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_ConfigLab::
	msgbox RogueHub_Sign_ConfigLab_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_Safari::
	msgbox RogueHub_Sign_Safari_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_Shop::
	msgbox RogueHub_Sign_Shop_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_BikeShop::
	msgbox RogueHub_Sign_BikeShop_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_NameRater::
	msgbox RogueHub_Sign_NameRater_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_Bank::
	msgbox RogueHub_Sign_Bank_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_ClothesShop::
	msgbox RogueHub_Sign_ClothesShop_Text_0, MSGBOX_SIGN
	return


RogueHub_Sign_Bakery::
	msgbox RogueHub_Sign_Bakery_Text_0, MSGBOX_SIGN
	return


RogueHub_EnterSafari::
	lockall
	applymovement OBJ_EVENT_ID_PLAYER, Common_Movement_WalkInPlaceFasterUp
	waitmovement 0
	showmoneybox 0, 0
	msgbox RogueHub_EnterSafari_Text_0, MSGBOX_YESNO
	setvar VAR_0x8000, 0
	setvar VAR_0x8001, 0
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_EnterSafari_2
RogueHub_EnterSafari_1:
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_EnterSafari_5
RogueHub_EnterSafari_4:
	releaseall
	return

RogueHub_EnterSafari_2:
	questcollected QUEST_Collector2
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_EnterSafari_8
RogueHub_EnterSafari_7:
	bufferitemname STR_VAR_1, ITEM_SAFARI_BALL
	message RogueHub_EnterSafari_Text_3
	waitmessage
	multichoice 20, 6, MULTI_MECHADOLL1_Q1, NO
	switch VAR_RESULT
	case 0, RogueHub_EnterSafari_11
	case 1, RogueHub_EnterSafari_12
	case 2, RogueHub_EnterSafari_13
	case MULTI_B_PRESSED, RogueHub_EnterSafari_14
	goto RogueHub_EnterSafari_1

RogueHub_EnterSafari_5:
	msgbox RogueHub_EnterSafari_Text_4
	closemessage
	hidemoneybox
	applymovement OBJ_EVENT_ID_PLAYER, Route121_SafariZoneEntrance_Movement_BackAwayFromCounter
	waitmovement 0
	goto RogueHub_EnterSafari_4

RogueHub_EnterSafari_8:
	msgbox RogueHub_EnterSafari_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_EnterSafari_15
	setvar VAR_ROGUE_SAFARI_GENERATION, 0
	goto RogueHub_EnterSafari_7

RogueHub_EnterSafari_11:
	setvar VAR_0x8000, 3
	setvar VAR_0x8001, 1000
	call RogueHub_EnterSafari1
	goto RogueHub_EnterSafari_1

RogueHub_EnterSafari_12:
	setvar VAR_0x8000, 6
	setvar VAR_0x8001, 1800
	call RogueHub_EnterSafari1
	goto RogueHub_EnterSafari_1

RogueHub_EnterSafari_13:
	setvar VAR_0x8000, 9
	setvar VAR_0x8001, 2500
	call RogueHub_EnterSafari1
	goto RogueHub_EnterSafari_1

RogueHub_EnterSafari_14:
	setvar VAR_RESULT, NO
	goto RogueHub_EnterSafari_1

RogueHub_EnterSafari_15:
	message RogueHub_EnterSafari_Text_2
	waitmessage
	goto_if_set FLAG_ROGUE_EXPANSION_ACTIVE, RogueHub_EnterSafari_21
RogueHub_EnterSafari_20:
	multichoice 20, 6, MULTI_GENERATIONS_UPTO3, NO
RogueHub_EnterSafari_18:
	switch VAR_RESULT
	case MULTI_B_PRESSED, RogueHub_EnterSafari_25
	copyvar VAR_ROGUE_SAFARI_GENERATION, VAR_RESULT
	addvar VAR_ROGUE_SAFARI_GENERATION, 1
	goto RogueHub_EnterSafari_7

RogueHub_EnterSafari_19:
	multichoice 20, 0, MULTI_GENERATIONS_UPTO8, NO
	goto RogueHub_EnterSafari_18

RogueHub_EnterSafari_21:
	goto_if_set FLAG_ROGUE_MET_POKABBIE, RogueHub_EnterSafari_19
	goto RogueHub_EnterSafari_20

RogueHub_EnterSafari_25:
	setvar VAR_ROGUE_SAFARI_GENERATION, 0
	goto RogueHub_EnterSafari_7


RogueHub_EnterSafari1::
	checkmoney VAR_0x8001
	buffernumberstring STR_VAR_1, VAR_0x8001
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_EnterSafari1_1
	msgbox RogueHub_EnterSafari1_Text_2, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_EnterSafari1_4
	return

RogueHub_EnterSafari1_1:
	msgbox RogueHub_EnterSafari1_Text_0
	msgbox RogueHub_EnterSafari1_Text_1
	return

RogueHub_EnterSafari1_4:
	playse SE_SHOP
	removemoney VAR_0x8001
	updatemoneybox
	msgbox RogueHub_EnterSafari1_Text_3
	closemessage
	hidemoneybox
	applymovement OBJ_EVENT_ID_PLAYER, Route121_SafariZoneEntrance_Movement_EnterSafariZone
	waitmovement 0
	special EnterSafariMode
	setvar VAR_SAFARI_ZONE_STATE, 2
	clearflag FLAG_GOOD_LUCK_SAFARI_ZONE
	warp MAP_SAFARI_ZONE_SOUTH, 32, 33
	waitstate
	return


RogueHub_HintSafariShinies::
	lock
	faceplayer
	msgbox RogueHub_HintSafariShinies_Text_0, MSGBOX_DEFAULT
	release
	return


RogueHub_ListSafariShinies::
	lock
	setvar VAR_0x8004, 0
	msgbox RogueHub_ListSafariShinies_Text_0, MSGBOX_DEFAULT
	special Rogue_GetBufferedShinySpecies
	compare VAR_RESULT, SPECIES_NONE
	goto_if_eq RogueHub_ListSafariShinies_2
RogueHub_ListSafariShinies_6:
	compare VAR_RESULT, SPECIES_NONE
	goto_if_ne RogueHub_ListSafariShinies_7
	msgbox RogueHub_ListSafariShinies_Text_3, MSGBOX_DEFAULT
RogueHub_ListSafariShinies_1:
	release
	return

RogueHub_ListSafariShinies_2:
	msgbox RogueHub_ListSafariShinies_Text_1, MSGBOX_DEFAULT
	goto RogueHub_ListSafariShinies_1

RogueHub_ListSafariShinies_7:
	bufferspeciesname STR_VAR_1, VAR_RESULT
	msgbox RogueHub_ListSafariShinies_Text_2, MSGBOX_DEFAULT
	addvar VAR_0x8004, 1
	special Rogue_GetBufferedShinySpecies
	goto RogueHub_ListSafariShinies_6


RogueHub_ChangeMonBall::
	lock
	faceplayer
	setvar VAR_0x8004, 0
	special BufferMonNickname
	msgbox RogueHub_ChangeMonBall_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_ChangeMonBall_2
RogueHub_ChangeMonBall_1:
	compare VAR_RESULT, NO
	goto_if_eq RogueHub_ChangeMonBall_5
RogueHub_ChangeMonBall_4:
	release
	return

RogueHub_ChangeMonBall_2:
	msgbox RogueHub_ChangeMonBall_Text_1
	fadescreen FADE_TO_BLACK
	special Bag_ChooseBall
	waitstate
	compare VAR_ITEM_ID, 0
	goto_if_eq RogueHub_ChangeMonBall_7
	removeitem VAR_ITEM_ID, 1
	special Rogue_ChangeMonBall
	msgbox RogueHub_ChangeMonBall_Text_2, MSGBOX_DEFAULT
	setvar VAR_RESULT, YES
	goto RogueHub_ChangeMonBall_1

RogueHub_ChangeMonBall_5:
	msgbox RogueHub_ChangeMonBall_Text_3, MSGBOX_DEFAULT
	goto RogueHub_ChangeMonBall_4

RogueHub_ChangeMonBall_7:
	setvar VAR_RESULT, NO
	goto RogueHub_ChangeMonBall_1


RogueHub_HintQuickRun1::
	lockall
	msgbox RogueHub_HintQuickRun1_Text_0, MSGBOX_DEFAULT
	releaseall
	return


RogueHub_HintQuickRun2::
	lockall
	msgbox RogueHub_HintQuickRun2_Text_0, MSGBOX_DEFAULT
	releaseall
	return


RogueHub_HintHMUsage::
	lock
	msgbox RogueHub_HintHMUsage_Text_0, MSGBOX_DEFAULT
	faceplayer
	msgbox RogueHub_HintHMUsage_Text_1, MSGBOX_DEFAULT
	release
	return


RogueHub_HintRouteAdjectives::
	lock
	faceplayer
	msgbox RogueHub_HintRouteAdjectives_Text_0, MSGBOX_DEFAULT
	msgbox RogueHub_HintRouteAdjectives_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq RogueHub_HintRouteAdjectives_2
RogueHub_HintRouteAdjectives_1:
	release
	return

RogueHub_HintRouteAdjectives_2:
	msgbox RogueHub_HintRouteAdjectives_Text_2, MSGBOX_DEFAULT
	msgbox RogueHub_HintRouteAdjectives_Text_3, MSGBOX_DEFAULT
	msgbox RogueHub_HintRouteAdjectives_Text_4, MSGBOX_DEFAULT
	goto RogueHub_HintRouteAdjectives_1


RogueHub_HintHubRespawns::
	lock
	faceplayer
	msgbox RogueHub_HintHubRespawns_Text_0, MSGBOX_DEFAULT
	release
	return


RogueHub_Intro0_Text_0:
	.string "SCOTT: Well we're here!\p"
	.string "The POKéMON HUB!\p"
	.string "Sorry again for making you sit in the\n"
	.string "back with all the supplies.\p"
	.string "I'm sure you're raring to go, but first\n"
	.string "you should go speak the PROF.$"

RogueHub_SpeakScott_Text_0:
	.string "SCOTT: You should go and speak to find\n"
	.string "the PROF. first$"

RogueHub_SpeakScott_Text_1:
	.string "SCOTT: Uh oh!\p"
	.string "I'm not suppose to be standing here...\p"
	.string "You probably loaded this SAVE from an\n"
	.string "old game version.\p"
	.string "Try entering a house and checking if I'm\n"
	.string "still here.$"

RogueHub_IntroPokabbie_Text_0:
	.string "{PLAYER}!$"

RogueHub_IntroPokabbie_Text_1:
	.string "???: Congratulations {PLAYER}!\n"
	.string "I can't believe you managed to defeat\l"
	.string "the CHAMPION!\p"
	.string "Oh of course.\n"
	.string "Where I my manners!\n"
	.string "Allow me to introduce myself!$"

RogueHub_IntroPokabbie_Text_2:
	.string "The name's Abbie!\n"
	.string "But you can call me Pokabbie!$"

RogueHub_IntroPokabbie_Text_3:
	.string "POKABBIE: I'm the DEV behind EMERALD\n"
	.string "ROGUE!\p"
	.string "As a reward for beating the CHAMPION I\n"
	.string "would like to welcome you to my LAB!\p"
	.string "I've uncovered the secret entrance\n"
	.string "inside the CONFIG LAB, so feel free to\l"
	.string "come take a look at some point.$"

RogueHub_SpeakBikeShop_Text_0:
	.string "Whoa {PLAYER}!\p"
	.string "You're looking much faster now, so here\n"
	.string "take the {STR_VAR_1}!$"

RogueHub_SpeakBikeShop_Text_1:
	.string "Hey {PLAYER}!\p"
	.string "Keep up the quick pace!$"

RogueHub_SpeakBikeShop_Text_2:
	.string "Do you want to swap to the\n"
	.string "{STR_VAR_1}?$"

RogueHub_SpeakBikeShop_Text_3:
	.string "Finally!\p"
	.string "You must be this {PLAYER} I've been\n"
	.string "told about.\p"
	.string "You seem a bit slower than I expected...\p"
	.string "I know!\n"
	.string "I have a couple old BIKEs I've been\l"
	.string "looking to get rid of.\p"
	.string "You can have them!$"

RogueHub_SpeakBikeShop_Text_4:
	.string "On second thought...\n"
	.string "You look like you're struggling to carry\l"
	.string "that much.\p"
	.string "Tell you what. You take that one and if\n"
	.string "you ever want to swap it out for the\l"
	.string "other one, let me know$"

RogueHub_SpeakSafariWelcome_Text_0:
	.string "Welcome to the SAFARI ZONE!\p"
	.string "We use the data from your POKéDEX to\n"
	.string "breed POKéMON and let them roam\l"
	.string "freely!\p"
	.string "As a thanks for providing the POKéMON\n"
	.string "data for us, we'll allow you to enter\l"
	.string "and capture a few POKéMON of your\l"
	.string "choosing which you may then use as a\l"
	.string "PARTNER in future adventures!$"

RogueHub_SpeakBank_Text_0:
	.string "Why hello!\n"
	.string "You must be {PLAYER} if I'm not\l"
	.string "mistaken?\p"
	.string "Wonderful!\n"
	.string "You see I'm rather useful when it comes\l"
	.string "to managing money and I was hoping to\l"
	.string "setup an ADVENTURE FUND for yourself.$"

RogueHub_SpeakBank_Text_1:
	.string "What's an ADVENTURE FUND?\n"
	.string "In essence, if you donate your HUB\l"
	.string "MONEY to myself, I will invest it on\l"
	.string "your behalf and then every time you\l"
	.string "head out on another adventure, I will\l"
	.string "give you some extra money to start\l"
	.string "with.\p"
	.string "So rather than having to start with ¥0,\n"
	.string "by donating money to myself, you can\l"
	.string "start with ¥1000, ¥2000, ¥10000!\p"
	.string "The sky is the limit really!\n"
	.string "Or rather I guess the limit would be\l"
	.string "your HUB MONEY.$"

RogueHub_SpeakBank_Text_2:
	.string "Here, let me get you started with a gift.\p"
	.string "I shall add ¥500 to your ADVENTURE FUND\n"
	.string "out of my own pocket!$"

RogueHub_SpeakBank_Text_3:
	.string "Your fund currently has\n"
	.string "¥{STR_VAR_2}.\p"
	.string "I'm afraid you can't donate any more\n"
	.string "money at the moment.$"

RogueHub_SpeakBank_Text_4:
	.string "Your fund currently has ¥{STR_VAR_2}.\nWould you like to deposit another ¥500?$"

RogueHub_SpeakBank_Text_5:
	.string "I see...\n"
	.string "You know where I am, if you change your\l"
	.string "mind.$"

RogueHub_SpeakBank_Text_6:
	.string "I'm afraid you seem to be lacking the\n"
	.string "required funds currently.$"

RogueHub_SpeakBank_Text_7:
	.string "Thankyou very much!\n"
	.string "Your fund now contains\l"
	.string "¥{STR_VAR_2}.$"

RogueHub_SpeakBerryMerchant_Text_0:
	.string "Oh hello {PLAYER}!\n"
	.string "I hope you don't mind, but I've setup\l"
	.string "some berry patches.\p"
	.string "You may be given berries are rewards\n"
	.string "for completing quest that you're\l"
	.string "welcome to plant in this field!$"

RogueHub_SpeakBerryMerchant_Text_1:
	.string "If you ever run out of berries or if\n"
	.string "you'd like to buy some rather than grow\l"
	.string "them, then please do come and speak\l"
	.string "with me.$"

RogueHub_SpeakBerryMerchant_Text_2:
	.string "Hi {PLAYER}!\nWould you like to buy some berries?$"

RogueHub_SpeakBerryMerchant_Text_3:
	.string "Glad to be of help!$"

RogueHub_Sign_AdventureSign_Text_0:
	.string "Adventures await beyond this point...$"

RogueHub_Sign_ProfLab_Text_0:
	.string "PROF. BIRCH's LAB$"

RogueHub_Sign_ConfigLab_Text_0:
	.string "CONFIG LAB$"

RogueHub_Sign_Safari_Text_0:
	.string "SAFARI ZONE$"

RogueHub_Sign_Shop_Text_0:
	.string "OUTDOOR MART$"

RogueHub_Sign_BikeShop_Text_0:
	.string "BIKE SHOP$"

RogueHub_Sign_NameRater_Text_0:
	.string "NAME RATER's HOUSE$"

RogueHub_Sign_Bank_Text_0:
	.string "BANK$"

RogueHub_Sign_ClothesShop_Text_0:
	.string "CLOTHES SHOP$"

RogueHub_Sign_Bakery_Text_0:
	.string "BAKERY$"

RogueHub_EnterSafari_Text_0:
	.string "Welcome to the SAFARI ZONE!\n"
	.string "Would like to enter?$"

RogueHub_EnterSafari_Text_1:
	.string "You can choose to either have POKéMON\n"
	.string "from every GENERATION appear or\l"
	.string "select a specific GENERATION to catch\l"
	.string "from.\p"
	.string "Would you like to limit to a single\n"
	.string "GENERATION?$"

RogueHub_EnterSafari_Text_2:
	.string "Which GENERATION would you like to\n"
	.string "enable?$"

RogueHub_EnterSafari_Text_3:
	.string "How many {STR_VAR_1}s would you\n"
	.string "like?$"

RogueHub_EnterSafari_Text_4:
	.string "Please Come again another time.$"

RogueHub_EnterSafari1_Text_0:
	.string "That will cost ¥{STR_VAR_1}.$"

RogueHub_EnterSafari1_Text_1:
	.string "Oh..\n"
	.string "You don't appear to have enough money\l"
	.string "right now.$"

RogueHub_EnterSafari1_Text_2:
	.string "That will cost ¥{STR_VAR_1}.\n"
	.string "Would you like to enter?$"

RogueHub_EnterSafari1_Text_3:
	.string "Have fun!$"

RogueHub_HintSafariShinies_Text_0:
	.string "The Safari Zone have some very\n"
	.string "dedicated staff!\p"
	.string "They even keep track of the last few\n"
	.string "shiny POKéMON you've caught whilst on\l"
	.string "an Adventure!\p"
	.string "So if you catch a shiny whilst on an\n"
	.string "Adventure you'll be able to find it in\l"
	.string "the Safari Zone super easy!\p"
	.string "If you've forgotten what those shinies\n"
	.string "are, go check the list behind me.\p"
	.string "What dedicated staff!$"

RogueHub_ListSafariShinies_Text_0:
	.string "It reads...\p"
	.string "Shiny Breeding Programme:$"

RogueHub_ListSafariShinies_Text_1:
	.string "No programs currently active.$"

RogueHub_ListSafariShinies_Text_2:
	.string "-{STR_VAR_1}$"

RogueHub_ListSafariShinies_Text_3:
	.string "That's the end of the list.$"

RogueHub_ChangeMonBall_Text_0:
	.string "Well hey there!\p"
	.string "I can change the POKéBALL of\n"
	.string "{STR_VAR_1} if you'd like?$"

RogueHub_ChangeMonBall_Text_1:
	.string "Which type of BALL should I change it\n"
	.string "to?$"

RogueHub_ChangeMonBall_Text_2:
	.string "And it's done!$"

RogueHub_ChangeMonBall_Text_3:
	.string "Oh... OK...$"

RogueHub_HintQuickRun1_Text_0:
	.string "I'm double tapping B to run super fast!\p"
	.string "Same as if you want to escape from Wild\n"
	.string "POKéMON faster!\p"
	.string "So fast!$"

RogueHub_HintQuickRun2_Text_0:
	.string "Get back here!\n"
	.string "B B B B B B!$"

RogueHub_HintHMUsage_Text_0:
	.string "If only I had a surf HM...$"

RogueHub_HintHMUsage_Text_1:
	.string "You don't even need to teach them to\n"
	.string "your POKéMON to use them anymore!\p"
	.string "IF ONLY I COULD SURF!$"

RogueHub_HintRouteAdjectives_Text_0:
	.string "I took me a while, but I finally figured\n"
	.string "out what the route names mean!$"

RogueHub_HintRouteAdjectives_Text_1:
	.string "Do you want me to tell you?$"

RogueHub_HintRouteAdjectives_Text_2:
	.string "Route names are split into 2\n"
	.string "adjectives.\p"
	.string "The Difficulty and the Type Hint.$"

RogueHub_HintRouteAdjectives_Text_3:
	.string "There are 3 difficulties.\n"
	.string "Calm, Average and Tough.\p"
	.string "Calm routes have more wild POKéMON,\n"
	.string "fewer items and the Trainers aren't as\l"
	.string "skilled.\p"
	.string "Tough routes have few wild POKéMON, but\n"
	.string "more items and the Trainers are more\l"
	.string "skilled.\p"
	.string "Average routes are the middle ground\n"
	.string "for both of those.$"

RogueHub_HintRouteAdjectives_Text_4:
	.string "The 2nd Adjective is the Type Hint, so\n"
	.string "it gives you an idea of what types of\l"
	.string "wild POKéMON might turn up on a route.\p"
	.string "For example a WARM route will have\n"
	.string "chance of having wild FIRE type\l"
	.string "POKéMON.\p"
	.string "Though it doesn't mean that the route\n"
	.string "will always be home to that type.\p"
	.string "It's just that its more likely to than\n"
	.string "the other routes.$"

RogueHub_HintHubRespawns_Text_0:
	.string "I got lost and ended up back in the HUB.\p"
	.string "I'm not really sure how that works, but\n"
	.string "it sure is convenient!$"
