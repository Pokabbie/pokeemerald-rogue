Rogue_Route_Chaos0_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Route_Chaos0_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Route_Chaos0_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Route_Chaos0_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerNorth
	.2byte 0

Rogue_Route_Chaos0_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Route_Chaos0_OnFrame0
	.2byte 0


Rogue_Route_Chaos0_OnFrame0::
	call Rogue_SetDesiredWeather
	goto_if_set FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE, Rogue_Route_Chaos0_OnFrame0_2
Rogue_Route_Chaos0_OnFrame0_1:
	setvar VAR_TEMP_1, 1
	end

Rogue_Route_Chaos0_OnFrame0_2:
	lockall
	special DoSealedChamberShakingEffect_Short
	waitstate
	playse SE_DOOR
	delay 10
	random 3
	switch VAR_RESULT
	case 0, Rogue_Route_Chaos0_OnFrame0_6
	case 1, Rogue_Route_Chaos0_OnFrame0_7
	case 2, Rogue_Route_Chaos0_OnFrame0_8
Rogue_Route_Chaos0_OnFrame0_4:
	releaseall
	goto Rogue_Route_Chaos0_OnFrame0_1

Rogue_Route_Chaos0_OnFrame0_6:
	setmetatile 15, 6, 0x1A1, TRUE
	goto Rogue_Route_Chaos0_OnFrame0_4

Rogue_Route_Chaos0_OnFrame0_7:
	setmetatile 42, 63, 0x1A1, TRUE
	goto Rogue_Route_Chaos0_OnFrame0_4

Rogue_Route_Chaos0_OnFrame0_8:
	setmetatile 66, 6, 0x32E, TRUE
	setmetatile 66, 7, 0x336, FALSE
	goto Rogue_Route_Chaos0_OnFrame0_4

