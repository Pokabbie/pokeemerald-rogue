Rogue_Route_Mountain3_MapScripts::
	map_script MAP_SCRIPT_ON_RESUME, Rogue_Route_Mountain3_OnResume
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_Route_Mountain3_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	map_script MAP_SCRIPT_ON_FRAME_TABLE, Rogue_Route_Mountain3_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE
	.byte 0

Rogue_Route_Mountain3_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Common_TurnPlayerWest
	.2byte 0

Rogue_Route_Mountain3_MapScripts_MAP_SCRIPT_ON_FRAME_TABLE:
	map_script_2 VAR_TEMP_1, 0, Rogue_Route_Mountain3_OnFrame0
	.2byte 0


Rogue_Route_Mountain3_OnResume::
	setstepcallback STEP_CB_ASH
	return


Rogue_Route_Mountain3_OnFrame0::
	call Rogue_SetDesiredWeather
	setvar VAR_TEMP_1, 1
	end

