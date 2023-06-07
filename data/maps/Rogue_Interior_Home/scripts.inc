Rogue_Interior_Home_MapScripts::
	.byte 0


Rogue_Interior_Home_TurnOnPC::
	lockall
	setvar VAR_0x8004, PC_LOCATION_BRENDANS_HOUSE
	special DoPCTurnOnEffect
	playse SE_PC_ON
	msgbox Rogue_Interior_Home_TurnOnPC_Text_0, MSGBOX_DEFAULT
	special BedroomPC
	waitstate
	setvar VAR_0x8004, PC_LOCATION_BRENDANS_HOUSE
	playse SE_PC_OFF
	special DoPCTurnOffEffect
	releaseall
	return


Rogue_Interior_Home_TurnOnPCLowerFloor::
	hubupgradeunlocked HUB_UPGRADE_HOME_UPPER_FLOOR
	compare VAR_RESULT, NO
	goto_if_eq Rogue_Interior_Home_TurnOnPCLowerFloor_1
	return

Rogue_Interior_Home_TurnOnPCLowerFloor_1:
	lockall
	setvar VAR_0x8004, PC_LOCATION_MAYS_HOUSE
	special DoPCTurnOnEffect
	playse SE_PC_ON
	msgbox Rogue_Interior_Home_TurnOnPC_Text_0, MSGBOX_DEFAULT
	special BedroomPC
	waitstate
	setvar VAR_0x8004, PC_LOCATION_MAYS_HOUSE
	playse SE_PC_OFF
	special DoPCTurnOffEffect
	releaseall
	return


Rogue_Interior_Home_NoteBook::
	lockall
	msgbox Rogue_Interior_Home_NoteBook_Text_0, MSGBOX_DEFAULT
	releaseall
	return


Rogue_Interior_Home_TurnOnPC_Text_0:
	.string "{PLAYER} booted up the PC.$"

Rogue_Interior_Home_NoteBook_Text_0:
	.string "It's a notebook!\p"
	.string "It has a bunch of incoherent\n"
	.string "scribblings from…\p"
	.string "someone called…\p"
	.string "Tate?\p"
	.string "Why's it here?$"
