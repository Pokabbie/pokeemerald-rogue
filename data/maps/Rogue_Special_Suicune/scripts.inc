Rogue_Special_Suicune_MapScripts::
	.byte 0


Rogue_Special_TalkSuicune::
	lockall
	playmoncry SPECIES_SUICUNE, CRY_MODE_ENCOUNTER
	waitmoncry
	seteventmon SPECIES_SUICUNE, 5
	setflag FLAG_TEMP_1
	special BattleSetup_StartLegendaryBattle
	waitstate
	removeobject 1
	releaseall
	return

