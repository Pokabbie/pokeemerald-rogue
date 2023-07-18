Rogue_Interior_ConfigLab_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_Interior_ConfigLab_OnLoad
	.byte 0


Rogue_Interior_ConfigLab_OnLoad::
	setvar VAR_FOLLOW_MON_0, SPECIES_DITTO
	goto_if_set FLAG_ROGUE_UNCOVERRED_POKABBIE, Rogue_Interior_ConfigLab_OnLoad_1
	return

Rogue_Interior_ConfigLab_OnLoad_1:
	call Rogue_Interior_ConfigLab_DrawBasementDoor
	return


Rogue_Interior_ConfigLab_DrawBasementDoor::
	setmetatile 2, 13, 0x27C, TRUE
	setmetatile 3, 13, 0x27D, TRUE
	setmetatile 4, 13, 0x27E, TRUE
	setmetatile 2, 14, 0x284, TRUE
	setmetatile 3, 14, 0x283, FALSE
	setmetatile 4, 14, 0x286, TRUE
	setmetatile 2, 15, 0x28C, TRUE
	setmetatile 4, 15, 0x28E, FALSE
	special DrawWholeMapView
	return


Rogue_Interior_ConfigLab_TalkHiddenDoor::
	goto_if_unset FLAG_ROGUE_UNCOVERRED_POKABBIE, Rogue_Interior_ConfigLab_TalkHiddenDoor_1
	return

Rogue_Interior_ConfigLab_TalkHiddenDoor_1:
	lock
	msgbox Rogue_Interior_ConfigLab_TalkHiddenDoor_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Interior_ConfigLab_TalkHiddenDoor_4
Rogue_Interior_ConfigLab_TalkHiddenDoor_3:
	release
	return

Rogue_Interior_ConfigLab_TalkHiddenDoor_4:
	msgbox Rogue_Interior_ConfigLab_TalkHiddenDoor_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Interior_ConfigLab_TalkHiddenDoor_6
	goto Rogue_Interior_ConfigLab_TalkHiddenDoor_3

Rogue_Interior_ConfigLab_TalkHiddenDoor_6:
	setflag FLAG_ROGUE_UNCOVERRED_POKABBIE
	waitse
	playse SE_TRUCK_DOOR
	call Rogue_Interior_ConfigLab_DrawBasementDoor
	goto Rogue_Interior_ConfigLab_TalkHiddenDoor_3


Rogue_Interior_ConfigLab_TalkLanette::
	lock
	faceplayer
	msgbox Rogue_Interior_ConfigLab_TalkLanette_Text_0, MSGBOX_DEFAULT
	goto_if_unset FLAG_ROGUE_MET_LANETTE, Rogue_Interior_ConfigLab_TalkLanette_2
	msgbox Rogue_Interior_ConfigLab_TalkLanette_Text_3, MSGBOX_DEFAULT
Rogue_Interior_ConfigLab_TalkLanette_1:
	special Special_ViewDifficultyConfigMenu
	waitstate
	goto_if_unset FLAG_ROGUE_MET_LANETTE, Rogue_Interior_ConfigLab_TalkLanette_6
Rogue_Interior_ConfigLab_TalkLanette_5:
	msgbox Rogue_Interior_ConfigLab_TalkLanette_Text_5
	release
	return

Rogue_Interior_ConfigLab_TalkLanette_2:
	msgbox Rogue_Interior_ConfigLab_TalkLanette_Text_1
	msgbox Rogue_Interior_ConfigLab_TalkLanette_Text_2
	goto Rogue_Interior_ConfigLab_TalkLanette_1

Rogue_Interior_ConfigLab_TalkLanette_6:
	setflag FLAG_ROGUE_MET_LANETTE
	msgbox Rogue_Interior_ConfigLab_TalkLanette_Text_4
	goto Rogue_Interior_ConfigLab_TalkLanette_5


Rogue_Interior_ConfigLab_TalkHiddenDoor_Text_0:
	.string "What's this?\p"
	.string "A hidden switch!\p"
	.string "Press the switch?$"

Rogue_Interior_ConfigLab_TalkHiddenDoor_Text_1:
	.string "WARNING!\n"
	.string "Pressing this switch will skip ahead and\l"
	.string "activate post-game content.\p"
	.string "You're recommended to complete a run\n"
	.string "normally before pressing this switch.\p"
	.string "Press the switch?$"

Rogue_Interior_ConfigLab_TalkLanette_Text_0:
	.string "LANETTE: Oh, hi {PLAYER}!$"

Rogue_Interior_ConfigLab_TalkLanette_Text_1:
	.string "Now everything has settled a bit, I can\n"
	.string "finally continue my research.\p"
	.string "I've had a bit of a shift in my research.\n"
	.string "I'm now trying to figure what is\l"
	.string "happening out there whilst people are\l"
	.string "out on Adventures.$"

Rogue_Interior_ConfigLab_TalkLanette_Text_2:
	.string "Speaking of Adventures…\p"
	.string "I did figure out how to adjust the\n"
	.string "difficulty of the Adventures you go\l"
	.string "on.\p"
	.string "Here, take a look!$"

Rogue_Interior_ConfigLab_TalkLanette_Text_3:
	.string "Did you want to adjust your Adventure\n"
	.string "settings?$"

Rogue_Interior_ConfigLab_TalkLanette_Text_4:
	.string "I don't really understand it all myself\n"
	.string "yet.\p"
	.string "But I'm sure, if you keep exploring, I'll\n"
	.string "eventually gather enough data to be\l"
	.string "able to unlock the secrets of\l"
	.string "Adventures and share them with you!$"

Rogue_Interior_ConfigLab_TalkLanette_Text_5:
	.string "If you ever want to adjust your\n"
	.string "Adventure settings again, please do\l"
	.string "come talk to me.$"
