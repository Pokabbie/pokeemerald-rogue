Rogue_Route_Field1_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Route_Field1_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Route_Field1_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Route_Field1_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerSouth
	.2byte 0

Rogue_Route_Field1_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Route_Field1_OnFrame0
	.2byte 0


Rogue_Route_Field1_OnFrame0::
	goto_if_set FLAG_ROGUE_WEATHER_ACTIVE, Rogue_Route_Field1_OnFrame0_2
Rogue_Route_Field1_OnFrame0_1:
	goto_if_set FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE, Rogue_Route_Field1_OnFrame0_5
Rogue_Route_Field1_OnFrame0_4:
	setvar VAR_TEMP_1, 1
	end

Rogue_Route_Field1_OnFrame0_2:
	setweather WEATHER_FOG_DIAGONAL
	doweather
	goto Rogue_Route_Field1_OnFrame0_1

Rogue_Route_Field1_OnFrame0_5:
	lockall
	special DoSealedChamberShakingEffect_Short
	waitstate
	playse SE_DOOR
	delay 10
	random 2
	switch VAR_RESULT
	case 0, Rogue_Route_Field1_OnFrame0_9
	case 1, Rogue_Route_Field1_OnFrame0_10
Rogue_Route_Field1_OnFrame0_7:
	releaseall
	goto Rogue_Route_Field1_OnFrame0_4

Rogue_Route_Field1_OnFrame0_9:
	setmetatile 56, 4, 0x064, FALSE
	setmetatile 56, 5, 0x0A7, FALSE
	goto Rogue_Route_Field1_OnFrame0_7

Rogue_Route_Field1_OnFrame0_10:
	setmetatile 22, 5, 0x019, TRUE
	setmetatile 22, 6, 0x021, TRUE
	goto Rogue_Route_Field1_OnFrame0_7

