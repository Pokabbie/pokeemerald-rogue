Rogue_HubTransition_MapScripts::
	map_script MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE, Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE
	.byte 0

Rogue_HubTransition_MapScripts_MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE:
	applymovement OBJ_EVENT_ID_PLAYER, LookNorth
	waitmovement 0
	return


LookNorth:
	face_up
	step_end
