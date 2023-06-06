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
	clearflag FLAG_TEMP_3
	clearflag FLAG_TEMP_4
	clearflag FLAG_TEMP_5
	clearflag FLAG_TEMP_6
	return

