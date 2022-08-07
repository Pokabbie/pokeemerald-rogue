Rogue_Route_Field0_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Route_Field0_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Route_Field0_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Route_Field0_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerWest
	.2byte 0

Rogue_Route_Field0_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Route_Field0_OnFrame0
	.2byte 0


Rogue_Route_Field0_OnFrame0::
	goto_if_set FLAG_ROGUE_WEATHER_ACTIVE, Rogue_Route_Field0_OnFrame0_2
Rogue_Route_Field0_OnFrame0_1:
	goto_if_set FLAG_ROGUE_SPECIAL_ENCOUNTER_ACTIVE, Rogue_Route_Field0_OnFrame0_5
Rogue_Route_Field0_OnFrame0_4:
	setvar VAR_TEMP_1, 1
	end

Rogue_Route_Field0_OnFrame0_2:
	setweather WEATHER_SUNNY_CLOUDS
	doweather
	goto Rogue_Route_Field0_OnFrame0_1

Rogue_Route_Field0_OnFrame0_5:
	lockall
	special DoSealedChamberShakingEffect_Short
	waitstate
	playse SE_DOOR
	delay 10
	random 2
	switch VAR_RESULT
	case 0, Rogue_Route_Field0_OnFrame0_9
	case 1, Rogue_Route_Field0_OnFrame0_10
Rogue_Route_Field0_OnFrame0_7:
	releaseall
	goto Rogue_Route_Field0_OnFrame0_4

Rogue_Route_Field0_OnFrame0_9:
	setmetatile 32, 69, METATILE_General_CaveEntrance_Top, FALSE
	setmetatile 32, 70, METATILE_General_CaveEntrance_Bottom, FALSE
	goto Rogue_Route_Field0_OnFrame0_7

Rogue_Route_Field0_OnFrame0_10:
	setmetatile 17, 46, 0x282, FALSE
	goto Rogue_Route_Field0_OnFrame0_7

