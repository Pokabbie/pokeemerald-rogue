Rogue_Special_Entei_MapScripts::
	.byte 0


Rogue_Special_TalkEntei::
	lockall
	playmoncry SPECIES_ENTEI, CRY_MODE_ENCOUNTER
	waitmoncry
	seteventmon SPECIES_ENTEI, 5
	setflag FLAG_TEMP_1
	special BattleSetup_StartLegendaryBattle
	waitstate
	removeobject 1
	releaseall
	return

