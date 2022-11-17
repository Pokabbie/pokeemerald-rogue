Rogue_Hub_ProfLab_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Rogue_Hub_ProfLab_OnTransition
	.byte 0


Rogue_Hub_ProfLab_OnTransition::
	switch VAR_STARTER_MON
	case 1, Rogue_Hub_ProfLab_OnTransition_2
	case 2, Rogue_Hub_ProfLab_OnTransition_3
	case 3, Rogue_Hub_ProfLab_OnTransition_4
	return

Rogue_Hub_ProfLab_OnTransition_2:
	setflag FLAG_TEMP_1
	return

Rogue_Hub_ProfLab_OnTransition_3:
	setflag FLAG_TEMP_2
	return

Rogue_Hub_ProfLab_OnTransition_4:
	setflag FLAG_TEMP_3
	return


Rogue_Hub_ProfLab_SpeakProf::
	lockall
	faceplayer
	goto_if_set FLAG_ROGUE_RUN_ACTIVE, Rogue_Hub_ProfLab_SpeakProf_2
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_le Rogue_Hub_ProfLab_SpeakProf_8
	call Rogue_Hub_ProfLab_ProfExplain
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Hub_ProfLab_SpeakProf_11
Rogue_Hub_ProfLab_SpeakProf_1:
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Hub_ProfLab_SpeakProf_6
Rogue_Hub_ProfLab_SpeakProf_5:
	releaseall
	return

Rogue_Hub_ProfLab_SpeakProf_2:
	msgbox Rogue_Hub_ProfLab_SpeakProf_Text_0
	call Rogue_Hub_ProfLab_ProfShowEvos
	goto Rogue_Hub_ProfLab_SpeakProf_1

Rogue_Hub_ProfLab_SpeakProf_6:
	msgbox Rogue_Hub_ProfLab_SpeakProf_Text_2, MSGBOX_DEFAULT
	goto Rogue_Hub_ProfLab_SpeakProf_5

Rogue_Hub_ProfLab_SpeakProf_8:
	msgbox Rogue_Hub_ProfLab_SpeakProf_Text_1, MSGBOX_DEFAULT
	goto Rogue_Hub_ProfLab_SpeakProf_1

Rogue_Hub_ProfLab_SpeakProf_11:
	call Rogue_Hub_ProfLab_ProfShowEvos
	goto Rogue_Hub_ProfLab_SpeakProf_1


Rogue_Hub_ProfLab_ProfExplain::
	msgbox Rogue_Hub_ProfLab_ProfExplain_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_ProfExplain_2
Rogue_Hub_ProfLab_ProfExplain_1:
	return

Rogue_Hub_ProfLab_ProfExplain_2:
	msgbox Rogue_Hub_ProfLab_ProfExplain_Text_1, MSGBOX_DEFAULT
	msgbox Rogue_Hub_ProfLab_ProfExplain_Text_2, MSGBOX_DEFAULT
	goto Rogue_Hub_ProfLab_ProfExplain_1


Rogue_Hub_ProfLab_ProfCycleEvos::
Rogue_Hub_ProfLab_ProfCycleEvos_1:
	compare VAR_0x8005, VAR_RESULT
	goto_if_lt Rogue_Hub_ProfLab_ProfCycleEvos_2
	return

Rogue_Hub_ProfLab_ProfCycleEvos_2:
	special Rogue_GetMonEvoParams
	addvar VAR_0x8005, 1
	buffernumberstring STR_VAR_2, VAR_0x8005
	switch VAR_0x8006
	case EVO_FRIENDSHIP, Rogue_Hub_ProfLab_ProfCycleEvos_5
	case EVO_FRIENDSHIP_DAY, Rogue_Hub_ProfLab_ProfCycleEvos_6
	case EVO_FRIENDSHIP_NIGHT, Rogue_Hub_ProfLab_ProfCycleEvos_7
	case EVO_LEVEL, Rogue_Hub_ProfLab_ProfCycleEvos_8
	case EVO_LEVEL_NINJASK, Rogue_Hub_ProfLab_ProfCycleEvos_8
	case EVO_TRADE, Rogue_Hub_ProfLab_ProfCycleEvos_9
	case EVO_TRADE_ITEM, Rogue_Hub_ProfLab_ProfCycleEvos_10
	case EVO_ITEM, Rogue_Hub_ProfLab_ProfCycleEvos_11
	case EVO_LEVEL_ATK_GT_DEF, Rogue_Hub_ProfLab_ProfCycleEvos_12
	case EVO_LEVEL_ATK_EQ_DEF, Rogue_Hub_ProfLab_ProfCycleEvos_13
	case EVO_LEVEL_ATK_LT_DEF, Rogue_Hub_ProfLab_ProfCycleEvos_14
	case EVO_LEVEL_SILCOON, Rogue_Hub_ProfLab_ProfCycleEvos_15
	case EVO_LEVEL_CASCOON, Rogue_Hub_ProfLab_ProfCycleEvos_15
	case EVO_LEVEL_SHEDINJA, Rogue_Hub_ProfLab_ProfCycleEvos_16
	case EVO_BEAUTY, Rogue_Hub_ProfLab_ProfCycleEvos_17
	case EVO_LEVEL_ITEM, Rogue_Hub_ProfLab_ProfCycleEvos_18
	case EVO_LEVEL_FEMALE, Rogue_Hub_ProfLab_ProfCycleEvos_19
	case EVO_LEVEL_MALE, Rogue_Hub_ProfLab_ProfCycleEvos_20
	case EVO_LEVEL_NIGHT, Rogue_Hub_ProfLab_ProfCycleEvos_21
	case EVO_LEVEL_DAY, Rogue_Hub_ProfLab_ProfCycleEvos_22
	case EVO_LEVEL_DUSK, Rogue_Hub_ProfLab_ProfCycleEvos_23
	case EVO_ITEM_HOLD_DAY, Rogue_Hub_ProfLab_ProfCycleEvos_24
	case EVO_ITEM_HOLD_NIGHT, Rogue_Hub_ProfLab_ProfCycleEvos_25
	case EVO_MOVE, Rogue_Hub_ProfLab_ProfCycleEvos_26
	case EVO_MOVE_TYPE, Rogue_Hub_ProfLab_ProfCycleEvos_27
	case EVO_MAPSEC, Rogue_Hub_ProfLab_ProfCycleEvos_28
	case EVO_SPECIFIC_MAP, Rogue_Hub_ProfLab_ProfCycleEvos_28
	case EVO_ITEM_MALE, Rogue_Hub_ProfLab_ProfCycleEvos_29
	case EVO_ITEM_FEMALE, Rogue_Hub_ProfLab_ProfCycleEvos_30
	case EVO_LEVEL_RAIN, Rogue_Hub_ProfLab_ProfCycleEvos_31
	case EVO_SPECIFIC_MON_IN_PARTY, Rogue_Hub_ProfLab_ProfCycleEvos_32
	case EVO_LEVEL_DARK_TYPE_MON_IN_PARTY, Rogue_Hub_ProfLab_ProfCycleEvos_33
	case EVO_TRADE_SPECIFIC_MON, Rogue_Hub_ProfLab_ProfCycleEvos_34
	case EVO_LEVEL_NATURE_AMPED, Rogue_Hub_ProfLab_ProfCycleEvos_35
	case EVO_LEVEL_NATURE_LOW_KEY, Rogue_Hub_ProfLab_ProfCycleEvos_35
	case EVO_CRITICAL_HITS, Rogue_Hub_ProfLab_ProfCycleEvos_36
	case EVO_SCRIPT_TRIGGER_DMG, Rogue_Hub_ProfLab_ProfCycleEvos_37
	case EVO_DARK_SCROLL, Rogue_Hub_ProfLab_ProfCycleEvos_38
	case EVO_WATER_SCROLL, Rogue_Hub_ProfLab_ProfCycleEvos_38
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_34
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_5:
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_0
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_6:
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_1
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_7:
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_2
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_8:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_3
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_9:
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_4
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_10:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_5
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_11:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_6
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_12:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_7
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_13:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_8
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_14:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_9
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_15:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_10
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_16:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_11
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_17:
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_12
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_18:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_13
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_19:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_14
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_20:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_15
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_21:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_16
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_22:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_17
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_23:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_18
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_24:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_19
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_25:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_20
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_26:
	buffermovename STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_21
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_27:
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_22
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_28:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_23
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_29:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_24
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_30:
	bufferitemname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_25
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_31:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_26
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_32:
	bufferspeciesname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_27
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_33:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_28
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_34:
	bufferspeciesname STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_29
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_35:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_30
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_36:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_31
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_37:
	buffernumberstring STR_VAR_3, VAR_0x8007
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_32
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1

Rogue_Hub_ProfLab_ProfCycleEvos_38:
	msgbox Rogue_Hub_ProfLab_ProfCycleEvos_Text_33
	goto Rogue_Hub_ProfLab_ProfCycleEvos_1


Rogue_Hub_ProfLab_ProfShowEvos::
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_ProfShowEvos_1
	return

Rogue_Hub_ProfLab_ProfShowEvos_1:
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_1
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Hub_ProfLab_ProfShowEvos_4
	specialvar VAR_RESULT, Rogue_GetMonEvoCount
	special BufferMonNickname
	compare VAR_RESULT, 0
	goto_if_eq Rogue_Hub_ProfLab_ProfShowEvos_7
	compare VAR_RESULT, 1
	goto_if_eq Rogue_Hub_ProfLab_ProfShowEvos_8
	buffernumberstring STR_VAR_2, VAR_RESULT
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_6
	setvar VAR_0x8005, 0
	call Rogue_Hub_ProfLab_ProfCycleEvos
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_5, MSGBOX_DEFAULT
Rogue_Hub_ProfLab_ProfShowEvos_3:
	setvar VAR_RESULT, YES
	return

Rogue_Hub_ProfLab_ProfShowEvos_4:
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_2, MSGBOX_DEFAULT
	goto Rogue_Hub_ProfLab_ProfShowEvos_3

Rogue_Hub_ProfLab_ProfShowEvos_7:
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_3
	goto Rogue_Hub_ProfLab_ProfShowEvos_3

Rogue_Hub_ProfLab_ProfShowEvos_8:
	buffernumberstring STR_VAR_2, VAR_RESULT
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_4
	setvar VAR_0x8005, 0
	call Rogue_Hub_ProfLab_ProfCycleEvos
	msgbox Rogue_Hub_ProfLab_ProfShowEvos_Text_5, MSGBOX_DEFAULT
	goto Rogue_Hub_ProfLab_ProfShowEvos_3


Rogue_Hub_ProfLab_TriggerPC::
	lockall
	msgbox Rogue_Hub_ProfLab_TriggerPC_Text_0, MSGBOX_DEFAULT
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkBackwards
	waitmovement 0
	releaseall
	return


Rogue_Hub_ProfLab_WalkBackwards:
	walk_down
	step_end

Rogue_Hub_ProfLab_IntroCommon::
	msgbox Rogue_Hub_ProfLab_IntroCommon_Text_0, MSGBOX_DEFAULT
	turnobject 1, DIR_EAST
	msgbox Rogue_Hub_ProfLab_IntroCommon_Text_1, MSGBOX_DEFAULT
	return


Rogue_Hub_ProfLab_IntroRunAwayCommon::
	bufferspeciesname STR_VAR_1, SPECIES_SUNKERN
	givemon SPECIES_SUNKERN, 7, ITEM_ORAN_BERRY
	playfanfare MUS_OBTAIN_ITEM
	message Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_0
	waitmessage
	waitfanfare
	msgbox Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_IntroRunAwayCommon_2
Rogue_Hub_ProfLab_IntroRunAwayCommon_1:
	setvar VAR_STARTER_MON, 4
	call Rogue_Hub_ProfLab_PostStarterSelect
	return

Rogue_Hub_ProfLab_IntroRunAwayCommon_2:
	call Common_EventScript_NameReceivedPartyMon
	goto Rogue_Hub_ProfLab_IntroRunAwayCommon_1


Rogue_Hub_ProfLab_IntroTriggerL::
	lockall
	compare VAR_TEMP_F, YES
	goto_if_ne Rogue_Hub_ProfLab_IntroTriggerL_2
	turnobject 1, DIR_SOUTH
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	msgbox Rogue_Hub_ProfLab_IntroTriggerL_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_IntroTriggerL_5
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkUpOne
	waitmovement 0
Rogue_Hub_ProfLab_IntroTriggerL_1:
	releaseall
	return

Rogue_Hub_ProfLab_IntroTriggerL_2:
	setvar VAR_TEMP_F, YES
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkToProfL
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroCommon
	goto Rogue_Hub_ProfLab_IntroTriggerL_1

Rogue_Hub_ProfLab_IntroTriggerL_5:
	msgbox Rogue_Hub_ProfLab_IntroTriggerL_Text_1, MSGBOX_DEFAULT
	applymovement 1, Rogue_Hub_ProfLab_WalkFromProfL
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroRunAwayCommon
	goto Rogue_Hub_ProfLab_IntroTriggerL_1


Rogue_Hub_ProfLab_IntroTriggerR::
	lockall
	compare VAR_TEMP_F, YES
	goto_if_ne Rogue_Hub_ProfLab_IntroTriggerR_2
	turnobject 1, DIR_SOUTH
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	msgbox Rogue_Hub_ProfLab_IntroTriggerR_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_IntroTriggerR_5
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkUpOne
	waitmovement 0
Rogue_Hub_ProfLab_IntroTriggerR_1:
	releaseall
	return

Rogue_Hub_ProfLab_IntroTriggerR_2:
	setvar VAR_TEMP_F, YES
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkToProfR
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroCommon
	goto Rogue_Hub_ProfLab_IntroTriggerR_1

Rogue_Hub_ProfLab_IntroTriggerR_5:
	msgbox Rogue_Hub_ProfLab_IntroTriggerL_Text_1, MSGBOX_DEFAULT
	applymovement 1, Rogue_Hub_ProfLab_WalkFromProfR
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroRunAwayCommon
	goto Rogue_Hub_ProfLab_IntroTriggerR_1


Rogue_Hub_ProfLab_WalkUpOne:
	walk_up
	step_end

Rogue_Hub_ProfLab_WalkToProfL:
	walk_up
	walk_up
	walk_up
	step_end

Rogue_Hub_ProfLab_WalkToProfR:
	walk_left
	walk_up
	walk_up
	walk_up
	step_end

Rogue_Hub_ProfLab_WalkFromProfL:
	walk_down
	walk_down
	walk_down
	step_end

Rogue_Hub_ProfLab_WalkFromProfR:
	walk_right
	walk_down
	walk_down
	walk_down
	step_end

Rogue_Hub_ProfLab_PokeballCommon::
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_le Rogue_Hub_ProfLab_PokeballCommon_2
	bufferspeciesname STR_VAR_1, VAR_0x8001
	msgbox Rogue_Hub_ProfLab_PokeballCommon_Text_1
Rogue_Hub_ProfLab_PokeballCommon_1:
	setvar VAR_RESULT, NO
	return

Rogue_Hub_ProfLab_PokeballCommon_2:
	turnobject 1, DIR_EAST
	showmonpic VAR_0x8001, 10, 3
	playmoncry VAR_0x8001, CRY_MODE_NORMAL
	waitmoncry
	bufferspeciesname STR_VAR_1, VAR_0x8001
	msgbox Rogue_Hub_ProfLab_PokeballCommon_Text_0, MSGBOX_YESNO
	hidemonpic
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_PokeballCommon_5
	goto Rogue_Hub_ProfLab_PokeballCommon_1

Rogue_Hub_ProfLab_PokeballCommon_5:
	removeobject VAR_LAST_TALKED
	givemon VAR_0x8001, 7, ITEM_ORAN_BERRY
	playfanfare MUS_OBTAIN_ITEM
	message Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_0
	waitmessage
	waitfanfare
	msgbox Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_PokeballCommon_8
Rogue_Hub_ProfLab_PokeballCommon_7:
	turnobject OBJ_EVENT_ID_PLAYER, DIR_WEST
	call Rogue_Hub_ProfLab_PostStarterSelect
	setvar VAR_RESULT, YES
	return

Rogue_Hub_ProfLab_PokeballCommon_8:
	call Common_EventScript_GetGiftMonPartySlot
	call Common_EventScript_NameReceivedPartyMon
	goto Rogue_Hub_ProfLab_PokeballCommon_7


Rogue_Hub_ProfLab_PostStarterSelect::
	msgbox Rogue_Hub_ProfLab_PostStarterSelect_Text_0, MSGBOX_DEFAULT
	giveitem ITEM_POKE_BALL, 5
	giveitem ITEM_POTION, 1
	setflag FLAG_SYS_POKEDEX_GET
	setflag FLAG_SYS_POKEMON_GET
	setvar VAR_LITTLEROOT_INTRO_STATE, 4
	msgbox Rogue_Hub_ProfLab_PostStarterSelect_Text_1, MSGBOX_DEFAULT
	call Rogue_Hub_ProfLab_ProfExplain
	return


Rogue_Hub_ProfLab_Pokeball0::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_STARTER0
	call Rogue_Hub_ProfLab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_Pokeball0_2
Rogue_Hub_ProfLab_Pokeball0_1:
	release
	return

Rogue_Hub_ProfLab_Pokeball0_2:
	setvar VAR_STARTER_MON, 1
	setflag FLAG_TEMP_1
	goto Rogue_Hub_ProfLab_Pokeball0_1


Rogue_Hub_ProfLab_Pokeball1::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_STARTER1
	call Rogue_Hub_ProfLab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_Pokeball1_2
Rogue_Hub_ProfLab_Pokeball1_1:
	release
	return

Rogue_Hub_ProfLab_Pokeball1_2:
	setvar VAR_STARTER_MON, 2
	setflag FLAG_TEMP_2
	goto Rogue_Hub_ProfLab_Pokeball1_1


Rogue_Hub_ProfLab_Pokeball2::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_STARTER2
	call Rogue_Hub_ProfLab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_Pokeball2_2
Rogue_Hub_ProfLab_Pokeball2_1:
	release
	return

Rogue_Hub_ProfLab_Pokeball2_2:
	setvar VAR_STARTER_MON, 3
	setflag FLAG_TEMP_3
	goto Rogue_Hub_ProfLab_Pokeball2_1


Rogue_Hub_ProfLab_SpeakProf_Text_0:
	.string "BIRCH: Hey {PLAYER}!\n"
	.string "Hope you're enjoying the adventure so\l"
	.string "far!$"

Rogue_Hub_ProfLab_SpeakProf_Text_1:
	.string "BIRCH: Go ahead!\n"
	.string "Pick a PARTNER POKéMON!$"

Rogue_Hub_ProfLab_SpeakProf_Text_2:
	.string "BIRCH: OK then!\n"
	.string "If you ever need an explaination, you\l"
	.string "know where to find me.$"

Rogue_Hub_ProfLab_ProfExplain_Text_0:
	.string "BIRCH: Would you like me to explain how\n"
	.string "adventures work?$"

Rogue_Hub_ProfLab_ProfExplain_Text_1:
	.string "BIRCH: You never know what you're\n"
	.string "gonna see on an adventure!\p"
	.string "The routes will swap around, the\n"
	.string "POKéMON and TRAINERS you encounter\l"
	.string "will also change!\p"
	.string "When you finish your adventure you'll\n"
	.string "be escorted back to the HUB and we'll\l"
	.string "restore your BAG and POKéMON back to\l"
	.string "their original states.\p"
	.string "So, for example, if you evolved your\n"
	.string "PARTNER POKéMON whilst on an\l"
	.string "adventure, it would unevolve when you\l"
	.string "come back to the HUB.$"

Rogue_Hub_ProfLab_ProfExplain_Text_2:
	.string "When you return from adventures, you'll\n"
	.string "get given reward money which you can\l"
	.string "then spend on anything you'd like in\l"
	.string "the HUB.\p"
	.string "Whatever you buy or use in the HUB will\n"
	.string "be permanent.\p"
	.string "So if you went and bought extra potions\n"
	.string "in the HUB but used them during an\l"
	.string "adventure, once you returned you'd\l"
	.string "find that they've been placed right\l"
	.string "back in to you BAG.\p"
	.string "The best way to learn about adventures\n"
	.string "is to go on many of them!\p"
	.string "So go ahead!\n"
	.string "Explore and have fun!$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_0:
	.string "{STR_VAR_2}. get high friendship.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_1:
	.string "{STR_VAR_2}. get high friendship\n"
	.string "during the day.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_2:
	.string "{STR_VAR_2}. get high friendship\n"
	.string "during the night.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_3:
	.string "{STR_VAR_2}. raise to lvl {STR_VAR_3}.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_4:
	.string "{STR_VAR_2}. trade.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_5:
	.string "{STR_VAR_2}. trade whilst holding\n"
	.string "{STR_VAR_3}.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_6:
	.string "{STR_VAR_2}. use a {STR_VAR_3}.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_7:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} whilst attack is higher\l"
	.string "than defence.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_8:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} whilst attack is equal\l"
	.string "to defence.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_9:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} whilst attack is lower\l"
	.string "than defence.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_10:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} with a specific\l"
	.string "personality.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_11:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} with a spare slot in\l"
	.string "your party available.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_12:
	.string "{STR_VAR_2}. get high beauty.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_13:
	.string "{STR_VAR_2}. raise by a lvl whilst\n"
	.string "holding {STR_VAR_3}.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_14:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} when female.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_15:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} when male.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_16:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} during the night.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_17:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} during the day.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_18:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} during dusk.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_19:
	.string "{STR_VAR_2}. raise by a lvl whilst\n"
	.string "holding {STR_VAR_3} during the day.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_20:
	.string "{STR_VAR_2}. raise by a lvl whilst\n"
	.string "holding {STR_VAR_3} during the\l"
	.string "night.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_21:
	.string "{STR_VAR_2}. raise by a lvl whilst\n"
	.string "knowing {STR_VAR_3}.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_22:
	.string "{STR_VAR_2}. raise by a lvl whilst\n"
	.string "knowing a FAIRY move.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_23:
	.string "{STR_VAR_2}. raise by a lvl whilst\n"
	.string "holding {STR_VAR_3} when in ???\l"
	.string "location.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_24:
	.string "{STR_VAR_2}. use a {STR_VAR_3}\n"
	.string "when male.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_25:
	.string "{STR_VAR_2}. use a {STR_VAR_3}\n"
	.string "when female.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_26:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} whilst it is raining.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_27:
	.string "{STR_VAR_2}. raise by a lvl whilst\n"
	.string "{STR_VAR_3} is in the party.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_28:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} whilst a DARK type\l"
	.string "POKéMON is in the party.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_29:
	.string "{STR_VAR_2}. trade for a\n"
	.string "{STR_VAR_3}.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_30:
	.string "{STR_VAR_2}. raise to lvl\n"
	.string "{STR_VAR_3} with a specific nature.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_31:
	.string "{STR_VAR_2}. deal {STR_VAR_3}\n"
	.string "critical hits in a single battle.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_32:
	.string "{STR_VAR_2}. when hp less than\n"
	.string "{STR_VAR_3} interact with ???.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_33:
	.string "{STR_VAR_2}. interact with ???.$"

Rogue_Hub_ProfLab_ProfCycleEvos_Text_34:
	.string "{STR_VAR_2}. UNKNOWN EVO TYPE,\n"
	.string "{STR_VAR_3}.$"

Rogue_Hub_ProfLab_ProfShowEvos_Text_0:
	.string "BIRCH: Would you like to check how a\n"
	.string "POKéMON evolves?$"

Rogue_Hub_ProfLab_ProfShowEvos_Text_1:
	.string "Who would you like information on?$"

Rogue_Hub_ProfLab_ProfShowEvos_Text_2:
	.string "BIRCH: OK then!\n"
	.string "If you ever need any help, you know\l"
	.string "where to find me.$"

Rogue_Hub_ProfLab_ProfShowEvos_Text_3:
	.string "{STR_VAR_1} doesn't appear to\n"
	.string "currently have access to any\l"
	.string "evolutions.$"

Rogue_Hub_ProfLab_ProfShowEvos_Text_4:
	.string "{STR_VAR_1} currently has access to\n"
	.string "{STR_VAR_2} evolution.$"

Rogue_Hub_ProfLab_ProfShowEvos_Text_5:
	.string "BIRCH: If you ever need any more help,\n"
	.string "you know where to find me.$"

Rogue_Hub_ProfLab_ProfShowEvos_Text_6:
	.string "{STR_VAR_1} currently has access to\n"
	.string "{STR_VAR_2} evolutions.$"

Rogue_Hub_ProfLab_TriggerPC_Text_0:
	.string "(I shouldn't use this now)$"

Rogue_Hub_ProfLab_IntroCommon_Text_0:
	.string "BIRCH: {PLAYER}!\n"
	.string "You finally made it!\p"
	.string "I assume you're itching to go on\n"
	.string "adventures, but before you start you\l"
	.string "need a POKéMON of your own.$"

Rogue_Hub_ProfLab_IntroCommon_Text_1:
	.string "BIRCH: I went out and caught a few for\n"
	.string "you to pick from this morning.\p"
	.string "Pick whichever you like!$"

Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_0:
	.string "{PLAYER} obtained a {STR_VAR_1}.$"

Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_1:
	.string "BIRCH: Would you like to give\n"
	.string "{STR_VAR_1} a nickname?$"

Rogue_Hub_ProfLab_IntroTriggerL_Text_0:
	.string "BIRCH: Oh...\p"
	.string "Would you prefer some different\n"
	.string "POKéMON options?$"

Rogue_Hub_ProfLab_IntroTriggerL_Text_1:
	.string "BIRCH: OK, here you go!$"

Rogue_Hub_ProfLab_IntroTriggerR_Text_0:
	.string "BIRCH: Oh...\p"
	.string "Would you prefer a different POKéMON?$"

Rogue_Hub_ProfLab_PokeballCommon_Text_0:
	.string "BIRCH: You'd like to take\n"
	.string "{STR_VAR_1}?$"

Rogue_Hub_ProfLab_PokeballCommon_Text_1:
	.string "(Poor {STR_VAR_1}...)\p"
	.string "(Must be lonely...)$"

Rogue_Hub_ProfLab_PostStarterSelect_Text_0:
	.string "BIRCH: I also threw some items and a\n"
	.string "POKéDEX in there too!$"

Rogue_Hub_ProfLab_PostStarterSelect_Text_1:
	.string "You're now ready to go on adventures!$"
