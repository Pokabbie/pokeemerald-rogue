Rogue_Encounter_Lab_MapScripts::
	.byte 0


Rogue_Encounter_Lab_SpeakProf::
	lockall
	faceplayer
	goto_if_set FLAG_TEMP_4, Rogue_Encounter_Lab_SpeakProf_2
	msgbox Rogue_Encounter_Lab_SpeakProf_Text_1
Rogue_Encounter_Lab_SpeakProf_1:
	releaseall
	return

Rogue_Encounter_Lab_SpeakProf_2:
	msgbox Rogue_Encounter_Lab_SpeakProf_Text_0
	goto Rogue_Encounter_Lab_SpeakProf_1


Rogue_Encounter_Lab_PokeballCommon::
	goto_if_unset FLAG_TEMP_4, Rogue_Encounter_Lab_PokeballCommon_2
	special Rogue_BufferLabMonName
	msgbox Rogue_Encounter_Lab_PokeballCommon_Text_2
Rogue_Encounter_Lab_PokeballCommon_1:
	setvar VAR_RESULT, NO
	return

Rogue_Encounter_Lab_PokeballCommon_2:
	turnobject 1, DIR_WEST
	showmonpic VAR_0x8001, 10, 3
	playmoncry VAR_0x8001, CRY_MODE_NORMAL
	waitmoncry
	special Rogue_BufferLabMonName
	msgbox Rogue_Encounter_Lab_PokeballCommon_Text_0, MSGBOX_YESNO
	hidemonpic
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_Lab_PokeballCommon_5
	goto Rogue_Encounter_Lab_PokeballCommon_1

Rogue_Encounter_Lab_PokeballCommon_5:
	specialvar VAR_TEMP_0, CalculatePlayerPartyCount
	specialvar VAR_TEMP_1, Rogue_CalcMaxPartySize
	compare VAR_TEMP_0, VAR_TEMP_1
	goto_if_ge Rogue_Encounter_Lab_PokeballCommon_8
Rogue_Encounter_Lab_PokeballCommon_7:
	setvar VAR_RESULT, YES
	return

Rogue_Encounter_Lab_PokeballCommon_8:
	msgbox Rogue_Encounter_Lab_PokeballCommon_Text_1
	special ChoosePartyMon
	waitstate
	compare VAR_0x8004, PARTY_NOTHING_CHOSEN
	goto_if_eq Rogue_Encounter_Lab_PokeballCommon_10
	special Rogue_ReleaseMonInSlot_NoLabBuffering
	goto Rogue_Encounter_Lab_PokeballCommon_7

Rogue_Encounter_Lab_PokeballCommon_10:
	setvar VAR_RESULT, NO
	return


Rogue_Encounter_Lab_Pokeball_GiveMon::
	special Rogue_BufferLabMonName
	special Rogue_GiveLabMon
	removeobject VAR_LAST_TALKED
	playfanfare MUS_OBTAIN_ITEM
	message Rogue_Encounter_Lab_Pokeball_GiveMon_Text_0
	waitmessage
	waitfanfare
	random 100
	compare VAR_ROGUE_DIFFICULTY, 12
	goto_if_ge Rogue_Encounter_Lab_Pokeball_GiveMon_2
	compare VAR_ROGUE_DIFFICULTY, 8
	goto_if_ge Rogue_Encounter_Lab_Pokeball_GiveMon_3
	compare VAR_ROGUE_DIFFICULTY, 5
	goto_if_ge Rogue_Encounter_Lab_Pokeball_GiveMon_4
	compare VAR_RESULT, 25
	goto_if_lt Rogue_Encounter_Lab_Pokeball_GiveMon_18
	setvar VAR_RESULT, NO
Rogue_Encounter_Lab_Pokeball_GiveMon_1:
	msgbox Rogue_Encounter_Lab_Pokeball_GiveMon_Text_1
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_Lab_Pokeball_GiveMon_9
	msgbox Rogue_Encounter_Lab_Pokeball_GiveMon_Text_3
	return

Rogue_Encounter_Lab_Pokeball_GiveMon_2:
	setvar VAR_RESULT, YES
	goto Rogue_Encounter_Lab_Pokeball_GiveMon_1

Rogue_Encounter_Lab_Pokeball_GiveMon_3:
	compare VAR_RESULT, 60
	goto_if_lt Rogue_Encounter_Lab_Pokeball_GiveMon_12
	setvar VAR_RESULT, NO
	goto Rogue_Encounter_Lab_Pokeball_GiveMon_1

Rogue_Encounter_Lab_Pokeball_GiveMon_4:
	compare VAR_RESULT, 40
	goto_if_lt Rogue_Encounter_Lab_Pokeball_GiveMon_15
	setvar VAR_RESULT, NO
	goto Rogue_Encounter_Lab_Pokeball_GiveMon_1

Rogue_Encounter_Lab_Pokeball_GiveMon_9:
	msgbox Rogue_Encounter_Lab_Pokeball_GiveMon_Text_2
	finditem VAR_ROGUE_ITEM10, 1
	return

Rogue_Encounter_Lab_Pokeball_GiveMon_12:
	setvar VAR_RESULT, YES
	goto Rogue_Encounter_Lab_Pokeball_GiveMon_1

Rogue_Encounter_Lab_Pokeball_GiveMon_15:
	setvar VAR_RESULT, YES
	goto Rogue_Encounter_Lab_Pokeball_GiveMon_1

Rogue_Encounter_Lab_Pokeball_GiveMon_18:
	setvar VAR_RESULT, YES
	goto Rogue_Encounter_Lab_Pokeball_GiveMon_1


Rogue_Encounter_Lab_Pokeball0::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	setvar VAR_0x8002, 0
	call Rogue_Encounter_Lab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_Lab_Pokeball0_2
Rogue_Encounter_Lab_Pokeball0_1:
	release
	return

Rogue_Encounter_Lab_Pokeball0_2:
	copyvar VAR_0x8001, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA
	setvar VAR_0x8002, 0
	call Rogue_Encounter_Lab_Pokeball_GiveMon
	setflag FLAG_TEMP_4
	setflag FLAG_TEMP_1
	goto Rogue_Encounter_Lab_Pokeball0_1


Rogue_Encounter_Lab_Pokeball1::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1
	setvar VAR_0x8002, 1
	call Rogue_Encounter_Lab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_Lab_Pokeball1_2
Rogue_Encounter_Lab_Pokeball1_1:
	release
	return

Rogue_Encounter_Lab_Pokeball1_2:
	copyvar VAR_0x8001, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA1
	setvar VAR_0x8002, 1
	call Rogue_Encounter_Lab_Pokeball_GiveMon
	setflag FLAG_TEMP_4
	setflag FLAG_TEMP_2
	goto Rogue_Encounter_Lab_Pokeball1_1


Rogue_Encounter_Lab_Pokeball2::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2
	setvar VAR_0x8002, 2
	call Rogue_Encounter_Lab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Encounter_Lab_Pokeball2_2
Rogue_Encounter_Lab_Pokeball2_1:
	release
	return

Rogue_Encounter_Lab_Pokeball2_2:
	copyvar VAR_0x8001, VAR_ROGUE_SPECIAL_ENCOUNTER_DATA2
	setvar VAR_0x8002, 2
	call Rogue_Encounter_Lab_Pokeball_GiveMon
	setflag FLAG_TEMP_4
	setflag FLAG_TEMP_3
	goto Rogue_Encounter_Lab_Pokeball2_1


Rogue_Encounter_Lab_SpeakProf_Text_0:
	.string "BIRCH: Hey {PLAYER}!\p"
	.string "Don't worry!\n"
	.string "I'll try my best to track down any more\l"
	.string "POKéMON that faint!$"

Rogue_Encounter_Lab_SpeakProf_Text_1:
	.string "BIRCH: Hey {PLAYER}!\n"
	.string "I managed to catch some of the POKéMON\l"
	.string "that had fainted!\p"
	.string "I wanted to give you all of them back,\n"
	.string "but I got told that would make the\l"
	.string "Adventures 'Too Easy' whatever that\l"
	.string "means...$"

Rogue_Encounter_Lab_PokeballCommon_Text_0:
	.string "BIRCH: You'd like to take\n"
	.string "{STR_VAR_1}?$"

Rogue_Encounter_Lab_PokeballCommon_Text_1:
	.string "Oh it looks like you're going to need to\n"
	.string "release a POKéMON to make room.$"

Rogue_Encounter_Lab_PokeballCommon_Text_2:
	.string "(Poor {STR_VAR_1}...)\p"
	.string "(Must be lonely...)$"

Rogue_Encounter_Lab_Pokeball_GiveMon_Text_0:
	.string "{PLAYER} obtained {STR_VAR_1}.$"

Rogue_Encounter_Lab_Pokeball_GiveMon_Text_1:
	.string "BIRCH: Sometimes the POKéMON manage\n"
	.string "to pickup some items before I catch\l"
	.string "them$"

Rogue_Encounter_Lab_Pokeball_GiveMon_Text_2:
	.string "And would you look at that!\p"
	.string "It looks like {STR_VAR_1} is holding\n"
	.string "something!$"

Rogue_Encounter_Lab_Pokeball_GiveMon_Text_3:
	.string "It doesn't look like {STR_VAR_1} had\n"
	.string "one though.$"
