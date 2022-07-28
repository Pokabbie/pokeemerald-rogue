Rogue_Hub_ProfLab_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, Rogue_Hub_ProfLab_OnTransition
	.byte 0


Rogue_Hub_ProfLab_OnTransition::
	switch VAR_STARTER_MON
	case 1, Rogue_Hub_ProfLab_OnTransition_2
	case 2, Rogue_Hub_ProfLab_OnTransition_3
	case 3, Rogue_Hub_ProfLab_OnTransition_4
	return

Rogue_Hub_ProfLab_OnTransition_2:
	setflag FLAG_TEMP_1
	return

Rogue_Hub_ProfLab_OnTransition_3:
	setflag FLAG_TEMP_2
	return

Rogue_Hub_ProfLab_OnTransition_4:
	setflag FLAG_TEMP_3
	return


Rogue_Hub_ProfLab_SpeakProf::
	lock
	faceplayer
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_le Rogue_Hub_ProfLab_SpeakProf_2
	call Rogue_Hub_ProfLab_ProfExplain
Rogue_Hub_ProfLab_SpeakProf_1:
	release
	return

Rogue_Hub_ProfLab_SpeakProf_2:
	msgbox Rogue_Hub_ProfLab_SpeakProf_Text_0, MSGBOX_DEFAULT
	goto Rogue_Hub_ProfLab_SpeakProf_1


Rogue_Hub_ProfLab_ProfExplain::
	msgbox Rogue_Hub_ProfLab_ProfExplain_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_ProfExplain_2
	msgbox Rogue_Hub_ProfLab_ProfExplain_Text_2, MSGBOX_DEFAULT
Rogue_Hub_ProfLab_ProfExplain_1:
	return

Rogue_Hub_ProfLab_ProfExplain_2:
	msgbox Rogue_Hub_ProfLab_ProfExplain_Text_1, MSGBOX_DEFAULT
	goto Rogue_Hub_ProfLab_ProfExplain_1


Rogue_Hub_ProfLab_TriggerPC::
	lockall
	msgbox Rogue_Hub_ProfLab_TriggerPC_Text_0, MSGBOX_DEFAULT
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkBackwards
	waitmovement 0
	releaseall
	return


Rogue_Hub_ProfLab_WalkBackwards:
	walk_down
	step_end

Rogue_Hub_ProfLab_IntroCommon::
	msgbox Rogue_Hub_ProfLab_IntroCommon_Text_0, MSGBOX_DEFAULT
	turnobject 1, DIR_EAST
	msgbox Rogue_Hub_ProfLab_IntroCommon_Text_1, MSGBOX_DEFAULT
	return


Rogue_Hub_ProfLab_IntroRunAwayCommon::
	bufferspeciesname STR_VAR_1, SPECIES_SUNKERN
	givemon SPECIES_SUNKERN, 7, ITEM_ORAN_BERRY
	playfanfare MUS_OBTAIN_ITEM
	message Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_0
	waitmessage
	waitfanfare
	msgbox Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_IntroRunAwayCommon_2
Rogue_Hub_ProfLab_IntroRunAwayCommon_1:
	setvar VAR_STARTER_MON, 4
	call Rogue_Hub_ProfLab_PostStarterSelect
	return

Rogue_Hub_ProfLab_IntroRunAwayCommon_2:
	call Common_EventScript_NameReceivedPartyMon
	goto Rogue_Hub_ProfLab_IntroRunAwayCommon_1


Rogue_Hub_ProfLab_IntroTriggerL::
	lockall
	compare VAR_TEMP_F, YES
	goto_if_ne Rogue_Hub_ProfLab_IntroTriggerL_2
	turnobject 1, DIR_SOUTH
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	msgbox Rogue_Hub_ProfLab_IntroTriggerL_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_IntroTriggerL_5
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkUpOne
	waitmovement 0
Rogue_Hub_ProfLab_IntroTriggerL_1:
	releaseall
	return

Rogue_Hub_ProfLab_IntroTriggerL_2:
	setvar VAR_TEMP_F, YES
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkToProfL
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroCommon
	goto Rogue_Hub_ProfLab_IntroTriggerL_1

Rogue_Hub_ProfLab_IntroTriggerL_5:
	msgbox Rogue_Hub_ProfLab_IntroTriggerL_Text_1, MSGBOX_DEFAULT
	applymovement 1, Rogue_Hub_ProfLab_WalkFromProfL
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroRunAwayCommon
	goto Rogue_Hub_ProfLab_IntroTriggerL_1


Rogue_Hub_ProfLab_IntroTriggerR::
	lockall
	compare VAR_TEMP_F, YES
	goto_if_ne Rogue_Hub_ProfLab_IntroTriggerR_2
	turnobject 1, DIR_SOUTH
	turnobject OBJ_EVENT_ID_PLAYER, DIR_NORTH
	msgbox Rogue_Hub_ProfLab_IntroTriggerL_Text_0, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_IntroTriggerR_5
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkUpOne
	waitmovement 0
Rogue_Hub_ProfLab_IntroTriggerR_1:
	releaseall
	return

Rogue_Hub_ProfLab_IntroTriggerR_2:
	setvar VAR_TEMP_F, YES
	applymovement OBJ_EVENT_ID_PLAYER, Rogue_Hub_ProfLab_WalkToProfR
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroCommon
	goto Rogue_Hub_ProfLab_IntroTriggerR_1

Rogue_Hub_ProfLab_IntroTriggerR_5:
	msgbox Rogue_Hub_ProfLab_IntroTriggerL_Text_1, MSGBOX_DEFAULT
	applymovement 1, Rogue_Hub_ProfLab_WalkFromProfR
	waitmovement 0
	call Rogue_Hub_ProfLab_IntroRunAwayCommon
	goto Rogue_Hub_ProfLab_IntroTriggerR_1


Rogue_Hub_ProfLab_WalkUpOne:
	walk_up
	step_end

Rogue_Hub_ProfLab_WalkToProfL:
	walk_up
	walk_up
	walk_up
	step_end

Rogue_Hub_ProfLab_WalkToProfR:
	walk_left
	walk_up
	walk_up
	walk_up
	step_end

Rogue_Hub_ProfLab_WalkFromProfL:
	walk_down
	walk_down
	walk_down
	step_end

Rogue_Hub_ProfLab_WalkFromProfR:
	walk_right
	walk_down
	walk_down
	walk_down
	step_end

Rogue_Hub_ProfLab_PokeballCommon::
	compare VAR_LITTLEROOT_INTRO_STATE, 3
	goto_if_le Rogue_Hub_ProfLab_PokeballCommon_2
	bufferspeciesname STR_VAR_1, VAR_0x8001
	msgbox Rogue_Hub_ProfLab_PokeballCommon_Text_1
Rogue_Hub_ProfLab_PokeballCommon_1:
	setvar VAR_RESULT, NO
	return

Rogue_Hub_ProfLab_PokeballCommon_2:
	turnobject 1, DIR_EAST
	showmonpic VAR_0x8001, 10, 3
	playmoncry VAR_0x8001, CRY_MODE_NORMAL
	waitmoncry
	bufferspeciesname STR_VAR_1, VAR_0x8001
	msgbox Rogue_Hub_ProfLab_PokeballCommon_Text_0, MSGBOX_YESNO
	hidemonpic
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_PokeballCommon_5
	goto Rogue_Hub_ProfLab_PokeballCommon_1

Rogue_Hub_ProfLab_PokeballCommon_5:
	removeobject VAR_LAST_TALKED
	givemon VAR_0x8001, 7, ITEM_ORAN_BERRY
	playfanfare MUS_OBTAIN_ITEM
	message Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_0
	waitmessage
	waitfanfare
	msgbox Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_1, MSGBOX_YESNO
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_PokeballCommon_8
Rogue_Hub_ProfLab_PokeballCommon_7:
	turnobject OBJ_EVENT_ID_PLAYER, DIR_WEST
	call Rogue_Hub_ProfLab_PostStarterSelect
	setvar VAR_RESULT, YES
	return

Rogue_Hub_ProfLab_PokeballCommon_8:
	call Common_EventScript_NameReceivedPartyMon
	goto Rogue_Hub_ProfLab_PokeballCommon_7


Rogue_Hub_ProfLab_PostStarterSelect::
	msgbox Rogue_Hub_ProfLab_PostStarterSelect_Text_0, MSGBOX_DEFAULT
	giveitem ITEM_POKE_BALL, 5
	giveitem ITEM_POTION, 1
	setflag FLAG_SYS_POKEDEX_GET
	setflag FLAG_SYS_POKEMON_GET
	setvar VAR_LITTLEROOT_INTRO_STATE, 4
	msgbox Rogue_Hub_ProfLab_PostStarterSelect_Text_1, MSGBOX_DEFAULT
	call Rogue_Hub_ProfLab_ProfExplain
	return


Rogue_Hub_ProfLab_Pokeball0::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_STARTER0
	call Rogue_Hub_ProfLab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_Pokeball0_2
Rogue_Hub_ProfLab_Pokeball0_1:
	release
	return

Rogue_Hub_ProfLab_Pokeball0_2:
	setvar VAR_STARTER_MON, 1
	setflag FLAG_TEMP_1
	goto Rogue_Hub_ProfLab_Pokeball0_1


Rogue_Hub_ProfLab_Pokeball1::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_STARTER1
	call Rogue_Hub_ProfLab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_Pokeball1_2
Rogue_Hub_ProfLab_Pokeball1_1:
	release
	return

Rogue_Hub_ProfLab_Pokeball1_2:
	setvar VAR_STARTER_MON, 2
	setflag FLAG_TEMP_2
	goto Rogue_Hub_ProfLab_Pokeball1_1


Rogue_Hub_ProfLab_Pokeball2::
	lock
	copyvar VAR_0x8001, VAR_ROGUE_STARTER2
	call Rogue_Hub_ProfLab_PokeballCommon
	compare VAR_RESULT, YES
	goto_if_eq Rogue_Hub_ProfLab_Pokeball2_2
Rogue_Hub_ProfLab_Pokeball2_1:
	release
	return

Rogue_Hub_ProfLab_Pokeball2_2:
	setvar VAR_STARTER_MON, 3
	setflag FLAG_TEMP_3
	goto Rogue_Hub_ProfLab_Pokeball2_1


Rogue_Hub_ProfLab_SpeakProf_Text_0:
	.string "BIRCH: Go ahead!\n"
	.string "Pick a PARTNER POKéMON!$"

Rogue_Hub_ProfLab_ProfExplain_Text_0:
	.string "BIRCH: Would you like me to explain how\n"
	.string "adventures work?$"

Rogue_Hub_ProfLab_ProfExplain_Text_1:
	.string "BIRCH: TODO$"

Rogue_Hub_ProfLab_ProfExplain_Text_2:
	.string "BIRCH: OK then!\n"
	.string "If you ever need an explaination, you\l"
	.string "know where to find me.$"

Rogue_Hub_ProfLab_TriggerPC_Text_0:
	.string "(I shouldn't use this now)$"

Rogue_Hub_ProfLab_IntroCommon_Text_0:
	.string "BIRCH: {PLAYER}!\n"
	.string "You finally made it!\p"
	.string "I assume you're itching to go on\n"
	.string "adventures, but before you start you\l"
	.string "need a POKéMON of your own.$"

Rogue_Hub_ProfLab_IntroCommon_Text_1:
	.string "BIRCH: I went out and caught a few for\n"
	.string "you to pick from this morning.\p"
	.string "Pick whichever you like!$"

Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_0:
	.string "{PLAYER} obtained a {STR_VAR_1}.$"

Rogue_Hub_ProfLab_IntroRunAwayCommon_Text_1:
	.string "BIRCH: Would you like to give\n"
	.string "{STR_VAR_1} a nickname?$"

Rogue_Hub_ProfLab_IntroTriggerL_Text_0:
	.string "BIRCH: Oh...\p"
	.string "Would you prefer some different POKéMON?$"

Rogue_Hub_ProfLab_IntroTriggerL_Text_1:
	.string "BIRCH: OK, you can have this one!$"

Rogue_Hub_ProfLab_PokeballCommon_Text_0:
	.string "BIRCH: You'd like to take\n"
	.string "{STR_VAR_1}?$"

Rogue_Hub_ProfLab_PokeballCommon_Text_1:
	.string "(Poor {STR_VAR_1}...)\p"
	.string "(Must be lonely...)$"

Rogue_Hub_ProfLab_PostStarterSelect_Text_0:
	.string "BIRCH: I also threw some items and a\n"
	.string "POKéDEX in there too!$"

Rogue_Hub_ProfLab_PostStarterSelect_Text_1:
	.string "You're now ready to go on adventures!$"
