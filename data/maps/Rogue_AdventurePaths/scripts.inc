Rogue_AdventurePaths_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, Rogue_AdventurePaths_RedrawMap
	.byte 0


Rogue_AdventurePaths_RedrawMap::
	special Rogue_ApplyAdventureMetatiles
	return

