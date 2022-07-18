.set LOCALID_NURSE, 1

Rogue_Hub_PokemonCentre_MapScripts::
	.byte 0

Rogue_Hub_PokemonCentre_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end
