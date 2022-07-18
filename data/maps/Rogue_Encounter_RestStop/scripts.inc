Rogue_Encounter_RestStop_MapScripts::
	.byte 0


Rogue_Encounter_RestStop_Heal::
	lock
	faceplayer
	msgbox Rogue_Encounter_RestStop_Heal_Text_0, MSGBOX_DEFAULT
	call Common_EventScript_OutOfCenterPartyHeal
	msgbox Rogue_Encounter_RestStop_Heal_Text_1, MSGBOX_DEFAULT
	release
	return


Rogue_Encounter_RestStop_Heal_Text_0:
	.string "You look tired.\n"
	.string "Let me patch you up!$"

Rogue_Encounter_RestStop_Heal_Text_1:
	.string "Take care!$"
