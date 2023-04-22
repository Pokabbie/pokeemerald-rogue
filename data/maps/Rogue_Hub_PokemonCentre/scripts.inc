Rogue_Hub_PokemonCentre_MapScripts::
	.byte 0


Rogue_Hub_PokemonCentre_EventScript_Nurse::
	lock
	special Rogue_IsMultiplayerActive
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Hub_PokemonCentre_EventScript_Nurse_2
	msgbox Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_1, MSGBOX_DEFAULT
	msgbox Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_2, MSGBOX_DEFAULT
	message Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_3
	waitmessage
	multichoice 16, 6, MULTI_LINK_HOST_JOIN, FALSE
	switch VAR_RESULT
	case 0, Rogue_Hub_PokemonCentre_EventScript_Nurse_7
	case 1, Rogue_Hub_PokemonCentre_EventScript_Nurse_8
	case 2, Rogue_Hub_PokemonCentre_EventScript_Nurse_9
Rogue_Hub_PokemonCentre_EventScript_Nurse_5:
	compare VAR_RESULT, FALSE
	goto_if_eq Rogue_Hub_PokemonCentre_EventScript_Nurse_10
Rogue_Hub_PokemonCentre_EventScript_Nurse_1:
	release
	return

Rogue_Hub_PokemonCentre_EventScript_Nurse_2:
	msgbox Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_0, MSGBOX_DEFAULT
	goto Rogue_Hub_PokemonCentre_EventScript_Nurse_1

Rogue_Hub_PokemonCentre_EventScript_Nurse_7:
	message Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_4
	waitmessage
	special Rogue_JoinMultiplayer
	waitstate
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Hub_PokemonCentre_EventScript_Nurse_12
	goto Rogue_Hub_PokemonCentre_EventScript_Nurse_5

Rogue_Hub_PokemonCentre_EventScript_Nurse_8:
	message Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_6
	waitmessage
	special Rogue_HostMultiplayer
	waitstate
	compare VAR_RESULT, TRUE
	goto_if_eq Rogue_Hub_PokemonCentre_EventScript_Nurse_14
	goto Rogue_Hub_PokemonCentre_EventScript_Nurse_5

Rogue_Hub_PokemonCentre_EventScript_Nurse_9:
	setvar VAR_RESULT, FALSE
	goto Rogue_Hub_PokemonCentre_EventScript_Nurse_5

Rogue_Hub_PokemonCentre_EventScript_Nurse_10:
	msgbox Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_8
	goto Rogue_Hub_PokemonCentre_EventScript_Nurse_1

Rogue_Hub_PokemonCentre_EventScript_Nurse_12:
	msgbox Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_5
	goto Rogue_Hub_PokemonCentre_EventScript_Nurse_5

Rogue_Hub_PokemonCentre_EventScript_Nurse_14:
	msgbox Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_7
	goto Rogue_Hub_PokemonCentre_EventScript_Nurse_5


Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_0:
	.string "Hello {PLAYER}!\p"
	.string "I hope you've been enjoying your\n"
	.string "multiplayer Adventure!$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_1:
	.string "Hello {PLAYER}!\p"
	.string "I can help you to setup multiplayer\n"
	.string "Adventures for you and your friends!$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_2:
	.string "Please launch RogueAssistant.exe now.$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_3:
	.string "Please decide which of you will become\n"
	.string "the GROUP HOST.\p"
	.string "The other players must then choose\n"
	.string "JOIN GROUP.$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_4:
	.string "Attempting to join group\n"
	.string "… … B Button: Cancel$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_5:
	.string "All set!\n"
	.string "Have fun on your Adventures!$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_6:
	.string "Attempting to setup group\n"
	.string "… … B Button: Cancel$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_7:
	.string "All set!\n"
	.string "Your friends should now be able to join\l"
	.string "you for Adventures!$"

Rogue_Hub_PokemonCentre_EventScript_Nurse_Text_8:
	.string "If you change your mind, please feel\n"
	.string "free to speak with me again.$"
