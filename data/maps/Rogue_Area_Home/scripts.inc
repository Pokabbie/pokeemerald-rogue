Rogue_Area_Home_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_Area_Home_OnTransition
	.byte 0


Rogue_Area_Home_OnTransition::
	setflag FLAG_TEMP_1
	setflag FLAG_TEMP_2
	setflag FLAG_TEMP_3
	setflag FLAG_TEMP_4
	setflag FLAG_TEMP_5
	setflag FLAG_TEMP_6
	hubupgradeunlocked HUB_UPGRADE_HOME_BERRY_FIELD1
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Area_Home_OnTransition_2
Rogue_Area_Home_OnTransition_1:
	hubupgradeunlocked HUB_UPGRADE_HOME_BERRY_FIELD2
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Area_Home_OnTransition_5
Rogue_Area_Home_OnTransition_4:
	hubupgradeunlocked HUB_UPGRADE_HOME_GRASS_FIELD
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Area_Home_OnTransition_7
	return

Rogue_Area_Home_OnTransition_2:
	clearflag FLAG_TEMP_1
	goto Rogue_Area_Home_OnTransition_1

Rogue_Area_Home_OnTransition_5:
	clearflag FLAG_TEMP_2
	goto Rogue_Area_Home_OnTransition_4

Rogue_Area_Home_OnTransition_7:
	special RogueHub_SetRandomFollowMonsFromPC
	compare VAR_FOLLOW_MON_0, SPECIES_NONE
	goto_if_ne Rogue_Area_Home_OnTransition_10
Rogue_Area_Home_OnTransition_9:
	compare VAR_FOLLOW_MON_1, SPECIES_NONE
	goto_if_ne Rogue_Area_Home_OnTransition_13
Rogue_Area_Home_OnTransition_12:
	compare VAR_FOLLOW_MON_2, SPECIES_NONE
	goto_if_ne Rogue_Area_Home_OnTransition_16
Rogue_Area_Home_OnTransition_15:
	compare VAR_FOLLOW_MON_3, SPECIES_NONE
	goto_if_ne Rogue_Area_Home_OnTransition_18
	return

Rogue_Area_Home_OnTransition_10:
	clearflag FLAG_TEMP_3
	goto Rogue_Area_Home_OnTransition_9

Rogue_Area_Home_OnTransition_13:
	clearflag FLAG_TEMP_4
	goto Rogue_Area_Home_OnTransition_12

Rogue_Area_Home_OnTransition_16:
	clearflag FLAG_TEMP_5
	goto Rogue_Area_Home_OnTransition_15

Rogue_Area_Home_OnTransition_18:
	clearflag FLAG_TEMP_6
	return

