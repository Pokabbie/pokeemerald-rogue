Rogue_Special_Celebi_MapScripts::
	.byte 0


Rogue_Special_TalkCelebi::
	lockall
	playmoncry SPECIES_CELEBI, CRY_MODE_ENCOUNTER
	waitmoncry
	seteventmon SPECIES_CELEBI, 5
	setflag FLAG_TEMP_1
	special BattleSetup_StartLegendaryBattle
	waitstate
	removeobject 1
	releaseall
	return

