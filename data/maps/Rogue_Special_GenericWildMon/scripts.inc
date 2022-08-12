Rogue_Special_GenericWildMon_MapScripts::
	.byte 0


Rogue_Special_TalkGenericWildMon::
	lockall
	playmoncry VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, CRY_MODE_ENCOUNTER
	waitmoncry
	seteventmon VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, 5
	setflag FLAG_TEMP_1
	special BattleSetup_StartLegendaryBattle
	waitstate
	removeobject 1
	releaseall
	return

