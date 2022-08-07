Rogue_Special_Jirachi_MapScripts::
	.byte 0


Rogue_Special_TalkJirachi::
	lockall
	playmoncry SPECIES_JIRACHI, CRY_MODE_ENCOUNTER
	waitmoncry
	seteventmon SPECIES_JIRACHI, 5
	setflag FLAG_TEMP_1
	special BattleSetup_StartLegendaryBattle
	waitstate
	removeobject 1
	releaseall
	return

