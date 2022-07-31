Rogue_Special_Zapdos_MapScripts::
	.byte 0


Rogue_Special_TalkZapdos::
	lockall
	playmoncry SPECIES_ZAPDOS, CRY_MODE_ENCOUNTER
	waitmoncry
	seteventmon SPECIES_ZAPDOS, 5
	setflag FLAG_TEMP_1
	special BattleSetup_StartLegendaryBattle
	waitstate
	removeobject 1
	releaseall
	return

